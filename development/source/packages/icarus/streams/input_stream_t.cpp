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
//	input_stream_t.cp			   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "input_stream_t.h"
#include "st_mem_track.h"

BEGIN_ICARUS_NAMESPACE

const int				TEMP_BUFFER_SIZE = 1024;

static u08*				s_temp_buffer = 0;

input_stream_t::input_stream_t()
{
	st_mem_track		track( false );

	if( !s_temp_buffer )
		s_temp_buffer = new u08[ TEMP_BUFFER_SIZE ];
}

input_stream_t::~input_stream_t()
{
}

void
input_stream_t::read_string(
	istring&			string )
{
	u32					length = read_u32();

	string.clear();
	
	if( length < 1 )
		return;
	
	string.reserve( length );
		
	u08*				buffer = s_temp_buffer;
	const int			buffer_size = TEMP_BUFFER_SIZE / sizeof( char );
		
	while( length >= buffer_size )
	{
		read_bytes( buffer, buffer_size * sizeof( char ) );
		length -= buffer_size;
		string.append( (char*)buffer, buffer_size );
	}
	
	if( length > 0 )
	{
		read_bytes( buffer, length * sizeof( char ) );
		string.append( (char*)buffer, length );
	}
}

void
input_stream_t::read_bytes(
	void*				buffer,
	fpos_t				count )
{
	if( read_some_bytes( buffer, count ) != count )
		throw_icarus_error( "io read error" );
}

void
input_stream_t::skip(
	fpos_t				count )
{
	if( !count )
		return;

	while( count >= TEMP_BUFFER_SIZE )
	{
		read_bytes( s_temp_buffer, count );
		count -= TEMP_BUFFER_SIZE;
	}
	
	if( count > 0 )
		read_bytes( s_temp_buffer, count );
}

END_ICARUS_NAMESPACE
