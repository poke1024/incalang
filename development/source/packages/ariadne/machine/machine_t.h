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
//	machine_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_MACHINE_H
#define ARIADNE_MACHINE_H

//#pragma once

#include "machine.h"
#include "machine_func_t.h"
#include "htab_t.h"
#include "xheap_t.h"

#ifdef VM_PROJECT
#include "lib_static_t.h"
#endif

BEGIN_MACHINE_NAMESPACE

class class_t;
class func_t;
class memory_t;
class fiber_t;

class machine_t {
public:
						machine_t(
							memory_t*			memory,
							const char*			base_path );
						
	virtual				~machine_t();

	void				import_class(
							input_stream_t*		stream );

	class_t*			read_class_id(
							input_stream_t*		stream );

	func_id_t			read_func_id(
							input_stream_t*		stream );
	
	func_id_t			find_or_create_func_id(
							const char*			name,
							const long			length );

	func_id_t			get_constructor_func_id() const;

	func_id_t			get_destructor_func_id() const;
		
	memory_t*			get_memory() const;
	
#ifdef VM_PROJECT
	fiber_t*			first_fiber() const;
	
	void				broadcast_message(
							message_t			message,
							void*				param );
#endif

	void				func_id_to_name(
							func_id_t			func_id,
							istring&			text );
							
	// ---------------------------------------------------------------------------

	class_t*			get_root_class() const;
	
	void				set_refcon(
							void*				refcon );
								
	void*				get_refcon() const;

	istring				file_path(
							const char*			path );

	void*				xalloc(
							isize_t				size );

	// ---------------------------------------------------------------------------

#if VM_PROJECT
	void				request_stop();

	void				cancel_stop();
#endif

	// ---------------------------------------------------------------------------

	void				set_string_class(
							class_t*			clss );

	class_t*			get_string_class() const;
	
#ifdef VM_PROJECT
	lib_static_t		m_lib_static;

	static machine_t*	current();
	
protected:
	friend class fiber_t;
	
	void				register_fiber(
							fiber_t*			fiber );

	void				unregister_fiber(
							fiber_t*			fiber );
#endif

private:
	memory_t*			m_memory;
	class_t*			m_root_class;
	xheap_t				m_xheap;
	
	htab_t*				m_func_htab;
	
	func_id_t			m_constructor_func_id;
	func_id_t			m_destructor_func_id;
			
	void*				m_refcon;
	istring				m_base_path;
	class_t*			m_string_class;
	
#ifdef VM_PROJECT
	fiber_t*			m_fibers;
#endif
};

// ---------------------------------------------------------------------------

inline func_id_t
machine_t::get_constructor_func_id() const
{
	return m_constructor_func_id;
}

inline func_id_t
machine_t::get_destructor_func_id() const
{
	return m_destructor_func_id;
}

#ifdef VM_PROJECT
inline fiber_t*
machine_t::first_fiber() const
{
	return m_fibers;
}
#endif

inline memory_t*
machine_t::get_memory() const
{
	return m_memory;
}

inline void*
machine_t::get_refcon() const
{
	return m_refcon;
}

inline void*
machine_t::xalloc(
	isize_t				size )
{
	return m_xheap.alloc( size );
}

inline class_t*
machine_t::get_string_class() const
{
	return m_string_class;
}

END_MACHINE_NAMESPACE

#endif

