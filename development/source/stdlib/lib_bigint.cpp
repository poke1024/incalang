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
//	lib_bigint.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "bigint_t.h"

#include "string_t.h"
#include "lib_string.h"
#include "lib_bigint.h"
#include "machine_class_t.h"

BEGIN_MACHINE_NAMESPACE

static void
init_bigint(
	u08*				block,
	machine_t&			m )
{
	u08*				ptr = m.m_lib_static.bigint_template;
	
	if( not ptr )
	{
		class_t*		rootclss = m.get_root_class();
		class_t*		bigint_class;

		bigint_class = rootclss->find_class( "BigInt", 6 );
		
		ptr = m.get_memory()->alloc( BIGINT_CLASS_SIZE );		
		bigint_class->init_instance( ptr );
		m.m_lib_static.bigint_template = ptr;
	}
	
	imemcpy(
		block, ptr, BIGINT_CLASS_SIZE );

	INSTANCE_SELF( bigint_wrap_t, block )->bigint = new bigint_t(
		m.m_lib_static.bigint_pool );
}

// ---------------------------------------------------------------------------

#define ARITHMETIC_FUNCTION( name, callname )								\
STDLIB_FUNCTION( name )														\
	machine_t&			m = *f.machine();									\
	bigint_t*			a = pop_bigint( f );								\
	bigint_t*			b = pop_bigint( f );								\
	u08					block[ BIGINT_CLASS_SIZE ];							\
																			\
	init_bigint( block, m );												\
																			\
	INSTANCE_SELF( bigint_wrap_t, block )->bigint->callname(				\
		a, b );																\
																			\
	f.push_block( block, BIGINT_CLASS_SIZE ); 								\
END_STDLIB_FUNCTION

#define COMPARE_FUNCTION( name, op )										\
STDLIB_FUNCTION( name )														\
	machine_t&			m = *f.machine();									\
	bigint_t*			a = pop_bigint( f );								\
	bigint_t*			b = pop_bigint( f );								\
																			\
	f.push_bool( bigint_t::compare( a, b ) op 0 );							\
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------


STDLIB_CLASS_FUNCTION( bigint_create, bigint_wrap_t )
	machine_t&			m = *f.machine();
	
	self->bigint = new bigint_t( m.m_lib_static.bigint_pool );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( bigint_create_copy, bigint_wrap_t )
	machine_t&			m = *f.machine();
	bigint_t*			bigint = pop_bigint( f );

	self->bigint = new bigint_t( m.m_lib_static.bigint_pool );
	self->bigint->set( bigint );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( bigint_destroy, bigint_wrap_t )
	f;
	delete self->bigint;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( bigint_assign, bigint_wrap_t )
	instance_t*			instance = f.pop_instance();
	bigint_t*			a = INSTANCE_SELF( bigint_wrap_t, instance )->bigint;
	bigint_t*			b = pop_bigint( f );
	
	a->set( b );
	f.push_ptr( instance );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

ARITHMETIC_FUNCTION( bigint_add, add )

ARITHMETIC_FUNCTION( bigint_sub, sub )

ARITHMETIC_FUNCTION( bigint_mul, mul )

ARITHMETIC_FUNCTION( bigint_div, div )

ARITHMETIC_FUNCTION( bigint_mod, mod )

STDLIB_FUNCTION( bigint_shl )
	machine_t&			m = *f.machine();
	bigint_t*			bigint = pop_bigint( f );
	long				bits = f.pop_int();
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->shift_left(
			bigint, bits );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_shr )
	machine_t&			m = *f.machine();
	bigint_t*			bigint = pop_bigint( f );
	long				bits = f.pop_int();
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->shift_right(
			bigint, bits );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_negate )
	machine_t&			m = *f.machine();
	bigint_t*			bigint = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	bigint_t*			r;
	r = INSTANCE_SELF( bigint_wrap_t,
		block )->bigint;
	r->set( bigint );
	r->negate();

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_inc )
	machine_t&			m = *f.machine();
	instance_t*			instance = f.pop_instance();
	bigint_t			one( m.m_lib_static.bigint_pool );
	bigint_t			tmp( m.m_lib_static.bigint_pool );
	bigint_t*			bigint;
	
	bigint = INSTANCE_SELF( bigint_wrap_t, instance )->bigint;

	one.set_int( 1 );
	tmp.add( bigint, &one );
	bigint->set( &tmp );

	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_int_to_bigint )
	machine_t&			m = *f.machine();
	long				value = f.pop_int();
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->set_int( value );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_long_to_bigint )
	machine_t&			m = *f.machine();
	s64					value = f.pop_long();
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->set_long( value );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

COMPARE_FUNCTION( bigint_cmp_eq, == )

COMPARE_FUNCTION( bigint_cmp_ne, != )

COMPARE_FUNCTION( bigint_cmp_lt, < )

COMPARE_FUNCTION( bigint_cmp_gt, > )

COMPARE_FUNCTION( bigint_cmp_le, <= )

COMPARE_FUNCTION( bigint_cmp_ge, >= )

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( bigint_assign_string )
	instance_t*			instance = f.pop_instance();
	string_t*			string = pop_string( f );
	bigint_t*			bigint;
	
	bigint = INSTANCE_SELF( bigint_wrap_t, instance )->bigint;
	bigint->set_string(
		string->text, string->length );

	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_val )
	bigint_t*			bigint = pop_bigint( f );

	f.push_long( bigint->integer() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_str_bigint )
	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];

	instance_t*			instance = f.pop_instance();	
	bigint_t*			bigint;
	istring				text;
	
	bigint = INSTANCE_SELF( bigint_wrap_t, instance )->bigint;
	bigint->to_string( text, 10 );

	init_string( block, m );
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		text.data(), text.length() );

	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_bin_bigint )
	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];

	instance_t*			instance = f.pop_instance();	
	bigint_t*			bigint;
	istring				text;
	
	bigint = INSTANCE_SELF( bigint_wrap_t, instance )->bigint;
	bigint->to_string( text, 2 );

	init_string( block, m );
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		text.data(), text.length() );

	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_hex_bigint )
	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];

	instance_t*			instance = f.pop_instance();	
	bigint_t*			bigint;
	istring				text;
	
	bigint = INSTANCE_SELF( bigint_wrap_t, instance )->bigint;
	bigint->to_string( text, 16 );

	init_string( block, m );
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		text.data(), text.length() );

	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_oct_bigint )
	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];

	instance_t*			instance = f.pop_instance();	
	bigint_t*			bigint;
	istring				text;
	
	bigint = INSTANCE_SELF( bigint_wrap_t, instance )->bigint;
	bigint->to_string( text, 8 );

	init_string( block, m );
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		text.data(), text.length() );

	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_abs )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t, block )->bigint->abs(
		x );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_sgn )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	f.push_int( x->sgn() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_sqrt )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->sqrt( x );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_sqr )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->mul( x, x );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_odd )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	f.push_bool( x->uint().btst( 0 ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_even )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	f.push_bool( x->uint().btst( 0 ) == 0 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_bclr )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	long				bit = f.pop_int();
	u08					block[ BIGINT_CLASS_SIZE ];
	bigint_t*			y;
	init_bigint( block, m );

	y = INSTANCE_SELF( bigint_wrap_t,
		block )->bigint;
	y->set( x );
	y->bclear( bit );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_bset )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	long				bit = f.pop_int();
	u08					block[ BIGINT_CLASS_SIZE ];
	bigint_t*			y;
	init_bigint( block, m );

	y = INSTANCE_SELF( bigint_wrap_t,
		block )->bigint;
	y->set( x, bit + 1 );
	y->bset( bit );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_bchg )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	long				bit = f.pop_int();
	u08					block[ BIGINT_CLASS_SIZE ];
	bigint_t*			y;
	init_bigint( block, m );

	y = INSTANCE_SELF( bigint_wrap_t,
		block )->bigint;
	y->set( x, bit + 1 );
	y->bchg( bit );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_btst )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	long				bit = f.pop_int();
	f.push_bool( x->btst( bit ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_bitlen )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	f.push_int( x->uint().bitlen() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_pow )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	bigint_t*			y = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->pow( x, y );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_powmod )
	machine_t&			m = *f.machine();
	bigint_t*			x = pop_bigint( f );
	bigint_t*			y = pop_bigint( f );
	bigint_t*			z = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->powmod( x, y, z );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_gcd )
	machine_t&			machine = *f.machine();
	bigint_t*			m = pop_bigint( f );
	bigint_t*			n = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, machine );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->gcd( m, n );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_egcd )
	machine_t&			machine = *f.machine();
	bigint_t*			m = pop_bigint( f );
	bigint_t*			n = pop_bigint( f );
	bigint_t*			x = pop_bigint( f );
	bigint_t*			y = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, machine );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->egcd( m, n, x, y );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_lcm )
	machine_t&			machine = *f.machine();
	bigint_t*			m = pop_bigint( f );
	bigint_t*			n = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, machine );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->lcm( m, n );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_modinv )
	machine_t&			machine = *f.machine();
	bigint_t*			x = pop_bigint( f );
	bigint_t*			n = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, machine );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->modinv( x, n );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_bigpow )
	machine_t&			m = *f.machine();
	u64					x = f.pop_long();
	u64					y = f.pop_long();
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->pow( x, y );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( bigint_random )
	machine_t&			m = *f.machine();
	bigint_t*			n = pop_bigint( f );
	u08					block[ BIGINT_CLASS_SIZE ];
	init_bigint( block, m );

	INSTANCE_SELF( bigint_wrap_t,
		block )->bigint->random( n );

	f.push_block( block, BIGINT_CLASS_SIZE );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_bigint(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	class_t*			clss;

	clss = rootclss->find_class( "BigInt", 6 );
	if( !clss )
		throw_icarus_error( "BigInt class not found" );

	register_native_function(
		m, clss, "create", bigint_create );

	register_native_function(
		m, clss, "create;.BigInt::&", bigint_create_copy );

	register_native_function(
		m, clss, "destroy", bigint_destroy );

	register_native_function(
		m, rootclss, "#+;.BigInt::&;.BigInt::&",
		bigint_add );

	register_native_function(
		m, rootclss, "#-;.BigInt::&;.BigInt::&",
		bigint_sub );

	register_native_function(
		m, rootclss, "#*;.BigInt::&;.BigInt::&",
		bigint_mul );

	register_native_function(
		m, rootclss, "#/;.BigInt::&;.BigInt::&",
		bigint_div );

	register_native_function(
		m, rootclss, "#%;.BigInt::&;.BigInt::&",
		bigint_mod );

	register_native_function(
		m, rootclss, "#<<;.BigInt::&;i",
		bigint_shl );

	register_native_function(
		m, rootclss, "#>>;.BigInt::&;i",
		bigint_shr );

	register_native_function(
		m, rootclss, "#-;.BigInt::&",
		bigint_negate );

	register_native_function(
		m, rootclss, "#++;BigInt::&",
		bigint_inc );

	register_native_function(
		m, rootclss, "#c;BigInt::;i",
		bigint_int_to_bigint );

	register_native_function(
		m, rootclss, "#c;BigInt::;l",
		bigint_long_to_bigint );


	register_native_function(
		m, rootclss, "#=;BigInt::&;.BigInt::&",
		bigint_assign );

	register_native_function(
		m, rootclss, "#==;.BigInt::&;.BigInt::&",
		bigint_cmp_eq );

	register_native_function(
		m, rootclss, "#!=;.BigInt::&;.BigInt::&",
		bigint_cmp_ne );

	register_native_function(
		m, rootclss, "#<;.BigInt::&;.BigInt::&",
		bigint_cmp_lt );

	register_native_function(
		m, rootclss, "#>;.BigInt::&;.BigInt::&",
		bigint_cmp_gt );

	register_native_function(
		m, rootclss, "#<=;.BigInt::&;.BigInt::&",
		bigint_cmp_le );

	register_native_function(
		m, rootclss, "#>=;.BigInt::&;.BigInt::&",
		bigint_cmp_ge );


	register_native_function(
		m, rootclss, "#=;BigInt::&;.String::&",
		bigint_assign_string );	

	register_native_function(
		m, rootclss, "val;.BigInt::&",
		bigint_val );

	register_native_function(
		m, rootclss, "str;.BigInt::&",
		string_method_str_bigint );

	register_native_function(
		m, rootclss, "bin;.BigInt::&",
		string_method_bin_bigint );

	register_native_function(
		m, rootclss, "hex;.BigInt::&",
		string_method_hex_bigint );

	register_native_function(
		m, rootclss, "oct;.BigInt::&",
		string_method_oct_bigint );


	register_native_function(
		m, rootclss, "abs;.BigInt::&",
		bigint_abs );

	register_native_function(
		m, rootclss, "sgn;.BigInt::&",
		bigint_sgn );

	register_native_function(
		m, rootclss, "sqrt;.BigInt::&",
		bigint_sqrt );

	register_native_function(
		m, rootclss, "sqr;.BigInt::&",
		bigint_sqr );

	register_native_function(
		m, rootclss, "odd;.BigInt::&",
		bigint_odd );

	register_native_function(
		m, rootclss, "even;.BigInt::&",
		bigint_even );

	register_native_function(
		m, rootclss, "bclr;.BigInt::&;i",
		bigint_bclr );

	register_native_function(
		m, rootclss, "bset;.BigInt::&;i",
		bigint_bset );

	register_native_function(
		m, rootclss, "bchg;.BigInt::&;i",
		bigint_bchg );

	register_native_function(
		m, rootclss, "btst;.BigInt::&;i",
		bigint_btst );

	register_native_function(
		m, rootclss, "bitlen;.BigInt::&",
		bigint_bitlen );
		
	register_native_function(
		m, rootclss, "pow;.BigInt::&;.BigInt::&",
		bigint_pow );

	register_native_function(
		m, rootclss, "powmod;.BigInt::&;.BigInt::&;.BigInt::&",
		bigint_powmod );

	register_native_function(
		m, rootclss, "gcd;.BigInt::&;.BigInt::&",
		bigint_gcd );

	register_native_function(
		m, rootclss, "egcd;.BigInt::&;.BigInt::&;BigInt::&;BigInt::&",
		bigint_egcd );

	register_native_function(
		m, rootclss, "lcm;.BigInt::&;.BigInt::&",
		bigint_lcm );

	register_native_function(
		m, rootclss, "modinv;.BigInt::&;.BigInt::&",
		bigint_modinv );

	register_native_function(
		m, rootclss, "bigpow;L;L",
		bigint_bigpow );

	register_native_function(
		m, rootclss, "random;.BigInt::&",
		bigint_random );
}

END_MACHINE_NAMESPACE
