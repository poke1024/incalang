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
//	eval_expr.cp			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "expr_t.h"
#include "type_t.h"

BEGIN_COMPILER_NAMESPACE

static expr_t*
eval_bool_iexpr(
	expr_t*				expr,
	expr_t*				left,
	expr_t*				right,
	intval_t			lval,
	intval_t			rval )
{
	intval_t			nval;

	switch( expr->code )
	{
		case EXPR_LOGICAL_AND:
			nval = lval && rval;
			break;

		case EXPR_LOGICAL_OR:
			nval = lval || rval;
			break;
	
		case EXPR_LT:
			if( ( (int_type_t*)left->type )->is_signed &&
				( (int_type_t*)right->type )->is_signed )
				nval = (long)lval < (long)rval;
			else
				nval = lval < rval;
			break;
		
		case EXPR_GT:
			if( ( (int_type_t*)left->type )->is_signed &&
				( (int_type_t*)right->type )->is_signed )
				nval = (long)lval > (long)rval;
			else
				nval = lval > rval;
			break;
			
		case EXPR_LE:
			if( ( (int_type_t*)left->type )->is_signed &&
				( (int_type_t*)right->type )->is_signed )
				nval = (long)lval <= (long)rval;
			else
				nval = lval <= rval;
			break;
			
		case EXPR_GE:
			if( ( (int_type_t*)left->type )->is_signed &&
				( (int_type_t*)right->type )->is_signed )
				nval = (long)lval >= (long)rval;
			else
				nval = lval >= rval;
			break;
			
		case EXPR_EQ:
			nval = lval == rval;
			break;
			
		case EXPR_NE:
			nval = lval != rval;
			break;
	}

	return make_iconst_expr( nval, expr->type );
}

static expr_t*
eval_arith_iexpr(
	expr_t*				expr,
	expr_t*				left,
	expr_t*				right,
	intval_t			lval,
	intval_t			rval )
{
	intval_t			nval;

	switch( expr->code )
	{
		case EXPR_ADD:
			nval = lval + rval;
			break;

		case EXPR_SUB:
			nval = lval - rval;
			break;
			
		case EXPR_MUL:
			nval = lval * rval;
			break;
			
		case EXPR_DIV:
			if( rval == 0 )
			{
				compile_error( &expr->filepos,
					ERR_DIV_BY_ZERO );
				return expr;
			}

			if( ( (int_type_t*)left->type )->is_signed &&
				( (int_type_t*)right->type )->is_signed )
				nval = (long)lval / (long)rval;
			else
				nval = lval / rval;
			break;
			
		case EXPR_MOD:
			if( ( (int_type_t*)left->type )->is_signed &&
				( (int_type_t*)right->type )->is_signed )
				nval = (long)lval % (long)rval;
			else
				nval = lval % rval;
			break;
			
		case EXPR_SHL:
			nval = lval << rval;
			break;
			
		case EXPR_SHR:
			if( ( (int_type_t*)left->type )->is_signed )
				nval = (long)lval >> rval;
			else
				nval = lval >> rval;
			break;
			
		case EXPR_BITWISE_AND:
			nval = lval & rval;
			break;
			
		case EXPR_BITWISE_XOR:
			nval = lval ^ rval;
			break;
			
		case EXPR_BITWISE_OR:
			nval = lval | rval;
			break;
	}

	return make_iconst_expr( nval, expr->type );
}

static expr_t*
eval_bool_fexpr(
	expr_t*				expr,
	expr_t*				/*left*/,
	expr_t*				/*right*/,
	double				lval,
	double				rval )
{
	intval_t			nval;

	switch( expr->code )
	{
		case EXPR_LOGICAL_AND:
			nval = lval && rval;
			break;

		case EXPR_LOGICAL_OR:
			nval = lval || rval;
			break;
	
		case EXPR_LT:
			nval = lval < rval;
			break;
		
		case EXPR_GT:
			nval = lval > rval;
			break;
			
		case EXPR_LE:
			nval = lval <= rval;
			break;
			
		case EXPR_GE:
			nval = lval >= rval;
			break;
			
		case EXPR_EQ:
			nval = lval == rval;
			break;
			
		case EXPR_NE:
			nval = lval != rval;
			break;
	}

	return make_iconst_expr( nval, expr->type );
}

static expr_t*
eval_arith_fexpr(
	expr_t*				expr,
	expr_t*				/*left*/,
	expr_t*				/*right*/,
	double				lval,
	double				rval )
{
	double				nval;

	switch( expr->code )
	{
		case EXPR_ADD:
			nval = lval + rval;
			break;

		case EXPR_SUB:
			nval = lval - rval;
			break;
			
		case EXPR_MUL:
			nval = lval * rval;
			break;
			
		case EXPR_DIV:
			if( rval == 0 )
			{
				compile_error( &expr->filepos,
					ERR_DIV_BY_ZERO );
				return expr;
			}
			nval = lval / rval;
			break;
	}

	return make_fconst_expr( nval, expr->type );
}

static expr_t*
eval_iexpr(
	expr_t*				expr,
	expr_t*				left,
	expr_t*				right,
	intval_t			lval,
	intval_t			rval )
{
	switch( expr->code )
	{
		case EXPR_LOGICAL_AND:
		case EXPR_LOGICAL_OR:
		case EXPR_LT:
		case EXPR_GT:
		case EXPR_LE:
		case EXPR_GE:
		case EXPR_EQ:
		case EXPR_NE:
			return eval_bool_iexpr( expr, left, right, lval, rval );
			
		case EXPR_ADD:
		case EXPR_SUB:
		case EXPR_MUL:
		case EXPR_DIV:
		case EXPR_MOD:
		case EXPR_SHL:
		case EXPR_SHR:
		case EXPR_BITWISE_AND:
		case EXPR_BITWISE_XOR:
		case EXPR_BITWISE_OR:
			return eval_arith_iexpr( expr, left, right, lval, rval );
	}
	
	return expr;
}
static expr_t*
eval_fexpr(
	expr_t*				expr,
	expr_t*				left,
	expr_t*				right,
	double				lval,
	double				rval )
{
	switch( expr->code )
	{
		case EXPR_LOGICAL_AND:
		case EXPR_LOGICAL_OR:
		case EXPR_LT:
		case EXPR_GT:
		case EXPR_LE:
		case EXPR_GE:
		case EXPR_EQ:
		case EXPR_NE:
			return eval_bool_fexpr( expr, left, right, lval, rval );
			
		case EXPR_ADD:
		case EXPR_SUB:
		case EXPR_MUL:
		case EXPR_DIV:
			return eval_arith_fexpr( expr, left, right, lval, rval );
	}
	
	return expr;
}

static expr_t*
eval_binary_expr(
	binary_expr_t*		expr )
{
	expr_t*				left;
	expr_t*				right;

	left = eval_expr( expr->child[ 0 ] );
	right = eval_expr( expr->child[ 1 ] );

	const e_expr_code		lcode = left->code;

	if( lcode == EXPR_ICONST )
	{
		const e_expr_code	rcode = right->code;
	
		if( rcode == EXPR_ICONST )
		{
			return eval_iexpr(
				expr, left, right,
				( (iconst_expr_t*)left )->val,
				( (iconst_expr_t*)right )->val );
		}
		else if( rcode == EXPR_FCONST )
		{
			return eval_fexpr(
				expr, left, right,
				( (iconst_expr_t*)left )->val,
				( (fconst_expr_t*)right )->val );
		}
	}
	else if( lcode == EXPR_FCONST )
	{
		const e_expr_code	rcode = right->code;

		if( rcode == EXPR_ICONST )
		{
			return eval_fexpr(
				expr, left, right,
				( (fconst_expr_t*)left )->val,
				( (iconst_expr_t*)right )->val );
		}
		else if( rcode == EXPR_FCONST )
		{
			return eval_fexpr(
				expr, left, right,
				( (fconst_expr_t*)left )->val,
				( (fconst_expr_t*)right )->val );
		}
	}
	
	expr->child[ 0 ] = left;
	expr->child[ 1 ] = right;

	return expr;
}

static expr_t*
eval_unary_expr(
	unary_expr_t*		expr )
{
	expr_t*				child;
	
	child = eval_expr( expr->child );
	
	const e_expr_code	code = child->code;

	if( code == EXPR_ICONST )
	{
		intval_t		val = ( (iconst_expr_t*)child )->val;
		
		switch( expr->code )
		{
			case EXPR_LOGICAL_NOT:
				return make_iconst_expr( !val, expr->type );
				
			case EXPR_BITWISE_NOT:
				return make_iconst_expr( ~val, expr->type );
				
			case EXPR_NEGATE:
				return make_iconst_expr( -val, expr->type );
		}
	}
	else if( code == EXPR_FCONST )
	{
		double			val = ( (fconst_expr_t*)child )->val;
		
		switch( expr->code )
		{
			case EXPR_LOGICAL_NOT:
				return make_iconst_expr( !val, expr->type );
				
			case EXPR_NEGATE:
				return make_fconst_expr( -val, expr->type );
		}
	}
	
	expr->child = child;
	
	return expr;
}

static expr_t*
eval_sizeof(
	unary_expr_t*		expr )
{
	expr_t*				child = expr->child;

	return make_iconst_expr(
		get_type_size( child->type ), expr->type );
}

static expr_t*
eval_icast(
	expr_t*				expr,
	intval_t			val )
{
	type_t*				type = expr->type;
	
	switch( type->code )
	{
		case TYPE_CHAR:
			if( is_type_signed( type ) )
				val = (s08)val;
			else
				val = (u08)val;
			return make_iconst_expr( val, type );
			
		case TYPE_SHORT:
			if( is_type_signed( type ) )
				val = (s16)val;
			else
				val = (u16)val;
			return make_iconst_expr( val, type );

		case TYPE_INT:
		case TYPE_LONG:
			if( is_type_signed( type ) )
				val = (s32)val;
			else
				val = (u32)val;
			return make_iconst_expr( val, type );

		case TYPE_WIDE:
			return expr;
	
		case TYPE_FLOAT:
		case TYPE_DOUBLE:
			return make_fconst_expr( val, type );

		case TYPE_BOOL:
			return make_iconst_expr( val != 0 ? 1 : 0, type );

		default:
			compile_error( &expr->filepos, ERR_INTERNAL );		
	}
	
	return expr;
}

static expr_t*
eval_fcast(
	expr_t*				expr,
	fltval_t			val )
{
	type_t*				type = expr->type;
	intval_t			ival;
	
	switch( type->code )
	{
		case TYPE_CHAR:
			if( is_type_signed( type ) )
				ival = (s08)val;
			else
				ival = (u08)val;
			return make_iconst_expr( ival, type );
			
		case TYPE_SHORT:
			if( is_type_signed( type ) )
				ival = (s16)val;
			else
				ival = (u16)val;
			return make_iconst_expr( ival, type );

		case TYPE_INT:
		case TYPE_LONG:
			if( is_type_signed( type ) )
				ival = (s32)val;
			else
				ival = (u32)val;
			return make_iconst_expr( ival, type );

		case TYPE_WIDE:
			if( is_type_signed( type ) )
				ival = (s64)val;
			else
				ival = (u64)val;
			return make_iconst_expr( ival, type );
	
		case TYPE_FLOAT:
		case TYPE_DOUBLE:
			return make_fconst_expr( val, type );

		case TYPE_BOOL:
			return make_iconst_expr( val != 0 ? 1 : 0, type );

		default:
			compile_error( &expr->filepos, ERR_INTERNAL );		
	}
	
	return expr;
}

static expr_t*
eval_cast(
	unary_expr_t*		expr )
{
	expr_t*				child;

	child = eval_expr( expr->child );

	const e_expr_code	code = child->code;

	if( code == EXPR_ICONST )
	{
		return eval_icast(
			expr, ( (iconst_expr_t*)child )->val );
	}
	else if( code == EXPR_FCONST )
	{
		return eval_fcast(
			expr, ( (fconst_expr_t*)child )->val );
	}
	
	expr->child = child;
	
	return expr;
}

inline expr_t*
eval_id(
	expr_t*				expr )
{
	intval_t			ival;
	fltval_t			fval;

	if( get_expr_iconst( &expr, ival ) )
		return make_iconst_expr( ival, expr->type );

	if( get_expr_fconst( &expr, fval ) )
		return make_fconst_expr( fval, expr->type );
	
	return expr;
}

inline void
eval_call_func_expr(
	binary_expr_t*		expr )
{
	expr_t*				child;
	
	if( ( child = expr->child[ 1 ] ) != 0 )
		expr->child[ 1 ] = eval_expr( child );
}

expr_t*
eval_expr(
	expr_t*				expr )
{
	switch( expr->code )
	{
		case EXPR_ICONST:
		case EXPR_FCONST:
			break;
			
		case EXPR_ID:
			return eval_id( (id_expr_t*)expr );
	
		case EXPR_SIZEOF:
			return eval_sizeof( (unary_expr_t*)expr );
	
		case EXPR_TYPECAST:
			return eval_cast( (unary_expr_t*)expr );
	
		case EXPR_LOGICAL_AND:
		case EXPR_LOGICAL_OR:

		case EXPR_BITWISE_AND:
		case EXPR_BITWISE_XOR:
		case EXPR_BITWISE_OR:

		case EXPR_ADD:
		case EXPR_SUB:
		case EXPR_MUL:
		case EXPR_DIV:
		case EXPR_MOD:
		case EXPR_SHL:
		case EXPR_SHR:

		case EXPR_LT:
		case EXPR_GT:
		case EXPR_LE:
		case EXPR_GE:
		case EXPR_EQ:
		case EXPR_NE:
			return eval_binary_expr( (binary_expr_t*)expr );
			
		case EXPR_LOGICAL_NOT:
		case EXPR_BITWISE_NOT:
		case EXPR_NEGATE:
			return eval_unary_expr( (unary_expr_t*)expr );
			
		case EXPR_COMMA:
			( (binary_expr_t*)expr )->child[ 0 ] =
				eval_expr( ( (binary_expr_t*)expr )->child[ 0 ] );
			( (binary_expr_t*)expr )->child[ 1 ] =
				eval_expr( ( (binary_expr_t*)expr )->child[ 1 ] );
			break;
			
		case EXPR_ASSIGN:
		case EXPR_INITIALIZE:
		case EXPR_MUL_ASSIGN:
		case EXPR_DIV_ASSIGN:
		case EXPR_MOD_ASSIGN:
		case EXPR_ADD_ASSIGN:
		case EXPR_SUB_ASSIGN:
		case EXPR_SHL_ASSIGN:
		case EXPR_SHR_ASSIGN:
		case EXPR_AND_ASSIGN:
		case EXPR_XOR_ASSIGN:
		case EXPR_OR_ASSIGN:
			( (binary_expr_t*)expr )->child[ 1 ] =
				eval_expr( ( (binary_expr_t*)expr )->child[ 1 ] );
			break;

		case EXPR_CALL_FUNC:
			eval_call_func_expr( (binary_expr_t*)expr );
			break;

		case EXPR_BIN_OPERATOR:
			( (binary_expr_t*)expr )->child[ 0 ] =
				eval_expr( ( (binary_expr_t*)expr )->child[ 0 ] );
			break;
			
		case EXPR_PREUNA_OPERATOR:
		case EXPR_POSTUNA_OPERATOR:
			( (unary_expr_t*)expr )->child =
				eval_expr( ( (unary_expr_t*)expr )->child );
			break;
	}
	
	return expr;
}

END_COMPILER_NAMESPACE
