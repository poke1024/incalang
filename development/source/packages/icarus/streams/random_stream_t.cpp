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
//	random_stream_t.cp		   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "random_stream_t.h"

BEGIN_ICARUS_NAMESPACE

fpos_t
random_stream_t::length()
{
	const fpos_t		old_position = tell();	

	seek( 0, seek_mode_end );
	
	const fpos_t		byte_count = tell();
	
	seek( old_position, seek_mode_begin );
	
	return byte_count;
}

void
random_stream_t::skip(
	fpos_t				count )
{
	seek( count, seek_mode_relative );
}

END_ICARUS_NAMESPACE
