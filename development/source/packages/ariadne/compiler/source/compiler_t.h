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
//	run.h					   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_RUN_H
#define ARIADNE_RUN_H

//#pragma once

#include "compiler.h"
#include "output_stream_t.h"
#include <string>
#include "xheap_t.h"

#if EMBEDDED_COMPILER
#include "lexer.h"
#endif

BEGIN_COMPILER_NAMESPACE

struct error_t {
	error_t*			next;

	istring				what;

#if EMBEDDED_COMPILER
	fileref_t			fpos;
#else
	istring				file;
	u32					pos;
	u32					line;
	u32					column;
	
	istring				portion;
#endif
};

 class compiler_t {
 public:

						compiler_t(
							output_stream_t*		stream );

	virtual				~compiler_t();

	error_t*			run();
	
	static bool			compiling();

	static xheap_t*		heap();

	long				get_stack_size();

	void				set_stack_size(
							long					size );						

#if EMBEDDED_COMPILER && 0
	typedef 			aklabeth::code_document_t	document_t;
	
	static document_t*	document();

	document_t*			m_document;	
#endif

protected:
	friend void			compile_error(
							const fileref_t*		fpos,
							err_t					err,
							const char*				data_one,
							const char*				data_two );

	friend void			compile_setting(
							const char*				setting );

	void				clear_errors();

	static compiler_t*	s_instance;
		
	error_t*			m_errors;
	error_t**			m_errors_link;

	long				m_stack_size;

	output_stream_t*	m_output;

	xheap_t*			m_global_heap;

public:
	static bool			has_errors();
	
	error_t*			error() const;
};

inline xheap_t*
compiler_t::heap()
{
	return s_instance->m_global_heap;
}

END_COMPILER_NAMESPACE

#endif

