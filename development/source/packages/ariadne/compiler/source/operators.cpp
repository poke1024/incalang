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
//	operators.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "operators.h"

#include "expr_t.h"
#include "type_t.h"
#include "scope_t.h"
#include "parser.h"
#include "pickfunc.h"
#include "func_list_t.h"
#include "class_t.h"

BEGIN_COMPILER_NAMESPACE

#define					OPERATOR_METHODS	1

static const char*
operator_name(
	e_expr_code			code )
{
	switch( code )
	{
		case EXPR_ACCESS_ARRAY:
			return "#[]";

		case EXPR_ACCESS_DOT:
			return "#.";
			
		case EXPR_DEREFERENCE:
			return "#*";
			
		case EXPR_ADDRESS_OF:
			return "#&";
				
		case EXPR_LOGICAL_NOT:
			return "#!";
			
		case EXPR_LOGICAL_AND:
			return "#&&";
			
		case EXPR_LOGICAL_OR:
			return "#||";
			
		case EXPR_BITWISE_NOT:
			return "#~";
			
		case EXPR_BITWISE_AND:
			return "#&";
			
		case EXPR_BITWISE_XOR:
			return "#^";

		case EXPR_BITWISE_OR:
			return "#|";
	
		case EXPR_ADD:
			return "#+";

		case EXPR_SUB:
		case EXPR_NEGATE:
			return "#-";

		case EXPR_MUL:
			return "#*";

		case EXPR_DIV:
			return "#/";
	
		case EXPR_MOD:
			return "#%";
			
		case EXPR_SHL:
			return "#<<";

		case EXPR_SHR:
			return "#>>";
			
		case EXPR_LT:
			return "#<";

		case EXPR_GT:
			return "#>";

		case EXPR_LE:
			return "#<=";

		case EXPR_GE:
			return "#>=";

		case EXPR_EQ:
			return "#==";

		case EXPR_NE:
			return "#!=";
	
		case EXPR_ASSIGN:
		case EXPR_INITIALIZE:
			return "#=";

		case EXPR_MUL_ASSIGN:
			return "#*=";

		case EXPR_DIV_ASSIGN:
			return "#/=";

		case EXPR_MOD_ASSIGN:
			return "#%=";

		case EXPR_ADD_ASSIGN:
			return "#+=";

		case EXPR_SUB_ASSIGN:
			return "#-=";

		case EXPR_SHL_ASSIGN:
			return "#<<=";

		case EXPR_SHR_ASSIGN:
			return "#>>=";

		case EXPR_AND_ASSIGN:
			return "#&=";

		case EXPR_XOR_ASSIGN:
			return "#^=";
			
		case EXPR_OR_ASSIGN:
			return "#|=";
			
		case EXPR_PREINC:
		case EXPR_POSTINC:
			return "#++";
		
		case EXPR_PREDEC:
		case EXPR_POSTDEC:
			return "#--";
		
	}
	
	compile_error( 0, ERR_INTERNAL );
	return "";
}	

static func_t*
find_binary_operator_func(
	scope_t*			scope,
	const char*			name,
	type_t*				left,
	type_t*				right,
	fileref_t*			filepos,
	type_list_t*		typelist )
{
	func_t*				func = 0;
	func_list_t			func_list;

	func_list_init( &func_list );

	if( scope_find_functions( cur_scope(),
		name, filepos, SCOPE_ALL, &func_list ) )
	{
		typelist[ 0 ].next = &typelist[ 1 ];
		typelist[ 0 ].type = left;

		typelist[ 1 ].next = 0;
		typelist[ 1 ].type = right;

		func = pick_function( scope, func_list.head,
			typelist, 0, filepos, PICKFUNC_QUIET );
	}

#if OPERATOR_METHODS
	if( func == 0 )
	{
		left = pure_type( left );
		if( left->code == TYPE_CLASS )
		{
			class_t*	clss = ( (class_type_t*)left )->clss;

			if( scope_find_functions( clss->scope,
				name, filepos, SCOPE_ALL, &func_list ) )
			{
				typelist[ 0 ].next = 0;
				typelist[ 0 ].type = right;

				func = pick_function( scope, func_list.head,
					typelist, 0, filepos, PICKFUNC_QUIET );
			}
		}
	}
#endif

	return func;
}

static void
make_binary_operator_call(
	binary_expr_t*		expr,
	func_t*				func,
	expr_t*				left,
	expr_t*				right,
	type_list_t*		typelist )
{
	expr_t*				param;

#if OPERATOR_METHODS
	if( func->param->next == 0 )
		param = right;
	else
#endif
		param = make_binary_expr(
			EXPR_COMMA, left, right );
		
	cast_param_expr( &param, typelist );

	func_expr_t*		funcexpr;

	funcexpr = (func_expr_t*)make_func_expr( 0, 0, param );
	funcexpr->func = func;
	funcexpr->qual = 0;
	funcexpr->scope = cur_scope();
	funcexpr->type = func->decl->type;

#if OPERATOR_METHODS
	if( func->param->next == 0 )
	{
		funcexpr->qual = make_unary_expr(
			EXPR_ADDRESS_OF, left );
		funcexpr->virtcall = true;
	}
#endif

	expr->code = EXPR_BIN_OPERATOR;
	expr->child[ 0 ] = funcexpr;
	expr->type = func->decl->type;
}

static func_t*
find_unary_operator_func(
	scope_t*			scope,
	const char*			name,
	type_t*				type,
	fileref_t*			filepos,
	type_list_t*		typelist )
{
	func_t*				func = 0;
	func_list_t			func_list;

	func_list_init( &func_list );

	if( scope_find_functions( cur_scope(),
		name, filepos, SCOPE_ALL, &func_list ) )
	{
		typelist->next = 0;
		typelist->type = type;

		func = pick_function( scope, func_list.head,
			typelist, 0, filepos, PICKFUNC_QUIET );
	}

#if OPERATOR_METHODS
	if( func == 0 )
	{
		type = pure_type( type );
		if( type->code == TYPE_CLASS )
		{
			class_t*	clss = ( (class_type_t*)type )->clss;

			if( scope_find_functions( clss->scope,
				name, filepos, SCOPE_ALL, &func_list ) )
			{
				func = pick_function( scope, func_list.head,
					0, 0, filepos, PICKFUNC_QUIET );
			}
		}
	}
#endif

	return func;
}

static void
make_unary_operator_call(
	unary_expr_t*		expr,
	func_t*				func,
	expr_t*				param,
	type_list_t*		typelist )
{
#if OPERATOR_METHODS
	if( func->param == 0 )
		param = 0;
#endif

	cast_param_expr( &param, typelist );

	func_expr_t*		funcexpr;

	funcexpr = (func_expr_t*)make_func_expr( 0, 0, param );
	funcexpr->func = func;
	funcexpr->qual = 0;
	funcexpr->scope = cur_scope();
	funcexpr->type = func->decl->type;

#if OPERATOR_METHODS
	if( func->param == 0 )
	{
		funcexpr->qual = make_unary_expr(
			EXPR_ADDRESS_OF, expr );
		funcexpr->virtcall = true;
	}
#endif

	switch( expr->code )
	{
		case EXPR_POSTINC:
		case EXPR_POSTDEC:
			expr->code = EXPR_POSTUNA_OPERATOR;
			break;
	
		case EXPR_PREINC:
		case EXPR_PREDEC:
		default:
			expr->code = EXPR_PREUNA_OPERATOR;
			break;
	}
	
	expr->child = funcexpr;
	expr->type = func->decl->type;
}

bool
unary_overload(
	expr_t*				expr,
	expr_t*				child )
{
	scope_t*			scope = cur_scope();

	type_list_t			typelist[ 1 ];
	func_t*				func;
	
	func = find_unary_operator_func(
		scope, operator_name( expr->code ),
		child->type, &expr->filepos, typelist );

	if( func )
	{
		make_unary_operator_call(
			(unary_expr_t*)expr, func,
			child, typelist );
		return true;
	}
	
	return false;
}

bool
binary_overload(
	expr_t*				expr,
	expr_t*				left,
	expr_t*				right )
{
	scope_t*			scope = cur_scope();

	type_list_t			typelist[ 2 ];
	func_t*				func;
	
	func = find_binary_operator_func(
		scope, operator_name( expr->code ),
		left->type, right->type,
		&expr->filepos, typelist );

	if( func )
	{
		make_binary_operator_call(
			(binary_expr_t*)expr, func,
			left, right, typelist );
		return true;
	}
	
	return false;
}

END_COMPILER_NAMESPACE
