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
//	lib_string.h			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"
#include "fiber_t.h"
#include "string_t.h"

BEGIN_MACHINE_NAMESPACE

#define STRING_CLASS_SIZE ( sizeof( string_t ) + CLASS_HEADER_SIZE )

void
init_string(
	u08*				block,
	machine_t&			m );

inline string_t*
pop_string(
	fiber_t&			f )
{
	instance_t*			param = f.pop_instance();
	string_t*			string = INSTANCE_SELF( string_t, param );

#if CMA_MACHINE
	long				length = string->length;
	if( length )
		f.check_access( string->text, length + 1 );
#endif

	return string;
}

END_MACHINE_NAMESPACE
