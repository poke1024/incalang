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
//	matrix_t.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"
#include "point_t.h"

BEGIN_MACHINE_NAMESPACE

typedef float matrix_t[ 4 ][ 4 ];

void
matrix_set_rotate_about_axis(
	matrix_t&			matrix,
	const point_t*		origin,
	const point_t*		orientation,
	float				angle );

void
matrix_transform(
	matrix_t&			m,
	const point_t*		p,
	point_t*			q );

END_MACHINE_NAMESPACE
