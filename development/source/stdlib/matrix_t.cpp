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
//	matrix_t.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "matrix_t.h"
#include <math.h>

BEGIN_MACHINE_NAMESPACE

using namespace std;

void
matrix_set_rotate_about_axis(
	matrix_t&			matrix,
	const point_t*		origin,
	const point_t*		orientation,
	float				angle )
{
	// [translate -origin]
	// [rotate around vector]
	// [translate origin]
	
	float				st, ct, c_0 = 0, c_1 = 1, _1_ct;
	float				vx, vy, vz;
	float				m0, m1, m2;
	float				tx, ty, tz;
	
	st = sin( angle );
	ct = cos( angle );
	_1_ct = c_1 - ct;
	
	vx = -orientation->x;
	vy = -orientation->y;
	vz = -orientation->z;

	tx = origin->x;
	ty = origin->y;
	tz = origin->z;

	matrix[ 0 ][ 0 ] = m0 = vx * vx + ( c_1 - vx * vx ) * ct;
	matrix[ 0 ][ 1 ] = m1 = vx * vy * _1_ct + vz * st;
	matrix[ 0 ][ 2 ] = m2 = vx * vz * _1_ct - vy * st;
	matrix[ 0 ][ 3 ] = tx - tx * m0 - ty * m1 - tz * m2;

	matrix[ 1 ][ 0 ] = m0 = vx * vy * _1_ct - vz * st;
	matrix[ 1 ][ 1 ] = m1 = vy * vy + ( c_1 - vy * vy ) * ct;
	matrix[ 1 ][ 2 ] = m2 = vy * vz *_1_ct + vx * st;
	matrix[ 1 ][ 3 ] = ty - tx * m0 - ty * m1 - tz * m2;

	matrix[ 2 ][ 0 ] = m0 = vx * vz * _1_ct + vy * st;
	matrix[ 2 ][ 1 ] = m1 = vy * vz *_1_ct - vx * st;
	matrix[ 2 ][ 2 ] = m2 = vz * vz + ( c_1 - vz * vz ) * ct;
	matrix[ 2 ][ 3 ] = tz - tx * m0 - ty * m1 - tz * m2;
	
	matrix[ 3 ][ 0 ] = c_0;
	matrix[ 3 ][ 1 ] = c_0;
	matrix[ 3 ][ 2 ] = c_0;
	matrix[ 3 ][ 3 ] = c_1;
}

void
matrix_transform(
	matrix_t&			m,
	const point_t*		p,
	point_t*			q )
{
	float				x = p->x;
	float				y = p->y;
	float				z = p->z;

	q->x = m[ 0 ][ 0 ] * x + m[ 0 ][ 1 ] * y + m[ 0 ][ 2 ] * z;
	q->y = m[ 1 ][ 0 ] * x + m[ 1 ][ 1 ] * y + m[ 1 ][ 2 ] * z;
	q->z = m[ 2 ][ 0 ] * x + m[ 2 ][ 1 ] * y + m[ 2 ][ 2 ] * z;
}

END_MACHINE_NAMESPACE
