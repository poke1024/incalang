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
//	typedist.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "type_t.h"
#include "class_t.h"
#include "expr_t.h"

BEGIN_COMPILER_NAMESPACE

enum {
	DISTANCE_ENUM_TO_INT		= 2,
	DISTANCE_BOOL_TO_ARITH		= 2,
	
	DISTANCE_ARITH_TO_BOOL		= 8192,
	DISTANCE_ARITH_EXPAND		= 2,
	DISTANCE_ARITH_REDUCE		= 8192,

	DISTANCE_PTR_TO_BOOL		= 8192
};

extern u64						s_fcid;

static long
bind_template(
	type_t*				temptype,
	type_t*				bindtype,
	bool				tolerant = true )
{
	template_type_t*	t =
		(template_type_t*)temptype;
	long				typedist;
	
	if( t->fcid == s_fcid )
	{
		if( tolerant )
		{
			typedist = type_distance(
				bindtype, t->bind );
		}
		else
		{
			if( is_same_type( bindtype, t->bind ) )
				typedist = 0;
			else
				typedist = DISTANCE_MAX;
		}
	}
	else
	{
		typedist = 0;
		t->bind = bindtype;
		t->fcid = s_fcid;
	}
	
	return typedist;
}

static bool
same_array_dimensions(
	expr_t*				exprone,
	expr_t*				exprtwo )
{
	// FIXME handle multidimensional arrays

	if( !exprone || !exprtwo )
		return false;
		
	if( exprone->code != EXPR_ICONST ||
		exprtwo->code != EXPR_ICONST )
		return false;
		
	return ( (iconst_expr_t*)exprone )->val ==
		( (iconst_expr_t*)exprtwo )->val;
}

static long
ptr_type_distance(
	ptr_type_t*			fromtype,
	ptr_type_t*			totype )
{
	type_t*				downfromtype = fromtype->down;
	type_t*				downtotype = totype->down;
	
	downfromtype = pure_type( downfromtype );
	downtotype = pure_type( downtotype );
	
	if( ( downfromtype->quals & TC_CONST ) && 
		( downtotype->quals & TC_CONST ) == 0 )
	{
		return DISTANCE_MAX;
	}

	if( downtotype->code == TYPE_VOID )
		return 0;

	if( fromtype->is_array )
	{
		if( is_same_type(
			downfromtype,
			downtotype,
			true ) == false )
		{
			return DISTANCE_MAX;
		}
	
		if( totype->is_array )
		{
			if( same_array_dimensions(
				eval_expr( fromtype->array_expr ),
				eval_expr( totype->array_expr ) ) == false )
			{
				return DISTANCE_MAX;
			}
		}

		return 0;
	}
	else if( totype->is_array )
		return DISTANCE_MAX;
	
	if( downfromtype->code == TYPE_CLASS &&
		downtotype->code == TYPE_CLASS )
	{
		class_t*		fromclss;
		class_t*		toclss;
		
		fromclss = ( (class_type_t*)downfromtype )->clss;
		toclss = ( (class_type_t*)downtotype )->clss;

		if( fromclss == toclss )
			return 0;

		if( is_base_class( fromclss, toclss ) == FOUND )
			return 1;
			
		return DISTANCE_MAX;
	}
					
	if( is_same_type(
		downfromtype,
		downtotype,
		true ) )
	{
		return 0;
	}
		
	return DISTANCE_MAX;
}

static long
ref_type_distance(
	type_t*				fromtype,
	type_t*				totype )
{
	fromtype = pure_type( fromtype );
	totype = pure_type( totype );
	
	if( fromtype->code == TYPE_CLASS &&
		totype->code == TYPE_CLASS )
	{
		class_t*		fromclss;
		class_t*		toclss;
		
		fromclss = ( (class_type_t*)fromtype )->clss;
		toclss = ( (class_type_t*)totype )->clss;

		if( fromclss == toclss )
			return 0;

		if( is_base_class( fromclss, toclss ) == FOUND )
			return 1;

		return DISTANCE_MAX;
	}
	else if( totype->code == TYPE_TEMPLATE )
		return bind_template( totype, fromtype, false );
	else if( is_same_type( fromtype, totype ) )
		return 0;

	return DISTANCE_MAX;
}

inline long
arith_distance(
	e_type_code			fromcode,
	e_type_code			tocode,
	bool				signchange )
{
	long				dist;
	
	if( signchange )
		dist = 1;
	else
		dist = 0;

	if( fromcode == tocode )
		return dist;
		
	if( fromcode < tocode )
		dist += DISTANCE_ARITH_EXPAND * ( tocode - fromcode );
	else
		dist += DISTANCE_ARITH_REDUCE * ( fromcode - tocode );

	return dist;
}

long
type_distance(
	type_t*				fromtype,
	type_t*				totype )
{
	totype = reduce_type( totype );

	const e_type_code	tocode = totype->code;

	if( tocode == TYPE_REFERENCE )
	{
		return ref_type_distance(
			fromtype, totype );
	}
			
	const e_type_code	fromcode = fromtype->code;

	switch( fromcode )
	{
		case TYPE_ENUM:
		
			switch( tocode )
			{
				case TYPE_ENUM:
					if( fromtype == totype )
						return 0;
					
					return DISTANCE_MAX;
			
				TYPE_CASE_ARITHMETIC
					return DISTANCE_ENUM_TO_INT;

				case TYPE_BOOL:
					return DISTANCE_ENUM_TO_INT +
						DISTANCE_ARITH_TO_BOOL;

				case TYPE_TEMPLATE:
					return bind_template( totype, fromtype );
			}
			
			break;

		case TYPE_BOOL:
			
			switch( tocode )
			{
				TYPE_CASE_ARITHMETIC
					return DISTANCE_BOOL_TO_ARITH;
				
				case TYPE_BOOL:
					return 0;

				case TYPE_TEMPLATE:
					return bind_template( totype, fromtype );
			}
			
			break;
			
		TYPE_CASE_INTEGER
			
			switch( tocode )
			{
				TYPE_CASE_INTEGER
					return arith_distance( fromcode, tocode,
						( (int_type_t*)fromtype )->is_signed !=
						( (int_type_t*)totype )->is_signed );
				
				case TYPE_FLOAT:
				case TYPE_DOUBLE:
					return arith_distance( fromcode, tocode, false );
				
				case TYPE_BOOL:
					return DISTANCE_ARITH_TO_BOOL;

				case TYPE_TEMPLATE:
					return bind_template( totype, fromtype );
			}
			
			break;
			
		case TYPE_FLOAT:
		case TYPE_DOUBLE:
			
			switch( tocode )
			{
				TYPE_CASE_ARITHMETIC
					return arith_distance( fromcode, tocode, false );
				
				case TYPE_BOOL:
					return DISTANCE_ARITH_TO_BOOL;

				case TYPE_TEMPLATE:
					return bind_template( totype, fromtype );
			}
			
			break;
			
		case TYPE_POINTER:
		
			switch( tocode )
			{
				case TYPE_BOOL:
					return DISTANCE_PTR_TO_BOOL;
					
				case TYPE_POINTER:
					return ptr_type_distance(
						(ptr_type_t*)fromtype,
						(ptr_type_t*)totype );

				case TYPE_TEMPLATE:
					return bind_template( totype, fromtype );
			}
			
			break;
	
		case TYPE_REFERENCE:
			return type_distance(
				( (ref_type_t*)fromtype )->down,
				totype );
							
		case TYPE_NULL:
			
			if( tocode == TYPE_POINTER )
				return 0;
				
			break;
	
		case TYPE_CLASS:
		
			if( tocode == TYPE_CLASS )
			{
				class_t*		fromclss;
				class_t*		toclss;
		
				fromclss = ( (class_type_t*)fromtype )->clss;
				toclss = ( (class_type_t*)totype )->clss;

				if( fromclss == toclss )
					return 0;
			}
			else if( tocode == TYPE_TEMPLATE )
				return bind_template( totype, fromtype );
			
			break;
								
		case TYPE_TYPEDEF:
			return type_distance(
				( (typedef_type_t*)fromtype )->type, totype );
	}
	
	return DISTANCE_MAX;
}

END_COMPILER_NAMESPACE
