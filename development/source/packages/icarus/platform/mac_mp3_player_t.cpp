// ===========================================================================
//	mac_mp3_player_t.cpp	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "mp3_player_t.h"
#include "file_stream_t.h"
#include <Movies.h>
#include <string.h>
#include <stdio.h>

BEGIN_ICARUS_NAMESPACE

// ===========================================================================

#define	kInputBufferSize			32768
#define	kOutputBufferMargin			256

#define	kMaxValue					65536
#define	kBitsPerByte				8

#if PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
	#pragma pack(2)
#endif

typedef struct {
	long	atomSize;			// how big this structure is (big endian)
	long	atomType;			// atom type
	char	waveData[ 28 ];
} AtomQDMCWaveFormatEx;

typedef struct {
	AudioFormatAtom			formatData;
	AtomQDMCWaveFormatEx	endianData;
	AudioTerminatorAtom		terminatorData;
} AudioCompressionAtom, *AudioCompressionAtomPtr, **AudioCompressionAtomHandle;

#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
	#pragma pack()
#endif

// ===========================================================================

static OSErr
PreloadMP3File(
	FSSpec*				inMP3File,
	AudioFormatAtomPtr*	outAudioAtom,
	CmpSoundHeaderPtr	outSoundHeader )
{
	Movie				theMovie;
	short				refNum;
	short				resID = 0; // we want the first movie
	Boolean				wasChanged;
	
	OSErr				err = noErr;
	
	err = OpenMovieFile( inMP3File, &refNum, fsRdPerm );
	if( err )
		return err;

	// instantiate the movie
	err = NewMovieFromFile( &theMovie, refNum, &resID,
		NULL, newMovieActive, &wasChanged );
	CloseMovieFile( refNum );
	if( err )
		return err;
	refNum = 0;	
	
	Track				theTrack;
			
	// get the first sound track
	theTrack = GetMovieIndTrackType( theMovie, 1,
		SoundMediaType, movieTrackMediaType );
	if( theTrack != NULL )
	{
		Media			theMedia;

		// get the sound track media
		theMedia = GetTrackMedia( theTrack );
		if( theMedia != NULL )
		{			
			Size		size;
			Handle		extension;
			
			// Version 1 of this record includes four extra
			// fields to store information about compression
			// ratios. It also defines how other extensions
			// are added to the SoundDescription record.
			// All other additions to the SoundDescription
			// record are made using QT atoms. That means
			// one or more atoms can be appended to the end
			// of the SoundDescription record using the standard
			// [size, type] mechanism used throughout the QuickTime
			// movie resource architecture.
			// http://developer.apple.com/techpubs/
			//   quicktime/qtdevdocs/RM/frameset.htm
			SoundDescriptionV1Handle sourceSoundDescription =
				(SoundDescriptionV1Handle)NewHandle( 0 );
				
			if( sourceSoundDescription == 0 )
				return notEnoughMemoryErr;
			
			// get the description of the sample data
			GetMediaSampleDescription( theMedia, 1,
				(SampleDescriptionHandle)sourceSoundDescription );
			err = GetMoviesError();

			if( err == noErr )
			{
				extension = NewHandle( 0 );
				if( extension == 0 )
					err = notEnoughMemoryErr;
			}

			if( err )
			{
				DisposeHandle( (Handle)sourceSoundDescription );
				return err;
			}
			
			// get the "magic" decompression atom
			// This extension to the SoundDescription information stores
			// data specific to a given audio decompressor. Some audio
			// decompression algorithms require a set of out-of-stream
			// values to configure the decompressor which are stored in
			// a siDecompressionParams atom. The contents of the
			// siDecompressionParams atom are dependent
			// on the audio decompressor.
			err = GetSoundDescriptionExtension(
				(SoundDescriptionHandle)sourceSoundDescription,
				&extension, siDecompressionParams );
			
			if( noErr == err )
			{
				size = GetHandleSize( extension );
				HLock( extension );
				*outAudioAtom = (AudioFormatAtom*)NewPtr( size );
				err = MemError();
				// copy the atom data to our buffer...
				BlockMoveData( *extension, *outAudioAtom, size );
				HUnlock( extension );
			}
			else
				// if it doesn't have an atom, that's ok
				err = noErr;
			
			
			// set up our sound header
			outSoundHeader->format =
				(*sourceSoundDescription)->desc.dataFormat;
			outSoundHeader->numChannels =
				(*sourceSoundDescription)->desc.numChannels;
			outSoundHeader->sampleSize =
				(*sourceSoundDescription)->desc.sampleSize;
			outSoundHeader->sampleRate =
				(*sourceSoundDescription)->desc.sampleRate;
			
			DisposeHandle( extension );
			DisposeHandle( (Handle)sourceSoundDescription );
		}
	}
	
	return err;
}

// ===========================================================================

static pascal void
AudioCallBack(
	SndChannelPtr			channel,
	SndCommand*				command )
{
	( (mac_mp3_player_t*)channel->userInfo )->callback( command );
}

static pascal void
AudioDeferred(
	long					data )
{
	( (mac_mp3_player_t*)data )->deferred();
}

// ===========================================================================

mac_mp3_player_t::mac_mp3_player_t()
{
	m_channel = 0;
	m_handle = 0;
	m_audio_buffer1 = 0;
	m_audio_buffer2 = 0;
	m_sc = 0;
	m_decompression_atom = 0;
	m_audio_running = false;
}

mac_mp3_player_t::~mac_mp3_player_t()
{
	if( m_channel )
		stop();
}

void
mac_mp3_player_t::start(
	const char*				path )
{
	if( m_channel )
		stop();

	OSErr					err;
	FSSpec					spec;
	char					buf[ 10 ];
	
	sprintf( buf, "**fm%04ld", (long)TickCount() );
	memcpy( spec.name + 1, buf, 8 );
	spec.name[ 0 ] = 8;
	err = FindFolder(
		kOnSystemDisk, kTemporaryFolderType, true,
		&spec.vRefNum, &spec.parID );
	if( err != noErr )
		return;

	file_stream_t			stream( path, "rb" );
	u32						mp3_length;
	
	mp3_length = stream.length();
	m_handle = NewHandle( mp3_length );
	if( m_handle == 0 )
		return;
	HLock( m_handle );
	stream.read_bytes( *m_handle, mp3_length );
	HUnlock( m_handle );
	m_size = mp3_length;
		
	err = FSpCreate( &spec, 'TVOD', 'Mp3 ', smSystemScript );
	if( err == noErr )
	{
		short				refnum;
		err = FSpOpenDF( &spec, fsWrPerm, &refnum );
		if( err == noErr )
		{
			long			count = mp3_length;
			
			if( count > 8192 )
				count = 8192;
			
			HLock( m_handle );
			err = FSWrite( refnum, &count, *m_handle );
			HUnlock( m_handle );
			FSClose( refnum );
		}
	}
	
	CmpSoundHeader			sound_header;

	if( err == noErr )
		err = PreloadMP3File(
			&spec,
			(AudioFormatAtomPtr*)&m_decompression_atom,
			&sound_header );
	FSpDelete( &spec );

	if( err )
	{
		cleanup();
		return;
	}

	SoundComponentData		input_format;

	input_format.flags = 0;
	input_format.format = sound_header.format;
	input_format.numChannels = sound_header.numChannels;
	input_format.sampleSize = sound_header.sampleSize;
	input_format.sampleRate = sound_header.sampleRate;
	input_format.sampleCount = 0;
	input_format.buffer = nil;
	input_format.reserved = 0;
	
	SoundComponentData		output_format;

	output_format.flags = 0;
	output_format.format = kSoundNotCompressed;
	output_format.numChannels = input_format.numChannels;
	output_format.sampleSize = input_format.sampleSize;
	output_format.sampleRate = input_format.sampleRate;
	output_format.sampleCount = 0;
	output_format.buffer = nil;
	output_format.reserved = 0;

	err = SoundConverterOpen( &input_format, &output_format, &m_sc );
	if( err == noErr )
		err = SoundConverterSetInfo(
			m_sc, siDecompressionParams, m_decompression_atom );
	if( err == noErr )
		err = SoundConverterGetBufferSizes(
			m_sc, kInputBufferSize, &m_input_frames,
			&m_input_bytes, &m_output_bytes );
	if( err == noErr )
		err = SoundConverterBeginConversion( m_sc );

	m_audio_buffer1 = 0;
	m_audio_buffer2 = 0;

	if( err == noErr )
	{
		m_audio_buffer1 = NewPtr( m_output_bytes + kOutputBufferMargin );
		if( m_audio_buffer1 == 0 )
			err = notEnoughMemoryErr;
	}

	if( err == noErr )
	{
		m_audio_buffer2 = NewPtr( m_output_bytes + kOutputBufferMargin );
		if( m_audio_buffer2 == 0 )
			err = notEnoughMemoryErr;
	}

	UniversalProcPtr		routine = NewSndCallBackUPP( AudioCallBack );

	if( err == noErr )
		err = SndNewChannel( &m_channel,
			sampledSynth, initStereo, routine );
	
	if( err )
	{
		cleanup();
		return;
	}
	
	m_channel->userInfo = (long)this;
	
	audio_clean_buffer( (u16*)m_audio_buffer1 );
	audio_clean_buffer( (u16*)m_audio_buffer2 );

	m_audio_buffer_header.numChannels = input_format.numChannels;
	m_audio_buffer_header.sampleRate = input_format.sampleRate;
	m_audio_buffer_header.loopStart = 0;
	m_audio_buffer_header.loopEnd = 0;
	m_audio_buffer_header.encode = extSH;
	m_audio_buffer_header.baseFrequency = kMiddleC;
	m_audio_buffer_header.numFrames = m_output_bytes /
		( input_format.numChannels * input_format.sampleSize / kBitsPerByte );

	m_audio_buffer_header.markerChunk = nil;
	m_audio_buffer_header.instrumentChunks = nil;
	m_audio_buffer_header.AESRecording = nil;
	m_audio_buffer_header.sampleSize = input_format.sampleSize;
	m_audio_buffer_header.futureUse1 = 0;
	m_audio_buffer_header.futureUse2 = 0;
	m_audio_buffer_header.futureUse3 = 0;
	m_audio_buffer_header.futureUse4 = 0;

	SndCommand					command;
	
	command.cmd = soundCmd;
	command.param1 = 0;
	command.param2 = (long)&m_audio_buffer_header;
	SndDoImmediate( m_channel, &command );

	m_dtask.qType = dtQType;
	m_dtask.dtFlags = 0;
	m_dtask.dtAddr = NewDeferredTaskProc( AudioDeferred );
	m_dtask.dtReserved = 0;
	
	m_total_input_bytes = 0;
	m_current_buffer = m_audio_buffer1;
	deferred();

	m_audio_running = true;
	m_deferred_task_fired = true;
	m_loop_point = 0;

	command.cmd = nullCmd;
	command.param1 = 0;
	command.param2 = 0;
	callback( &command );
}

void
mac_mp3_player_t::stop()
{
	if( m_channel == 0 )
		return;
		
	m_audio_running = false;
	audio_clean_buffer( (u16*)m_audio_buffer1 );
	audio_clean_buffer( (u16*)m_audio_buffer2 );

	cleanup();
}

void
mac_mp3_player_t::set_volume(
	u16						volume )
{	
	if( m_channel == 0 )
		return;

	SndCommand				command;
		
	command.cmd = volumeCmd;
	command.param1 = 0;
	command.param2 = ( ( (u32)volume ) << 16 ) | volume;
	SndDoImmediate( m_channel, &command );
}

void
mac_mp3_player_t::cleanup()
{
	if( m_channel )
		SndDisposeChannel( m_channel, true );
	m_channel = 0;

	if( m_audio_buffer1 )
		DisposePtr( m_audio_buffer1 );
	if( m_audio_buffer2 )
		DisposePtr( m_audio_buffer2 );
	m_audio_buffer1 = 0;
	m_audio_buffer2 = 0;

	if( m_handle )
		DisposeHandle( m_handle );
	m_handle = 0;

	if( m_sc )
		SoundConverterClose( m_sc );
	m_sc = 0;

	if( m_decompression_atom )
		DisposePtr( m_decompression_atom );
	m_decompression_atom = 0;
}

void
mac_mp3_player_t::audio_clean_buffer(
	u16*					buffer )
{
	memset( buffer, 0, m_output_bytes );
}

void
mac_mp3_player_t::callback(
	SndCommand*				/*command*/ )
{
	SndCommand				theCommand;
	
	if( !m_audio_running )
		return;
	
	m_audio_buffer_header.samplePtr = m_current_buffer;
	theCommand.cmd = bufferCmd;
	theCommand.param1 = 0;
	theCommand.param2 = (long)&m_audio_buffer_header;
	SndDoCommand( m_channel, &theCommand, true );
	
	if( m_deferred_task_fired )
	{
		if( m_current_buffer == m_audio_buffer1 )
			m_current_buffer = m_audio_buffer2;
		else
			m_current_buffer = m_audio_buffer1;

		m_dtask.dtParam = (long)this;
		m_deferred_task_fired = false;

		DTInstall( (DeferredTaskPtr)&m_dtask );
	}
	
	theCommand.cmd = callBackCmd;
	theCommand.param1 = 0;
	theCommand.param2 = 0;
	SndDoCommand( m_channel, &theCommand, true );
}

void
mac_mp3_player_t::deferred()
{
	u16*					buffer = (u16*)m_current_buffer;
	
	Ptr						input_ptr;
	unsigned long			output_frames_local;
	unsigned long			output_bytes_local;

	m_deferred_task_fired = true;

	if( m_total_input_bytes + m_input_bytes > m_size )
		m_total_input_bytes = m_loop_point;
	
	HLock( m_handle );
	input_ptr = *m_handle + m_total_input_bytes;
	SoundConverterConvertBuffer(
		m_sc, input_ptr, m_input_frames, buffer,
		&output_frames_local, &output_bytes_local );
	HUnlock( m_handle );
	
	m_total_input_bytes += m_input_bytes;
	
	m_audio_buffer_header.numFrames = output_frames_local;
}

END_ICARUS_NAMESPACE
