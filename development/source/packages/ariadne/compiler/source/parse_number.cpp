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
//	parse_number.cp			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "parse_number.h"
#include "expr_t.h"
#include "type_t.h"
#include <climits>
#include <cmath>

BEGIN_COMPILER_NAMESPACE

static expr_t*
parse_num_is(
	intval_t				val,
	const char*				s );

static expr_t*
parse_num_fs(
	fltval_t				val,
	const char*				s );

static fltval_t
fastexpo(
	int						expo )
{
	static fltval_t			s_expos[ 51 ] = { 1e0,
		1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10,
		1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20,
		1e21, 1e22, 1e23, 1e24, 1e25, 1e26, 1e27, 1e28, 1e29, 1e30,
		1e31, 1e32, 1e33, 1e34, 1e35, 1e36, 1e37, 1e38, 1e39, 1e40,
		1e41, 1e42, 1e43, 1e44, 1e45, 1e46, 1e47, 1e48, 1e49, 1e50 };
	
	if( expo <= 50 )
		return s_expos[ expo ];
	
	int a = expo / 2;
	int b = expo - a;
	
	return fastexpo( a ) * fastexpo( b );
}

expr_t*
parse_hex_const(
	const char*				s )
{
	intval_t				val = 0;
	char					c;
	int						cnt = 0;

	s += 2;
	
	while( true )
	{
		int					d;
	
		c = *s++;
				
		if( c >= '0' && c <= '9' )
			d = c - '0';
		else if( c >= 'a' && c <= 'f' )
			d = c - 'a' + 10;
		else if( c >= 'A' && c <= 'F' )
			d = c - 'A' + 10;
		else
			break;
			
		if( ++cnt > INTVAL_BITS / 4L )
		{
			compile_error( 0, ERR_NUMBER_TOO_BIG );
			break;
		}
		
		val = ( val << 4 ) | d;
	}
	
	return parse_num_is( val, s - 1 );
}

expr_t*
parse_oct_const(
	const char*				s )
{
	intval_t				val = 0;
	char					c;

	s += 1;
	
	while( true )
	{
		c = *s++;
		if( c >= '0' && c <= '7' )
		{
			if( val >> ( INTVAL_BITS - 3L ) )
			{
				compile_error( 0, ERR_NUMBER_TOO_BIG );
				break;
			}
		
			const int		d = c - '0';
		
			val = ( val << 3 ) | d;
		}
		else if( c >= '8' && c <= '9' )
		{
			compile_error( 0, ERR_INVALID_NUMBER );
			break;
		}
		else
			break;
	}

	return parse_num_is( val, s - 1 );
}

expr_t*
parse_bin_const(
	const char*				s )
{
	intval_t				val = 0;
	char					c;
	int						cnt = 0;

	s += 2;
	
	while( true )
	{
		c = *s++;
				
		if( c >= '0' && c <= '1' )
		{
			const int		d = c - '0';
			
			val = ( val << 1 ) | d;
		}
		else
			break;
			
		if( ++cnt > INTVAL_BITS )
		{
			compile_error( 0, ERR_NUMBER_TOO_BIG );
			break;
		}
	}
	
	return parse_num_is( val, s - 1 );
}

expr_t*
parse_int_const(
	const char*				s )
{
	intval_t				val = 0;
	char					c;

	while( true )
	{
		c = *s++;
		if( c >= '0' && c <= '9' )
		{
			const int		d = c - '0';
		
			if( val > INTVAL_MAX / 10L )
			{
				compile_error( 0, ERR_NUMBER_TOO_BIG );
				break;
			}
			
			val *= 10;
			
			if( val > INTVAL_MAX - d )
			{
				compile_error( 0, ERR_NUMBER_TOO_BIG );
				break;
			}

			val += d;
		}
		else
			break;
	}

	return parse_num_is( val, s - 1 );
}

expr_t*
parse_chr_const(
	const char*				s )
{
	intval_t				val = 0;
	char					c;
	int						cnt = 0;

	s += 1;
	
	while( true )
	{
		c = *s++;
		if( c == '\'' )
			break;
			
		if( ++cnt > 8 )
		{
			compile_error( 0, ERR_NUMBER_TOO_BIG );
			break;
		}
			
		if( c == '\\' )
			c = *s++;
			
		val = ( val << 8 ) | (int)( (unsigned char)c );
	}

	e_type_code				type_code;

	if( val <= 0xff )
		type_code = TYPE_CHAR;
	else if( val <= 0xffff )
		type_code = TYPE_SHORT;
#if __VISC__
	else if( val <= 0xffffffff )
#else
	else if( val <= 0xffffffffLL )
#endif
		type_code = TYPE_LONG;
	else
		type_code = TYPE_WIDE;

	int_type_t*				t;

	t = make_int_type( type_code, type_code == TYPE_CHAR );

	return make_iconst_expr( val, t );
}

expr_t*
parse_flt_const(
	const char*				s )
{
	fltval_t				val = 0;
	char					c;
	//int					cnt = 0;

	while( true )
	{
		c = *s++;
		if( c >= '0' && c <= '9' )
		{
			const int		d = c - '0';
			val = val * 10. + d;
		}
		else
			break;
	}

	if( c == '.' )
	{
		fltval_t			w = 0.1;
	
		while( true )
		{
			c = *s++;
			if( c >= '0' && c <= '9' )
			{
				const int		d = c - '0';
				val = val + d * w;
			}
			else
				break;
		
			w *= 0.1;
		}
	}
	
	if( c == 'e' || c == 'E' )
	{
		bool is_expo_negative = false;
	
		c = *s++;
		if( c == '+' )
			c = *s++;
		else if( c == '-' )
		{
			c = *s++;
			is_expo_negative = true;
		}

		int expo = 0;
		
		while( c >= '0' && c <= '9' )
		{
			expo = 10 * expo + c - '0';
			
			if( expo > 255 )
			{
				compile_error( 0, ERR_INVALID_NUMBER );
				break;
			}
			
			c = *s++;
		}

		if( is_expo_negative )
			val /= fastexpo( expo );
		else
			val *= fastexpo( expo );
	}

	return parse_num_fs( val, s - 1 );
}

expr_t*
parse_num_is(
	intval_t				val,
	const char*				s )
{
	bool					is_signed = true;
	char					c = *s;
	
	e_type_code				type_code;

	if( val > 2147483647L )
		type_code = TYPE_WIDE;
	else if( val > 32767 )
		type_code = TYPE_LONG;
	else
		type_code = TYPE_INT;

	while( c != '\0' )
	{
		if( c == 'u' || c == 'U' )
			is_signed = false;
		else if( c == 'l' || c == 'L' )
		{
			if( type_code == TYPE_INT )
				type_code = TYPE_LONG;
		}
		else
		{
			compile_error( 0, ERR_INVALID_NUMBER );
			break;
		}
		c = *s++;
	}
		
	int_type_t*				t;
	
	t = make_int_type( type_code, is_signed );
	
	return make_iconst_expr( val, t );
}

expr_t*
parse_num_fs(
	fltval_t				val,
	const char*				s )
{
	bool					is_long = true;
	char					c = *s;
	
	if( c == 'l' || c == 'L' )
		is_long = true;
	else if( c == 'f' || c == 'F' )
		is_long = false;
		
	type_t*					t;
	
	t = make_type( is_long ? TYPE_DOUBLE : TYPE_FLOAT );
	
	return make_fconst_expr( val, t );
}

END_COMPILER_NAMESPACE
