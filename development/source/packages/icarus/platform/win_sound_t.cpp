// ===========================================================================
//	win_sound_t.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#if 0

#include "win_sound_t.h"
#include "file_stream_t.h"

BEGIN_ICARUS_NAMESPACE

extern LPDIRECTSOUND	lpds;

enum {
	CHUNK_TYPE_RIFF		= 'RIFF',
	CHUNK_TYPE_FORMAT	= 'fmt ',
	CHUNK_TYPE_DATA		= 'data'
	//WAVE_FORMAT_PCM		= 1
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

win_sound_t::win_sound_t() :

	m_buffer( 0 )
{
}

win_sound_t::~win_sound_t()
{
	dispose();
}

void
win_sound_t::dispose()
{
	sound_t::dispose();

	if( m_buffer )
	{
		m_buffer->Release();
		m_buffer = 0;
	}
}

void
win_sound_t::load(
	const char*			path )
{
	dispose();

	file_stream_t		stream( path, "rb" );
	
	read_wave( stream );
}

void
win_sound_t::read_wave(
	random_stream_t&	stream )
{	
	if( !lpds )
		return;

	find_chunk_of_type( stream, CHUNK_TYPE_FORMAT );

	WAVEFORMATEX		wavefmt;

	stream.read_u32();
	wavefmt.wFormatTag = swap_endian16( stream.read_u16() );
	wavefmt.nChannels = swap_endian16( stream.read_u16() );
	wavefmt.nSamplesPerSec = swap_endian32( stream.read_u32() );
	wavefmt.nAvgBytesPerSec = swap_endian32( stream.read_u32() );
	wavefmt.nBlockAlign = swap_endian16( stream.read_u16() );
	wavefmt.wBitsPerSample = swap_endian16( stream.read_u16() );
	wavefmt.cbSize = sizeof( wavefmt );

	if( wavefmt.wFormatTag != WAVE_FORMAT_PCM )
		throw_icarus_error( "wave file isn't PCM format" );

	long				data_size;

	find_chunk_of_type( stream, CHUNK_TYPE_DATA );
	
	data_size = swap_endian32( stream.read_u32() );

	DSBUFFERDESC		bufdesc;

	memset( &bufdesc, 0, sizeof( DSBUFFERDESC ) );
	bufdesc.dwSize = sizeof( DSBUFFERDESC );
	bufdesc.dwFlags =
		DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	bufdesc.dwBufferBytes = data_size;
	bufdesc.lpwfxFormat = &wavefmt;
	if( lpds->CreateSoundBuffer( &bufdesc, &m_buffer, NULL ) != DS_OK )
		throw_icarus_error( "dsound error" );

	void*				write1 = 0;
	void*				write2 = 0;
	unsigned long		length1, length2;
	
	m_buffer->Lock( 0, data_size,
		&write1, &length1, &write2, &length2, 0 );
	if( write1 > 0 )
		stream.read_bytes( write1, length1 );
	if( write2 > 0 )
		stream.read_bytes( write2, length2 );
	m_buffer->Unlock( write1, length1, write2, length2 );

	/*long				num_frames;
	
	num_frames = ( data_size * 8L ) / ( num_channels * bits_per_sample );

	m_samples = new u08[ data_size ];
	stream.read_bytes( m_samples, data_size );*/
}

END_ICARUS_NAMESPACE

#endif

