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
//	lib_bigint.h			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine.h"

BEGIN_MACHINE_NAMESPACE

class bigint_t;

struct bigint_wrap_t {
	bigint_t*			bigint;
};

#define BIGINT_CLASS_SIZE ( sizeof( bigint_wrap_t ) + CLASS_HEADER_SIZE )

inline bigint_t*
pop_bigint(
	fiber_t&			f )
{
	instance_t*			param = f.pop_instance();
	bigint_t*			bigint;
	
	bigint = INSTANCE_SELF( bigint_wrap_t, param )->bigint;

	return bigint;
}

END_MACHINE_NAMESPACE
