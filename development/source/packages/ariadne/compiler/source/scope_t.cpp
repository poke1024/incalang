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
//	scope_t.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "scope_t.h"
#include "type_t.h"
#include "xhtab_t.h"
#include "xalloc.h"
#include "parser.h"
#include "expr_t.h"
#include "class_t.h"
#include "func_list_t.h"
#include "output.h"
#include "templates.h"

#include <cstring>
#include <cstdlib>

BEGIN_COMPILER_NAMESPACE

using namespace std;

// note. the current way of handling inheritance is a recursive one, we
// simply go down the scopes of the inherited classes (via the different
// class_find_inherited_something calls) to track what we're searching
// for. a more efficient way would be to initially set up single hash
// tables which have all the information from the base classes, so we
// can access the needed elements in constant time.

// note two. namespaces are currently implemented as scopes that
// implicitly declare all their variables and functions as static.
// not the best solution, probably.

#define ALIGNMENT		4

static int				s_scope_index = -1;
static int				s_scope_max = 0;
static scope_t**		s_scopes = 0;
static scope_t*			s_global_scope = 0;
static findtag_t		s_findtag = 1;

static void
make_resolve(
	decl_t*				decl,
	int					modifiers,
	int					access,
	resolve_t**			link,
	fileref_t*			filepos )
{
	resolve_t*			resolve;
		
	resolve = (resolve_t*)xalloc( sizeof( resolve_t ) );
		
	resolve->decl = decl;
	resolve->type = decl->type;
	resolve->modifiers = modifiers;
	resolve->access = access;
		
	resolve->filepos = *filepos;
		
	resolve->next = *link;
	*link = resolve;
}

void
init_scopes()
{
	s_scope_index = -1;
	s_global_scope = 0;
}

scope_t*
make_scope()
{
	scope_t*			scope = (scope_t*)xalloc( sizeof( scope_t ) );
	
	scope->up = 0;
	
	scope->offset = 0;
	scope->static_offset = 0;
	scope->clss = 0;
	scope->access = ACC_PRIVATE;
	scope->flags = 0;
	scope->findtag = 0;

	scope->variables = xhcreate( 6 );
	scope->typedefs = xhcreate( 5 );
	scope->functions = xhcreate( 6 );
	scope->funccount = 0;
	scope->enums = xhcreate( 3 );
	scope->casters = 0;
	
	scope->vrresolve = 0;
	scope->vuresolve = 0;
	scope->vpresolve = 0;
	
	scope->depcount = 0;
	scope->init_code = 0;

	scope->usings = 0;
	scope->urusings = 0;

	return scope;
}

void
scope_export_variables(
	scope_t*			scope )
{
#if EMIT_TRACEBACK_TABLES
	xhtab_t*			t = scope->variables;
	
	pos_t				oldpos;
	pos_t				newpos;

	out_tell_pos( &oldpos );
	out_word( 0 );
	out_word( 0 );
	
	if( xhfirst( t ) )
	{
		variable_t*		var;
		variable_t*		staticvars = 0;
		long			localcount = 0;
		long			staticcount = 0;
	
		do {
			var = (variable_t*)xhstuff( t );
		
			if( var->modifiers & MOD_STATIC )
			{
				var->help = staticvars;
				staticvars = var;
				staticcount++;
			}
			else
			{
				out_string( var->decl->name );
				output_type_string( var->decl->type, true );
				out_long( var->offset );
				localcount++;
			}
			
		} while( xhnext( t ) );
		
		var = staticvars;
		while( var )
		{
			out_string( var->decl->name );
			output_type_string( var->decl->type, true );
			out_long( var->offset );
			var = var->help;
		}
	
		out_tell_pos( &newpos );
		out_seek_pos( &oldpos );
		out_word( localcount );
		out_word( staticcount );
		out_seek_pos( &newpos );
	}
#else
	out_word( 0 );
#endif
}

inline bool
check_type_array_const(
	type_t*				type )
{
	if( type->code != TYPE_POINTER )
		return true;
	
	ptr_type_t*			ptrtype = (ptr_type_t*)type;
	
	if( ptrtype->is_array )
	{
		expr_t*			expr = ptrtype->array_expr;
	
		if( type_expr( expr ) == false )
			return false;
			
		expr = eval_expr( expr );
		
		ptrtype->array_expr = expr;
	
		intval_t		ival;
	
		if( get_expr_iconst( &expr, ival ) == false )
		{
			compile_error( &expr->filepos,
				ERR_EXPECTED_CONSTANT );
			return false;
		}
		
		ptrtype->array_expr = expr;
	}
	
	return check_type_array_const( ptrtype->down );
}

void
scope_enter_variable(
	scope_t*			scope,
	decl_t*				decl,
	int					modifiers,
	int					access,
	fileref_t*			filepos )
{
	type_t*				type = decl->type;

	if( check_type_array_const( type ) == false )
		return;

	const char*			name = decl->name;
	
	const long			namelen = strlen( name );
	xhtab_t*			table = scope->variables;

	if( xhfind( table,
		(const u08*)name, namelen ) )
	{
		compile_error( filepos, ERR_VARIABLE_REDEFINED, name );
		return;
	}
	
	variable_t*			var = (variable_t*)xalloc( sizeof( variable_t ) );
	
	decl->var = var;
	var->decl = decl;
	var->modifiers = modifiers;
	var->access = access;
	var->lifetime = 0;
	
	xhadd( table, (const u08*)name,
		namelen, var );
		
	if( table->count > 0xffff )
	{
		compile_error( filepos, ERR_TOO_MANY_VARIABLES );
		return;
	}
	
	const u32			type_size = get_type_size( type );
	
	if( modifiers & MOD_STATIC )
	{	
		while( !scope->clss )
			scope = scope->up;
	
		u32				offset = scope->static_offset;
		
		int				phase = offset & ( ALIGNMENT - 1 );
		if( phase )
			offset += ALIGNMENT - phase;
		
		var->offset = offset;
		scope->static_offset = offset + type_size;
	}
	else if( modifiers & MOD_PARAM )
	{
		u32				offset = scope->offset - type_size;
		
		int				phase = ( -offset ) & ( ALIGNMENT - 1 );
		if( phase )
			offset -= ALIGNMENT - phase;

		var->offset = offset;
		scope->offset = offset;
	}
	else
	{
		u32				offset = scope->offset;
		
		int				phase = offset & ( ALIGNMENT - 1 );
		if( phase )
			offset += ALIGNMENT - phase;
		
		var->offset = offset;
		scope->offset = offset + type_size;
	}

	var->scope = scope;
}

void
scope_enter_type(
	scope_t*			scope,
	const char*			name,
	type_t*				type,
	fileref_t*			filepos )
{
	const long			namelen = strlen( name );

	if( xhfind( scope->typedefs,
		(const u08*)name, namelen ) )
	{
		compile_error( filepos, ERR_TYPE_REDEFINED );
		return;
	}

	typedef_t*			typedata = (typedef_t*)xalloc( sizeof( typedef_t ) );
		
	typedata->name = name;
	typedata->type = type;
	
	xhadd( scope->typedefs, (const u08*)name,
		namelen, typedata );
}

static void
scope_declare_caster(
	scope_t*			scope,
	func_t*				func )
{
	decl_spec_t*		spec = func->decl->spec;
	type_t*				rettype = spec->type;

	if( rettype->code == TYPE_VOID )
	{
		compile_error( &spec->filepos,
			ERR_ILLEGAL_OPERATOR_DECLARATION );
		return;
	}
	
	decl_t*				param = func->param;

	if( param == 0 || param->next )
	{
		compile_error( &spec->filepos,
			ERR_ILLEGAL_OPERATOR_DECLARATION );
		return;
	}
	
	type_t*				paramtype = param->spec->type;
	caster_t*			caster;
	
	caster = (caster_t*)xalloc( sizeof( caster_t ) );
	
	caster->from = paramtype;
	caster->to = rettype;
	caster->func = func;

	caster->next = scope->casters;
	scope->casters = caster;
}

void
scope_declare_variable(
	scope_t*			scope,
	decl_t*				decl,
	int					modifiers,
	int					access,
	fileref_t*			filepos )
{
	type_seq_t*			tmpltype;

	if( ( tmpltype = decl->tmpltype ) != 0 )
	{
		decl->type = instantiate_template_type(
			decl, tmpltype, filepos );
	}

	if( scope->flags & SCF_NAMESPACE )
		modifiers |= MOD_STATIC;

	type_t*				type = decl->type;
	const bool			is_resolved = is_type_resolved( type );

	if( is_type_size_known( type ) )
	{
		scope_enter_variable( scope, decl,
			modifiers, access, filepos );
			
		if( !is_resolved )
			make_resolve( decl, modifiers,
				access, &scope->vpresolve, filepos );
	}
	else
	{
		resolve_t**		link;
	
		if( is_resolved )
			link = &scope->vrresolve;
		else
			link = &scope->vuresolve;
			
		make_resolve( decl, modifiers,
			access, link, filepos );
	}
}

void
scope_declare_type(
	scope_t*			scope,
	const char*			name,
	type_t*				type,
	fileref_t*			filepos )
{
	// all types initially declared here should be
	// unresolved. this is needed since we don't
	// the order of declaration take influence on
	// the way we bind types, like in the example
	// below. therefore all binding is done in the
	// resolving after everything has been parsed.
	
	// the same thing holds for variables with
	// types that are qualnames.
	
	//	typedef char* T;
	//	struct Y {
	// 		T a; 	// error: ÕTÕ refers to ::T
	//				// but when reevaluated is Y::T
	// 		typedef long T;
	// 		T b;
	// 	};

	scope_enter_type( scope, name, type, filepos );
}

func_t*
scope_declare_function(
	scope_t*			scope,
	decl_t*				decl,
	int					access,
	stmt_t*				stmt )
{
	const int			flags = scope->flags;

	if( flags & SCF_NAMESPACE )
		decl->spec->modifiers |= MOD_STATIC;
	else if( flags & SCF_TEMPLATE )
		decl->spec->modifiers |= MOD_TEMPLATE | MOD_STATIC;

	func_t*				func;
	
	func = (func_t*)xalloc( sizeof( func_t ) );
	func->scope = scope;
	func->decl = decl;
	func->access = access;
	func->stmt = stmt;
	func->same = 0;
	func->param = decl->func->param;
	func->tmap = 0;
	func->comp = 0;
	func->code = 0;

	const char*			name = decl->name;

	if( name[ 0 ] == '#' &&
		name[ 1 ] == 'c' &&
		name[ 2 ] == '\0' )
	{
		scope_declare_caster( scope, func );
	}

	xhtab_t*			table = scope->functions;
	const long			namelen = strlen( name );

	if( xhfind( table, (const u08*)name, namelen ) )
	{
		func->same = (func_t*)xhstuff( table );
		table->ipos->stuff = func;
	}
	else
	{
		xhadd( table, (const u08*)name,
			namelen, func );
	}
	
	scope->funccount += 1;
		
	return func;
}

void
scope_declare_enum(
	scope_t*			scope,
	const char*			name,
	enum_list_t*		list,
	fileref_t*			filepos )
{
	type_t*				type;

	if( name )
	{
		scope_t*		frame = scope;
		class_t*		clss = frame->clss;
		
		while( clss == 0 )
		{
			frame = frame->up;
			if( frame == 0 )
			{
				compile_error( 0, ERR_INTERNAL );
				return;
			}
			clss = frame->clss;
		}
	
		type = make_enum_type( name, scope->clss );
		scope_declare_type( scope, name, type, filepos );
	}
	else
		type = make_int_type( TYPE_INT, true );

	xhtab_t*			table = scope->enums;
	enum_node_t*		node = list->first;
	enum_t*				item;
	
	while( node )
	{
		item = (enum_t*)xalloc( sizeof( enum_t ) );
		item->type = type;
		item->node = node;
		
		const char*		itemname = node->name;
		long			itemnamelen = strlen( itemname );
		
		if( xhfind( table, (const u08*)itemname, itemnamelen ) )
		{
			compile_error( &node->expr->filepos,
				ERR_ENUM_CONSTANT_DEFINED_TWICE );
			return;
		}
		
		xhadd( table, (const u08*)itemname, itemnamelen, item );
		
		node = node->next;
	}
}

// ===========================================================================

// we use a look up scheme very similar to that of C++. here's
// an example (namespaces are treated like classes)

//	namespace M {
// 		class B { };
// 	}
// 	namespace N {
// 		class Y : public M::B {
// 			class X {
// 				int a[i];
// 			};
// 		};
// 	}

// The following scopes are searched for a declaration of i:
// 1) scope of class N::Y::X, before the use of i
// 2) scope of class N::Y, before the definition of N::Y::X
// 3) scope of N::YÕs base class M::B
// 4) scope of namespace N, before the definition of N::Y
// 5) global scope, before the definition of N

bool
scope_find_variable_recursive(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	findtag_t			findtag,
	variable_t**		outvar )
{
	if( scope->findtag == findtag )
		return false;

	scope->findtag = findtag;

	const long			namelen = strlen( name );

	bool				has_instance = true;
	
	variable_t*			var;

	while( scope )
	{
		xhtab_t*		table = scope->variables;

		if( xhfind( table, (const u08*)name, namelen ) )
		{
			var = (variable_t*)xhstuff( table );
		
			if( has_instance || ( var->modifiers & MOD_STATIC ) )
			{
				*outvar = var;
				return true;
			}
		}
		
		if( flags & SCOPE_USING )
		{
			using_t*	node = scope->usings;
			while( node )
			{
				if( scope_find_variable_recursive( node->scope,
					name, filepos, flags, findtag, outvar ) )
				{
					return true;
				}
				node = node->next;
			}
		}

		class_t*		clss = scope->clss;
		
		if( clss && ( flags & SCOPE_INHERITED ) )
		{
			if( !class_find_inherited_variable(
				clss, name, filepos, findtag, &var ) )
			{
				return false;
			}
			
			if( var )
			{
				*outvar = var;
				return true;
			}
			
			has_instance = false;
		}

		if( flags & SCOPE_EMBEDDING )
			scope = scope->up;
		else
			break;
	}
	
	*outvar = 0;
	return true;
}

bool
scope_find_variable(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	variable_t**		outvar )
{
	return scope_find_variable_recursive(
		scope, name, filepos, flags, s_findtag++, outvar );
}

bool
scope_find_type_recursive(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	findtag_t			findtag,
	typedef_t**			outtype )
{
	if( scope->findtag == findtag )
		return false;

	scope->findtag = findtag;

	const long			namelen = strlen( name );

	while( scope )
	{
		xhtab_t*		table = scope->typedefs;
	
		if( xhfind( table, (const u08*)name, namelen ) )
		{
			*outtype = (typedef_t*)xhstuff( table );
			return true;
		}

		if( flags & SCOPE_USING )
		{
			using_t*	node = scope->usings;
			while( node )
			{
				if( scope_find_type_recursive( node->scope,
					name, filepos, flags, findtag, outtype ) )
				{
					return true;
				}
				node = node->next;
			}
		}

		class_t*		clss = scope->clss;
		
		if( clss && ( flags & SCOPE_INHERITED ) )
		{
			typedef_t*	typedata;
		
			if( !class_find_inherited_type(
				clss, name, filepos, findtag, &typedata ) )
			{
				return false;
			}
				
			if( typedata )
			{
				*outtype = typedata;
				return true;
			}
		}
		
		if( flags & SCOPE_EMBEDDING )
			scope = scope->up;
		else
			break;
	}
			
	*outtype = 0;
	return true;
}

bool
scope_find_type(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	typedef_t**			outtype )
{
	return scope_find_type_recursive(
		scope, name, filepos, flags, s_findtag++, outtype );
}

bool
scope_find_functions_recursive(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	findtag_t			findtag,
	func_list_t*		outfunc )
{
	if( scope->findtag == findtag )
		return false;

	scope->findtag = findtag;

	// note that for inherited functions, only the function of the
	// deepest class overriding the function will be included in
	// the returned list. this is necessary to implement correct
	// tolerant function picking (see pickfunc.cp)

	const long			namelen = strlen( name );
	
	while( scope )
	{
		xhtab_t*		table = scope->functions;
	
		if( xhfind( table, (const u08*)name, namelen ) )
		{
			func_t*		func;

			func = (func_t*)xhstuff( table );

			do {
				func_list_add( outfunc, func );
				func = func->same;
			} while( func );
		}

		if( flags & SCOPE_USING )
		{
			using_t*	node = scope->usings;
			while( node )
			{
				if( scope_find_functions_recursive( node->scope,
					name, filepos, flags, findtag, outfunc ) )
				{
					return true;
				}
				node = node->next;
			}
		}

		class_t*		clss = scope->clss;

		if( clss && ( flags & SCOPE_INHERITED ) )
		{
			if( !class_find_inherited_funcs(
				clss, name, filepos, findtag, outfunc ) )
			{
				return false;
			}
		}
		
		if( flags & SCOPE_EMBEDDING )
			scope = scope->up;
		else
			break;
	}
	
	return true;
}

bool
scope_find_functions(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	func_list_t*		outfunc )
{
	return scope_find_functions_recursive(
		scope, name, filepos, flags, s_findtag++, outfunc );
}

bool
scope_find_enum_recursive(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	findtag_t			findtag,
	enum_t**			outenum )
{
	if( scope->findtag == findtag )
		return false;

	scope->findtag = findtag;

	const long			namelen = strlen( name );

	enum_t*				node;

	while( scope )
	{
		xhtab_t*		table = scope->enums;

		if( xhfind( table, (const u08*)name, namelen ) )
		{
			node = (enum_t*)xhstuff( table );
			*outenum = node;
			return true;
		}

		if( flags & SCOPE_USING )
		{
			using_t*	node = scope->usings;
			while( node )
			{
				if( scope_find_enum_recursive( node->scope,
					name, filepos, flags, findtag, outenum ) )
				{
					return true;
				}
				node = node->next;
			}
		}

		class_t*		clss = scope->clss;
		
		if( clss && ( flags & SCOPE_INHERITED ) )
		{
			if( !class_find_inherited_enum(
				clss, name, filepos, findtag, &node ) )
			{
				return false;
			}
			
			if( node )
			{
				*outenum = node;
				return true;
			}
		}

		if( flags & SCOPE_EMBEDDING )
			scope = scope->up;
		else
			break;
	}
	
	*outenum = 0;
	return true;
}

bool
scope_find_enum(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	enum_t**			outenum )
{
	return scope_find_enum_recursive(
		scope, name, filepos, flags, s_findtag++, outenum );
}

// ===========================================================================

bool
scope_find_caster(
	scope_t*			scope,
	type_t*				fromtype,
	type_t*				totype,
	fileref_t*			/*filepos*/,
	int					flags,
	caster_t**			outcaster )
{
	while( scope )
	{
		caster_t*		node = scope->casters;

		while( node )
		{
			if( can_assign_type( fromtype, node->from ) &&
				can_assign_type( node->to, totype ) )
			{
				*outcaster = node;
				return true;
			}
			node = node->next;
		}
				
		if( flags & SCOPE_EMBEDDING )
			scope = scope->up;
		else
			break;
	}
			
	*outcaster = 0;
	return false;
}

unsigned long
get_scope_size(
	scope_t*			scope )
{
	if( scope->vuresolve == 0 && scope->vrresolve == 0 )
	{
		size_t			size = scope->offset;
		
		if( size & ( ALIGNMENT - 1 ) )
			size += ALIGNMENT - ( size & ( ALIGNMENT - 1 ) );
	
		return size;
	}
		
	compile_error( 0, ERR_INCOMPLETE_TYPE );

	return 0;
}

unsigned long
get_static_scope_size(
	scope_t*			scope )
{
	size_t			size = scope->static_offset;
		
	if( size & ( ALIGNMENT - 1 ) )
		size += ALIGNMENT - ( size & ( ALIGNMENT - 1 ) );

	return size;
}

bool
is_scope_size_known(
	scope_t*			scope )
{
	return scope->vuresolve == 0 && scope->vrresolve == 0;
}

qualname_t*
make_qualname(
	const char*			name )
{
	qualname_t*			qualname;
	
	qualname = (qualname_t*)xalloc( sizeof( qualname_t ) );
	
	qualname->name = name;
	qualname->down = 0;
	qualname->scope = 0;
	
	get_filepos( &qualname->filepos );
	
	return qualname;
}

qualhead_t*
make_qualhead(
	qualname_t*			name )
{
	qualhead_t*			qualhead;
	
	qualhead = (qualhead_t*)xalloc( sizeof( qualhead_t ) );
	
	qualhead->name = name;
	qualhead->next = 0;
	qualhead->type = 0;
	get_filepos( &qualhead->filepos );
	
	return qualhead;
}

static scope_t*
scope_resolve_qualname(
	qualname_t**		qualnamep )
{
	qualname_t*			qualname = *qualnamep;
	scope_t*			scope = qualname->scope;

	typedef_t*			typedata;
	
	while( qualname->down )
	{	
		if( !scope_find_type( scope, qualname->name,
			&qualname->filepos, SCOPE_ALL, &typedata ) )
		{
			return 0;
		}
			
		if( !typedata )
		{
			istring	name;
			qualname_to_string( qualname, name );
			compile_error( &qualname->filepos,
				ERR_UNDECLARED_QUALNAME, qualname->name );
			return 0;
		}
			
		type_t*			type;
		
		type = reduce_type( typedata->type );
		if( type->code != TYPE_CLASS )
		{
			compile_error( &qualname->filepos, ERR_EXPECTED_CLASS );
			return 0;
		}
			
		scope = ( (class_type_t*)type )->clss->scope;
		
		qualname = qualname->down;
	}
	
	*qualnamep = qualname;
	
	return scope;
}

type_t*
scope_find_qualname_type(
	qualname_t*			qualname )
{
	scope_t*			scope;

	scope = scope_resolve_qualname( &qualname );

	if( !scope )
		return 0;

	typedef_t*			typedata;

	if( !scope_find_type( scope, qualname->name,
		&qualname->filepos, SCOPE_ALL, &typedata ) )
	{
		return 0;
	}

	if( !typedata )
	{
		istring			name;
		qualname_to_string( qualname, name );
		compile_error( &qualname->filepos,
			ERR_UNDECLARED_TYPE, name.c_str() );
		return 0;
	}
		
	return typedata->type;
}

int
scope_find_qualname_identifier(
	qualname_t*			qualname,
	identifier_t*		identifier )
{
	scope_t*			myscope = qualname->scope;

	scope_t*			scope;

	scope = scope_resolve_qualname( &qualname );

	if( !scope )
		return 0;

	variable_t*			var;

	if( !scope_find_variable( scope, qualname->name,
		&qualname->filepos, SCOPE_ALL, &var ) )
	{
		return 0;
	}

	if( !var )
	{
		enum_t*			enumerator;
	
		if( !scope_find_enum( scope, qualname->name,
			&qualname->filepos, SCOPE_ALL, &enumerator ) )
		{
			return 0;
		}
		
		if( enumerator )
		{
			identifier->enumerator = enumerator;
			return identifier_enumerator;
		}
	
		istring			name;
		
		qualname_to_string( qualname, name );
	
		compile_error( &qualname->filepos,
			ERR_UNDECLARED_VARIABLE, name.c_str() );
		return 0;
	}
	
	if( myscope != scope && !( var->modifiers & MOD_STATIC ) )
	{
		class_t*		clss = myscope->clss;
		class_t*		baseclss = scope->clss;
		
		bool			is_inherited = false;
		
		if( clss && baseclss )
			switch( is_base_class( clss, baseclss ) )
			{
				case NOT_FOUND:
					break;
			
				case FOUND:
					is_inherited = true;
					break;
					
				case AMBIVALENT:
					compile_error( &qualname->filepos,
						ERR_AMBIGOUS_VARIABLE );
					return 0;
					
				default:
					return 0;
			}
	
		if( !is_inherited )
		{
			compile_error( &qualname->filepos,
				ERR_VARIABLE_NOT_STATIC );
			return 0;
		}
	}
	
	identifier->variable = var;
	
	return identifier_variable;
}

static class_t*
get_scope_class(
	scope_t*			scope )
{
	class_t*			clss;
	
	while( scope )
	{
		clss = scope->clss;
		if( clss )
			return clss;
		scope = scope->up;
	}
	
	return 0;
}

static void
remove_non_static_functions(
	func_t**			link )
{
	func_t*				func = *link;

	while( func )
	{
		if( !( func->decl->spec->modifiers & MOD_STATIC ) )
		{
			func_t*		next = func->next;
			*link = next;
			func = next;
		}
		else
		{
			link = &func->next;
			func = *link;
		}
	}
}

func_t*
scope_find_qualname_functions(
	qualname_t*			qualname )
{
	scope_t*			myscope = qualname->scope;

	scope_t*			scope;

	scope = scope_resolve_qualname( &qualname );

	if( !scope )
		return 0;

	func_list_t			list;

	func_list_init( &list );

	if( !scope_find_functions( scope, qualname->name,
		&qualname->filepos, SCOPE_ALL, &list ) )
	{
		return 0;
	}
	
	func_t*				func = func_list_head( &list );

	if( !func )
	{
		istring			name;
		
		qualname_to_string(	qualname, name );
	
		compile_error( &qualname->filepos,
			ERR_FUNCTION_UNDEFINED, name.c_str() );
		return 0;
	}
	
	if( myscope != scope )
	{
		class_t*		clss = get_scope_class( myscope );
		class_t*		baseclss = get_scope_class( scope );
		
		bool			is_inherited = false;
		
		if( clss && baseclss )
			switch( is_base_class( clss, baseclss ) )
			{
				case NOT_FOUND:
					break;
			
				case FOUND:
					is_inherited = true;
					break;
					
				case AMBIVALENT:
					compile_error( &qualname->filepos,
						ERR_FUNCTION_AMBIVALENT );
					return 0;
					
				default:
					return 0;
			}
	
		if( !is_inherited )
		{
			remove_non_static_functions( &func );
		
			if( !func )
			{
				compile_error( &qualname->filepos,
					ERR_FUNCTION_NOT_STATIC );
				return 0;
			}
		}
	}
		
	return func;
}

void
scope_make_namespace(
	scope_t*			scope )
{
	scope->access = ACC_PUBLIC;
	scope->flags |= SCF_NAMESPACE;
}

void
scope_make_template(
	scope_t*			scope,
	typename_list_t*	list )
{
	scope->access = ACC_PUBLIC;
	scope->flags |= SCF_TEMPLATE;

	fileref_t			filepos;
	get_filepos( &filepos );

	type_t*				type;
	int					index = 0;

	while( list )
	{
		if( index >= MAXTPCOUNT )
		{
			compile_error( 0,
				ERR_TOO_MANY_TEMPLATE_PARAMETERS );
			break;
		}

		type = make_template_type(
			list->name, index );
		scope_declare_type( scope,
			list->name, type, &filepos );
		list = list->next;
		index++;
	}
}

void
scope_make_dll(
	scope_t*			scope )
{
	scope->access = ACC_PUBLIC;
	scope->flags |= SCF_DLL;
}

void
scope_make_global(
	scope_t*			scope )
{
	s_global_scope = scope;
}

void
scope_add_unresolved_using(
	scope_t*			scope,
	qualname_t*			name,
	fileref_t*			filepos )
{
	unresolved_using_t*	node;

	node = (unresolved_using_t*)xalloc(
		sizeof( unresolved_using_t ) );
	node->name = name;
	node->filepos = *filepos;

	node->next = scope->urusings;
	scope->urusings = node;
}

void
scope_add_using(
	scope_t*			scope,
	scope_t*			what )
{
	using_t*			node;
	using_t**			link;

	link = &scope->usings;
	node = *link;
	while( node )
	{
		if( node->scope == what )
			return;
		link = &node->next;
		node = *link;
	}

	node = (using_t*)xalloc( sizeof( using_t ) );
	node->scope = what;
	node->next = 0;
	*link = node;
}

// ---------------------------------------------------------------------------

void
scope_to_string(
	scope_t*			scope,
	istring&			name )
{
	while( scope )
	{
		class_t*		clss = scope->clss;

		if( !clss )
			break;

		if( name.length() && clss->name[ 0 ] != '\0' )
			name.insert( 0, "::" );

		name.insert( 0, clss->name );

		scope = scope->up;
	}
	
	if( name.length() < 1 )
		name = "global space";
}

void
qualname_to_string(
	qualname_t*			qualname,
	istring&			name )
{
	while( qualname )
	{
		name.append( qualname->name );
		qualname = qualname->down;
		if( qualname )
			name.append( "::" );
	}
}

// ---------------------------------------------------------------------------

scope_t*
push_scope()
{
	int					index = s_scope_index;

	if( !s_scopes )
	{
		s_scope_max = 32;
		s_scopes = (scope_t**)imalloc(
			s_scope_max * sizeof( scope_t* ) );
	}

	if( ++index >= s_scope_max )
	{
		s_scope_max *= 2;
		s_scopes = (scope_t**)irealloc(
			s_scopes, s_scope_max * sizeof( scope_t* ) );
	}
	
	s_scope_index = index;

	scope_t*			scope = make_scope();

	if( index > 0 )
		scope->up = s_scopes[ index - 1 ];

	s_scopes[ index ] = scope;
	
	return scope;
}

void
pop_scope()
{
	int					index = s_scope_index;
	
	if( --index < 0 )
	{
		compile_error( 0, ERR_INTERNAL );
		return;
	}

	s_scope_index = index;
}

void
undo_pop_scope()
{
	s_scope_index++;
}

scope_t*
cur_scope()
{
	return s_scopes[ s_scope_index ];
}

scope_t*
global_scope()
{
	return s_global_scope;
}

void
push_global_scope()
{
	push_scope();
	s_global_scope = cur_scope();
}

void
declare_scope_access(
	int					access )
{
	s_scopes[ s_scope_index ]->access = access;
}

void
declare_variable(
	decl_t*				decl,
	int					modifiers,
	fileref_t*			filepos )
{
	scope_t*			scope;
	
	scope = s_scopes[ s_scope_index ];

	scope_declare_variable(
		scope, decl,
		modifiers, scope->access, filepos );
}

void
declare_type(
	const char*			name,
	type_t*				type,
	fileref_t*			filepos )
{
	scope_declare_type(
		s_scopes[ s_scope_index ], name, type, filepos );
}

void
declare_function(
	decl_t*				decl,
	stmt_t*				stmt )
{
	scope_t*			scope;
	
	scope = s_scopes[ s_scope_index ];

	scope_declare_function(
		scope, decl, scope->access, stmt );
}

void
declare_enum(
	const char*			name,
	enum_list_t*		list,
	fileref_t*			filepos )
{
	scope_declare_enum(
		s_scopes[ s_scope_index ], name, list, filepos );
}

END_COMPILER_NAMESPACE
