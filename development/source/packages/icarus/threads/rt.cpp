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
//	rt.cpp					   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "rt.h"

#if __GNUC__
#include <stdio.h>
#endif

BEGIN_RT_NAMESPACE

void
rt_trace(
	const char*						error )
{
	fprintf( stderr, error );
}

void
rt_fatal(
	const char*						error )
{
	fprintf( stderr, error );
	exit( 1 );
}

bool
rt_key_down(
	long							key )
{
	// TODO
	return false;
}

void
rt_input_update()
{
}

END_RT_NAMESPACE

