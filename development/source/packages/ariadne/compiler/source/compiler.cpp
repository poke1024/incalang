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
//	compiler.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"
#include "compiler_t.h"
#include "reader_t.h"
#include "pickfunc.h"

#include "scope_t.h"
#include "xhtab_t.h"
#include "type_t.h"

BEGIN_COMPILER_NAMESPACE

static const char*		s_err_text[] = {
	"internal error",
	"invalid qualifiers",
	"number too big",
	"invalid number",
	"type mismatch",
	"illegal type cast",
	"please name this type '%s' instead of '%s'",
	"please name this variable '%s' instead of '%s'",
	"expected pointer",
	"illegal type",
	"expected lvalue",
	"expected constant",
	"expected integer constant",
	"syntax error",
	"incomplete type",
	"undeclared type '%s'",
	"undeclared type/namespace/class '%s'",
	"illegal modifier",
	"undeclared variable '%s'",
	"expected class",
	"expected namespace",
	"variable '%s' defined twice",
	"type redefined",
	"function '%s' defined twice in %s",
	"function call ambivalent, candidates are %s",
	"no function matching %s found",
	"return value expected for function '%s'",
	"variable not static",
	"function not static",
	"ambigous access to variable",
	"ambigous access to type",
	"ambigous access to enum value",
	"can't instantiate abstract class",
	"illegal abstract variable",
	"illegal access to protected/private member",
	"construction of this type is protected/private",
	"destruction of this type is protected/private",
	"illegal assignment to constant",
	"too many initializers",
	"references have to be initialized",
	"case without enclosing switch block",
	"this integer constant is too big",
	"too many cases in this switch block",
	"case statement defined twice for same value",
	"can't continue in a switch block",
	"default defined twice in this switch block",
	"case or default expected",
	"enum constant defined twice",
	"illegal operator declaration",
	"too many parameters",
	"too many template type parameters",
	"unresolvable cycle in 'using namespace' graph",
	"unresolvable cycle in template graph",
	"cannot determine template function return type",
	"cannot determine type of template type '%s'",
	"cannot instantiate non-template type as template",
	"wrong number of type parameters for template type",
	
	"branch too long",
	"too many functions in class",
	"too many variables in class/function",
	"cycle in base class graph",
	"feature not implemented",
	"division by zero",
	
	"file could not be opened",
	"line too long (>8 kb)",
	"illegal preprocessor directive",
	"unterminated C comment",
	"include file '%s' not found",
	"illegal compiler setting" };
		
// ---------------------------------------------------------------------------
		
static bool
get_template_info(
	istring&			s );

#if !EMBEDDED_COMPILER
static long
error_file_info(
	reader_t*			reader,
	const fileref_t*	fpos,
	istring&			path,
	istring&			portion,
	u32&				column )
{
	if( reader )
		reader->lookup_fileid( fpos->id, path );
		
	source_stream_t		file( path.c_str(), "r" );
	long				line = 1;
	long				pppline = 0;
	long				ppline = 0;
	long				pline = 0;
	
	for( long i = 0; i < fpos->pos; i++ )
	{
		char			c = file.read_char();
		
		if( c == '\n' )
		{
			pppline = ppline;
			ppline = pline;
			pline = i;
			line += 1;
		}
	}
	
	long				filelength = file.length();
	int					nlcnt = 0;
		
	file.seek( pline + 1, seek_mode_begin );

	portion = "\t";
		
	for( long i = pline + 1; i < filelength; i++ )
	{
		char			c = file.read_char();
		
		if( c == '\n' )
		{
			if( ++nlcnt == 1 )
				break;
			portion.append( "\n\t" );
		}
		else
			portion.append( 1, c );
	}
	
	column = fpos->pos - pline - 1;
	
	portion.append( "\n\t" );
	portion.append( column, ' ' );
	portion.append( "^" );

	return line;
}
#endif

void
compile_error(
	const fileref_t*	fpos,
	err_t				err,
	const char*			data_one,
	const char*			data_two )
{
	compiler_t*			compiler = compiler_t::s_instance;
	
	if( !compiler )
		return;

#if EMBEDDED_COMPILER
	if( *compiler->m_errors_link )
		return;
#endif

	fileref_t			fposrec;
	
	if( !fpos )
	{
		get_filepos( &fposrec );
		fpos = &fposrec;
	}
	
	static char			buf[ 256 ];

	switch( err )
	{
		case ERR_ILLEGAL_TYPENAME:
		case ERR_ILLEGAL_VARNAME:
			if( data_one == 0 || data_two == 0 )
				istrcpy( buf, "internal error" );
			else
				isnprintf( buf, 255,
					s_err_text[ err - ERR_INTERNAL ],
					data_one, data_two );
			break;

		case ERR_FUNCTION_REDEFINED:
			if( data_one == 0 || data_two == 0 )
				istrcpy( buf, "internal error" );
			else
				isnprintf( buf, 255,
					s_err_text[ err - ERR_INTERNAL ],
					user_func_name( data_one ), data_two );
			break;

		case ERR_FUNCTION_UNDEFINED:
		case ERR_EXPECTED_RETURN_VALUE:
		case ERR_UNDETERMINED_TEMPLATE_TYPE:
			if( data_one == 0 )
				istrcpy( buf, "internal error" );
			else
				isnprintf( buf, 255,
					s_err_text[ err - ERR_INTERNAL ],
					user_func_name( data_one ) );
			break;

		case ERR_UNDECLARED_VARIABLE:
		case ERR_VARIABLE_REDEFINED:
		case ERR_UNDECLARED_TYPE:
		case ERR_UNDECLARED_QUALNAME:
		case ERR_FUNCTION_AMBIVALENT:
		case ERR_INCLUDE_FILE_NOT_FOUND:
			if( data_one == 0 )
				istrcpy( buf, "internal error" );
			else
				isnprintf( buf, 255,
					s_err_text[ err - ERR_INTERNAL ],
					data_one );
			break;
			
		default:
			istrcpy( buf, s_err_text[ err - ERR_INTERNAL ] );
			break;
	}
	
	error_t*			error;
		
	error = new error_t;
	error->what = buf;

	istring				s;

	if( get_template_info( s ) )
		error->what.append( s );

#if EMBEDDED_COMPILER
	error->fpos = *fpos;
#else
	error->pos = fpos->pos;
	error->file = "unknown file";
	error->line = 0;
	error->column = 0;
	
	try {
		error->line = error_file_info(
			compiler->m_reader, fpos,
			error->file, error->portion,
			error->column );
	}
	catch(...)
	{
	}

	error_t*			node;

	node = compiler->m_errors;
	while( node )
	{
		if( node->line == error->line )
		{
			delete error;
			return;
		}
		node = node->next;
	}
#endif
		
	error->next = 0;
	*compiler->m_errors_link = error;
	compiler->m_errors_link = &error->next;
}

static bool
get_template_info(
	istring&			s )
{
	scope_t*			scope = cur_scope();

	while( true )
	{
		if( scope->flags & SCF_TEMPLATE )
			break;
		else if( scope->up == 0 )
			return false;
		scope = scope->up;
	}

	s = " (with ";

	xhtab_t*			table = scope->typedefs;
	istring				t;
	long				index = 0;

	if( xhfirst( table ) )
	{
		typedef_t*		node;
		type_t*			type;
	
		do
		{
			node = (typedef_t*)xhstuff( table );
			type = node->type;
			type_to_string( type, t );
			if( index > 0 )
				s.append( ", " );
			s.append( node->name );
			s.append( " as " );
			s.append( t );
			index++;
		} while( xhnext( table ) );
	}

	s.append( ")" );

	return index > 0;
}

void
compile_setting(
	const char*			setting )
{
	int					len = strlen( setting );

	if( len > 10 &&
		strncmp( setting, "stack_size", 10 ) == 0 )
	{
		long size = 0;
		sscanf( setting, "stack_size=%ld", &size );
		if( size )
			compiler_t::s_instance->set_stack_size( size );
	}

	//compile_error( ERR_ILLEGAL_COMPILER_SETTING );
}

END_COMPILER_NAMESPACE
