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
//	machine_class_t.h		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef MACHINE_CLASS_H
#define MACHINE_CLASS_H

//#pragma once

#include "machine.h"
#include "machine_func_t.h"
#include "output_stream_t.h"
#include "htab_t.h"

BEGIN_MACHINE_NAMESPACE

class machine_t;
class fiber_t;
class class_t;

struct class_header_t {
	class_t*			clss;
	class_t*			baseclss;
	unsigned long		offset;
};

typedef u32 class_header_ptr_t;

union instance_t {
	class_header_ptr_t	header;
	instance_t*			next;
};

#define CLASS_HEADER( instance ) \
	( (class_header_t*)( (instance)->header ) )

enum found_t {
	NOT_FOUND,
	FOUND,
	AMBIVALENT
};

struct sourcemap_t;
class bytecode_func_t;

class class_t {
public:

						class_t(
							machine_t*			machine,
							const char*			name,
							long				name_length,
							class_t*			up );
						
						~class_t();

	void				import(
							input_stream_t*		stream );
							
	void				finalize();

#ifdef VM_PROJECT
	void				init(
							fiber_t*			fiber,
							func_id_t			id );
#endif

	class_t*			find_class(
							const char*			name,
							const long			length );

	class_t*			find_or_create_class(
							const char*			name,
							const long			length );

	func_t*				find_func_by_id(
							func_id_t			id,
							unsigned long*		offset ) const;
														
	bool				isa(
							class_t*			clss ) const;

	found_t				get_base_class_offset(
							class_t*			baseclss,
							unsigned long*		outoffset );

#ifdef VM_PROJECT
	instance_t*			create_instance(
							fiber_t*			fiber,
							int					ext = 0 );

	instance_t*			create_instance_array(
							fiber_t*			fiber,
							unsigned long		arraysize,
							int					ext = 0 );
	
	void				destroy_instance(
							fiber_t*			fiber,
							instance_t*			instance,
							int					ext = 0 );

	void				free_pool();
#endif

	void				export_reference(
							output_stream_t&	stream );

	class_t*			import_reference(
							input_stream_t&		stream );

	void				get_name(
							istring&			name ) const;

	long				size() const;

	void				init_instance(
							u08*				block );

	static void			get_full_name(
							class_t*			clss,
							istring&			name );
	
#if VM_PROJECT
	static void			get_type_name(
							class_t*			clss,
							istring&			name );

	u08*				get_static_data() const;

	sourcemap_t*		inverse_sourcemap(
							long				srcpos,
							bytecode_func_t**	mfunc );
#endif

//protected:
	friend class machine_t;
	friend class fiber_t;
	friend class bytecode_func_t;
	friend class disassembler_t;

#ifdef VM_PROJECT
	instance_t*			alloc_instance();
	
	void				free_instance(
							instance_t*			instance );

	instance_t*			alloc_instance_array(
							unsigned long		arraysize );
	
	void				free_instance_array(
							instance_t*			instance );
#endif

	void				import_all_functions(
							input_stream_t*		stream );

	func_t*				import_function(
							input_stream_t*		stream );

	void				import_variables(
							input_stream_t*		stream );

	unsigned long		count_class_headers() const;
	
	class_header_t*		setup_class_headers(
							class_t*			clss,
							unsigned long*		offset,
							class_header_t*		header );

	func_t*				find_func_by_id_flat(
							func_id_t			id ) const;

	machine_t*			m_machine;
	const char*			m_name;
	long				m_name_length;
	class_t*			m_up;

	func_t*				m_constructor;
	func_t*				m_destructor;

	unsigned short		m_base_count;
	class_t**			m_base_table;

	htab_t*				m_class_htab;

	long				m_func_count;
	func_t**			m_func_table;

	unsigned long		m_class_header_count;
	class_header_t*		m_class_headers;
	
	unsigned long		m_size;
	unsigned long		m_partial_size;
	unsigned long		m_static_size;
	
	unsigned long		m_local_var_count;
	unsigned long		m_static_var_count;
	variable_t*			m_local_vars;
	variable_t*			m_static_vars;
	
	instance_t*			m_instance_pool;
	u08*				m_static_data;

	void*				m_dll;
};

inline long
class_t::size() const
{
	return m_size;
}

END_MACHINE_NAMESPACE

#endif

