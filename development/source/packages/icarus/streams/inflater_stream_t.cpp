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
//	inflater_stream_t.h		   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "inflater_stream_t.h"
#include <algorithm>

BEGIN_ICARUS_NAMESPACE

const s32				Z_BUFSIZE = 16384;

inflater_stream_t::inflater_stream_t(
	input_stream_t&		stream,
	u32					avail ) :
	
	m_input_stream( stream )							
{
	m_total_avail = avail;

	m_buffer = new u08[ Z_BUFSIZE ];

	z_stream*			zip_stream = &m_zip_stream;

	zip_stream->zalloc = (alloc_func)0;
	zip_stream->zfree = (free_func)0;
	zip_stream->opaque = (voidpf)0;
	zip_stream->next_in =  Z_NULL;
	zip_stream->next_out =  Z_NULL;
	zip_stream->avail_in = zip_stream->avail_out = 0;
	zip_stream->next_in  = m_buffer;
	
	int						zerr;
	
	icarus_debug_disable( 0 );
	// since we don't track inflateEnd further down, don't
	// track this too or we get memory leak reports
	zerr = inflateInit( zip_stream );
	icarus_debug_enable( 0 );

	if( zerr != Z_OK )
	{
		delete[] m_buffer;
		throw_icarus_error( "zlib inflateInit failed" );
	}
	
	zip_stream->avail_out = Z_BUFSIZE;
}

inflater_stream_t::~inflater_stream_t()
{
	icarus_debug_disable( 0 );
	// SpotLight reports a problem that zlib is releasing
	// a C++ object, but this doesn't seem to be correct.
	inflateEnd( &m_zip_stream );
	icarus_debug_enable( 0 );

	delete[] m_buffer;
}

fpos_t
inflater_stream_t::read_some_bytes(
	void*				buffer,
	fpos_t				count )
{
	z_stream*			stream = &m_zip_stream;

	stream->next_out = (Bytef*)buffer;
	stream->avail_out = count;
	
	while( stream->avail_out != 0 )
	{
		if( stream->avail_in == 0 )
		{
			if( m_total_avail == 0 )
				throw_icarus_error( "read beyond eof" );
			
			const s32	byte_count = std::min( m_total_avail, Z_BUFSIZE );
			
			m_input_stream.read_bytes( m_buffer, byte_count );
			
			m_total_avail -= byte_count;
			
			stream->avail_in = byte_count;
			
			stream->next_in = m_buffer;
		}
		
		int				zerr;
		
		zerr = inflate( stream, Z_NO_FLUSH );
		
		if( zerr == Z_STREAM_END )
			break;
		else if( zerr != Z_OK )
			throw_icarus_error( "zlib inflate error" );
	}
	
	return count - stream->avail_out;
}

END_ICARUS_NAMESPACE
