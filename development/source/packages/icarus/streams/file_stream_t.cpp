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
//	file_stream_t.cp		   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "file_stream_t.h"

BEGIN_ICARUS_NAMESPACE

file_stream_t::file_stream_t(
	const char*			filename,
	const char*			permission )
{
	m_file = ifopen( filename, permission );
	if( !m_file )
	{
		istring			s;
		s = "file '";
		s.append( filename );
		s.append( "' could not be opened with permission '" );
		s.append( permission );
		s.append( "'" );
		throw_icarus_error( s.c_str() );
	}
}

file_stream_t::~file_stream_t()
{
	ifclose( m_file );
}

void
file_stream_t::seek(
	fpos_t				where,
	seek_mode			mode )
{
	ifseek( m_file, where, mode );

	if( iferror( m_file ) )
		throw_icarus_error( "io seek error" );
}

fpos_t
file_stream_t::tell()
{
	return iftell( m_file );
}

fpos_t
file_stream_t::read_some_bytes(
	void*				buffer,
	fpos_t				count )
{
	return ifread( buffer, count, m_file );
}

fpos_t
file_stream_t::write_some_bytes(
	const void*			buffer,
	fpos_t				count )
{
	return ifwrite( buffer, count, m_file );
}

void
file_stream_t::flush()
{
	ifflush( m_file );
}

END_ICARUS_NAMESPACE
