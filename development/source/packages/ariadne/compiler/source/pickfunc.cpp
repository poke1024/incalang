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
//	pickfunc.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "pickfunc.h"
#include "scope_t.h"
#include "type_t.h"
#include "lexer.h"
#include "parser.h"
#include "expr_t.h"
#include "templates.h"

BEGIN_COMPILER_NAMESPACE

#define MAXPARAM		128

// ===========================================================================

istring					s_name;

const char*
user_func_name(
	const char*			name )
{
	if( name[ 0 ] == '#' )
	{
		s_name = "operator";
		s_name.append( name + 1 );
	}
	else
		s_name = name;
		
	return s_name.c_str();
}

static void
func_to_string(
	func_t*				func,
	istring&			text )
{
	text.assign( user_func_name( func->decl->name ) );
	text.append( "(" );

	decl_t*			param = func->param;
	istring			s;

	while( param )
	{
		s.clear();
		type_to_string( param->type, s );
		if( param->init )
			text.append( "[" );
		text.append( s );
		if( param->init )
			text.append( "]" );
		if( param->next )
			text.append( "," );
		param = param->next;
	}
	
	text.append( ")" );
}

static void
func_to_string(
	const char*			name,
	type_list_t*		typelist,
	istring&			text )
{
	text.assign( user_func_name( name ) );
	text.append( "(" );

	istring				s;

	while( typelist )
	{
		s.clear();
		type_to_string( typelist->type, s );
		text.append( s );
		if( typelist->next )
			text.append( "," );
		typelist = typelist->next;
	}
	
	text.append( ")" );
}

// ===========================================================================

static bool
filter_candidates(
	func_t**		list )
{
	func_t*			candidate = *list;
	func_t*			newcand = 0;

	while( candidate )
	{
		if( candidate->decl->spec->modifiers & MOD_TEMPLATE )
			;
		else if( newcand == 0 )
			newcand = candidate;
		else
			return false;
		candidate = candidate->comp;
	}

	if( newcand == 0 )
		return false;

	*list = newcand;
	return true;
}

// ===========================================================================

u64						s_fcid = 1;

static bool
pick_function_tolerant(
	scope_t*			scope,
	func_t*				func,
	type_list_t*		typelist,
	type_seq_t*			tmpltype,
	fileref_t*			filepos,
	func_t**			result )
{
	s_fcid += 1;

	static caster_t*	cast[ MAXPARAM ];

	// we're using the comp field of the funcs to keep track of
	// candidate functions (yes, that's sort of a hack here)

	func_t*				candidate = 0;
	long				candidate_distance = 1L << 30;
	int					candidate_count = 0;

	while( func )
	{
		decl_t*			param = func->param;
		type_list_t*	typenode = typelist;
		long			distance = 0;
		int				index = 0;
		int				castcount = 0;
	
		while( param && typenode )
		{
			type_t*		calltype = typenode->type;
			type_t*		paramtype = param->type;
			long		typedist;
		
			calltype = reduce_type( calltype );
			paramtype = reduce_type( paramtype );

			typedist = type_distance(
				calltype, paramtype );

			if( typedist < DISTANCE_MAX )
			{
				cast[ index ] = 0;
				goto match;
			}

			if( scope == 0 )
				goto next_function;
			
			caster_t*	caster;
		
			if( scope_find_caster( scope,
				calltype, paramtype,
				filepos, SCOPE_ALL, &caster ) == false )
			{
				goto next_function;
				//return false;
			}
			
			if( caster == 0 )
				goto next_function;
				
			cast[ index ] = caster;
			castcount++;

			typedist = DISTANCE_MAX - 1;

match:			
			distance += typedist;
			
			param = param->next;
			typenode = typenode->next;
			
			if( ++index >= MAXPARAM )
			{
				compile_error( 
					filepos, ERR_TOO_MANY_PARAMETERS );
				return false;
			}
		}
		
		if( index > 1 && castcount == index )
			goto next_function;
		
		while( param && param->init )
			param = param->next;
		
		if( !param && !typenode )
		{
			if( candidate )
			{
				if( distance == candidate_distance )
				{
					func->comp = candidate;
					candidate = func;
					candidate_count += 1;
				}
				else if( distance < candidate_distance )
				{
					func->comp = 0;
					candidate = func;
					candidate_distance = distance;
					candidate_count = 1;
				}
			}
			else
			{
				func->comp = 0;
				candidate = func;
				candidate_distance = distance;
				candidate_count = 1;
			}
			
			if( candidate == func )
			{
				typenode = typelist;
				index = 0;
				while( typenode )
				{
					typenode->cast = cast[ index ];
					typenode = typenode->next;
					index++;
				}
			}
		}			
	
next_function:
		func = func->next;
	}

	if( candidate_count > 1 )
	{
		if( filter_candidates( &candidate ) )
			candidate_count = 1;
	}

	if( candidate_count > 1 )
	{
		istring			text;
		istring			s;
				
		while( candidate )
		{
			if( text.length() )
				text.append( ", " );
			//text.append( "\n" );
			func_to_string( candidate, s );
			text.append( s );
			candidate = candidate->comp;
		}
	
		compile_error( filepos, ERR_FUNCTION_AMBIVALENT, text.c_str() );
		return false;
	}
	
	if( candidate &&
		( candidate->decl->spec->modifiers & MOD_TEMPLATE ) )
	{
		if( tmpltype == 0 )
			candidate = instantiate_template_func_implicit(
				candidate );
		else
			candidate = instantiate_template_func_explicit(
				candidate, tmpltype, filepos );
	}

	*result = candidate;

	return true;
}

func_t*
pick_function(
	scope_t*			scope,
	func_t*				func,
	type_list_t*		typelist,
	type_seq_t*			tmpltype,
	fileref_t*			filepos,
	int					flags )
{
	func_t*				result;

	if( pick_function_tolerant( scope,
		func, typelist, tmpltype, filepos, &result ) == false )
	{
		return 0;
	}
		
	if( ( result == 0 ) &&
		( flags & PICKFUNC_QUIET ) == 0 )
	{
		istring			text;
	
		func_to_string( func->decl->name, typelist, text );
		compile_error( filepos, ERR_FUNCTION_UNDEFINED, text.c_str() );
	}
		
	return result;
}

END_COMPILER_NAMESPACE
