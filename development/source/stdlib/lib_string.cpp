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
//	lib_string.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "string_t.h"
#include "lib_string.h"
#include "machine_class_t.h"
#include "safe_memory_t.h"
#include "machine_error.h"

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

#define STRING_CLASS_FUNCTION( NAME )										\
	STDLIB_CLASS_FUNCTION( NAME, string_t )									\
	machine_t&			m = *f.machine();									\
	check_string_object( f, self );

#define STRING_FUNCTION( NAME )												\
	STDLIB_FUNCTION( NAME )													\
	machine_t&			m = *f.machine();									\
	string_t*			self = pop_string( f );

// ---------------------------------------------------------------------------

void
init_string(
	u08*				block,
	machine_t&			m )
{
	u08*				ptr = m.m_lib_static.string_template;
	
	if( not ptr )
	{
		class_t*		string_class = m.get_string_class();
		
		ptr = m.get_memory()->alloc( STRING_CLASS_SIZE );
		string_class->init_instance( ptr );
		m.m_lib_static.string_template = ptr;
	}
	
	imemcpy(
		block, ptr, STRING_CLASS_SIZE );
}

void
check_string_object(
	fiber_t&			f,
	string_t*			string )
{
#if SAFE_MACHINE
	long				length = string->length;

	if( length )
		f.check_access( string->text, length + 1 );
#endif
}

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( string_method_add )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	string_t*			b = pop_string( f );
	u08					block[ STRING_CLASS_SIZE ];
	
	init_string( block, m );	

	string_add( m,
		INSTANCE_SELF( string_t, block ),
		a, b );
	
	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

/*STDLIB_FUNCTION( string_method_assign_add )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	string_t*			b = pop_string( f );
	std::string			text;
	
	text.assign( a->text, a->length );
	text.append( b->text, b->length );

	string_dispose( m, a );

	string_create( m, a,
		text.data(), text.length() );
	
	f.push_ptr( SELF_INSTANCE( a ) );
END_STDLIB_FUNCTION*/

STDLIB_FUNCTION( string_method_assign )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	string_t*			b = pop_string( f );
	
	string_assign( m, a, b );
	
	f.push_ptr( SELF_INSTANCE( a ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_cmp_eq )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	string_t*			b = pop_string( f );

	f.push_bool( string_equal( m, a, b ) );
END_STDLIB_FUNCTION


STDLIB_FUNCTION( string_method_cmp_ne )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	string_t*			b = pop_string( f );

	f.push_bool( not string_equal( m, a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_cmp_lt )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	string_t*			b = pop_string( f );

	f.push_bool( string_compare( m, a, b ) < 0 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_cmp_gt )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	string_t*			b = pop_string( f );

	f.push_bool( string_compare( m, a, b ) > 0 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_cmp_le )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	string_t*			b = pop_string( f );

	f.push_bool( string_compare( m, a, b ) <= 0 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_cmp_ge )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	string_t*			b = pop_string( f );

	f.push_bool( string_compare( m, a, b ) >= 0 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_access )
	machine_t&			m = *f.machine();
	string_t*			a = pop_string( f );
	long				index = f.pop_int();

	if( index < 0 || index >= a->length )
		f.fatal_error( ERR_ILLEGAL_STRING_INDEX );

	f.push_ptr( &a->text[ index ] );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( string_method_create, string_t )
	f;
	self->text = 0;
	self->length = 0;
END_STDLIB_FUNCTION

STRING_CLASS_FUNCTION( string_method_destroy )
	string_dispose( m, self );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( string_method_create_copy, string_t )
	machine_t&			m = *f.machine();
	instance_t*			param = f.pop_instance();
	string_t*			string = INSTANCE_SELF( string_t, param );

	string_create( m, self, string->text, string->length );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STRING_FUNCTION( string_method_length )
	f.push_int( self->length );
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_upper )
	u08					block[ STRING_CLASS_SIZE ];
	
	init_string( block, m );
		
	string_upper( m,
		INSTANCE_SELF( string_t, block ),
		self );
	
	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_lower )
	u08					block[ STRING_CLASS_SIZE ];
	
	init_string( block, m );
		
	string_lower( m,
		INSTANCE_SELF( string_t, block ),
		self );
	
	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_left )
	long				count = f.pop_int();
	u08					block[ STRING_CLASS_SIZE ];
	
	init_string( block, m );
		
	string_left( m,
		INSTANCE_SELF( string_t, block ),
		self, count );
	
	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_right )
	long				count = f.pop_int();
	u08					block[ STRING_CLASS_SIZE ];
	
	init_string( block, m );
		
	string_right( m,
		INSTANCE_SELF( string_t, block ),
		self, count );
	
	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_mid_one )
	long				index = f.pop_int();
	u08					block[ STRING_CLASS_SIZE ];
	
	init_string( block, m );
		
	string_mid( m,
		INSTANCE_SELF( string_t, block ),
		self, index );
	
	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_mid_two )
	long				index = f.pop_int();
	long				count = f.pop_int();
	u08					block[ STRING_CLASS_SIZE ];
	
	init_string( block, m );
		
	string_mid( m,
		INSTANCE_SELF( string_t, block ),
		self, index, count );
	
	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_instr )
	string_t*			what = pop_string( f );
	long				index = f.pop_int();
		
	f.push_int( string_instr(
		m, self, what, index ) );
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_grep )
	string_t*			what = pop_string( f );
	long				index = f.pop_int();
	int					flags = f.pop_byte();
		
	f.push_int( string_grep(
		m, self, what, index, flags ) );
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_asc )
	f.push_int( string_asc( m, self ) );
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_val )
	f.push_double( string_val( m, self ) );
END_STDLIB_FUNCTION

STRING_FUNCTION( string_method_valn )
	f.push_int( string_valn( m, self ) );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( string_method_string )
	machine_t&			m = *f.machine();
	long				count = f.pop_int();
	instance_t*			a = f.pop_instance();
	u08					block[ STRING_CLASS_SIZE ];
	
	init_string( block, m );
		
	string_string( m,
		INSTANCE_SELF( string_t, block ),
		INSTANCE_SELF( string_t, a ),
		count );
	
	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_chr )
	machine_t&			m = *f.machine();
	char				c = f.pop_int();
	u08					block[ STRING_CLASS_SIZE ];

	init_string( block, m );
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		&c, 1 );

	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_str_double )
	machine_t&			m = *f.machine();
	double				val = f.pop_double();
	u08					block[ STRING_CLASS_SIZE ];
	char				buf[ 64 + 1 ];
	
	isnprintf( buf, 64, "%lg", val );

	init_string( block, m );
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		buf, istrlen( buf ) );

	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_bin )
	machine_t&			m = *f.machine();
	u64					val = f.pop_long();
	u08					block[ STRING_CLASS_SIZE ];

	init_string( block, m );
	
	char				buf[ 70 ];
	char*				p = buf;
	bool				nonnull = false;
	
	for( int i = 63; i >= 0; i-- )
	{
		if( ( val >> i ) & 1 )
		{
			*p++ = '1';
			nonnull = true;
		}
		else if( nonnull )
			*p++ = '0';
	}

	if( not nonnull )
		*p++ = '0';
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		buf, p - buf );

	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_oct )
	machine_t&			m = *f.machine();
	u64					val = f.pop_long();
	u08					block[ STRING_CLASS_SIZE ];

	init_string( block, m );
	
	char				buf[ 40 ];
	char*				p = buf;
	bool				nonnull = false;
	int					j = 21 * 3;
	
	for( int i = 0; i < 22; i++, j -= 3 )
	{
		int				digit;
		
		digit = ( val >> j ) & 7;

		if( digit )
		{
			*p++ = '0' + digit;
			nonnull = true;
		}
		else if( nonnull )
			*p++ = '0';
	}

	if( not nonnull )
		*p++ = '0';
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		buf, p - buf );

	f.push_block( block, STRING_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( string_method_hex )
	machine_t&			m = *f.machine();
	u64					val = f.pop_long();
	u08					block[ STRING_CLASS_SIZE ];

	init_string( block, m );
	
	char				buf[ 40 ];
	char*				p = buf;
	bool				nonnull = false;
	int					j = 60;
	
	for( int i = 0; i < 16; i++, j -= 4 )
	{
		int				digit;
		
		digit = ( val >> j ) & 15;

		if( digit )
		{
			if( digit < 10 )
				*p++ = '0' + digit;
			else
				*p++ = 'A' + digit - 10;
			
			nonnull = true;
		}
		else if( nonnull )
			*p++ = '0';
	}

	if( not nonnull )
		*p++ = '0';
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		buf, p - buf );

	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_string(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "#+;.String::&;.String::&",
		string_method_add );

	/*register_native_function(
		m, rootclss, "#+=;String::&;.String::&",
		string_method_assign_add );*/

	register_native_function(
		m, rootclss, "#=;String::&;.String::&",
		string_method_assign );

	register_native_function(
		m, rootclss, "#==;.String::&;.String::&",
		string_method_cmp_eq );

	register_native_function(
		m, rootclss, "#!=;.String::&;.String::&",
		string_method_cmp_ne );

	register_native_function(
		m, rootclss, "#<;.String::&;.String::&",
		string_method_cmp_lt );

	register_native_function(
		m, rootclss, "#>;.String::&;.String::&",
		string_method_cmp_gt );

	register_native_function(
		m, rootclss, "#<=;.String::&;.String::&",
		string_method_cmp_le );

	register_native_function(
		m, rootclss, "#>=;.String::&;.String::&",
		string_method_cmp_ge );

	register_native_function(
		m, rootclss, "#[];.String::&;i",
		string_method_access );

	// ---

	class_t*			clss;

	clss = rootclss->find_class( "String", 6 );
	if( !clss )
		throw_icarus_error( "String class not found" );

	register_native_function(
		m, clss, "create;.String::&", string_method_create_copy );

	register_native_function(
		m, clss, "create", string_method_create );

	register_native_function(
		m, clss, "destroy", string_method_destroy );
		
	// ---

	register_native_function(
		m, rootclss, "len;.String::&", string_method_length );

	register_native_function(
		m, rootclss, "upper;.String::&", string_method_upper );

	register_native_function(
		m, rootclss, "lower;.String::&", string_method_lower );

	register_native_function(
		m, rootclss, "left;.String::&;i", string_method_left );

	register_native_function(
		m, rootclss, "right;.String::&;i", string_method_right );

	register_native_function(
		m, rootclss, "mid;.String::&;i", string_method_mid_one );

	register_native_function(
		m, rootclss, "mid;.String::&;i;i", string_method_mid_two );

	register_native_function(
		m, rootclss, "instr;.String::&;.String::&;i", string_method_instr );

	register_native_function(
		m, rootclss, "grep;.String::&;.String::&;i;B", string_method_grep );

	register_native_function(
		m, rootclss, "asc;.String::&", string_method_asc );

	register_native_function(
		m, rootclss, "val;.String::&", string_method_val );

	register_native_function(
		m, rootclss, "valn;.String::&", string_method_valn );

	// ---

	register_native_function(
		m, rootclss, "string;i;.String::&", string_method_string );

	register_native_function(
		m, rootclss, "chr;i", string_method_chr );

	register_native_function(
		m, rootclss, "str;d", string_method_str_double );

	register_native_function(
		m, rootclss, "bin;l", string_method_bin );

	register_native_function(
		m, rootclss, "oct;l", string_method_oct );	

	register_native_function(
		m, rootclss, "hex;l", string_method_hex );
}

END_MACHINE_NAMESPACE
