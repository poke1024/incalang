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
//	class_t.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "class_t.h"
#include "scope_t.h"
#include "type_t.h"
#include "xalloc.h"
#include "xhtab_t.h"
#include "output.h"
#include "parser.h"
#include "func_list_t.h"
#include "templates.h"
#include <cstring>

BEGIN_COMPILER_NAMESPACE

using namespace std;

static class_t*			s_cur_clss = 0;

#if ARIADNE_STRING_OBJECTS
class_t*				s_string_class = 0;
#endif

static u32				s_template_name = 0;

class_spec_t*
make_class_spec(
	const char*			name,
	int					access )
{
	class_spec_t*		spec;
	
	spec = (class_spec_t*)xalloc(
		sizeof( class_spec_t ) );
	spec->name = name;
	spec->access = access;
	
	return spec;
}

void
init_class()
{
	s_cur_clss = 0;
	s_string_class = 0;
	s_template_name = 1;
}

const char*
template_class_name()
{
	static char			buf[ 32 ];
	sprintf( buf, "#t%ld", (long)s_template_name );
	s_template_name++;
	return buf;
}

static class_t*
make_class(
	const char*			name,
	qualhead_t*			base )
{
	class_t*			clss;
	
	clss = (class_t*)xalloc( sizeof( class_t ) );

	clss->name = name;
	clss->base = base;
	clss->scope = 0;
	
	clss->size = 0;
	clss->partial_size = 0;
	//clss->static_size = 0;
	
	clss->cd_func[ CONSTRUCTOR ] = 0;
	clss->cd_func[ DESTRUCTOR ] = 0;
	clss->is_abstract = true;
	clss->friendclss = 0;
	
	clss->child = 0;
	clss->next = 0;
	
	clss->tmap = 0;

	clss->funcstat = 0;
	clss->comp = 0;
	
	long				basecount = 0;
	
	while( base )
	{
		basecount++;
		base = base->next;
	}
	
	clss->basecount = basecount;
	
	return clss;
}

class_t*
begin_class(
	class_spec_t*		spec,
	qualhead_t*			base )
{
	class_t*			clss;
	const char*			name = spec->name;
	
	clss = make_class( name, base );

#if ARIADNE_STRING_OBJECTS
	if( s_string_class == 0 &&
		strcmp( name, s_string_name ) == 0 )
	{
		s_string_class = clss;
	}
#endif

	class_t*			cur_clss = s_cur_clss;

	if( cur_clss )
	{
		clss->next = cur_clss->child;
		cur_clss->child = clss;
	}

	clss->embedding = cur_clss;
	s_cur_clss = clss;
				
	get_filepos( &clss->filepos );

	class_t*			embedding = cur_clss;

	if( embedding )
	{
		type_t*			type;
	
		type = make_class_type( clss );

		scope_enter_type( clss->embedding->scope,
			spec->name, type, &clss->filepos );
	}
	
	scope_t*			scope;
	
	scope = push_scope();
	
	scope->offset = CLASS_HEADER_SIZE;

	scope->access = spec->access;
	
	clss->scope = scope;
	scope->clss = clss;
	
	if( cur_clss )
		scope->up = cur_clss->scope;
				
	return clss;
}

void
end_class()
{
	class_t*			clss = s_cur_clss;
		
	pop_scope();
		
	s_cur_clss = clss->embedding;
}

void
add_class_friend(
	qualname_t*			name )
{
	class_t*			clss = s_cur_clss;
	
	if( clss == 0 )
		compile_error( 0, ERR_SYNTAX_ERROR );

	qualhead_t*			qualhead;

	qualhead = make_qualhead( name );

	qualhead->next = clss->friendclss;
	clss->friendclss = qualhead;
}

bool
is_class_friend(
	class_t*			clss,
	class_t*			friendclss )
{
	qualhead_t*			qualhead = clss->friendclss;
	
	while( qualhead )
	{
		type_t*			type = qualhead->type;

		if( !type )
		{
			compile_error( 0, ERR_INTERNAL );
			return false;
		}
		
		if( ( (class_type_t*)type )->clss == friendclss )
			return true;
				
		qualhead = qualhead->next;
	}
	
	return false;
}

bool
class_find_inherited_variable(
	class_t*			clss,
	const char*			name,
	fileref_t*			filepos,
	findtag_t			findtag,
	variable_t**		outvar )
{
	variable_t*			var = 0;
	variable_t*			varfound = 0;

	qualhead_t*			qualhead = clss->base;
	
	while( qualhead )
	{
		type_t*			type = qualhead->type;

		if( !type )
		{
			compile_error( 0, ERR_INTERNAL );
			return false;
		}
		
		if( !scope_find_variable_recursive(
			( (class_type_t*)type )->clss->scope,
			name, filepos, SCOPE_INHERITED, findtag, &var ) )
		{
			return false;
		}

		if( var )
		{
			if( varfound )
			{
				compile_error( filepos, ERR_AMBIGOUS_VARIABLE );
				return false;
			}
		
			varfound = var;
		}

		qualhead = qualhead->next;
	}
	
	*outvar = varfound;
	
	return true;
}

bool
class_find_inherited_type(
	class_t*			clss,
	const char*			name,
	fileref_t*			filepos,
	findtag_t			findtag,
	typedef_t**			outtype )
{
	typedef_t*			typedata = 0;
	typedef_t*			typedatafound = 0;

	qualhead_t*			qualhead = clss->base;
	
	while( qualhead )
	{
		type_t*			type = qualhead->type;

		if( !type )
		{		
			// we would end up here if during the
			// scope_resolve_base_classes step of
			// resolving, a base class is queried,
			// which has not yet been processed. 
			
			// this should not happen, since we
			// process the classes in a special order.
			
			compile_error( 0, ERR_INTERNAL );
			return false;
		}
				
		if( !scope_find_type_recursive(
			( (class_type_t*)type )->clss->scope,
			name, filepos, SCOPE_INHERITED, findtag, &typedata ) )
		{
			return false;
		}

		if( typedata )
		{
			if( typedatafound )
			{
				compile_error( filepos, ERR_AMBIGOUS_TYPE );
				return false;
			}
		
			typedatafound = typedata;
		}

		qualhead = qualhead->next;
	}
	
	*outtype = typedatafound;
	return true;
}

bool
class_find_inherited_funcs(
	class_t*			clss,
	const char*			name,
	fileref_t*			filepos,
	findtag_t			findtag,
	func_list_t*		outfunc )
{
	qualhead_t*			qualhead = clss->base;
	
	while( qualhead )
	{
		type_t*			type = qualhead->type;

		if( !type )
		{
			compile_error( 0, ERR_INTERNAL );
			return false;
		}
		
		if( !scope_find_functions_recursive(
			( (class_type_t*)type )->clss->scope,
			name, filepos, SCOPE_INHERITED, findtag, outfunc ) )
		{
			return false;
		}

		qualhead = qualhead->next;
	}
	
	return true;
}


bool
class_find_inherited_enum(
	class_t*			clss,
	const char*			name,
	fileref_t*			filepos,
	findtag_t			findtag,
	enum_t**			outenum )
{
	enum_t*				node = 0;
	enum_t*				nodefound = 0;

	qualhead_t*			qualhead = clss->base;
	
	while( qualhead )
	{
		type_t*			type = qualhead->type;

		if( !type )
		{
			compile_error( 0, ERR_INTERNAL );
			return false;
		}
		
		if( !scope_find_enum_recursive(
			( (class_type_t*)type )->clss->scope,
			name, filepos, SCOPE_INHERITED, findtag, &node ) )
		{
			return false;
		}

		if( node )
		{
			if( nodefound )
			{
				compile_error( filepos, ERR_AMBIGOUS_ENUM );
				return false;
			}
		
			nodefound = node;
		}

		qualhead = qualhead->next;
	}
	
	*outenum = nodefound;
	
	return true;
}

unsigned long
get_class_size(
	class_t*			clss )
{
	unsigned long		size;
	
	if( ( size = clss->size ) == 0 )
		compile_error( 0, ERR_INTERNAL );

	return size;
}

bool
is_class_size_known(
	class_t*			clss )
{
	return clss->size != 0;
}

static bool
is_function_defined(
	scope_t*			instancescope,
	func_t*				func )
{
	func_list_t			list;
				
	func_list_init( &list );

	if( !scope_find_functions( instancescope,
		func->decl->name, &func->decl->spec->filepos,
		SCOPE_INHERITED, &list ) )
	{
		return false;
	}

	func_t*				node = func_list_head( &list );
	
	while( node )
	{
		if( ( node->decl->spec->modifiers & MOD_ABSTRACT ) == 0 )
		{
			if( are_params_equal( func->param, node->param ) )
			{
				switch( is_base_class( node->scope->clss,
					func->scope->clss ) )
				{
					case ERROR:
						return false;
					
					case FOUND:
					case AMBIVALENT:
						return true;
				}
			}
		}
			
		node = node->next;
	}				

	return false;
}

bool
is_class_abstract(
	scope_t*			instancescope,
	class_t*			clss )
{
	// first check if all base classes are instantiable
	
	qualhead_t*			qualhead = clss->base;
	
	while( qualhead )
	{
		type_t*			type = qualhead->type;

		if( !type )
		{
			compile_error( 0, ERR_INTERNAL );
			return true;
		}
		
		if( is_class_abstract(
			instancescope, ( (class_type_t*)type )->clss ) )
		{
			return true;
		}

		qualhead = qualhead->next;
	}

	// now check that all abstract functions from this class
	// are defined in class deriving this class

	xhtab_t*			table = clss->scope->functions;
		
	if( xhfirst( table ) )
	{
		func_t*			func;
	
		xhtab_iterator_t	it;
	
		do
		{
			func = (func_t*)xhstuff( table );
			
			xhgetiterator( table, &it );
			
			do
			{
				if( func->decl->spec->modifiers & MOD_ABSTRACT )
				{
					if( !is_function_defined( instancescope, func ) )
					{
						return true;
					}
				}
				func = func->same;
			} while( func );
		
			xhsetiterator( table, &it );
			
		} while( xhnext( table ) );
	}

	return false;
}

void
output_class_id(
	class_t*			clss )
{
	unsigned long		count = 0;
		
	clss->tmp3.help = 0;
		
	while( true )
	{
		count++;
		
		class_t*		up;
		
		up = clss->embedding;
		if( up )
		{
			up->tmp3.help = clss;
			clss = up;
		}
		else
			break;
	}

	if( count > 0xffff )
	{
		compile_error(
			&clss->filepos,
			ERR_INTERNAL );
		return;
	}
	
	count -= 1;
	clss = clss->tmp3.help;
	
	out_word( count );

	while( clss )
	{
		out_string( clss->name );
		clss = clss->tmp3.help;
	}
}

void
output_func_id(
	func_t*				func )
{	
	pos_t				old_pos;
	
	out_tell_pos( &old_pos );
	out_word( 0 );
	
	const long			zero_offset = out_tell();
	
	const char*			name = func->decl->name;

	out_block( (u08*)name, strlen( name ) );
	
	decl_t*				param = func->param;

	while( param )
	{
		type_t*		type = param->type;
	
		if( !type )
		{
			compile_error( 0, ERR_INTERNAL );
			return;
		}
	
		out_byte( ';' );
				
		output_type_id( type );
		
		param = param->next;
	}
	
	const long			len = out_tell() - zero_offset;
	pos_t				new_pos;
	
	out_tell_pos( &new_pos );
	out_seek_pos( &old_pos );
	out_word( len );
	out_seek_pos( &new_pos );
	
}

found_t
is_base_class(
	class_t*			clss,
	class_t*			baseclss )
{
	if( baseclss == clss )
		return FOUND;

	qualhead_t*			qualhead = clss->base;
	bool				wasfound = false;
	
	while( qualhead )
	{
		type_t*			type = qualhead->type;
		
		if( !type )
		{
			compile_error( 0, ERR_INTERNAL );
			return ERROR;
		}
		
		switch( is_base_class(
			( (class_type_t*)type )->clss,
			baseclss ) )
		{
			case ERROR:
				return ERROR;

			case AMBIVALENT:
				return AMBIVALENT;

			case FOUND:
				if( wasfound )
					return AMBIVALENT;
				wasfound = true;
				break;				
		}
	
		qualhead = qualhead->next;
	}
	
	return wasfound ? FOUND : NOT_FOUND;
}

found_t
get_base_class_offset(
	class_t*			clss,
	class_t*			baseclss,
	unsigned long*		outoffset )
{
	if( baseclss == clss )
	{
		*outoffset = 0;
		return FOUND;
	}
		
	unsigned long		offset = clss->partial_size;

	qualhead_t*			qualhead = clss->base;
	bool				wasfound = false;
	
	while( qualhead )
	{
		type_t*			type = qualhead->type;
		
		if( !type )
		{
			compile_error( 0, ERR_INTERNAL );
			*outoffset = 0;
			return ERROR;
		}
		
		unsigned long	partialoffset;
		found_t			found;
		
		found = get_base_class_offset(
			( (class_type_t*)type )->clss,
			baseclss, &partialoffset );
			
		offset += partialoffset;
		
		switch( found )
		{
			case ERROR:
				*outoffset = 0;
				return ERROR;
			
			case AMBIVALENT:
				*outoffset = 0;
				return AMBIVALENT;
				
			case FOUND:
				if( wasfound )
				{
					*outoffset = 0;
					return AMBIVALENT;
				}
				
				*outoffset = offset;
				wasfound = true;
				break;
		}
			
		qualhead = qualhead->next;
	}
	
	if( wasfound )
		return FOUND;
	
	*outoffset = offset;
	return NOT_FOUND;
}

// ===========================================================================

bool					g_template_added;
bool					g_template_compiled;

enum {
	CCI_COMPILE_THIS	= 1,
	CCI_COMPILE_SUPER	= 2
};

static void
compile_classes_incremental(
	class_t*			clss,
	int					cciflags )
{
	scope_t*			scope = clss->scope;

	if( scope->flags & SCF_COMPILE )
		cciflags |= CCI_COMPILE_THIS;

	xhtab_t*			table = scope->functions;
	int					scopeflags = scope->flags;

	tt_replacer_t		replacer;
	bool				ttreplace = false;
		
	if( ( scopeflags & SCF_TTCLASS ) &&
		( cciflags & CCI_COMPILE_THIS ) )
	{
		tt_class_t*		ttclss = (tt_class_t*)clss;

		replacer.replace(
			ttclss->tpbind, ttclss->tpscope );
		ttreplace = true;
	}

	if( xhfirst( table ) )
	{
		func_t*			func;
		xhtab_iterator_t	it;
	
		do
		{
			func = (func_t*)xhstuff( table );
			
			xhgetiterator( table, &it );
			
			do {
				int modifiers = func->decl->spec->modifiers;

				if( cciflags && ( modifiers &
					( MOD_NATIVE | MOD_ABSTRACT |
					MOD_DLL | MOD_TEMPLATE ) ) == 0 )
				{
					out_open_inline();		
					compile_func( clss, func );
					func->code = out_close_inline();		
				}
				else if( modifiers & MOD_TEMPLATE )
				{				
					compile_template_func(
						clss, func );
				}

				func = func->same;
			} while( func );
	
			xhsetiterator( table, &it );
			
		} while( xhnext( table ) );
	}

	if( cciflags & CCI_COMPILE_THIS )
	{
		scope->flags &= ~SCF_COMPILE;
		cciflags &= ~CCI_COMPILE_THIS;
		cciflags |= CCI_COMPILE_SUPER;
	}
	
	if( scopeflags & SCF_TEMPLATE )
		return;

	class_t*			child = clss->child;
	
	while( child )
	{
		compile_classes_incremental( child, cciflags );
		child = child->next;
	}

	if( ttreplace )
		replacer.restore();
}

static void
compile_classes_initial(
	class_t*			clss )
{
	scope_t*			scope = clss->scope;

	if( scope->flags & SCF_TTCLASS )
		return;

	xhtab_t*			table = scope->functions;
		
	if( xhfirst( table ) )
	{
		func_t*			func;
		xhtab_iterator_t	it;
	
		do
		{
			func = (func_t*)xhstuff( table );
			xhgetiterator( table, &it );
			
			do {
				if( ( func->decl->spec->modifiers &
					( MOD_NATIVE | MOD_ABSTRACT | 
					MOD_DLL | MOD_TEMPLATE ) ) == 0 )
				{
					out_open_inline();		
					compile_func( clss, func );
					func->code = out_close_inline();		
				}
				func = func->same;
			} while( func );
	
			xhsetiterator( table, &it );
			
		} while( xhnext( table ) );
	}

	if( scope->static_offset > 0 )
	{
		out_open_inline();
		compile_static_cd_func( clss, CONSTRUCTOR );
		scope->init_code = out_close_inline();
	}

	if( scope->flags & SCF_TEMPLATE )
		return;

	class_t*			child = clss->child;
	
	while( child )
	{
		compile_classes_initial( child );
		child = child->next;
	}
}

void
compile_classes(
	class_t*			clss )
{
	bool				last_added = false;

	compile_classes_initial( clss );

	while( true )
	{
		g_template_added = false;
		g_template_compiled = false;
		compile_classes_incremental( clss, 0 );
		if( g_template_added == false )
			break;
		if( last_added && g_template_compiled == false )
		{
			compile_error( 0,
				ERR_TEMPLATE_CYCLE );
			break;
		}

		last_added = true;
	}
}

// ===========================================================================

static long
export_func(
	class_t*			clss,
	func_t*				func )
{
	if( func->decl->spec->modifiers & MOD_TEMPLATE )
	{
		functemp_map_t*	map;
		long			count = 0;

		for( map = func->tmap; map; map = map->next )
		{
			out_inline( map->code );
			count += 1;
		}

		return count;
	}
	else if( func->code )
		out_inline( func->code );
	else
		compile_func( clss, func );

	return 1;
}

static void
export_class(
	class_t*			clss )
{
	output_class_id( clss );

	out_long( clss->size );	

	out_long( clss->partial_size );

	out_word( clss->basecount );
	
	qualhead_t*			qualhead = clss->base;
	
	while( qualhead )
	{
		output_class_id(
			( (class_type_t*)qualhead->type )->clss );
		qualhead = qualhead->next;
	}

	scope_t*			scope = clss->scope;

	xhtab_t*			table = scope->functions;
				
	pos_t				oldpos;
	
	out_tell_pos( &oldpos );
	out_word( 0 );

	u32					funccount = 0;

	if( xhfirst( table ) )
	{
		func_t*			func;
	
		do
		{
			func = (func_t*)xhstuff( table );
			
			do {
				funccount += export_func( clss, func );
				func = func->same;
			} while( func );
	
		} while( xhnext( table ) );
	}

	void*				code = scope->init_code;

	if( code )
	{
		out_inline( code );
		funccount++;
	}

	if( funccount > 0xffff - 1 )
	{
		compile_error(
			&clss->filepos,
			ERR_TOO_MANY_FUNCTIONS );
		return;
	}

	pos_t				newpos;

	out_tell_pos( &newpos );
	out_seek_pos( &oldpos );
	out_word( funccount );
	out_seek_pos( &newpos );

	out_long( get_static_scope_size( scope ) );
	
	scope_export_variables( scope );
}

static long				s_class_count = 0;

static void
export_classes_recursively(
	class_t*			clss )
{
	export_class( clss );

	s_class_count += 1;
		
	if( clss->scope->flags & SCF_TEMPLATE )
		return;

	class_t*			child = clss->child;
	
	while( child )
	{
		export_classes_recursively( child );
		child = child->next;
	}
}

void
export_classes(
	class_t*			clss )
{
	pos_t				beginpos;
	
	out_tell_pos( &beginpos );
	out_long( 0 );
	
	s_class_count = 0;
	
	export_classes_recursively( clss );
	
	pos_t				endpos;
	
	out_tell_pos( &endpos );
	out_seek_pos( &beginpos );
	out_long( s_class_count );
	out_seek_pos( &endpos );	
}

END_COMPILER_NAMESPACE
