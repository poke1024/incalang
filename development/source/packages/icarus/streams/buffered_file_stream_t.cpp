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
//	buffered_file_stream_t.cp  ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "buffered_file_stream_t.h"

BEGIN_ICARUS_NAMESPACE

enum {
	BUFFER_SIZE			= 16384
};

inline void
buffered_file_stream_t::inval_cache()
{
	m_mark = 0;
}

// ---------------------------------------------------------------------------

buffered_file_stream_t::buffered_file_stream_t(
	const char*			filename,
	const char*			permission ) :
	
	file_stream_t( filename, permission ),
	m_mark( 0 )
{
	try {
		m_buffer = new u08[ BUFFER_SIZE ];
	} catch(...) {
		ifclose( m_file );
		throw;
	}
}

buffered_file_stream_t::~buffered_file_stream_t()
{
	delete[] m_buffer;
}

void
buffered_file_stream_t::seek(
	fpos_t				where,
	seek_mode			mode )
{
	ifseek( m_file, where, mode );

	if( iferror( m_file ) )
		throw_icarus_error( "io seek error" );

	inval_cache();
}

fpos_t
buffered_file_stream_t::tell()
{
	fpos_t				pos;

	if( m_mark )
		pos = m_offset + m_mark - m_buffer;
	else
		pos = iftell( m_file );
		
	return pos;
}

fpos_t
buffered_file_stream_t::read_some_bytes(
	void*				buffer,
	fpos_t				count )
{
	fpos_t				read = 0;

	while( true )
	{
		if( m_mark )
		{
			if( count <= m_cached )
			{
				std::memcpy( buffer, m_mark, count );
				m_mark += count;
				if( ( m_cached -= count  ) == 0 )
					fill_cache();
				return read + count;
			}
			else if( m_cached )
			{
				std::memcpy( buffer, m_mark, m_cached );
				buffer = (void*)( ( (u08*)buffer ) + m_cached );
				count -= m_cached;
				read += m_cached;
				fill_cache();
			}
			else
				return read;
		}
		else if( count < BUFFER_SIZE )
			fill_cache();
		else
			break;
	}

	return ifread( buffer, count, m_file );
}

fpos_t
buffered_file_stream_t::write_some_bytes(
	const void*			buffer,
	fpos_t				count )
{
	inval_cache();

	return ifwrite( buffer, count, m_file );
}

void
buffered_file_stream_t::flush()
{
	ifflush( m_file );
}

// ---------------------------------------------------------------------------

void
buffered_file_stream_t::fill_cache()
{
	m_offset = iftell( m_file );

	m_cached = ifread(
		m_buffer, BUFFER_SIZE, m_file );

	m_mark = m_buffer;
}

END_ICARUS_NAMESPACE
