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
//	expr_t.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "expr_t.h"
#include "type_t.h"
#include "xalloc.h"

#include "scope_t.h"
#include "class_t.h"
#include "parser.h"
#include "pickfunc.h"

BEGIN_COMPILER_NAMESPACE

static expr_t*
alloc_expr(
	size_t				size )
{
	expr_t*				expr = (expr_t*)xalloc( size );
	
	get_filepos( &expr->filepos );
	expr->type = 0;
		
	return expr;
}

expr_t*
make_iconst_expr(
	intval_t			val,
	type_t*				type )
{
	iconst_expr_t*		expr = (iconst_expr_t*)alloc_expr(
							sizeof( iconst_expr_t ) );

	expr->code = EXPR_ICONST;
	expr->type = type;
	expr->val = val;
	
	return expr;
}

expr_t*
make_fconst_expr(
	fltval_t			val,
	type_t*				type )
{
	fconst_expr_t*		expr = (fconst_expr_t*)alloc_expr(
							sizeof( fconst_expr_t ) );

	expr->code = EXPR_FCONST;
	expr->type = type;
	expr->val = val;
	
	return expr;
}

expr_t*
make_id_expr(
	qualname_t*			id )
{
	id_expr_t*			expr = (id_expr_t*)alloc_expr(
							sizeof( id_expr_t ) );

	expr->code = EXPR_ID;
	expr->id = id;
	expr->var = 0;
	
	return expr;
}

expr_t*
make_str_expr(
	const char*			str )
{
	str_expr_t*			expr = (str_expr_t*)alloc_expr(
							sizeof( str_expr_t ) );

	expr->code = EXPR_STR;
	expr->str = str;

#if ARIADNE_STRING_OBJECTS
	expr->type = 0;
#else
	expr->type = make_ptr_type(
		make_int_type( TYPE_CHAR, true ), 0 );
#endif
	
	return expr;
}

expr_t*
make_null_expr()
{
	expr_t*				expr = alloc_expr(
							sizeof( expr_t ) );

	expr->code = EXPR_NULL;
	expr->type = make_type( TYPE_NULL );
	
	return expr;
}

expr_t*
make_this_expr()
{
	expr_t*				expr = alloc_expr(
							sizeof( expr_t ) );

	expr->code = EXPR_THIS;
	
	return expr;
}

expr_t*
make_unary_expr(
	e_expr_code			code,
	expr_t*				child )
{
	unary_expr_t*		expr = (unary_expr_t*)alloc_expr(
							sizeof( unary_expr_t ) );

	expr->code = code;
	expr->child = child;
	
	return expr;
}

expr_t*
make_binary_expr(
	e_expr_code			code,
	expr_t*				left_child,
	expr_t*				right_child )
{
	binary_expr_t*		expr = (binary_expr_t*)alloc_expr(
							sizeof( binary_expr_t ) );

	expr->code = code;
	expr->child[ 0 ] = left_child;
	expr->child[ 1 ] = right_child;
	
	return expr;
}

expr_t*
make_func_expr(
	func_t*				func,
	expr_t*				left_child,
	expr_t*				right_child )
{
	func_expr_t*		expr = (func_expr_t*)alloc_expr(
							sizeof( func_expr_t ) );

	expr->code = EXPR_CALL_FUNC;
	expr->child[ 0 ] = left_child;
	expr->child[ 1 ] = right_child;
	expr->func = func;
	expr->tmpltype = 0;
	expr->virtcall = true;
	
	return expr;
}

expr_t*
make_tmpl_func_expr(
	func_t*				func,
	type_seq_t*			tmpltype,
	expr_t*				left_child,
	expr_t*				right_child )
{
	func_expr_t*		expr = (func_expr_t*)make_func_expr(
							func, left_child, right_child );
	expr->tmpltype = tmpltype;
	return expr;
}

expr_t*
make_new_expr(
	type_t*				type )
{
	new_expr_t*			expr = (new_expr_t*)alloc_expr(
							sizeof( new_expr_t ) );

	expr->code = EXPR_NEW;
	expr->newtype = type;
	
	return expr;
}

expr_t*
make_dtnew_expr(
	type_t*				type,
	expr_t*				name )
{
	dtnew_expr_t*		expr = (dtnew_expr_t*)alloc_expr(
							sizeof( dtnew_expr_t ) );

	expr->code = EXPR_DTNEW;
	expr->newtype = type;
	expr->newname = name;
	
	return expr;
}

expr_t*
make_delete_expr(
	expr_t*				object )
{
	delete_expr_t*		expr = (delete_expr_t*)alloc_expr(
							sizeof( delete_expr_t ) );

	expr->code = EXPR_DELETE;
	expr->delobject = object;
	
	return expr;
}

expr_t*
make_variable_expr(
	variable_t*			var )
{
	// used by the declaration initializer code for referencing
	// variables that are already known and don't have to be
	// looked up again (see inst_decl())
	
	// this is also why we use var->scope as scope. we're
	// initializing (like in int a = 3), so we can be sure
	// that we're in the variable's scope.
		
	id_expr_t*			expr = (id_expr_t*)alloc_expr(
							sizeof( id_expr_t ) );

	expr->code = EXPR_VARIABLE;
	expr->id = 0;
	expr->var = var;
	expr->scope = var->scope;
	expr->type = var->decl->type;
	
	return expr;
}

expr_t*
make_thrown_object_expr(
	type_t*				type )
{
	// used by the try-catch-block mechanism to transport the
	// object thrown into the catch block. actually we construct
	// a declaration of the form [catch block declaration] =
	// [thrown object expr], for example Exception &e = [toe].

	expr_t*				expr = alloc_expr(
							sizeof( expr_t ) );

	expr->code = EXPR_THROWN_OBJECT;
	expr->type = type;
	
	return expr;
}

END_COMPILER_NAMESPACE
