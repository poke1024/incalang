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
//	lib_array.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"

BEGIN_MACHINE_NAMESPACE

struct array_t {
	void*				data;
	u32					count;
	u32					alloc;
};

inline array_t*
pop_array(
	fiber_t&			f,
	size_t				size )
{
	instance_t*			param = f.pop_instance();
	array_t*			array = INSTANCE_SELF( array_t, param );

#if CMA_MACHINE
	f.check_access( array->data, array->alloc * size );
#endif

	return array;
}

END_MACHINE_NAMESPACE
