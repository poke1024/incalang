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
//	bytecode_func_t.h		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef BYTECODE_FUNC_H
#define BYTECODE_FUNC_H

//#pragma once

#include "machine_func_t.h"

BEGIN_MACHINE_NAMESPACE

struct call_t {
	u32					clssindx;
	func_id_t			funcid;
	func_t*				regufunc;
};

struct sourcemap_t {
	s32					offset;
	s32					srcpos;
};

struct lifetime_t : variable_t {
	s32					create;
	s32					destroy;
};

struct breakpoint_t {
	breakpoint_t*		next;
	s32					offset;
	u08					saved;
};

class class_t;

class bytecode_func_t : public func_t {
public:
						bytecode_func_t(
							func_id_t			id,
							class_t*			owner );
							
	virtual				~bytecode_func_t();

	void				import(
							input_stream_t*		stream,
							machine_t*			vm );
							
	virtual void		finalize(
							machine_t*			vm );

	u32					locals_size() const
							{ return m_locals_size; }

	long				find_source_location(
							long				offset );

	long				find_code_location(
							long				srcpos );

	long				get_code_offset(
							const u08*			code ) const;

	const u08*			find_break_location(
							const u08*			code ) const;
							
	void				add_breakpoint(
							long				offset,
							int					breakcode );
	
	void				remove_breakpoint(
							long				offset );

	u08					hide_breakpoint(
							long				offset );
							
//protected:
	friend class fiber_t;
	friend class disassembler_t;

	void				import_long_table(
							input_stream_t*		stream,
							machine_t*			vm );

	void				import_wide_table(
							input_stream_t*		stream,
							machine_t*			vm );

	void				import_float_table(
							input_stream_t*		stream,
							machine_t*			vm );

	void				import_double_table(
							input_stream_t*		stream,
							machine_t*			vm );

	void				import_string_table(
							input_stream_t*		stream,
							machine_t*			vm );

	void				import_call_table(
							input_stream_t*		stream,
							machine_t*			vm );

	void				import_class_table(
							input_stream_t*		stream,
							machine_t*			vm );

	void				import_lifetimes(
							input_stream_t*		stream,
							machine_t*			vm );

	void				import_sourcemaps(
							input_stream_t*		stream );

	void				setup_string_pool(
							machine_t*			vm );

	sourcemap_t*		find_sourcemap(
							long				offset ) const;

	sourcemap_t*		inverse_sourcemap(
							long				srcpos ) const;

	unsigned long		m_code_size;
	u08*				m_code_data;
	
	u32*				m_quad_table;
	u64*				m_octa_table;
	f32*				m_float_table;
	f64*				m_double_table;
		
	u32*				m_string_table;
	char*				m_string_data;
#if ARIADNE_STRING_OBJECTS
	long				m_string_count;
	u08*				m_string_pool;
#endif
	
	call_t*				m_call_table;
	unsigned long		m_call_count;
	class_t**			m_class_table;

	unsigned long		m_lifetime_count;
	lifetime_t*			m_lifetime_table;

	unsigned long		m_sourcemap_count;
	sourcemap_t*		m_sourcemap_table;
	
	breakpoint_t*		m_breakpoints;
	
	u32					m_locals_size;
};

END_MACHINE_NAMESPACE

#endif

