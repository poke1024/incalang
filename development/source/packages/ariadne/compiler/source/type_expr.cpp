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
//	type_expr.cp			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "expr_t.h"
#include "type_t.h"
#include "scope_t.h"
#include "class_t.h"
#include "parser.h"
#include "xalloc.h"
#include "pickfunc.h"
#include "operators.h"

// general note. note that these functions should always get called after
// the resolving step has taken place (i.e. after call of scope_resolve for
// the global scope). if any of these functions is called before that,
// it's an error.

BEGIN_COMPILER_NAMESPACE

extern func_t*			s_compiled_func;

extern bool				s_init_declaration;

static bool
get_expr_iconst_recursive(
	expr_t**			exprp,
	intval_t&			value )
{
	expr_t*				expr = *exprp;
	variable_t*			var;
	decl_init_t*		init;
	type_t*				type;
	
	if( expr->type == 0 )
		if( type_expr( expr ) == false )
			return false;

	switch( expr->code )
	{
		case EXPR_ICONST:
			value = ( (iconst_expr_t*)expr )->val;
			return value;
	
		case EXPR_ID:
			var = ( (id_expr_t*)expr )->var;
			type = var->decl->type;
			
			if( is_type_integral( type ) == false )
				return false;

			if( is_type_constant( type ) == false )
				return false;
			
			init = var->decl->init;
			
			if( init == 0 )
				return false;
				
			if( init->expr == 0 )
				return false;
				
			return get_expr_iconst_recursive( &init->expr, value );
			
		default:
			expr = eval_expr( expr );
			*exprp = expr;
			switch( expr->code )
			{
				case EXPR_ICONST:
				case EXPR_ID:
					return get_expr_iconst_recursive( exprp, value );
			}
			break;
	}
	
	return false;
}

static bool
get_expr_fconst_recursive(
	expr_t**			exprp,
	fltval_t&			value )
{
	expr_t*				expr = *exprp;
	variable_t*			var;
	decl_init_t*		init;
	type_t*				type;

	if( expr->type == 0 )
		if( type_expr( expr ) == false )
			return false;

	switch( expr->code )
	{
		case EXPR_ICONST:
			value = ( (iconst_expr_t*)expr )->val;
			return value;
	
		case EXPR_FCONST:
			value = ( (fconst_expr_t*)expr )->val;
			return value;
	
		case EXPR_ID:
			var = ( (id_expr_t*)expr )->var;
			type = var->decl->type;

			if( is_type_floating( type ) == false )
				return false;

			if( is_type_constant( type ) == false )
				return false;
			
			init = var->decl->init;
			
			if( init == 0 )
				return false;
				
			if( init->expr == 0 )
				return false;
				
			return get_expr_fconst_recursive( &init->expr, value );

		default:
			expr = eval_expr( expr );
			*exprp = expr;
			switch( expr->code )
			{
				case EXPR_ICONST:
				case EXPR_FCONST:
				case EXPR_ID:
					return get_expr_fconst_recursive( exprp, value );
			}
			break;
	}
	
	return false;
}

bool
get_expr_iconst(
	expr_t**			exprp,
	intval_t&			value )
{
	bool				success;
	long				index = xalloc_frame_index();

	if( index > 0 )
		close_xalloc_frame();
	success = get_expr_iconst_recursive( exprp, value );
	if( index > 0 )
		open_xalloc_frame();
	
	return success;
}

bool
get_expr_fconst(
	expr_t**			exprp,
	fltval_t&			value )
{
	bool				success;
	long				index = xalloc_frame_index();

	if( index > 0 )
		close_xalloc_frame();
	success = get_expr_fconst_recursive( exprp, value );
	if( index > 0 )
		open_xalloc_frame();
	
	return success;
}

static bool
is_lvalue(
	expr_t*				expr )
{
	switch( expr->code )
	{
		case EXPR_ID:
		case EXPR_VARIABLE:
		case EXPR_ACCESS_ARRAY:
		case EXPR_ACCESS_DOT:
		case EXPR_ACCESS_PTR:
			return true;
		
		case EXPR_DEREFERENCE:
			return true;

		case EXPR_BIN_OPERATOR:
			if( expr->type->code == TYPE_REFERENCE )
				return true;
			return false;

		case EXPR_CALL_FUNC:
			if( expr->type->code == TYPE_REFERENCE )
				return true;
			return false;

		//case EXPR_ASSIGN:
		//	return true;
	}
	
	return false;
}

static bool
check_if_access_valid(
	fileref_t*			fpos,
	func_t*				caller,
	class_t*			clss,
	int					access,
	err_t				error = ERR_ILLEGAL_ACCESS )
{
	if( access == ACC_PUBLIC )
		return true;

	if( caller == 0 )
	{
		compile_error( fpos, error );
		return false;
	}

	class_t*			callerclss = caller->scope->clss;

	if( callerclss == clss )
		return true;
		
	if( access == ACC_PRIVATE )
	{
		compile_error( fpos, error );
		return false;
	}
	
	if( is_class_friend( clss, callerclss ) )
		return true;
	
	bool				inherited = false;
		
	switch( is_base_class( callerclss, clss ) )
	{
		case FOUND:
		case AMBIVALENT:
			inherited = true;
			break;
	}
	
	if( inherited == false )
	{
		compile_error( fpos, error );
		return false;
	}

	return true;
}

bool
check_if_cd_access_valid(
	fileref_t*			fpos,
	func_t*				caller,
	type_t*				type,
	int					cd )
{
	if( type->code != TYPE_CLASS )
		return true;
		
	class_t*			clss = ( (class_type_t*)type )->clss;
	func_t*				cfunc = clss->cd_func[ cd ];
		
	if( cfunc )
	{
		if( check_if_access_valid(
			fpos, caller, clss, cfunc->access,
			cd == CONSTRUCTOR ?
				ERR_ILLEGAL_CONSTRUCTOR_ACCESS :
				ERR_ILLEGAL_DESTRUCTOR_ACCESS ) == false )
		{
			return false;
		}
			
		qualhead_t*		node = clss->base;
		
		while( node )
		{
			if( !node->type )
			{
				compile_error( fpos, ERR_INTERNAL );
				return false;
			}
			
			if( check_if_cd_access_valid(
				fpos, cfunc, node->type, cd ) == false )
			{
				return false;
			}
			
			node = node->next;
		}
	}

	return true;
}

static void
expand_impl_bool(
	expr_t**			exprp )
{
	expr_t*				expr = *exprp;

	type_t*				type = expr->type;

	if( type->code == TYPE_BOOL )
		return;

	if( expr->code == EXPR_ICONST )
	{
		iconst_expr_t*	cexpr = (iconst_expr_t*)expr;
		
		cexpr->val = ( cexpr->val ? 1 : 0 );
		expr->type = make_type( TYPE_BOOL );
	}
	else if( expr->code == EXPR_FCONST )
	{
		expr = make_iconst_expr(
			( (fconst_expr_t*)expr )->val ? 1 : 0,
			make_type( TYPE_BOOL ) );
	}
	else
	{
		expr_t*			zero;
					
		if( type->code == TYPE_POINTER )
			zero = make_null_expr();
		else if( is_type_integral( type ) )
			zero = make_iconst_expr( 0, type );
		else
			zero = make_fconst_expr( 0, type );
			
		expr = make_binary_expr( EXPR_NE, expr, zero );
		expr->type = make_type( TYPE_BOOL );
	}

	*exprp = expr;
}

static bool
type_param_expr(
	expr_t*				expr,
	type_list_t***		link )
{
	if( expr->code == EXPR_COMMA )
	{
		if( type_param_expr(
			( (binary_expr_t*)expr )->child[ 0 ],
			link ) == false )
			return false;
			
		if( type_param_expr(
			( (binary_expr_t*)expr )->child[ 1 ],
			link ) == false )
			return false;
	
		return true;
	}

	if( type_expr( expr ) == false )
		return false;
		
	type_list_t*		node;
	
	node = make_type_list(
		0, expr->type );

	**link = node;
	*link = &node->next;
		
	return true;
}

type_list_t*
cast_param_expr(
	expr_t**			exprp,
	type_list_t*		node )
{
	expr_t*				expr = *exprp;

	if( expr->code == EXPR_COMMA )
	{
		node = cast_param_expr(
			&( (binary_expr_t*)expr )->child[ 0 ],
			node );
			
		node = cast_param_expr(
			&( (binary_expr_t*)expr )->child[ 1 ],
			node );
		
		return node;
	}
	
	caster_t*			cast = node->cast;
	
	if( cast )
	{
		func_expr_t*	conv;
		func_t*			func = cast->func;
	
		conv = (func_expr_t*)make_func_expr(
			0, 0, expr );
		conv->func = func;
		conv->type = cast->to;
		*exprp = conv;
	}
	
	return node->next;
}

static void
expand_impl_bool_param(
	expr_t**			exprp,
	decl_t**			declp )
{
	expr_t*				expr = *exprp;

	if( expr->code == EXPR_COMMA )
	{
		expand_impl_bool_param(
			&( (binary_expr_t*)expr )->child[ 0 ],
			declp );
			
		expand_impl_bool_param(
			&( (binary_expr_t*)expr )->child[ 1 ],
			declp );
	}
	else
	{
		decl_t*			decl = *declp;

		if( decl )
		{
			if( decl->type->code == TYPE_BOOL )
				expand_impl_bool( exprp );
		
			*declp = decl->next;
		}
	}
}

static bool
type_sizeof_expr(
	expr_t*				expr )
{
	expr_t*				child;

	child = ( (unary_expr_t*)expr )->child;

	e_type_code			code;
	
	code = reduce_type( child->type )->code;
	
	if( code == TYPE_NONE || code == TYPE_VOID )
	{
		compile_error( &expr->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	return true;
}

static type_t*
get_new_elemtype(
	type_t*					type,
	bool					init )
{
	while( type->code == TYPE_POINTER )
	{
		ptr_type_t*			ptype;
		
		ptype = (ptr_type_t*)type;
		
		if( !ptype->is_array )
			break;
		
		expr_t*				elemexpr;
		
		elemexpr = ptype->array_expr;
		
		if( init )
			if( !type_expr( elemexpr ) )
				return 0;

		if( !is_type_integral( elemexpr->type ) )
		{
			compile_error( &elemexpr->filepos,	
				ERR_ILLEGAL_TYPE );
			return 0;
		}
		
		if( init )
			ptype->array_expr = eval_expr(
				elemexpr );
	
		type = ptype->down;
	}
	
	return type;
}

static bool
type_new_expr(
	new_expr_t*				expr,
	bool					instantiate )
{
	// for new expressions, resolving of the type of
	// the allocated entity hasn't taken place so far
	// (since expr->newtype shows up in no scope), so
	// we do it here.

	expr->newtype = resolve_type( expr->newtype );

	type_t*					type = expr->newtype;
	
	type_t*					elemtype = get_new_elemtype( type, true );
	
	if( !elemtype )
		return false;
	
	while( type->code == TYPE_POINTER )
	{
		ptr_type_t*			ptype;
		
		ptype = (ptr_type_t*)type;
		
		if( ptype->is_array )
		{
			expr_t*			expr = ptype->array_expr;
			
			if( !type_expr( expr ) )
				return false;
		
			expr = eval_expr( expr );
		
			ptype->array_expr = expr;
		}
			
		type = ptype->down;
	}
	
	if( instantiate )
	{
		if( is_type_abstract( elemtype ) )
		{
			compile_error( &expr->filepos,
				ERR_ABSTRACT_CLASS );
			return false;
		}

		if( check_if_cd_access_valid(
			&expr->filepos, s_compiled_func,
			elemtype, CONSTRUCTOR ) == false  )
		{
			return false;
		}
	}
	
	expr->elemtype = elemtype;
	expr->type = make_ptr_type( elemtype, 0 );
	
	return true;
}

static bool
is_type_char_ptr(
	type_t*					type )
{
	if( type->code != TYPE_POINTER )
		return false;
		
	ptr_type_t*				ptr;
	
	ptr = (ptr_type_t*)type;
	
	if( ptr->down->code != TYPE_CHAR )
		return false;
	
	return true;
}

static bool
type_dtnew_expr(
	dtnew_expr_t*			expr )
{
	if( type_new_expr( expr, false ) == false )
		return false;
		
	if( expr->elemtype->code != TYPE_CLASS )
	{
		compile_error( &expr->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}
		
	if( type_expr( expr->newname ) == false )
		return false;
		
	if( is_type_char_ptr( expr->newname->type ) == false )
	{
		compile_error( &expr->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}
		
	return true;
}

static bool
type_delete_expr(
	delete_expr_t*			expr )
{
	if( !type_expr( expr->delobject ) )
		return false;

	type_t*					type = expr->delobject->type;

	if( type->code != TYPE_POINTER )
	{
		compile_error( &expr->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	type = ( (ptr_type_t*)type )->down;

	type_t*					elemtype = get_new_elemtype( type, false );

	if( !elemtype )
		return false;

	if( check_if_cd_access_valid(
		&expr->filepos, s_compiled_func,
		elemtype, DESTRUCTOR ) == false )
	{
		return false;
	}	
	
	expr->elemtype = elemtype;
	expr->type = make_type( TYPE_VOID );
	
	return true;
}

static bool
type_cast_expr(
	unary_expr_t*			expr )
{
	expr_t*					child = expr->child;

	if( !type_expr( child ) )
		return false;
		
	type_t*					fromtype = child->type;
	type_t*					totype = expr->type;
	
	if( !is_type_resolved( totype ) )
	{
		totype = resolve_type( totype );
		if( !totype )
		{
			istring			name;
			
			if( expr->type->code == TYPE_UNRESOLVED )
			{
				qualname_to_string(
					( (unresolved_type_t*)expr->type )->qualname,
					name );
			}
			else
				name = "(unknown)";
		
			compile_error( &expr->filepos,
				ERR_UNDECLARED_TYPE, name.c_str() );
			return false;
		}
	}
	
	if( !can_cast_type( fromtype, totype ) )
	{
		caster_t*			caster;
		
		if( scope_find_caster( cur_scope(),
			fromtype, totype,
			&expr->filepos, SCOPE_ALL, &caster ) )
		{
			func_expr_t*	conv;
			func_t*			func = caster->func;
			
			conv = (func_expr_t*)make_func_expr( 0, 0, child );
			conv->func = func;
			conv->type = caster->to;

			expr->code = EXPR_PREUNA_OPERATOR;
			expr->child = conv;
			expr->type = conv->type;
			
			return true;
		}
				
		compile_error( &expr->filepos,
			ERR_ILLEGAL_TYPECAST );
		return false;
	}
	
	if( totype->code == TYPE_BOOL )
		expand_impl_bool( &expr->child );
	
	return true;
}

static bool
type_id_expr(
	id_expr_t*				expr )
{
#if ICARUS_SAFE
	if( sizeof( iconst_expr_t ) > sizeof( id_expr_t ) )
		throw_icarus_error( "enumerator node problem" );
#endif

	scope_t*				scope = cur_scope();
	
	qualname_t*				name = expr->id;
	identifier_t			identifier;
	variable_t*				var;
	enum_t*					enumerator;
	
	name->scope = scope;

	switch( scope_find_qualname_identifier( name, &identifier ) )
	{	
		case 0:
			return false;
			
		case identifier_variable:
			var = identifier.variable;
			
			expr->var = var;
			expr->scope = scope;
			expr->type = var->decl->type;			
			break;
		
		case identifier_enumerator:
			enumerator = identifier.enumerator;
			expr->code = EXPR_ICONST;
			expr->type = enumerator->type;
			( (iconst_expr_t*)expr )->val =
				enumerator->node->value;		
			break;

		default:
			compile_error( &expr->filepos, ERR_INTERNAL );
			return false;
	}

	
	return true;
}

static bool
type_array_expr(
	expr_t*				expr )
{
	expr_t*				left;
	expr_t*				right;

	left = ( (binary_expr_t*)expr )->child[ 0 ];
	right = ( (binary_expr_t*)expr )->child[ 1 ];
	
	if( !type_expr( left ) || !type_expr( right ) )
		return false;
		
	type_t*				ltype = pure_type( left->type );
		
	if( ltype->code != TYPE_POINTER )
	{
		if( binary_overload( expr, left, right ) )
			return true;
	
		compile_error( &left->filepos,
			ERR_EXPECTED_POINTER );
		return false;
	}

	if( !is_type_integral( right->type ) )
	{
		if( binary_overload( expr, left, right ) )
			return true;

		compile_error( &right->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	expr->type = ( (ptr_type_t*)ltype )->down;
	
	return true;
}

static bool
type_qualname_expr(
	expr_t*				expr,
	bool				ptr )
{
	expr_t*				left;
	expr_t*				right;

	left = ( (binary_expr_t*)expr )->child[ 0 ];
	right = ( (binary_expr_t*)expr )->child[ 1 ];
	
	if( !type_expr( left ) )
		return false;
		
	e_expr_code			lcode = left->code;
		
	switch( lcode )
	{
		case EXPR_ID:
		case EXPR_ACCESS_DOT:
		case EXPR_ACCESS_PTR:
		case EXPR_ACCESS_ARRAY:

		case EXPR_ASSIGN:
		case EXPR_CALL_FUNC:
			break;

		default:
			compile_error( &left->filepos,
				ERR_SYNTAX_ERROR );
			return false;
	}
	
	type_t*				type = pure_type( left->type );
	
	if( ptr )
	{
		if( type->code != TYPE_POINTER )
		{
			compile_error( &left->filepos,
				ERR_EXPECTED_POINTER );
			return false;
		}
		type = ( (ptr_type_t*)type )->down;
	}
	
	int					quals = type->quals;
	
	if( type->code != TYPE_CLASS )
	{
		compile_error( &left->filepos,
			ERR_EXPECTED_CLASS );
		return false;
	}

	class_t*			clss = ( (class_type_t*)type )->clss;
	scope_t*			scope = clss->scope;

	/*if( std::strcmp( clss->name, "Point" ) == 0 &&
		scope->up == global_scope() )
	{
		if( not type_expr( right ) )
			return false;
		
		if( binary_overload( (binary_expr_t*)expr, left, right ) )
			return true;
	}*/

	if( right->code != EXPR_ID )
	{
		compile_error( &right->filepos,
			ERR_SYNTAX_ERROR );
		return false;
	}
	
	qualname_t*			name;
	
	name = ( (id_expr_t*)right )->id;
	name->scope = scope;
	
	identifier_t		identifier;
	
	switch( scope_find_qualname_identifier( name, &identifier ) )
	{
		case 0:
			return false;
			
		case identifier_variable:
			break;
			
		default:			
			compile_error( &right->filepos, ERR_SYNTAX_ERROR );
			return false;
	}

	variable_t*			var = identifier.variable;
	
	if( check_if_access_valid( &right->filepos,
		s_compiled_func, var->scope->clss, var->access ) == false )
	{
		return false;
	}
				
	id_expr_t*			id = (id_expr_t*)right;
	
	id->var = var;
	id->scope = scope;
		
	if( quals == 0 )
		expr->type = var->decl->type;
	else
	{
		type = copy_type_shallow( var->decl->type );
		type->quals |= quals;
		expr->type = type;
	}
	
	return true;
}

static bool
type_func_expr(
	func_expr_t*		expr )
{
	expr_t*				left;
	expr_t*				right;

	left = expr->child[ 0 ];
	right = expr->child[ 1 ];
	
	if( !left )
		compile_error( 0, ERR_INTERNAL );
	
	e_expr_code			lcode = left->code;
	expr_t*				idexpr;
	expr_t*				qual;
				
	scope_t*			scope;
		
	if( lcode == EXPR_ID )
	{
		scope = cur_scope();
		idexpr = left;
		qual = 0;
	}
	else if( lcode == EXPR_ACCESS_DOT ||
		lcode == EXPR_ACCESS_PTR )
	{	
		idexpr = ( (binary_expr_t*)left )->child[ 1 ];
	
		qual =  ( (binary_expr_t*)left )->child[ 0 ];
		if( !type_expr( qual ) )
			return false;
	
		type_t*			type = qual->type;
		
		type = pure_type( type );
	
		if( lcode == EXPR_ACCESS_PTR )
		{
			if( type->code != TYPE_POINTER )
			{
				compile_error( &qual->filepos,
					ERR_EXPECTED_POINTER );
				return false;
			}
			type = ( (ptr_type_t*)type )->down;
		}
	
		if( type->code != TYPE_CLASS )
		{
			compile_error( &qual->filepos,
				ERR_EXPECTED_CLASS );
			return false;
		}
		
		if( lcode == EXPR_ACCESS_DOT )
		{
			qual = make_unary_expr( EXPR_ADDRESS_OF, qual );
			qual->type = make_ptr_type( type, 0 );
		}
	
		scope = ( (class_type_t*)type )->clss->scope;
	}
	else
		return false;	

	qualname_t*			funcid;
			
	funcid = ( (id_expr_t*)idexpr )->id;
	
	// if we're not in global scope already, set
	// scope to calling environment scope
	if( funcid->scope == 0 )
		funcid->scope = scope;

	func_t*				func;
	
	func = scope_find_qualname_functions( funcid );

	if( func == 0 )
		return false;

	if( funcid->down )
		// if it's something of the form classname::funcname, then
		// we are not calling a virtual, but a specific function
		expr->virtcall = false;

	type_list_t*		head = 0;
	type_list_t**		link = &head;

	if( right )
	{
		if( type_param_expr( right, &link ) == false )
			return false;
	}

	if( ( func = pick_function( scope, func, head,
		expr->tmpltype, &expr->filepos, 0 ) ) == 0 )
		return false;

	if( check_if_access_valid( &expr->filepos,
		s_compiled_func, func->scope->clss, func->access ) == false )
	{
		return false;
	}
		
	if( right )
	{
		cast_param_expr( &right, head );
		expr->child[ 1 ] = right;

		expr_t*			texpr = right;
		decl_t*			tdecl = func->decl->func->param;
		expand_impl_bool_param( &texpr, &tdecl );
	}

	expr->func = func;
	expr->qual = qual;
	expr->scope = scope;

	expr->type = func->decl->type;
	
	return true;
}

static bool
type_this_expr(
	expr_t*				expr )
{
	type_t*				clsstype;
	
	clsstype = make_class_type( s_compiled_func->scope->clss );
	
	expr->type = make_ptr_type( clsstype, 0 );
	
	return true;
}

static bool
type_addr_expr(
	expr_t*				expr )
{
	expr_t*				child;

	child = ( (unary_expr_t*)expr )->child;

	if( !is_lvalue( child ) )
	{
		if( unary_overload( expr, child ) )
			return true;

		compile_error( &child->filepos,
			ERR_EXPECTED_LVALUE );
		return false;
	}
	
	if( !type_expr( child ) )
		return false;
		
	expr->type = make_ptr_type( child->type, 0 );
	
	return true;
}

static bool
type_deref_expr(
	expr_t*				expr )
{
	expr_t*				child;

	child = ( (unary_expr_t*)expr )->child;

	if( !type_expr( child ) )
		return false;

	type_t*				type = pure_type( child->type );

	if( type->code != TYPE_POINTER )
	{
		if( unary_overload( expr, child ) )
			return true;

		compile_error( &child->filepos,
			ERR_EXPECTED_POINTER );
		return false;
	}
			
	expr->type = ( (ptr_type_t*)type )->down;
	
	return true;
}

static bool
type_incdec_expr(
	expr_t*				expr )
{
	expr_t*				child;

	child = ( (unary_expr_t*)expr )->child;
				
	if( !is_lvalue( child ) )
	{
		compile_error( &child->filepos,
			ERR_EXPECTED_LVALUE );
		return false;
	}
	
	if( !type_expr( child ) )
		return false;
		
	type_t*				type = reduce_type( child->type );

	if( is_type_constant( type ) )
	{
		compile_error( &child->filepos,
			ERR_CONSTANT_ALTERED );
		return false;
	}
	
	if( is_type_arithmetic( type ) == false &&
		type->code != TYPE_POINTER )
	{
		if( unary_overload( expr, child ) )
			return true;
	
		compile_error( &child->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}
		
	expr->type = type;
	
	return true;
}

static bool
type_loguna_expr(
	unary_expr_t*		expr )
{
	expr_t*				child;

	child = expr->child;
						
	if( !type_expr( child ) )
		return false;
		
	if( !is_type_boolean( child->type ) )
	{
		if( unary_overload( expr, child ) )
			return true;

		compile_error( &child->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}
	
	expand_impl_bool( &expr->child );
	
	expr->type = make_type( TYPE_BOOL );

	return true;
}

static bool
type_logbin_expr(
	binary_expr_t*		expr )
{
	expr_t*				left;
	expr_t*				right;

	left = expr->child[ 0 ];
	right = expr->child[ 1 ];

	if( !type_expr( left ) || !type_expr( right ) )
		return false;
		
	if( !is_type_boolean( left->type ) )
	{
		if( binary_overload( expr, left, right ) )
			return true;
	
		compile_error( &left->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	if( !is_type_boolean( right->type ) )
	{
		if( binary_overload( expr, left, right ) )
			return true;

		compile_error( &right->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	expand_impl_bool( &expr->child[ 0 ] );
	expand_impl_bool( &expr->child[ 1 ] );

	expr->type = make_type( TYPE_BOOL );
	
	return true;
}

static bool
type_bituna_expr(
	expr_t*				expr )
{
	expr_t*				child;

	child = ( (unary_expr_t*)expr )->child;
		
	if( !type_expr( child ) )
		return false;
		
	if( !is_type_integral( child->type ) )
	{
		if( unary_overload( expr, child ) )
			return true;

		compile_error( &child->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}
		
	expr->type = child->type;
	
	return true;
}

static bool
type_intbin_expr(
	expr_t*				expr )
{
	expr_t*				left;
	expr_t*				right;

	left = ( (binary_expr_t*)expr )->child[ 0 ];
	right = ( (binary_expr_t*)expr )->child[ 1 ];

	if( !type_expr( left ) || !type_expr( right ) )
		return false;
				
	if( !is_type_integral( left->type ) )
	{
		if( binary_overload( expr, left, right ) )
			return true;
	
		compile_error( &left->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	if( !is_type_integral( right->type ) )
	{
		if( binary_overload( expr, left, right ) )
			return true;

		compile_error( &right->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}
		
	expr->type = common_type( left->type, right->type );
	
	return true;
}

static bool
type_addsub_expr(
	expr_t*				expr )
{
	expr_t*				left;
	expr_t*				right;

	left = ( (binary_expr_t*)expr )->child[ 0 ];
	right = ( (binary_expr_t*)expr )->child[ 1 ];
	
	if( !type_expr( left ) || !type_expr( right ) )
		return false;

	type_t*				ctype;
		
	type_t*				ltype = reduce_type( left->type );
		
	if( ltype->code == TYPE_POINTER )
	{
		if( is_type_integral( right->type ) )
			ctype = left->type;
		else
			ctype = 0;
	}
	else
		ctype = common_type( left->type, right->type );
	
	if( !ctype )
	{
		if( binary_overload( expr, left, right ) )
			return true;
	
		compile_error( &left->filepos,
			ERR_TYPE_MISMATCH );
		return false;
	}
		
	expr->type = ctype;
	
	return true;
}

static bool
type_aribin_expr(
	expr_t*				expr )
{
	expr_t*				left;
	expr_t*				right;

	left = ( (binary_expr_t*)expr )->child[ 0 ];
	right = ( (binary_expr_t*)expr )->child[ 1 ];
	
	if( !type_expr( left ) || !type_expr( right ) )
		return false;
		
	type_t*				ctype;
	
	ctype = common_type( left->type, right->type );
	
	if( !ctype )
	{
		if( binary_overload( expr, left, right ) )
			return true;
	
		compile_error( &left->filepos,
			ERR_TYPE_MISMATCH );
		return false;
	}
		
	expr->type = ctype;
	
	return true;
}

static bool
type_ariuna_expr(
	unary_expr_t*		expr )
{
	expr_t*				child = expr->child;

	if( !type_expr( child ) )
		return false;
		
	type_t*				type = child->type;
		
	if( !is_type_arithmetic( type ) )
	{
		if( unary_overload( expr, child ) )
			return true;
	
		compile_error( &expr->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}
		
	expr->type = type;
	
	return true;
}

static bool
type_cmpbin_expr(
	expr_t*				expr )
{
	expr_t*				left;
	expr_t*				right;

	left = ( (binary_expr_t*)expr )->child[ 0 ];
	right = ( (binary_expr_t*)expr )->child[ 1 ];

	if( !type_expr( left ) || !type_expr( right ) )
		return false;

	bool				is_comparable;
		
	switch( expr->code )
	{
		case EXPR_EQ:
		case EXPR_NE:
			is_comparable = are_types_comparable_eq( 
				left->type, right->type );
			break;
			
		default:
			is_comparable = are_types_comparable_lg( 
				left->type, right->type );
			break;
	}
		
	if( !is_comparable )
	{
		if( binary_overload( expr, left, right ) )
			return true;
	
		compile_error( &left->filepos,
			ERR_TYPE_MISMATCH );
		return false;
	}
		
	expr->type = make_type( TYPE_BOOL );
	
	return true;
}

static bool
type_assign_expr(
	expr_t*				expr )
{
	expr_t*				left;
	expr_t*				right;

	left = ( (binary_expr_t*)expr )->child[ 0 ];
	right = ( (binary_expr_t*)expr )->child[ 1 ];

	if( !type_expr( left ) || !type_expr( right ) )
		return false;

	if( !is_lvalue( left ) )
	{
		compile_error( &left->filepos,
			ERR_EXPECTED_LVALUE );
		return false;
	}
	
	type_t*				ltype = left->type;
	type_t*				rtype = right->type;
	bool				init_decl = s_init_declaration;
		
	if( is_type_constant( ltype ) && init_decl == false )
	{
		compile_error( &left->filepos,
			ERR_CONSTANT_ALTERED );
		return false;
	}
	
	if( binary_overload( expr, left, right ) )
		return true;
		
	if( can_assign_type( rtype, ltype ) == false )
	{
		compile_error( &left->filepos,
			ERR_TYPE_MISMATCH );
		return false;
	}
	
	if( ltype->code == TYPE_BOOL )
		expand_impl_bool( &( (binary_expr_t*)expr )->child[ 1 ] );
	
	expr->type = left->type;
	
	return true;
}

static bool
factor_combass_expr(
	binary_expr_t*		expr )
{
	e_expr_code			code;

	switch( expr->code )
	{
		case EXPR_MUL_ASSIGN:
			code = EXPR_MUL;
			break;
		
		case EXPR_DIV_ASSIGN:
			code = EXPR_DIV;
			break;
		
		case EXPR_ADD_ASSIGN:
			code = EXPR_ADD;
			break;
		
		case EXPR_SUB_ASSIGN:
			code = EXPR_SUB;
			break;
		
		case EXPR_MOD_ASSIGN:
			code = EXPR_MOD;
			break;
		
		case EXPR_SHL_ASSIGN:
			code = EXPR_SHL;
			break;

		case EXPR_SHR_ASSIGN:
			code = EXPR_SHR;
			break;

		case EXPR_AND_ASSIGN:
			code = EXPR_BITWISE_AND;
			break;

		case EXPR_XOR_ASSIGN:	
			code = EXPR_BITWISE_XOR;
			break;

		case EXPR_OR_ASSIGN:
			code = EXPR_BITWISE_OR;
			break;
			
		default:
			return false;
	}
	
	expr_t*				node;
	
	node = make_binary_expr(
		code, expr->child[ 0 ], expr->child[ 1 ] );

	expr->code = EXPR_ASSIGN;
	expr->child[ 1 ] = node;

	return true;
}

static bool
type_ariass_expr(
	expr_t*				expr )
{
	expr_t*				left;
	expr_t*				right;

	left = ( (binary_expr_t*)expr )->child[ 0 ];
	right = ( (binary_expr_t*)expr )->child[ 1 ];

	if( !type_expr( left ) || !type_expr( right ) )
		return false;

	if( !is_lvalue( left ) )
	{
		compile_error( &left->filepos,
			ERR_EXPECTED_LVALUE );
		return false;
	}

	if( is_type_constant( left->type ) && s_init_declaration == false )
	{
		compile_error( &left->filepos,
			ERR_CONSTANT_ALTERED );
		return false;
	}
		
	if( !is_type_arithmetic( left->type ) )
	{
		if( binary_overload( expr, left, right ) )
			return true;
	
		if( factor_combass_expr( (binary_expr_t*)expr ) )
			return type_expr( expr );
	
		compile_error( &left->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	if( !is_type_arithmetic( right->type ) )
	{
		if( binary_overload( expr, left, right ) )
			return true;
	
		if( factor_combass_expr( (binary_expr_t*)expr ) )
			return type_expr( expr );
	
		compile_error( &right->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	expr->type = left->type;
	
	return true;
}

static bool
type_assint_expr(
	expr_t*				expr )
{
	expr_t*				left;
	expr_t*				right;

	left = ( (binary_expr_t*)expr )->child[ 0 ];
	right = ( (binary_expr_t*)expr )->child[ 1 ];

	if( !type_expr( left ) || !type_expr( right ) )
		return false;
		
	if( !is_lvalue( left ) )
	{
		compile_error( &left->filepos,
			ERR_EXPECTED_LVALUE );
		return false;
	}

	if( is_type_constant( left->type ) && s_init_declaration == false )
	{
		compile_error( &left->filepos,
			ERR_CONSTANT_ALTERED );
		return false;
	}
		
	if( !is_type_integral( left->type ) )
	{
		if( binary_overload( expr, left, right ) )
			return true;		
	
		if( factor_combass_expr( (binary_expr_t*)expr ) )
			return type_expr( expr );
			
		compile_error( &left->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	if( !is_type_integral( right->type ) )
	{
		if( binary_overload( expr, left, right ) )
			return true;

		if( factor_combass_expr( (binary_expr_t*)expr ) )
			return type_expr( expr );
			
		compile_error( &right->filepos,
			ERR_ILLEGAL_TYPE );
		return false;
	}

	expr->type = left->type;
	
	return true;
}

#if ARIADNE_STRING_OBJECTS
extern class_t*			s_string_class;

static bool
type_str_expr(
	str_expr_t*			expr )
{
	class_t*			clss;

	clss = s_string_class;
	
	if( clss == 0 )
	{
		compile_error( 0, ERR_INTERNAL );
		return false;
	}

	type_t*				type;

	type = make_class_type( clss );
	type->quals = TC_CONST;

	expr->type = make_ref_type( type, 0 );

	return true;
}
#endif

static bool
type_comma_expr(
	binary_expr_t*		expr )
{
	if( type_expr( expr->child[ 0 ] ) == false )
		return false;

	expr_t*				right = expr->child[ 1 ];

	if( type_expr( right ) == false )
		return false;
		
	expr->type = right->type;
	
	return true;
}

bool
type_expr(
	expr_t*				expr )
{
	switch( expr->code )
	{
		case EXPR_ICONST:
		case EXPR_FCONST:
		case EXPR_NULL:
		case EXPR_VARIABLE:
		case EXPR_THROWN_OBJECT:
			break;

		case EXPR_STR:
#if ARIADNE_STRING_OBJECTS
			return type_str_expr( (str_expr_t*)expr );
#else
			break;
#endif
			
		case EXPR_NEW:
			return type_new_expr( (new_expr_t*)expr, true );

		case EXPR_DTNEW:
			return type_dtnew_expr( (dtnew_expr_t*)expr );
			
		case EXPR_DELETE:
			return type_delete_expr( (delete_expr_t*)expr );
			
		case EXPR_SIZEOF:
			return type_sizeof_expr( expr );

		case EXPR_TYPECAST:
			return type_cast_expr( (unary_expr_t*)expr );		
			
		case EXPR_ID:
			return type_id_expr( (id_expr_t*)expr );
	
		case EXPR_ACCESS_ARRAY:
			return type_array_expr( expr );
	
		case EXPR_ACCESS_DOT:
			return type_qualname_expr( expr, false );
		
		case EXPR_ACCESS_PTR:
			return type_qualname_expr( expr, true );
			
		case EXPR_CALL_FUNC:
			return type_func_expr( (func_expr_t*)expr );
			
		case EXPR_THIS:
			return type_this_expr( expr );
		
		case EXPR_ADDRESS_OF:
			return type_addr_expr( expr );
	
		case EXPR_DEREFERENCE:
			return type_deref_expr( expr );
			
	
		case EXPR_PREINC:
		case EXPR_PREDEC:
		case EXPR_POSTINC:
		case EXPR_POSTDEC:
			return type_incdec_expr( expr );
			
		case EXPR_LOGICAL_NOT:
			return type_loguna_expr( (unary_expr_t*)expr );
			
		case EXPR_LOGICAL_AND:
		case EXPR_LOGICAL_OR:
			return type_logbin_expr( (binary_expr_t*)expr );
			
		case EXPR_BITWISE_NOT:
			return type_bituna_expr( expr );
			
		case EXPR_BITWISE_AND:
		case EXPR_BITWISE_XOR:
		case EXPR_BITWISE_OR:
			return type_intbin_expr( expr );
		
		case EXPR_ADD:
		case EXPR_SUB:
			return type_addsub_expr( expr );
		
		case EXPR_MOD:
		case EXPR_MUL:
		case EXPR_DIV:
			return type_aribin_expr( expr );
		
		case EXPR_NEGATE:
			return type_ariuna_expr( (unary_expr_t*)expr );
		
		case EXPR_SHL:
		case EXPR_SHR:
			return type_intbin_expr( expr );
			
		case EXPR_LT:
		case EXPR_GT:
		case EXPR_LE:
		case EXPR_GE:
		case EXPR_EQ:
		case EXPR_NE:
			return type_cmpbin_expr( expr );
		
		case EXPR_ASSIGN:
		case EXPR_INITIALIZE:
			return type_assign_expr( expr );
		
		case EXPR_MUL_ASSIGN:
		case EXPR_DIV_ASSIGN:
		case EXPR_ADD_ASSIGN:
		case EXPR_SUB_ASSIGN:
			return type_ariass_expr( expr );
		
		case EXPR_MOD_ASSIGN:
		case EXPR_SHL_ASSIGN:
		case EXPR_SHR_ASSIGN:
		case EXPR_AND_ASSIGN:
		case EXPR_XOR_ASSIGN:	
		case EXPR_OR_ASSIGN:
			return type_assint_expr( expr );

		case EXPR_COMMA:
			return type_comma_expr( (binary_expr_t*)expr );
		
		case EXPR_BIN_OPERATOR:
		case EXPR_PREUNA_OPERATOR:
		case EXPR_POSTUNA_OPERATOR:
			break;
		
		default:
			compile_error( 0, ERR_INTERNAL );
			return false;
	}

	return true;
}

END_COMPILER_NAMESPACE
