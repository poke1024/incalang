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
//	lib_complex.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"

#include "string_t.h"
#include "lib_string.h"
#include "lib_complex.h"
#include "machine_class_t.h"

#include <math.h>

BEGIN_MACHINE_NAMESPACE

static void
init_complex(
	u08*				block,
	machine_t&			m )
{
	u08*				ptr = m.m_lib_static.complex_template;
	
	if( not ptr )
	{
		class_t*		rootclss = m.get_root_class();
		class_t*		complex_class;

		complex_class = rootclss->find_class( "Complex", 7 );
		
		ptr = m.get_memory()->alloc( COMPLEX_CLASS_SIZE );		
		complex_class->init_instance( ptr );
		m.m_lib_static.complex_template = ptr;
	}
	
	imemcpy(
		block, ptr, COMPLEX_CLASS_SIZE );
}

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( complex_make )
	machine_t&			m = *f.machine();
	double				re = f.pop_double();
	double				im = f.pop_double();
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );
	c->re = re;
	c->im = im;
	f.push_block( block, COMPLEX_CLASS_SIZE ); 
END_STDLIB_FUNCTION

/*STDLIB_FUNCTION( complex_assign )
	instance_t*			instance = f.pop_instance();
	complex_t*			a = INSTANCE_SELF( complex_t, instance );
	complex_t*			b = pop_complex( f );
	
	a->re = b->re;
	a->im = b->im;
	f.push_ptr( instance );
END_STDLIB_FUNCTION*/

STDLIB_FUNCTION( complex_add )
	machine_t&			m = *f.machine();
	complex_t*			a = pop_complex( f );
	complex_t*			b = pop_complex( f );
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );
	
	c->re = a->re + b->re;
	c->im = a->im + b->im;
	
	f.push_block( block, COMPLEX_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_sub )
	machine_t&			m = *f.machine();
	complex_t*			a = pop_complex( f );
	complex_t*			b = pop_complex( f );
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );
	
	c->re = a->re - b->re;
	c->im = a->im - b->im;
	
	f.push_block( block, COMPLEX_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_mul )
	machine_t&			m = *f.machine();
	complex_t*			a = pop_complex( f );
	complex_t*			b = pop_complex( f );
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );
	
	double				are = a->re;
	double				aim = a->im;
	double				bre = b->re;
	double				bim = b->im;
	double				t1, t2;
	t1 = are * bre;
	t2 = aim * bim;
	c->re = t1 - t2;
	c->im = ( are + aim ) * ( bre + bim ) - ( t1 + t2 );
	
	f.push_block( block, COMPLEX_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_div )
	machine_t&			m = *f.machine();
	complex_t*			a = pop_complex( f );
	complex_t*			b = pop_complex( f );
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );
	
	double				are = a->re;
	double				aim = a->im;
	double				bre = b->re;
	double				bim = b->im;
	double				r, den;

	if( iabs( bre ) >= iabs( bim ) )
	{
		r = bim / bre;
		den = bre + r * bim;
		c->re = ( are + r * aim ) / den;
		c->im = ( aim - r * are ) / den;
	}
	else
	{
		r = bre / bim;
		den = bim + r * bre;
		c->re = ( are * r + aim ) / den;
		c->im = ( aim * r - are ) / den;
	}
	
	f.push_block( block, COMPLEX_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_negate )
	machine_t&			m = *f.machine();
	complex_t*			a = pop_complex( f );
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );
	
	c->re = -a->re;
	c->im = -a->im;
	
	f.push_block( block, COMPLEX_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_double_to_complex )
	double				x = f.pop_double();
	machine_t&			m = *f.machine();
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );
	c->re = x;
	c->im = 0;
	f.push_block( block, COMPLEX_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_abs )
	complex_t*			z = pop_complex( f );
	double				x, y, ans, temp;
	
	x = iabs( z->re );
	y = iabs( z->im );
	if( x == 0.0 )
		ans = y;
	else if( y == 0.0 )
		ans = x;
	else if( x > y )
	{
		temp = y / x;
		ans = x * isqrt( 1.0 + temp * temp );
	}
	else
	{
		temp = x / y;
		ans = y * isqrt( 1.0 + temp * temp );
	}
	
	f.push_double( ans );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_sqrt )
	machine_t&			m = *f.machine();
	complex_t*			z = pop_complex( f );
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );

	double				zre = z->re;
	double				zim = z->im;
	double				w;

	if( ( zre == 0.0 ) && ( zim == 0.0 ) )
	{
		c->re = 0.0;
		c->im = 0.0;
	}
	else
	{
		w = isqrt( ( isqrt( zre * zre + zim * zim ) +
			iabs( zre ) ) * 0.5 );
		if( zre >= 0.0 )
		{
			c->re = w;
			c->im = zim / ( w + w );
		}
		else
		{
			double		cim;
		
			c->im = cim = ( zim >= 0 ) ? w : -w;
			c->re = zim / ( cim + cim );
		}
	}
	
	f.push_block( block, COMPLEX_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_conjugate )
	machine_t&			m = *f.machine();
	complex_t*			z = pop_complex( f );
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );

	c->re = z->re;
	c->im = -z->im;
	
	f.push_block( block, COMPLEX_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_inverse )
	machine_t&			m = *f.machine();
	complex_t*			z = pop_complex( f );
	u08					block[ COMPLEX_CLASS_SIZE ];
	init_complex( block, m );
	complex_t*			c = INSTANCE_SELF( complex_t, block );

	double				zre = z->re;
	double				zim = z->im;
	double				s = 1.0 / ( zre * zre + zim * zim );
	c->re = zre * s;
	c->im = -zim * s;
  
	f.push_block( block, COMPLEX_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_real )
	complex_t*			z = pop_complex( f );
	f.push_double( z->re );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( complex_imag )
	complex_t*			z = pop_complex( f );
	f.push_double( z->im );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_complex(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	class_t*			clss;

	clss = rootclss->find_class( "Complex", 7 );
	if( !clss )
		throw_icarus_error( "Complex class not found" );

	register_native_function(
		m, rootclss, "complex;d;d",
		complex_make );

	/*register_native_function(
		m, rootclss, "#=;Complex::&;.Complex::&",
		complex_assign );*/

	register_native_function(
		m, rootclss, "#+;.Complex::&;.Complex::&",
		complex_add );

	register_native_function(
		m, rootclss, "#-;.Complex::&;.Complex::&",
		complex_sub );

	register_native_function(
		m, rootclss, "#*;.Complex::&;.Complex::&",
		complex_mul );

	register_native_function(
		m, rootclss, "#/;.Complex::&;.Complex::&",
		complex_div );

	register_native_function(
		m, rootclss, "#-;.Complex::&",
		complex_negate );

	register_native_function(
		m, rootclss, "#c;Complex::;d",
		complex_double_to_complex );

	register_native_function(
		m, rootclss, "abs;.Complex::&",
		complex_abs );

	register_native_function(
		m, rootclss, "sqrt;.Complex::&",
		complex_sqrt );

	register_native_function(
		m, rootclss, "conjugate;.Complex::&",
		complex_conjugate );

	register_native_function(
		m, rootclss, "inverse;.Complex::&",
		complex_inverse );

	register_native_function(
		m, rootclss, "real;.Complex::&",
		complex_real );

	register_native_function(
		m, rootclss, "imag;.Complex::&",
		complex_imag );
}

END_MACHINE_NAMESPACE
