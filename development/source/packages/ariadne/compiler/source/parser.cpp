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
//	parser.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "parser.h"
#include "expr_t.h"
#include "type_t.h"
#include "stmt_t.h"
#include "xalloc.h"
#include "class_t.h"
#include "scope_t.h"
#include <cassert>

#include <iostream>
#include <string>

BEGIN_COMPILER_NAMESPACE

using namespace std;

static bool
check_decl_init(
	decl_init_t*		init,
	type_t*				type );

decl_init_t*
make_decl_init()
{
	decl_init_t*		init;
	
	init = (decl_init_t*)xalloc( sizeof( decl_init_t ) );
	
	init->expr = 0;
	init->list = 0;
	init->next = 0;
	
	get_filepos( &init->filepos );

	return init;
}

decl_spec_t*
make_decl_spec(
	type_t*				type,
	int					modifiers )
{
	decl_spec_t*		spec;
	
	spec = (decl_spec_t*)xalloc( sizeof( decl_spec_t ) );

	spec->type = type;
	spec->modifiers = modifiers;	
	get_filepos( &spec->filepos );
	
	// tweak file position
	spec->filepos.pos += 1;
	
	if( modifiers & MOD_CONST )
	{
		type_t**		link;
		
		link = find_bottom_type( &spec->type );
		( *link )->quals |= TC_CONST;
	}
	
	return spec;
}

decl_func_t*
make_decl_func(
	decl_t*				param )
{
	decl_func_t*		func;
	
	func = (decl_func_t*)xalloc( sizeof( decl_func_t ) );
	func->param = param;

	return func;
}

decl_t*
make_decl(
	const char*			name )
{
	decl_t*				decl;
	
	decl = (decl_t*)xalloc( sizeof( decl_t ) );
	decl->name = name; // ? xstrdup( name ) : 0;
	decl->next = 0;
	
	decl->type = 0;
	decl->prefix = 0;
	decl->postfix = 0;
	
	decl->spec = 0;
	decl->init = 0;

	decl->func = 0;
	decl->tmpltype = 0;
	
	return decl;
}

array_decl_t*
make_array_decl(
	expr_t*				expr )
{
	array_decl_t*		decl;
	
	decl = (array_decl_t*)xalloc( sizeof( array_decl_t ) );
	decl->expr = expr;
	decl->up = 0;
	decl->down = 0;
	decl->tail = decl;
	
	return decl;
}

decl_t*
combine_array_decl(
	decl_t*				decl,
	array_decl_t*		array )
{	
	array_decl_t*		node = array;
	
	while( true )
	{
		array_decl_t*	down;

		down = node->down;
		if( down == 0 )
			break;
		node = down;
	}
	
	type_t*				postfix = decl->postfix;

	while( node )
	{
		expr_t*			expr = node->expr;
	
		ptr_type_t*		type;
		
		type = make_ptr_type( postfix, 0 );
		
		type->is_array = true;
		type->array_expr = expr;

		postfix = type;
		
		node = node->up;
	}
	
	decl->postfix = postfix;

	return decl;
}

enum_list_t*
make_enum_list()
{
	enum_list_t*		list;
	
	list = (enum_list_t*)xalloc( sizeof( enum_list_t ) );
	list->first = 0;
	list->link = &list->first;
	list->value = 1;
	
	return list;
}

void
append_enumerator(
	enum_list_t*		list,
	const char*			name,
	expr_t*				expr )
{
	enum_node_t*		node;
	
	node = (enum_node_t*)xalloc( sizeof( enum_node_t ) );
	node->name = name;
	node->expr = expr;
	
	if( expr == 0 )
		node->value = list->value++;
	else
	{
		if( type_expr( expr ) == false )
			return;
			
		expr = eval_expr( expr );
		
		if( expr->code != EXPR_ICONST )
		{
			compile_error( &expr->filepos,
				ERR_EXPECTED_INTEGER_CONSTANT );
			return;
		}

		iconst_expr_t*	iexpr = (iconst_expr_t*)expr;
		intval_t		ival = iexpr->val;
		enumval_t		eval = (enumval_t)ival;
			
		if( eval != ival )
		{
			compile_error( &expr->filepos,
				ERR_INTEGER_CONSTANT_TOO_BIG );
			return;
		}

		node->value = eval;
		list->value = eval + 1;
	}
	
	node->next = 0;
	*list->link = node;
	list->link = &node->next;
}

typename_list_t*
make_typename_list(
	typename_list_t*	next,
	const char*			name )
{
	typename_list_t*	list;
	
	list = (typename_list_t*)xalloc(
		sizeof( typename_list_t ) );
	list->next = next;
	list->name = name;

	return list;
}

// ---------------------------------------------------------------------------


inline type_t*
append_ptr_list(
	type_t*				head,
	type_t*				type )
{
	if( head == 0 )
		return type;

	type_t*				node = head;

	while( true )
	{
		type_t**		down;

		switch( node->code )
		{
			case TYPE_POINTER:
				down = &(( ptr_type_t*)node )->down;
				break;

			case TYPE_REFERENCE:	
				down = &(( ref_type_t*)node )->down;
				break;

			default:
				compile_error( 0, ERR_INTERNAL );
				return type;
		}

		if( ( node = *down ) == 0 )
		{
			*down = type;
			break;
		}
	}

	return head;

	/*ptr_type_t*			ptr;

	ptr = head;
	if( ptr )
	{
		while( ptr->down )
		{
			ptr = (ptr_type_t*)ptr->down;
			assert( ptr->code == TYPE_POINTER );
		}
			
		ptr->down = type;
		return head;
	}
	
	return type;*/
}

/*static bool
check_type_array_const(
	type_t*				type )
{
	if( type->code != TYPE_POINTER )
		return true;
	
	ptr_type_t*			ptrtype = (ptr_type_t*)type;
	
	if( ptrtype->is_array )
	{
		expr_t*			expr = ptrtype->array_expr;
	
		if( expr->code != EXPR_ICONST )
		{
			compile_error( &expr->filepos,
				ERR_EXPECTED_CONSTANT );
			return false;
		}
	}
	
	return check_type_array_const( ptrtype->down );
}*/

bool
finish_decl(
	decl_t*				decl,
	bool				resolve )
{
	if( decl->type )
		// called before
		return true;	
	
	if( !decl->spec )
		return false;

	type_t*				type;
	
	type = decl->spec->type;
	
	if( !type )
		return false;
	
	type = append_ptr_list(
		decl->prefix, type );
		
	type = append_ptr_list(
		decl->postfix, type );
		
	decl->postfix = 0;
			
	if( resolve && is_type_resolved( type ) == false )
	{
		type = resolve_type( type );
		if( !type )
		{
			compile_error( &decl->spec->filepos,
				ERR_INTERNAL );
			return false;
		}
	}

	decl->type = type;
	
	decl_func_t*		func = decl->func;
	
	if( func )
	{
		decl_t*			param = func->param;
	
		while( param )
		{
			if( finish_decl( param, resolve ) == false )
				return false;
			param = param->next;
		}
	}
	
	// note that finish_decl_init will be called when
	// all types have been resolved, specifically
	// inside inst_decl() in compfunc.cp
		
	return true;
}

void
decl_to_string(
	decl_t*				decl,
	istring&			str )
{
	str.clear();

	if( decl->spec )
	{
		if( decl->spec->modifiers & MOD_TYPEDEF )
			str.append( "typedef " );
		if( decl->spec->modifiers & MOD_STATIC )
			str.append( "static " );
		/*if( decl->spec->modifiers & MOD_CONST )
			str.append( "const " );*/
	}

	if( decl->type )
	{
		istring			tstr;
		type_to_string( decl->type, tstr );
		str.append( tstr );
		str.append( " " );
	}
	
	if( decl->name )
		str.append( decl->name );
	
	if( decl->tmpltype )
	{
		type_seq_t*		node = decl->tmpltype;
		istring			t;

		str.append( "<" );
		while( node )
		{
			type_to_string( node->type, t );
			str.append( t );
			node = node->next;
			if( node )
				str.append( "," );
		}
		str.append( ">" );
	}

	decl_func_t*		func = decl->func;
		
	if( func )
	{
		str.append( "(" );
		
		decl_t*			param = func->param;
		istring			pstr;
		
		while( param )
		{	
			decl_to_string( param, pstr );
			str.append( pstr );
			param = param->next;
			if( param )
				str.append( "," );
		}
		str.append( ")" );
	}
}

int
union_flags(
	int					flags,
	int					added )
{
	if( flags & added )
		compile_error( 0, ERR_SYNTAX_ERROR );

	return flags | added;
}

bool
are_params_equal(
	decl_t*				paramone,
	decl_t*				paramtwo )
{
	while( paramone && paramtwo )
	{
		if( !is_same_type( paramone->type, paramtwo->type ) )
			return false;
			
		paramone = paramone->next;
		paramtwo = paramtwo->next;
	}
	
	return !paramone && !paramtwo;
}

stmt_t*
parse_if_stmt(
	expr_t*				expr,
	stmt_t*				stmt_true,
	stmt_t*				stmt_false )
{
	return make_if_stmt( expr, stmt_true, stmt_false );
}

stmt_t*
parse_cond_stmt(
	e_stmt_code			code,
	expr_t*				expr,
	stmt_t*				stmt )
{
	return make_cond_stmt( code, expr, stmt );
}

stmt_t*
parse_expr_for_stmt(
	expr_t*				init,
	expr_t*				cond,
	expr_t*				iter,
	stmt_t*				stmt )
{	
	return make_for_stmt( init, cond, iter, stmt );
}

stmt_t*
parse_decl_for_stmt(
	decl_t*				init,
	expr_t*				cond,
	expr_t*				iter,
	stmt_t*				stmt )
{
	stmt_t*				node;
	
	node = make_decl_stmt( init );
	node->next = make_for_stmt( 0, cond, iter, stmt );

	return make_block_stmt( node );
}

void
parse_declaration(
	decl_t*				decl,
	int					amod,
	int					imod,
	bool				resolve )
{		
	for( ; decl; decl = decl->next )
	{
		if( decl->func )
		{
			// empty function declaration
			parse_function( decl, 0 );
			continue;
		}

		decl_spec_t*	spec = decl->spec;
	
		if( !spec )
		{
			compile_error( 0, ERR_INTERNAL );
			continue;
		}
			
		const int		modifiers = decl->spec->modifiers | amod;

		if( finish_decl( decl, resolve ) )
		{
			type_t*		type = decl->type;			
			const char*	name = decl->name;
			
			if( modifiers & imod )
			{
				compile_error( &spec->filepos, ERR_ILLEGAL_MODIFIER );
			}
			else if( modifiers & MOD_TYPEDEF )
			{
				if( !name )
					compile_error( &spec->filepos, ERR_SYNTAX_ERROR );
				else
				{
					if( is_type_name( name ) == false )
					{
						istring	 s( name );
						make_type_name( s );
					
						compile_error(
							&spec->filepos,
							ERR_ILLEGAL_TYPENAME, s.c_str(), name );
					}
				
					class_t*	clss = cur_scope()->clss;
				
					declare_type( name,	make_typedef_type(
						name, type, decl->tmpltype, clss ),
						&spec->filepos );
				}
			}
			else if( name && name[ 0 ] != '\0' )
			{
				if( is_type_name( name ) )
				{
					istring	 s( name );
					make_variable_name( s );
					
					compile_error(
						&spec->filepos,
						ERR_ILLEGAL_VARNAME, s.c_str(), name );
				}
			
				declare_variable( decl,
					modifiers, &spec->filepos );
			}
			else
			{
				compile_error( &decl->spec->filepos, ERR_SYNTAX_ERROR );
				return;
			}
		}
		else
		{
			compile_error( &spec->filepos, ERR_INTERNAL );
			return;
		}
	}
}

static bool
parse_function_params(
	decl_t*				decl )
{
	while( decl )
	{
		decl_spec_t*	spec = decl->spec;
	
		if( spec == 0 )
			continue;

		if( decl->func )
		{
			compile_error( &spec->filepos, ERR_SYNTAX_ERROR );
			return false;
		}
		else if( finish_decl( decl, false ) == false )
			return false;
			
		if( decl->spec->modifiers & MOD_TYPEDEF )
		{
			compile_error( &spec->filepos, ERR_ILLEGAL_MODIFIER );
			return false;
		}
		
		decl = decl->next;
	}

	return true;
}

void
parse_function(
	decl_t*				decl,
	stmt_t*				stmt )
{
	decl_spec_t*		spec = decl->spec;

	if( !spec || decl->next )
	{
		compile_error( &spec->filepos, ERR_SYNTAX_ERROR );
		return;
	}
	
	if( !finish_decl( decl, false ) )
		return;

	if( cur_scope()->flags & SCF_DLL )
		decl->spec->modifiers |= MOD_DLL | MOD_STATIC;

	const int			modifiers = decl->spec->modifiers;
	bool				invalidModifiers = false;

	if( modifiers & MOD_TYPEDEF )
		invalidModifiers = true;
	else if( modifiers & MOD_NATIVE )
	{
		if( modifiers & ( MOD_ABSTRACT | MOD_DLL ) )
			invalidModifiers = true;
		else if( stmt )
			invalidModifiers = true;
	}
	else if( modifiers & MOD_ABSTRACT )
	{
		if( modifiers & ( MOD_NATIVE | MOD_DLL ) )
			invalidModifiers = true;
		else if( stmt )
			invalidModifiers = true;
	}
	else if( modifiers & MOD_DLL )
	{
		if( modifiers & ( MOD_NATIVE | MOD_ABSTRACT ) )
			invalidModifiers = true;
		else if( stmt )
			invalidModifiers = true;
	}
	else if( !stmt )
	{
		compile_error( &spec->filepos, ERR_SYNTAX_ERROR );
		return;
	}

	if( invalidModifiers )
	{
		compile_error( &spec->filepos, ERR_ILLEGAL_MODIFIER );
		return;
	}
		
	decl_func_t*		func = decl->func;
	
	if( func == 0 )
	{	
		compile_error( 0, ERR_INTERNAL );
		return;
	}
		
	if( parse_function_params( func->param ) == false )
		return;
		
	declare_function( decl, stmt );
}

void
make_type_name(
	istring&			name )
{
	if( name.length() < 1 )
		return;
		
	name[ 0 ] = itoupper( name[ 0 ] );
}

void
make_variable_name(
	istring&			name )
{
	if( name.length() < 1 )
		return;
		
	name[ 0 ] = itolower( name[ 0 ] );
}

END_COMPILER_NAMESPACE
