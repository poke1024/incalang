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
//	machine.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine.h"

BEGIN_MACHINE_NAMESPACE

static void*			s_buffer = 0;
static long				s_buffer_size = 0;

void*
obtain_buffer(
	size_t				size )
{
	if( size <= s_buffer_size )
		return s_buffer;
		
	if( s_buffer_size == 0 )
		s_buffer_size = 1;
		
	while( size >= s_buffer_size )
		s_buffer_size *= 2;
		
	if( s_buffer )
		delete[] (u08*)s_buffer;
		
	s_buffer = new u08[ s_buffer_size ];
	
	return s_buffer;
}

END_MACHINE_NAMESPACE
