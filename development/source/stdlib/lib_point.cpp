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
//	lib_point.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"

#include "string_t.h"
#include "lib_string.h"
#include "lib_point.h"
#include "machine_class_t.h"
#include "matrix_t.h"

#include <math.h>

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

u08*
create_point_template(
	machine_t&			m )
{
	u08*				ptr = m.m_lib_static.point_template;
	
	if( not ptr )
	{
		class_t*		rootclss = m.get_root_class();
		class_t*		point_class;

		point_class = rootclss->find_class( "Point", 5 );
		
		ptr = m.get_memory()->alloc( POINT_CLASS_SIZE );		
		point_class->init_instance( ptr );
		m.m_lib_static.point_template = ptr;
	}
	
	return ptr;
}

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( point_make )
	machine_t&			m = *f.machine();
	float				x = f.pop_float();
	float				y = f.pop_float();
	float				z = f.pop_float();
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	p->x = x;
	p->y = y;
	p->z = z;
	
	f.push_block( block, POINT_CLASS_SIZE ); 
END_STDLIB_FUNCTION

/*STDLIB_FUNCTION( point_assign )
	instance_t*			instance = f.pop_instance();
	point_t*			a = INSTANCE_SELF( point_t, instance );
	point_t*			b = pop_point( f );
	
	a->x = b->x;
	a->y = b->y;
	a->z = b->z;
	f.push_ptr( instance );
END_STDLIB_FUNCTION*/

STDLIB_FUNCTION( point_add )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	point_t*			b = pop_point( f );
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	p->x = a->x + b->x;
	p->y = a->y + b->y;
	p->z = a->z + b->z;
	
	f.push_block( block, POINT_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_sub )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	point_t*			b = pop_point( f );
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	p->x = a->x - b->x;
	p->y = a->y - b->y;
	p->z = a->z - b->z;
	
	f.push_block( block, POINT_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_scale_right )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	float				s = f.pop_float();
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	p->x = a->x * s;
	p->y = a->y * s;
	p->z = a->z * s;
	
	f.push_block( block, POINT_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_scale_left )
	machine_t&			m = *f.machine();
	float				s = f.pop_float();
	point_t*			a = pop_point( f );
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	p->x = a->x * s;
	p->y = a->y * s;
	p->z = a->z * s;
	
	f.push_block( block, POINT_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_invscale_right )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	float				s = 1.0 / f.pop_float();
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	p->x = a->x * s;
	p->y = a->y * s;
	p->z = a->z * s;
	
	f.push_block( block, POINT_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_dot )
	point_t*			a = pop_point( f );
	point_t*			b = pop_point( f );
	
	f.push_float(
		a->x * b->x +
		a->y * b->y +
		a->z * b->z );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_cross )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	point_t*			b = pop_point( f );
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	p->x = a->y * b->z - a->z * b->y;
	p->y = a->z * b->x - a->x * b->z;
	p->z = a->x * b->y - a->y * b->x;
	
	f.push_block( block, POINT_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_negate )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	p->x = -a->x;
	p->y = -a->y;
	p->z = -a->z;
	
	f.push_block( block, POINT_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_length )
	point_t*			a = pop_point( f );
	float				ax = a->x;
	float				ay = a->y;
	float				az = a->z;
	f.push_float( isqrt( ax * ax + ay * ay + az * az ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_distance )
	point_t*			a = pop_point( f );
	point_t*			b = pop_point( f );
	float				ax = a->x - b->x;
	float				ay = a->y - b->y;
	float				az = a->z - b->z;
	f.push_float( isqrt( ax * ax + ay * ay + az * az ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_normalize )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	float				ax = a->x;
	float				ay = a->y;
	float				az = a->z;
	float				s;
	s = iinvsqrt( ax * ax + ay * ay + az * az );
	p->x = ax * s;
	p->y = ay * s;
	p->z = az * s;
	
	f.push_block( block, POINT_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_rotate )
	machine_t&			m = *f.machine();
	point_t*			p = pop_point( f );
	float				angle = f.pop_float();
	point_t*			from = pop_point( f );
	point_t*			to = pop_point( f );
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			r = INSTANCE_SELF( point_t, block );
	
	point_t				axis;
	matrix_t			matrix;
	
	axis.x = to->x - from->x;
	axis.y = to->y - from->y;
	axis.z = to->z - from->z;
	
	matrix_set_rotate_about_axis(
		matrix, from, &axis, angle );
		
	matrix_transform(
		matrix, p, r );
	
	f.push_block( block, POINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_mix )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	point_t*			b = pop_point( f );
	float				t = f.pop_float();
	float				s = 1 - t;

	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			r = INSTANCE_SELF( point_t, block );
		
	r->x = s * a->x + t * b->x;
	r->y = s * a->y + t * b->y;
	r->z = s * a->z + t * b->z;
	
	f.push_block( block, POINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_xcomp )
	point_t*			a = pop_point( f );
	f.push_float( a->x );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_ycomp )
	point_t*			a = pop_point( f );
	f.push_float( a->y );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_zcomp )
	point_t*			a = pop_point( f );
	f.push_float( a->z );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( point_comp )
	point_t*			a = pop_point( f );
	f.push_ptr( &a->x );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_point(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	class_t*			clss;

	clss = rootclss->find_class( "Point", 5 );
	if( !clss )
		throw_icarus_error( "Point class not found" );

	register_native_function(
		m, rootclss, "point;f;f;f",
		point_make );

	register_native_function(
		m, rootclss, "vector;f;f;f",
		point_make );

	register_native_function(
		m, rootclss, "color;f;f;f",
		point_make );

	register_native_function(
		m, rootclss, "#+;.Point::&;.Point::&",
		point_add );

	register_native_function(
		m, rootclss, "#-;.Point::&;.Point::&",
		point_sub );

	register_native_function(
		m, rootclss, "#*;.Point::&;f",
		point_scale_right );

	register_native_function(
		m, rootclss, "#*;f;.Point::&",
		point_scale_left );

	register_native_function(
		m, rootclss, "#/;.Point::&;f",
		point_invscale_right );

	register_native_function(
		m, rootclss, "#*;.Point::&;.Point::&",
		point_dot );

	register_native_function(
		m, rootclss, "#^;.Point::&;.Point::&",
		point_cross );

	register_native_function(
		m, rootclss, "#-;.Point::&",
		point_negate );

	register_native_function(
		m, rootclss, "length;.Point::&",
		point_length );

	register_native_function(
		m, rootclss, "distance;.Point::&;.Point::&",
		point_distance );

	register_native_function(
		m, rootclss, "normalize;.Point::&",
		point_normalize );

	register_native_function(
		m, rootclss, "rotate;.Point::&;f;.Point::&;.Point::&",
		point_rotate );

	register_native_function(
		m, rootclss, "mix;.Point::&;.Point::&;f",
		point_mix );

	register_native_function(
		m, rootclss, "xcomp;.Point::&",
		point_xcomp );

	register_native_function(
		m, rootclss, "ycomp;.Point::&",
		point_ycomp );

	register_native_function(
		m, rootclss, "zcomp;.Point::&",
		point_zcomp );

	register_native_function(
		m, rootclss, "comp;.Point::&",
		point_comp );
}

END_MACHINE_NAMESPACE
