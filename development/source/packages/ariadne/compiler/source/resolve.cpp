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
//	resolve.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "scope_t.h"
#include "type_t.h"
#include "xalloc.h"
#include "parser.h"
#include "xhtab_t.h"
#include "class_t.h"
#include "func_list_t.h"
#include "pickfunc.h"

#include "expr_t.h"
#include "output.h"

#include <iostream>

BEGIN_COMPILER_NAMESPACE

using namespace std;

static bool				s_constants_resolved = false;

void
init_resolve()
{
	s_constants_resolved = false;
}

bool
constants_resolved()
{
	return s_constants_resolved;
}

static type_t*
resolve_ptr_type(
	ptr_type_t*			type )
{
	type_t*				down;
			
	down = resolve_type( type->down );
			
	if( !down )
		return 0;
				
	type->down = down;
			
	return type;
}

static type_t*
resolve_ref_type(
	ref_type_t*			type )
{
	type_t*				down;
			
	down = resolve_type( type->down );
			
	if( !down )
		return 0;
				
	type->down = down;
			
	return type;
}

static type_t*
resolve_unresolved_type(
	unresolved_type_t*	type )
{
	type_t*				rtype;

	if( ( rtype = scope_find_qualname_type(
		type->qualname ) ) == 0 )
	{
		return 0;
	}

	int					quals = type->quals;
	
	if( ( quals & rtype->quals ) != quals )
	{
		rtype = copy_type_shallow( rtype );
		rtype->quals |= quals;
	}
	
	return rtype;
}

static type_t*
resolve_typedef_type(
	typedef_type_t*		type )
{
	type_t*				resolved;
	
	resolved = resolve_type( type->type );
	
	if( !resolved )
		return 0;
	
	type->type = resolved;

	//return resolved;
	return type;
}

type_t*
resolve_type(
	type_t*				type )
{
	bool				done = false;

	do {
		switch( type->code )
		{
			case TYPE_POINTER:
				type = resolve_ptr_type(
					(ptr_type_t*)type );
				done = true;
				break;
		
			case TYPE_REFERENCE:
				type = resolve_ref_type(
					(ref_type_t*)type );
				done = true;
				break;
		
			case TYPE_UNRESOLVED:
				type = resolve_unresolved_type(
					(unresolved_type_t*)type );
				break;
		
			case TYPE_TYPEDEF:
				type = resolve_typedef_type(
					(typedef_type_t*)type );
				done = true;
				break;
					
			default:
				done = true;
				break;
		}
	} while( type && !done );
	
	return type;
}

// ---------------------------------------------------------------------------

#define RESCURSIVE_DESCEND(INSTRUMENT)								\
	class_t*			child = scope->clss->child;					\
																	\
	while( child )													\
	{																\
		if( !INSTRUMENT( child->scope ) )							\
			success = false;										\
																	\
		child = child->next;										\
	}

// step #1.
// in this step we annotate each constant declaration
// initialization expression like "const int a = 3" with
// its types, so we can later at any time call eval_expr
// we cannot look at the constants inside functions at
// this moment. we do this as first step since we need
// the constants for pointer type expressions like the
// one in int a[ kSomeConstant] in the following steps.
// actually these array constants are checked when we
// enter a variable with such type in a scope via
// scope_enter_variable.

extern bool				s_init_declaration;

static bool
scope_type_constants(
	scope_t*			scope )
{
	// take a look at each constant declared in here

	xhtab_t*			t = scope->variables;

	bool				success = true;
	
	if( xhfirst( t ) )
	{
		variable_t*		var;
		decl_init_t*	init;
		expr_t*			expr;

		do {
			var = (variable_t*)xhstuff( t );
			init = var->decl->init;
			
			if( init == 0 )
				continue;

			if( is_type_constant( var->decl->type ) )
				continue;

			expr = init->expr;
			if( expr == 0 )
				continue;
				
			s_init_declaration = true;
			if( type_expr( expr ) == false )
				return false;
			s_init_declaration = false;

		} while( xhnext( t ) );
	}
	
	// check contained classes
	
	RESCURSIVE_DESCEND( scope_type_constants );

	return success;
}

// ---------------------------------------------------------------------------

// step #2.
// resolve all unresolved using namespace directives.

enum {
	RESOLVE_INCOMPLETE	= 1,
	RESOLVE_PROGRESS	= 2,
	RESOLVE_ERROR		= 4
};

static int
scope_incremental_resolve_using(
	scope_t*			scope )
{
	int					flags = 0;

	unresolved_using_t**	link;
	unresolved_using_t*	node;
	
	link = &scope->urusings;
	node = *link;

	while( node )
	{
		type_t*			type;
		scope_t*		what;

		type = scope_find_qualname_type(
			node->name );
		if( type )
		{
			if( type->code != TYPE_CLASS )
			{
				compile_error( &node->filepos,
					ERR_EXPECTED_NAMESPACE );
				return RESOLVE_ERROR;
			}

			what = ( (class_type_t*)type )->clss->scope;
			if( ( what->flags & SCF_NAMESPACE ) == 0 )
			{
				compile_error( &node->filepos,
					ERR_EXPECTED_NAMESPACE );
				return RESOLVE_ERROR;
			}

			*link = node->next;

			scope_add_using( scope, what );

			flags |= RESOLVE_PROGRESS;
		}
		else
		{
			link = &node->next;
			flags |= RESOLVE_INCOMPLETE;
		}

		node = *link;
	}

	class_t*			child = scope->clss->child;	

	while( child )
	{
		flags |= scope_incremental_resolve_using(
			child->scope );
		if( flags & RESOLVE_ERROR )
			return RESOLVE_ERROR;
		child = child->next;
	}
	
	return flags;
}

static bool
scope_resolve_all_using(
	scope_t*			scope )
{
	int					flags;

	while( true )
	{
		flags = scope_incremental_resolve_using( scope );
		if( flags & RESOLVE_ERROR )
			return false;
		if( ( flags & RESOLVE_INCOMPLETE ) == 0 )
			break;
		if( ( flags & RESOLVE_PROGRESS ) == 0 )
		{
			compile_error( 0, ERR_USING_NAMESPACE_CYCLE );
			return false;
		}
	}

	return true;
}

// ---------------------------------------------------------------------------

// step #3.
// completing a class depends on having all the class's variables and
// all the class's base classes complete. we prepared for the first
// condition in the last step. now take care of binding all inherited
// incomplete classes to each base class.

// helper functions and structures for the first resolution step

static bool
prepare_classes(
	scope_t*			scope )
{
	bool				success = true;
	
	class_t*			clss = scope->clss;
	
	clss->tmp1.basecount = 0;
	clss->tmp1.traversing = false;
	clss->tmp1.complete = false;

	RESCURSIVE_DESCEND( prepare_classes );

	return success;
}

static bool
visit_class(
	class_t*		clss );

static bool
resolve_qualname(
	qualhead_t*		qualhead )
{
	qualname_t*		qualname = qualhead->name;

	while( true )
	{
		if( !visit_class( qualname->scope->clss ) )
			return false;
	
		typedef_t*	typedata;
	
		if( !scope_find_type( qualname->scope, qualname->name,
			&qualname->filepos, SCOPE_ALL, &typedata ) )
		{
			return false;
		}
		
		if( !typedata )
		{
			istring	name;
			qualname_to_string( qualhead->name, name );
			compile_error( &qualhead->filepos,
				ERR_UNDECLARED_TYPE, name.c_str() );
			return false;
		}
		
		type_t*		comptype = typedata->type;
		type_t*		type = reduce_type( comptype );
		
		if( type->code == TYPE_UNRESOLVED )
		{
			qualname_t*	typequalname;

			typequalname = ( (unresolved_type_t*)type )->qualname;
					
			while( typequalname->down )
				typequalname = typequalname->down;
				
			typequalname->down = qualname->down;
			
			qualname = typequalname;
		}
		else
		{
			if( type->code != TYPE_CLASS )
			{
				compile_error( &qualhead->filepos, ERR_EXPECTED_CLASS );
				return false;
			}
			
			qualname = qualname->down;
			
			if( !qualname )
			{
				qualhead->type = comptype;
				return true;
			}
			else
			{
				qualname->scope = ( (class_type_t*)type )->clss->scope;
			}
		}
	}
}

static bool
visit_class(
	class_t*		clss )
{		
	if( clss->tmp1.complete )
		return true;

	if( clss->tmp1.traversing )
	{
		compile_error( &clss->filepos,
			ERR_CYCLE_IN_BASECLASS_GRAPH );
		return false;
	}
		
	clss->tmp1.traversing = true;

	if( clss->tmp1.basecount != clss->basecount )
	{
		for( qualhead_t* qualhead = clss->base;
			qualhead; qualhead = qualhead->next )
		{
			if( !resolve_qualname( qualhead ) )
				return false;
		}
		
		clss->tmp1.basecount = clss->basecount;
	}

	clss->tmp1.complete = true;
	
	class_t*		child = clss->child;

	while( child )
	{
		if( !visit_class( child->scope->clss ) )
			return false;

		child = child->next;
	}
		
	return true;
}

static bool
clear_dependency_fields(
	scope_t*			scope )
{
	class_t*			clss = scope->clss;
	bool				success = true;

	clss->tmp2.dependclss = 0;
	clss->tmp2.dependvars = 0;

	RESCURSIVE_DESCEND( clear_dependency_fields );
	
	return success;
}

static bool
establish_dependencies(
	scope_t*			scope )
{
	bool				success = true;
	
	class_t*			clss = scope->clss;

	for( qualhead_t* qualhead = clss->base;
		qualhead; qualhead = qualhead->next )
	{
		type_t*			type = qualhead->type;
	
		class_t*		baseclss =  ( (class_type_t*)type )->clss;

		class_list_t*	dependclss;

		dependclss = (class_list_t*)xalloc( sizeof( class_list_t ) );
		dependclss->clss = clss;
		dependclss->next = baseclss->tmp2.dependclss;
		baseclss->tmp2.dependclss = dependclss;
	}
	
	scope->depcount = clss->basecount;
	
	RESCURSIVE_DESCEND( establish_dependencies );
	
	return success;
}

static bool
scope_resolve_base_classes(
	scope_t*			scope )
{
	prepare_classes( scope );
	
	if( !visit_class( scope->clss ) )
		return false;

	clear_dependency_fields( scope );

	establish_dependencies( scope );

	return true;
}

// ---------------------------------------------------------------------------


// step #4.
// resolve all unresolved types by traversing all hash tables in the
// scopes and resolving all types. issue an error if type is not
// resolvable. note that we don't check if the type is resolved
// when entering it into the scope.
// this step is also used to determine if classes are abstract,
// i.e. if there are any abstract functions that are not overridden.

static bool
scope_resolve_all_types(
	scope_t*			scope )
{
	if( scope->flags & SCF_TEMPLATE )
		return true;

	xhtab_t*			t = scope->typedefs;

	bool				success = true;
	
	if( xhfirst( t ) )
	{
		do {
			typedef_t*	typedata = (typedef_t*)xhstuff( t );
			type_t*		type = typedata->type;
			
			type = resolve_type( type );
		
			if( !type )
			{
				error_undeclared_type( typedata->type );
				success = false;
			}
			else
				typedata->type = type;
		
		} while( xhnext( t ) );
	}

	class_t*			clss = scope->clss;
		
	RESCURSIVE_DESCEND( scope_resolve_all_types );
	
	return success;
}

// step #5.
// resolve all unresolved types in variable declarations. after this step
// has been completed for all scopes, all variables have resolved types,
// albeit some of these types may include classes not yet complete.

static bool
scope_resolve_all_variables(
	scope_t*			scope )
{
	resolve_t*			resolve = scope->vuresolve;	
	resolve_t**			rlink = &scope->vrresolve;
	
	bool				success = true;
	
	while( resolve )
	{
		resolve_t*		next = resolve->next;
	
		type_t*			type;
		
		type = resolve_type( resolve->type );
	
		if( !type )
		{
			error_undeclared_type( resolve->type );
			success = false;
		}
		else
		{
			resolve->type = type;
			resolve->next = *rlink;
			*rlink = resolve;
		}
		
		resolve = next;
	}
	
	scope->vuresolve = 0;
	
	caster_t*			caster = scope->casters;
	
	while( caster )
	{
		type_t*			type;
	
		type = resolve_type( caster->from );
		if( !type )
		{
			error_undeclared_type( caster->from );
			return false;
		}
		caster->from = type;

		type = resolve_type( caster->to );
		if( !type )
		{
			error_undeclared_type( caster->to );
			return false;
		}
		caster->to = type;

		func_t*			func = caster->func;
		istring			name;
		istring			retname;
	
		out_open_string( retname );
		output_type_id( caster->to, false );
		out_close_string();
		
		name.assign( func->decl->name );
		name.append( ";" );
		name.append( retname );
		func->decl->name = xstrdup( name.c_str() );		

		caster = caster->next;
	}
	
	RESCURSIVE_DESCEND( scope_resolve_all_variables );
	
	return success;
}

// step #6.
// all variables which were unresolved originally should now be able
// to be entered into class scope. the only exception are variables
// which are of type class, with a class not yet complete. bind
// these variables to the class in question.

inline class_t*
dependent_class(
	type_t*				type )
{
	switch( type->code )
	{
		case TYPE_CLASS:
			return ( (class_type_t*)type )->clss;
			
		case TYPE_POINTER:
			if( ( (ptr_type_t*)type )->is_array == false )
				return 0;
		
			return dependent_class(
				( (ptr_type_t*)type )->down );
	}
	
	return 0;
}

static bool
scope_bind_incomplete_variables(
	scope_t*			scope )
{
	if( scope->flags & SCF_TEMPLATE )
		return true;

	resolve_t*			resolve = scope->vrresolve;
	
	unsigned long		dependvarcount = 0;
	
	bool				success = true;
	
	while( resolve )
	{
		resolve_t*		next = resolve->next;
			
		type_t*			type = resolve->type;
		class_t*		clss;
		
		type = reduce_type( type );

		if( ( clss = dependent_class( type ) ) != 0 )
		{
			resolve->scope = scope;
			resolve->next = clss->tmp2.dependvars;
			clss->tmp2.dependvars = resolve;
			
			dependvarcount++;
		}
		else
		{
			resolve->decl->type = type;
		
			scope_enter_variable( scope, resolve->decl,
				resolve->modifiers, resolve->access,
				&resolve->filepos );
		}
		
		resolve = next;
	}
	
	scope->vrresolve = 0;
	
	scope->depcount += dependvarcount;
	
	RESCURSIVE_DESCEND( scope_bind_incomplete_variables );
	
	return success;
}

// step #7.
// complete all classes. since we call scope_complete_class recursively
// as soon as another class is complete, this should complete all
// classes within one step.

static bool
scope_complete_class(
	scope_t*			scope );

static bool
class_complete_dependent_variables(
	class_t*			clss );

static bool
class_complete_dependent_classes(
	class_t*			clss );
	
static void
class_compute_size(
	class_t*			clss );
	
static void
class_setup_constructor_destructor(
	class_t*			clss );
	
static bool
scope_complete_all_classes(
	scope_t*			scope )
{
	if( scope->flags & SCF_TEMPLATE )
		return true;

	bool				success = true;

	if( !scope->depcount )
		if( !scope_complete_class( scope ) )
			success = false;
	
	RESCURSIVE_DESCEND( scope_complete_all_classes );
	
	return success;
}

static bool
class_complete_friends(
	class_t*			clss )
{
	qualhead_t*			name = clss->friendclss;

	while( name )
	{
		if( resolve_qualname( name ) == false )
			return false;
			
		if( name->type->code != TYPE_CLASS )
		{
			compile_error( &name->filepos, ERR_TYPE_MISMATCH );
			return true;
		}
			
		name = name->next;
	}
	
	return true;
}

static bool
scope_complete_class(
	scope_t*			scope )
{
	class_t*			clss = scope->clss;

	class_compute_size( clss );
	
	class_setup_constructor_destructor( clss );

	class_complete_dependent_variables( clss );

	class_complete_dependent_classes( clss );
	
	class_complete_friends( clss );
		
	return true;
}

static bool
class_complete_dependent_variables(
	class_t*			clss )
{
	resolve_t*			resolve = clss->tmp2.dependvars;
	
	if( !resolve )
		return false;
	
	while( resolve )
	{
		scope_t*		scope = resolve->scope;
		
		// resolve->type should be that of a class of type clss
		// or of an fixed-size array of classes of type clss
			
		resolve->decl->type = resolve->type;
			
		scope_enter_variable( scope, resolve->decl,
			resolve->modifiers,
			resolve->access, &resolve->filepos );

		if( --scope->depcount == 0 )
			scope_complete_class( scope );
		
		resolve = resolve->next;
	}
	
	clss->tmp2.dependvars = 0;
	
	return true;
}

static bool
class_complete_dependent_classes(
	class_t*			clss )
{
	class_list_t*		node = clss->tmp2.dependclss;
	
	if( !node )
		return false;

	//clss->inherclss = node;
	
	while( node )
	{
		scope_t*		scope = node->clss->scope;
	
		if( --scope->depcount == 0 )
			scope_complete_class( scope );
	
		node = node->next;
	}
	
	clss->tmp2.dependclss = 0;
	
	return true;
}

static void
class_compute_size(
	class_t*			clss )
{
	scope_t*			scope = clss->scope;

	unsigned long		size;

	size = get_scope_size( scope );
	
	clss->partial_size = size;
	
	//clss->static_size = get_static_scope_size( scope );
	
	qualhead_t*			qualhead = clss->base;
	
	while( qualhead )
	{
		type_t*			type = qualhead->type;
		
		size += ( (class_type_t*)type )->clss->size;

		qualhead = qualhead->next;
	}
	
	clss->size = size;
}

enum {
	F_CONSTRUCTOR 		= 1,
	F_DESTRUCTOR		= 2,
	F_DONE				= 3
};

static func_t*
declare_cd_function(
	const char*			name,
	scope_t*			scope )
{
	decl_spec_t*		spec;
	
	spec = (decl_spec_t*)xalloc( sizeof( decl_spec_t ) );
	spec->type = make_type( TYPE_VOID );
	spec->modifiers = 0;
	spec->filepos.id = fileid_invalid;
	spec->filepos.pos = 0;

	decl_t*				decl;
	
	decl = (decl_t*)xalloc( sizeof( decl_t ) );
	
	decl->name = name;
	decl->next = 0;
	decl->spec = spec;
	decl->prefix = 0;
	decl->postfix = 0;
	decl->type = make_type( TYPE_VOID );
	decl->init = 0;
	decl->func = make_decl_func( 0 );

	stmt_t*				stmt;

	stmt = make_stmt( STMT_NOP );

	return scope_declare_function(
		scope, decl, ACC_PUBLIC, stmt );
}

static void
class_setup_constructor_destructor(
	class_t*			clss )
{
	scope_t*			scope = clss->scope;

	func_list_t			list;
	func_t*				func;

	func_list_init( &list );
	if( !scope_find_functions( scope, s_constructor_name,
		&clss->filepos, 0, &list ) )
		return;
	func = func_list_head( &list );
	clss->cd_func[ CONSTRUCTOR ] = ( func == 0 ) ? 0 :
		pick_function( 0, func, 0, 0,
			&clss->filepos, PICKFUNC_QUIET );
		
	func_list_init( &list );
	if( !scope_find_functions( scope, s_destructor_name,
		&clss->filepos, 0, &list ) )
		return;
	func = func_list_head( &list );
	clss->cd_func[ DESTRUCTOR ] = ( func == 0 ) ? 0 :
		pick_function( 0, func, 0, 0,
			&clss->filepos, PICKFUNC_QUIET );

	int					flags = 0;

	if( clss->cd_func[ CONSTRUCTOR ] )
		flags |= F_CONSTRUCTOR;
	
	if( clss->cd_func[ DESTRUCTOR ] )
		flags |= F_DESTRUCTOR;

	if( flags == F_DONE )
		return;

	qualhead_t*			qualhead = clss->base;
	
	while( qualhead && flags != F_DONE )
	{
		type_t*			type = qualhead->type;
		class_t*		clss = ( (class_type_t*)type )->clss;
		
		if( clss->cd_func[ CONSTRUCTOR ] )
			flags |= F_CONSTRUCTOR;
			
		if( clss->cd_func[ DESTRUCTOR ] )
			flags |= F_DESTRUCTOR;
		
		qualhead = qualhead->next;
	}

	if( flags != F_DONE )
	{
		xhtab_t*		t = scope->variables;

		variable_t*		var;
		type_t*			type;
		class_t*		clss;
	
		if( xhfirst( t ) )
		{

			do {
				var = (variable_t*)xhstuff( t );
				type = reduce_type( var->decl->type );
				if( type->code == TYPE_CLASS )
				{				
					clss = ( (class_type_t*)type )->clss;
					
					// we always need a constructor for a class that
					// has a class variable, no matter if the variable's
					// class has a constructor or not, since we have to
					// set up the rtinfo (class_header_t in machine)
					
					flags |= F_CONSTRUCTOR;

					if( clss->cd_func[ DESTRUCTOR ] )
						flags |= F_DESTRUCTOR;
				}
			} while( xhnext( t ) && flags != F_DONE );
		}
	}
	
	if( ( flags & F_CONSTRUCTOR ) && !clss->cd_func[ CONSTRUCTOR ] )
	{
		clss->cd_func[ CONSTRUCTOR ] = declare_cd_function(
			s_constructor_name, scope );
	}

	if( ( flags & F_DESTRUCTOR ) && !clss->cd_func[ DESTRUCTOR ] )
	{
		clss->cd_func[ DESTRUCTOR ] = declare_cd_function(
			s_destructor_name, scope );
	}
}

// step #8.
// check that really all classes are complete. if not, issue an error.
// also check, if any class has member variables of a type of a class
// that is abstract, making the class in question inself invalid.

static bool
scope_check_if_class_complete(
	scope_t*			scope )
{
	if( scope->flags & SCF_TEMPLATE )
		return true;

	bool				success = true;

	// check that class is complete

	if( scope->depcount )
	{
		compile_error( &scope->clss->filepos,
			ERR_INCOMPLETE_TYPE );
		success = false;
	}
	
	// check contained classes
	
	RESCURSIVE_DESCEND( scope_check_if_class_complete );

	return success;
}

static bool
scope_incremental_resolve(
	scope_t*			scope )
{
	if( !scope_type_constants( scope ) )
		return false;

	s_constants_resolved = true;

	if( !scope_resolve_all_using( scope ) )
		return false;

	if( !scope_resolve_base_classes( scope ) )
		return false;

	if( !scope_resolve_all_types( scope ) )
		return false;

	if( !scope_resolve_all_variables( scope ) )
		return false;

	if( !scope_bind_incomplete_variables( scope ) )
		return false;
		
	if( !scope_complete_all_classes( scope ) )
		return false;

	if( !scope_check_if_class_complete( scope ) )
		return false;
		
	return true;
}

// ---------------------------------------------------------------------------

static bool
resolve_decl(
	decl_t*				decl )
{
	type_t*				type = decl->type;
	
	if( !is_type_resolved( type ) )
	{
		type = resolve_type( type );
		if( !type )
		{
			error_undeclared_type( decl->type );
			return false;
		}
		else
			decl->type = type;
	}
	
	return true;
}

static bool
resolve_param_init(
	type_t*				type,
	decl_init_t*		init )
{
	expr_t*				expr = init->expr;
	
	if( expr == 0 )
	{
		compile_error( &init->filepos, ERR_SYNTAX_ERROR );
		return false;
	}
	
	if( type_expr( expr ) == false )
		return false;
		
	expr = eval_expr( expr );
	init->expr = expr;
	
	if( can_assign_type( expr->type, type ) == false )
	{
		compile_error( &init->filepos, ERR_ILLEGAL_TYPE );
		return false;
	}
		
	return true;
}

inline bool
finish_resolve_function(
	func_t*				func )
{
	decl_t*				decl;
		
	decl = func->decl;
	if( !resolve_decl( decl ) )
		return false;
		
	decl = func->param;
	while( decl )
	{
		if( !resolve_decl( decl ) )
			return false;
			
		decl_init_t*	init = decl->init;
		
		if( init && resolve_param_init( decl->type, init ) == false )
			return false;
			
		decl = decl->next;
	}
	
	return true;
}

static bool
scope_finish_resolve_all_functions(
	scope_t*			scope )
{
	xhtab_t*			table = scope->functions;
	
	if( !xhfirst( table ) )
		return true;
		
	do
	{
		func_t*			func = (func_t*)xhstuff( table );
	
		do {
			if( !finish_resolve_function( func ) )
				return false;
			func = func->same;
		} while( func );	
			
	} while( xhnext( table ) );
	
	return true;
}

inline bool
scope_check_function(
	scope_t*			scope,
	func_t*				func )
{
	func_list_t			list;

	func_list_init( &list );

	if( !scope_find_functions( scope, func->decl->name,
		&func->decl->spec->filepos, 0, &list ) )
	{
		return false;
	}

	func_t*				node = func_list_head( &list );
	
	while( node )
	{	
		if( node != func )
		{
			if( are_params_equal(
				node->param, func->param ) )
			{
				istring	scope_name;
				
				scope_to_string( func->scope, scope_name );
			
				compile_error( &func->decl->spec->filepos,
					ERR_FUNCTION_REDEFINED,
					func->decl->name,
					scope_name.c_str() );
				return false;
			}
		}
		
		node = node->next;
	}
	
	return true;

}

static bool
scope_finish_check_all_functions(
	scope_t*			scope )
{
	xhtab_t*			table = scope->functions;
	
	if( !xhfirst( table ) )
		return true;
		
	do
	{
		func_t*			func = (func_t*)xhstuff( table );
	
		do {
			if( !scope_check_function( scope, func ) )
				return false;
			func = func->same;
		} while( func );	

	} while( xhnext( table ) );
	
	return true;
}	

static bool
scope_finish_resolve(
	scope_t*			scope )
{
	bool				success = true;
	
	resolve_t*			resolve = scope->vpresolve;

	if( scope->flags & SCF_TEMPLATE )
		resolve = 0;

	while( resolve )
	{
		type_t*			type;
		
		type = resolve_type( resolve->type );
	
		if( !type )
		{
			error_undeclared_type( resolve->type );
			success = false;
		}

		resolve = resolve->next;
	}

	if( !scope_finish_resolve_all_functions( scope ) )
		success = false;

	if( !scope_finish_check_all_functions( scope ) )
		success = false;
		
	RESCURSIVE_DESCEND( scope_finish_resolve )
	
	return success;
}

// ---------------------------------------------------------------------------

// step #1.
// mark abstract classes.

static bool
scope_mark_abstract_classes(
	scope_t*			scope )
{
	//if( scope->flags & SCF_TEMPLATE )
	//	return true;

	bool				success = true;

	class_t*			clss = scope->clss;

	if( clss )
		clss->is_abstract = is_class_abstract( scope, clss );
		
	RESCURSIVE_DESCEND( scope_mark_abstract_classes );
	
	return success;
}

// step #2.
// check for abstract member variables in classes.

static bool
scope_check_abstract_members(
	scope_t*			scope )
{
	if( scope->flags & SCF_TEMPLATE )
		return true;

	bool				success = true;

	xhtab_t*			t = scope->variables;

	if( xhfirst( t ) )
	{
		do {
			variable_t*	var = (variable_t*)xhstuff( t );
			
			if( is_type_abstract( var->decl->type ) )
			{
				compile_error( &scope->clss->filepos,
					ERR_ABSTRACT_VARIABLE );
				success = false;
				break;
			}	
		} while( xhnext( t ) );
	}

	RESCURSIVE_DESCEND( scope_check_abstract_members );

	return success;
}

static bool
scope_abstract_resolve(
	scope_t*			scope )
{
	if( !scope_mark_abstract_classes( scope ) )
		return false;

	if( !scope_check_abstract_members( scope ) )
		return false;
	
	return true;
}

// ---------------------------------------------------------------------------

bool
scope_resolve(
	scope_t*			scope )
{
	if( !scope_incremental_resolve( scope ) )
		return false;
			
	if( !scope_finish_resolve( scope ) )
		return false;
	
	if( !scope_abstract_resolve( scope ) )
		return false;
		
	return true;
}

END_COMPILER_NAMESPACE
