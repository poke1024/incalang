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
//	pipe_stream_t.cpp		   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "pipe_stream_t.h"

BEGIN_ICARUS_NAMESPACE

pipe_stream_t::pipe_stream_t(
	input_stream_t*		in,
	output_stream_t*	out )
{
	m_in = in;
	m_out = out;
	m_write_flag = false;
}

fpos_t
pipe_stream_t::read_some_bytes(
	void*				buffer,
	fpos_t				count )
{
	if( m_write_flag )
	{
		m_out->flush();
		m_write_flag = false;
	}

	return m_in->read_some_bytes( buffer, count );
}

fpos_t
pipe_stream_t::write_some_bytes(
	const void*			buffer,
	fpos_t				count )
{
	m_write_flag = true;
	return m_out->write_some_bytes( buffer, count );
}

void
pipe_stream_t::flush()
{
	m_out->flush();
}

void
pipe_stream_t::seek(
	fpos_t				where,
	seek_mode			mode )
{
	throw_icarus_error( "illegal operation on pipe stream" );
}
							
fpos_t
pipe_stream_t::tell()
{
	throw_icarus_error( "illegal operation on pipe stream" );
	return 0;
}

END_ICARUS_NAMESPACE
