// This file is part of incalang.

// incalang is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.

// incalang is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Foobar; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// ===========================================================================
//	deflater_stream_t.h		   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "deflater_stream_t.h"

BEGIN_ICARUS_NAMESPACE

const s32				Z_BUFSIZE = 16384;

deflater_stream_t::deflater_stream_t(
	output_stream_t&	stream,
	int					level ) :
	
	m_output_stream( stream )							
{
	m_buffer = new u08[ Z_BUFSIZE ];

	z_stream*			zip_stream = &m_zip_stream;

	zip_stream->zalloc = (alloc_func)0;
	zip_stream->zfree = (free_func)0;
	zip_stream->opaque = (voidpf)0;
	zip_stream->next_in =  Z_NULL;
	zip_stream->next_out =  Z_NULL;
	zip_stream->avail_in = zip_stream->avail_out = 0;
	zip_stream->next_out = m_buffer;

	int						zerr;
	
	zerr = deflateInit( zip_stream, level );

	if( zerr != Z_OK )
	{
		delete[] m_buffer;
		throw_icarus_error( "zlib deflateInit failed" );
	}
	
	zip_stream->avail_out = Z_BUFSIZE;
}

deflater_stream_t::~deflater_stream_t()
{
	flush_buffers( Z_FINISH );

	deflateEnd( &m_zip_stream );

	delete[] m_buffer;
}

void
deflater_stream_t::set_params(
	int					level,
	int					strategy )
{
	int						zerr;

	zerr = deflateParams( &m_zip_stream, level, strategy );
	if( zerr != Z_OK )
		throw_icarus_error( "zlib deflateParams failed" );
}

fpos_t
deflater_stream_t::write_some_bytes(
	const void*			buffer,
	fpos_t				count )
{
	z_stream*			stream = &m_zip_stream;

	stream->next_in = (Bytef*)buffer;
	stream->avail_in = count;
	
	while( stream->avail_in != 0 )
	{
		if( stream->avail_out == 0 )
		{
			stream->next_out = m_buffer;
			
			m_output_stream.write_bytes( m_buffer, Z_BUFSIZE );

			stream->avail_out = Z_BUFSIZE;
		}
		
		int				zerr;
		
		zerr = deflate( stream, Z_NO_FLUSH );
		
		if( zerr != Z_OK )
			throw_icarus_error( "zlib deflate error" );
	}
	
	return count;
}

void
deflater_stream_t::flush()
{
	flush_buffers( Z_SYNC_FLUSH );
}

void
deflater_stream_t::flush_buffers(
	int					flush )
{
	z_stream*			stream = &m_zip_stream;

	stream->avail_in = 0;
	
	int					z_err;
	bool				done = false;
	
	while( true )
	{
		u32				length = Z_BUFSIZE - stream->avail_out;

		if( length != 0 )
		{
			m_output_stream.write_bytes( m_buffer, length );

			stream->next_out = m_buffer;
			stream->avail_out = Z_BUFSIZE;
		}
		
		if( done )
			break;
		
		z_err = deflate( stream, flush );

		// ignore the second of two consecutive flushes
		if( length == 0 && z_err == Z_BUF_ERROR )
			z_err = Z_OK;

		// deflate has finished flushing only when it hasn't used up
		// all the available space in the output buffer
		done = ( stream->avail_out != 0 || z_err == Z_STREAM_END );

		if( z_err != Z_OK && z_err != Z_STREAM_END )
			break;
	}
	
	if( z_err != Z_OK && z_err != Z_STREAM_END )
		throw_icarus_error( "zlib deflate error" );
}

END_ICARUS_NAMESPACE
