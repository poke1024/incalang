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
//	lib_utils.cpp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine.h"
#include "fiber_t.h"
#include "lib_utils.h"

BEGIN_MACHINE_NAMESPACE

void
print_pointer(
	char*				buf,
	int					size,
	void*				ptr )
{
	if( ptr )
		isnprintf( buf, size, "0x%lx", (long)ptr );
	else
		strcpy( buf, "<null>" );
}

END_MACHINE_NAMESPACE