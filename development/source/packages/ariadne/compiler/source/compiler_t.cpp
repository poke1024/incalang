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
//	compiler_t.cp			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler_t.h"
#include "output.h"
#include "preproc_t.h"
#include "file_stream_t.h"

#include "scope_t.h"
#include "xalloc.h"
#include "class_t.h"

// ---------------------------------------------------------------------------

int
yyparse();

void
yyerror(
	char*				/*msg*/ )
{
	compiler::compile_error( 0, compiler::ERR_SYNTAX_ERROR );
}

extern "C" {

int
yywrap()
{
	return 1;
}

}

// ---------------------------------------------------------------------------

BEGIN_ARIADNE_NAMESPACE

void
ariadne_error(
	err_t				/*err*/ )
{
	if( compiler::compiler_t::compiling() == false )
		throw_icarus_error( "ariadne error" );

	compiler::compile_error( 0, compiler::ERR_INTERNAL );
}

END_ARIADNE_NAMESPACE

// ---------------------------------------------------------------------------

BEGIN_COMPILER_NAMESPACE

compiler_t*				compiler_t::s_instance = 0;

void
init_resolve();

compiler_t::compiler_t(
	output_stream_t*	stream )
{
	m_output = stream;

#if EMBEDDED_COMPILER && 0
	m_document = 0;
#endif
	
	m_errors = 0;
	m_errors_link = &m_errors;

	m_stack_size = 0;
	
	s_instance = this;
}

compiler_t::~compiler_t()
{
	s_instance = 0;

	clear_errors();
}

error_t*
compiler_t::run()
{
	clear_errors();

	init_scopes();
	init_class();
	init_resolve();
	
	push_xalloc_frame();

	try
	{
		xheap_t			heap;

		m_global_heap = &heap;

		push_scope();

		class_spec_t*	spec;
		
		spec = make_class_spec( "", ACC_PUBLIC );

		class_t*		clss = begin_class( spec, 0 );

		scope_make_global( clss->scope );
		scope_make_namespace( clss->scope );

		// yydebug = 1;
		yyparse();

		if( !m_errors )
		{
			end_class();
			undo_pop_scope();
		}
				
		if( !m_errors )
			scope_resolve( clss->scope );
				
		if( !m_errors )
		{
			compile_classes( clss );

			out_open_stream( m_output );
			export_classes( clss );
			out_close_stream();
		}
		
	}
	catch(...)
	{
		compile_error( 0, ERR_INTERNAL );	
	}
	
	m_global_heap = 0;

	pop_xalloc_frame();
	
	xframe_t::free_unused_memory();
	out_free_unused_memory();

	return m_errors;
}

bool
compiler_t::compiling()
{
	return s_instance != 0;
}

#if EMBEDDED_COMPILER && 0
compiler_t::document_t*
compiler_t::document()
{
	return s_instance ? s_instance->m_document : 0;
}
#endif

void
compiler_t::clear_errors()
{
	while( m_errors )
	{
		error_t*		next = m_errors->next;
		delete m_errors;
		m_errors = next;
	}
	
	m_errors = 0;
	m_errors_link = &m_errors;
}

bool
compiler_t::has_errors()
{
	return s_instance->m_errors;
}

error_t*
compiler_t::error() const
{
	return m_errors;
}

long
compiler_t::get_stack_size()
{
	return m_stack_size;
}

void
compiler_t::set_stack_size(
	long					size )
{
	m_stack_size = size;
}

END_COMPILER_NAMESPACE
