// ===========================================================================
//	mac_sound_t.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "mac_sound_t.h"
#include "file_stream_t.h"

BEGIN_ICARUS_NAMESPACE

inline void
check_for_mac_sound_error(
	OSErr				err )
{
#if ICARUS_DEBUG
	if( err != noErr )
		throw_icarus_error( "error playing mac sound" );
#else
	err;
#endif
}

enum {
	CHUNK_TYPE_RIFF		= 'RIFF',
	CHUNK_TYPE_FORMAT	= 'fmt ',
	CHUNK_TYPE_DATA		= 'data',
	WAVE_FORMAT_PCM		= 1
};

void
find_chunk_of_type(
	random_stream_t&	stream,
	u32					type )
{
	stream.seek( 0, seek_mode_begin );

	while( true )
	{
		u32				chunk_type;
	
		chunk_type = stream.read_u32();
	
		if( chunk_type == type )
			break;
		
		u32				offset;
		
		if( chunk_type == CHUNK_TYPE_RIFF )
			offset = 4 + 4;
		else
		{
			offset = swap_endian32( stream.read_u32() );
			
			if( offset & 1 )
				offset += 1;
		}
		
		stream.seek( offset, seek_mode_relative );
	}
}

// ---------------------------------------------------------------------------

mac_sound_t::mac_sound_t() :

	m_samples( 0 )
{
}

mac_sound_t::~mac_sound_t()
{
	dispose();
}

void
mac_sound_t::dispose()
{
	sound_t::dispose();

	if( m_samples )
	{
		delete[] m_samples;
		m_samples = 0;
	}
}

void
mac_sound_t::load(
	const char*			path )
{
	dispose();

	file_stream_t		stream( path, "rb" );
	
	read_wave( stream );
}

void
mac_sound_t::read_wave(
	random_stream_t&	stream )
{	
	find_chunk_of_type( stream, CHUNK_TYPE_FORMAT );
	
	int					format_type;
	int					num_channels;
	long				sample_rate;
	int					bits_per_sample;
	
	stream.read_u32();
	format_type = swap_endian16( stream.read_u16() );
	num_channels = swap_endian16( stream.read_u16() );
	sample_rate = swap_endian32( stream.read_u32() );
	stream.read_u32();
	stream.read_u16();
	bits_per_sample = swap_endian16( stream.read_u16() );

	if( format_type != WAVE_FORMAT_PCM )
		throw_icarus_error( "wave file isn't PCM format" );

	long				data_size;

	find_chunk_of_type( stream, CHUNK_TYPE_DATA );
	
	data_size = swap_endian32( stream.read_u32() );

	long				num_frames;
	
	num_frames = ( data_size * 8L ) / ( num_channels * bits_per_sample );

	m_samples = new u08[ data_size ];
	stream.read_bytes( m_samples, data_size );

	m_header.samplePtr = (Ptr)m_samples;
	m_header.numChannels = num_channels;
	m_header.sampleRate = Long2Fix( sample_rate );
	m_header.loopStart = 0;
	m_header.loopEnd = 0;
	m_header.encode = cmpSH;
	m_header.baseFrequency = kMiddleC;
	m_header.numFrames = num_frames;
	m_header.markerChunk = NULL;
	m_header.format = kSoundNotCompressed;
	m_header.futureUse2 = 0;
	m_header.stateVars = NULL;
	m_header.leftOverSamples = NULL;
	m_header.compressionID = notCompressed;
	m_header.packetSize = 0;
	m_header.snthID = 0;
	m_header.sampleSize = bits_per_sample;
	
	if( bits_per_sample == 16 )
	{
		m_header.format = k16BitLittleEndianFormat;
		m_header.compressionID = fixedCompression;
	}
}

END_ICARUS_NAMESPACE
