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
//	output_stream_t.cp		   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "output_stream_t.h"

BEGIN_ICARUS_NAMESPACE

output_stream_t::~output_stream_t()
{
}

void
output_stream_t::reserve(
	fpos_t				size )
{
}

void
output_stream_t::write_bytes(
	const void*			buffer,
	fpos_t				count )
{
	if( write_some_bytes( buffer, count ) != count )
		throw_icarus_error( "io write error" );
}

void
output_stream_t::write_string(
	const char*			string )
{
	u32					length = std::strlen( string );

	write_u32( length );
	write_bytes( string, length );
}

END_ICARUS_NAMESPACE
