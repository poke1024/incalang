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
//	templates.cpp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "templates.h"

#include "scope_t.h"
#include "type_t.h"
#include "expr_t.h"
#include "compiler_t.h"
#include "parser.h"
#include "xhtab_t.h"

#include "class_t.h"
#include "xalloc.h"
#include "resolve.h"

BEGIN_COMPILER_NAMESPACE

extern bool				g_template_added;

static void
replace_template_type(
	type_t*				type,
	type_t**			bind,
	tt_save_t*			save,
	int&				savecount )
{
	type = stripped_type( type );

	template_type_t*	node;

	if( ( node = template_type( type ) ) == 0 )
		return;

	tt_type_t*			tt;

	tt = (tt_type_t*)node;

	save[ savecount ].addr = node;
	memcpy( &save[ savecount ].data, node,
		type_record_size( node ) );
	savecount++;

	type_t*				temp;
	
	temp = bind[ node->index ];

	memcpy( type, temp,
		type_record_size( temp ) );
}

static type_t*
safe_copy_type_shallow(
	type_t*				type )
{
	int					size;
	void*				ptr;
	
	size = type_record_size( type );
	ptr = compiler_t::heap()->alloc( size );
	memcpy( ptr, type, size );
	
	return (type_t*)ptr;	
}

static type_t*
create_template_type(
	type_t*				type,
	type_t*				bind )
{
	while( bind->code == TYPE_REFERENCE )
		bind = ( (ref_type_t*)bind )->down;

	while( true )
	{
		switch( type->code )
		{
			case TYPE_POINTER: {
				ptr_type_t* node;
				if( bind->code != TYPE_POINTER )
					return 0;
				node = (ptr_type_t*)safe_copy_type_shallow( type );
				node->down = create_template_type(
					( (ptr_type_t*)type )->down,
					( (ptr_type_t*)bind )->down );
				return node;
			}

			case TYPE_REFERENCE: {
				ref_type_t* node;
				node = (ref_type_t*)safe_copy_type_shallow( type );
				node->down = create_template_type(
					( (ref_type_t*)type )->down,
					bind );
				return node;
			}

			case TYPE_TEMPLATE:
				return safe_copy_type_shallow( bind );

			default:
				compile_error( 0, ERR_INTERNAL );
				return type;
		}
	}

	compile_error( 0, ERR_INTERNAL );
	return type;
}

inline void
fix_template_type(
	type_t**			typep,
	type_t**			bind )
{
	type_t*				type = *typep;
	template_type_t*	core;

	if( ( core = template_type( type ) ) )
	{
		*typep = create_template_type(
			type, bind[ core->index ] );
	}
}

static decl_t*
template_param(
	decl_t*				param,
	type_t**			bind,
	xheap_t*			heap )
{
	if( param == 0 )
		return 0;

	decl_t*				node;

	node = (decl_t*)heap->alloc(
		sizeof( decl_t ) );
	*node = *param;

	fix_template_type( &node->type, bind );

	node->next = template_param(
		param->next, bind, heap );

	return node;
}

static type_t*
template_return_type(
	func_t*				func,
	type_t**			bind )
{
	decl_t*				param = func->param;
	type_t*				rtype = func->decl->type;
	long				bindindex = 0;

	rtype = stripped_type( rtype );

	if( template_type( rtype ) == 0 )
		return rtype;

	while( param )
	{
		type_t*			type = param->type;

		if( template_type( type ) )
		{
			if( stripped_type( type ) == rtype )
			{
				return create_template_type(
					rtype, bind[ bindindex ] );
			}

			bindindex++;
		}

		param = param->next;
	}

	compile_error( &func->decl->spec->filepos,
		ERR_ILLEGAL_TEMPLATE_RETURN_TYPE );
	return rtype;
}

static func_t*
template_func(
	func_t*				func,
	type_t**			bind,
	xheap_t*			heap,
	bool				free_rtype = false )
{
	func_t*				tempfunc;

	tempfunc = (func_t*)heap->alloc(
		sizeof( func_t ) );
	*tempfunc = *func;

	tempfunc->param = template_param(
		func->param, bind, heap );

	decl_t*				decl;

	decl = (decl_t*)heap->alloc(
		sizeof( decl_t ) );
	*decl = *func->decl;
	if( free_rtype )
		fix_template_type( &decl->type, bind );
	else
		decl->type = template_return_type(
			func, bind );
	tempfunc->decl = decl;

	return tempfunc;
}

static func_t*
find_template_function(
	func_t*				func,
	type_t**			bind,
	int					bindcount )
{
	functemp_map_t*		map;

	map = func->tmap;
	while( map )
	{
		type_t**		exbind = map->bind;
		bool			match = true;

		for( int j = 0; j < bindcount; j++ )
		{
			if( can_assign_type(
				bind[ j ], exbind[ j ] ) == false )
			{
				match = false;
				break;
			}
		}

		if( match )
			return map->func;

		map = map->next;
	}

	return 0;
}

static func_t*
create_template_function(
	func_t*				func,
	type_t**			bind,
	int					bindcount )
{
	xheap_t*			heap;
	functemp_map_t*		map;

	heap = compiler_t::heap();
	
	map = (functemp_map_t*)heap->alloc(
		sizeof( functemp_map_t ) );

	map->bind = (type_t**)heap->alloc(
		bindcount * sizeof( type_t* ) );
	memcpy( map->bind, bind,
		bindcount * sizeof( type_t* ) );
	map->code = 0;
	map->func = template_func(
		func, bind, heap );

	map->next = func->tmap;
	func->tmap = map;

	g_template_added = true;

	return map->func;
}

static bool
determine_template_params(
	func_t*				func,
	type_t**			bind,
	int					tpcount )
{
	memset( bind, 0, sizeof( type_t* ) * tpcount );

	decl_t*				param = func->param;

	while( param )
	{
		type_t*			type = param->type;
		template_type_t*	core;
		int				index;

		if( ( core = template_type( type ) ) )
		{
			index = core->index;
			bind[ index ] = core->bind;
		}
		param = param->next;
	}

	for( int i = 0; i < tpcount; i++ )
	{
		if( bind[ i ] == 0 )
		{
			scope_t*	scope = func->scope;
			xhtab_t*	table = scope->typedefs;
			template_type_t*	node;

			if( xhfirst( table ) )
			{
				for( int j = 0; j < i; j++ )
					xhnext( table );
			}
			node = (template_type_t*)
				( (typedef_t*)xhstuff( table ) )->type;

			compile_error( &func->decl->spec->filepos,
				ERR_UNDETERMINED_TEMPLATE_TYPE,
				node->name );
			return false;
		}
	}
	
	return true;
}

func_t*
instantiate_template_func_explicit(
	func_t*				func,
	type_seq_t*			tmpltype,
	fileref_t*			filepos )
{
	scope_t*			scope = func->scope;

	if( ( scope->flags & SCF_TEMPLATE ) == 0 )
	{
		compile_error( filepos,
			ERR_TEMPLATE_INSTANCE_FOR_NON_TEMPLATE_TYPE );
		return func;
	}

	static type_t*		bind[ MAXTPCOUNT ];

	int					tpcount = 0;
	type_seq_t*			node;
	
	for( node = tmpltype; node; node = node->next )
	{
		if( tpcount >= MAXTPCOUNT )
			break;
		bind[ tpcount ] = node->type;
		tpcount++;
	}

	if( tpcount != scope->typedefs->count )
	{
		compile_error( filepos,
			ERR_WRONG_TYPE_COUNT_FOR_TEMPLATE_INSTANCE );
		return func;
	}
	
	func_t*				exist;

	exist = find_template_function(
		func, bind, tpcount );
	if( exist )
		return exist;

	return create_template_function(
		func, bind, tpcount );
}

func_t*
instantiate_template_func_implicit(
	func_t*				func )
{
	scope_t*			scope = func->scope;

	if( ( scope->flags & SCF_TEMPLATE ) == 0 )
	{
		compile_error( 0, ERR_INTERNAL );
		return func;
	}

	static type_t*		bind[ MAXTPCOUNT ];

	int					tpcount = scope->typedefs->count;

	if( determine_template_params(
		func, bind, tpcount ) == false )
	{
		return func;
	}

	func_t*				exist;

	exist = find_template_function(
		func, bind, tpcount );
	if( exist )
		return exist;

	return create_template_function(
		func, bind, tpcount );
}

// ===========================================================================

static type_t*
find_template_class(
	class_t*			clss,
	type_t**			bind,
	int					bindcount )
{
	clsstemp_map_t*		map;

	map = clss->tmap;
	while( map )
	{
		type_t**		exbind = map->bind;
		bool			match = true;

		for( int j = 0; j < bindcount; j++ )
		{
			if( is_same_type(
				bind[ j ], exbind[ j ] ) == false )
			{
				match = false;
				break;
			}
		}

		if( match )
			return map->clss;

		map = map->next;
	}

	return 0;
}

static void
fix_template_resolve(
	resolve_t*			resolve,
	resolve_t**			linkresolve,
	type_t**			bind )
{
	resolve_t*			tempresolve;
	decl_t*				tempdecl;

	while( resolve )
	{
		tempresolve = (resolve_t*)xalloc(
			sizeof( resolve_t ) );
		*tempresolve = *resolve;

		tempdecl = (decl_t*)xalloc(
			sizeof( decl_t ) );
		*tempdecl = *resolve->decl;
		tempresolve->decl = tempdecl;

		fix_template_type(
			&tempresolve->type, bind );
		*linkresolve = tempresolve;
		linkresolve = &tempresolve->next;
		resolve = resolve->next;
	}

	*linkresolve = 0;
}

static const char*
template_class_name(
	const char*			name,
	type_t**			bind,
	int					bindcount )
{
	istring				s;

	s.append( name );
	for( int i = 0; i < bindcount; i++ )
	{
		s.append( ";" );
		type_to_string( bind[ i ], s );
	}

	char*				t;
	long				len;

	len = s.length();
	t = (char*)xalloc( len + 1 );
	strcpy( t, s.c_str() );

	return t;
}

static type_t*
template_type(
	class_t*			clss,
	type_t**			bind,
	int					bindcount,
	xheap_t*			heap )
{
	tt_class_t*			tempclss;

	tempclss = (tt_class_t*)xalloc(
		sizeof( tt_class_t ) );
	memcpy( tempclss, clss,
		sizeof( class_t ) );
	tempclss->tpscope = clss->scope->up;
	tempclss->tpbind = bind;

	tempclss->name = template_class_name(
		clss->name, bind, bindcount );

	scope_t*			scope = clss->scope;
	scope_t*			tempscope;

	tempscope = (scope_t*)xalloc(
		sizeof( scope_t ) );
	*tempscope = *scope;

	tempscope->clss = tempclss;
	tempclss->scope = tempscope;

	xhtab_t*			table;
	xhtab_t*			temptable;
	resolve_t*			resolve;
	resolve_t*			tempresolve;
	resolve_t**			linkresolve;

	table = scope->variables;
	temptable = tempscope->variables = xhcreate( 6 );
	if( xhfirst( table ) )
	{
		variable_t*		var;
		variable_t*		tempvar;
		const char*		name;

		do {
			var = (variable_t*)xhstuff( table );
			tempvar = (variable_t*)xalloc(
				sizeof( variable_t ) );
			*tempvar = *var;

			fix_template_type(
				&tempvar->decl->type, bind );
			name = tempvar->decl->name;
			xhadd( temptable, (const u08*)name,
				(long)strlen( name ), tempvar );
		} while( xhnext( table ) );
	}

	fix_template_resolve(
		scope->vuresolve,
		&tempscope->vuresolve, bind );
	fix_template_resolve(
		scope->vrresolve,
		&tempscope->vrresolve, bind );
	fix_template_resolve(
		scope->vpresolve,
		&tempscope->vpresolve, bind );

	table = scope->functions;
	temptable = tempscope->functions = xhcreate( 5 );
	if( xhfirst( table ) )
	{
		func_t*			func;
		func_t*			tempfunc;
		func_t**		tempsame = 0;
		const char*		name;

		do {
			func = (func_t*)xhstuff( table );
			
			do {
				tempfunc = template_func(
					func, bind, heap, true );
				tempfunc->scope = tempscope;

				name = func->decl->name;
				if( tempsame == 0 )
				{
					xhadd( temptable, (const u08*)name,
						(long)strlen( name ), tempfunc );
				}
				else
					*tempsame = tempfunc;
		
				tempsame = &tempfunc->same;
				func = func->same;
			} while( func );

			*tempsame = 0;

		} while( xhnext( table ) );
	}

	return make_class_type( tempclss );
}

static type_t*
create_template_type(
	class_t*			clss,
	type_t**			bind,
	int					bindcount,
	fileref_t*			filepos )
{
	xheap_t*			heap;
	clsstemp_map_t*		map;

	heap = compiler_t::heap();
	
	map = (clsstemp_map_t*)heap->alloc(
		sizeof( clsstemp_map_t ) );

	map->bind = (type_t**)heap->alloc(
		bindcount * sizeof( type_t* ) );
	memcpy( map->bind, bind,
		bindcount * sizeof( type_t* ) );

	map->clss = template_type(
		clss, map->bind, bindcount, heap );

	map->next = clss->tmap;
	clss->tmap = map;

	g_template_added = true;

	class_t*			tempclss;
	scope_t*			scope;

	tempclss = ( (class_type_t*)map->clss )->clss;
	scope = tempclss->scope;

	scope_resolve( scope );

	class_t*			parent;

	parent = scope->up->up->clss;
	tempclss->next = parent->child;
	parent->child = tempclss;
	scope->flags |= SCF_COMPILE | SCF_TTCLASS;

	return map->clss;
}

type_t*
instantiate_template_type(
	decl_t*				decl,
	type_seq_t*			tmpltype,
	fileref_t*			filepos )
{
	type_t*				type = decl->type;
	if( type->code != TYPE_CLASS )
	{
		compile_error( &decl->spec->filepos,
			ERR_TEMPLATE_INSTANCE_FOR_NON_TEMPLATE_TYPE );
		return false;
	}
	class_t*			clss;
	scope_t*			scope;

	clss = ( (class_type_t*)type )->clss;
	scope = clss->scope->up;

	if( scope == 0 || ( scope->flags & SCF_TEMPLATE ) == 0 )
	{
		compile_error( &decl->spec->filepos,
			ERR_TEMPLATE_INSTANCE_FOR_NON_TEMPLATE_TYPE );
		return false;
	}

	static type_t*		bind[ MAXTPCOUNT ];

	int					tpcount = 0;
	type_seq_t*			node;
	
	for( node = tmpltype; node; node = node->next )
	{
		if( tpcount >= MAXTPCOUNT )
			break;
		bind[ tpcount ] = node->type;
		tpcount++;
	}

	if( tpcount + 1 != scope->typedefs->count )
	{
		compile_error( filepos,
			ERR_WRONG_TYPE_COUNT_FOR_TEMPLATE_INSTANCE );
		return false;
	}

	type_t*				exist;

	exist = find_template_class(
		clss, bind, tpcount );
	if( exist )
		return exist;

	close_xalloc_frame();

	exist = create_template_type(
		clss, bind, tpcount, filepos );

	open_xalloc_frame();

	return exist;
}

// ===========================================================================

void
tt_replacer_t::replace(
	type_t**		bind,
	decl_t*			param )
{
	m_savecount = 0;

	while( param )
	{
		replace_template_type( param->type,
			bind, m_save, m_savecount );
		param = param->next;
	}
}

void
tt_replacer_t::replace(
	type_t**		bind,
	scope_t*		scope )
{
	m_savecount = 0;

	xhtab_t*		table = scope->typedefs;
	template_type_t*	tt;
	type_t**		typep;
	type_t*			type;

	if( xhfirst( table ) )
	{
		do
		{
			typep = &( (typedef_t*)
				xhstuff( table ) )->type;
			type = *typep;
			if( type->code != TYPE_TEMPLATE )
				continue;
			tt = (template_type_t*)type;

			/*m_save[ m_savecount ].addr = type;
			memcpy( &m_save[ m_savecount ].data, type,
				type_record_size( type ) );
			m_savecount++;
			*typep = bind[ tt->index ];*/

			replace_template_type( tt,
				bind, m_save, m_savecount );
		} while( xhnext( table ) );
	}
}

void
tt_replacer_t::restore()
{
	for( int i = 0; i < m_savecount; i++ )
	{
		memcpy( m_save[ i ].addr,
			&m_save[ i ].data,
			type_record_size( &m_save[ i ].data._0 ) );
	}
}

END_COMPILER_NAMESPACE
