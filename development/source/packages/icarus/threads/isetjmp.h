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
//	isetjmp.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_SETJMP_H
#define ICARUS_SETJMP_H

//#pragma once

#include "rt.h"
#include "cpu_state_t.h"

BEGIN_RT_NAMESPACE

typedef cpu_state_t		ijmpbuf_t;

int
isetjmp(
	ijmpbuf_t*			jmpbuf,
	int					param );

void
ilongjmp(
	ijmpbuf_t*			jmpbuf );

END_RT_NAMESPACE

#endif

