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
//	fiber_t.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_FIBER_H
#define ARIADNE_FIBER_H

//#pragma once

#include "machine.h"
#include "safe_memory_t.h"
#include "machine_error.h"

BEGIN_MACHINE_NAMESPACE

class machine_t;
union instance_t;
class memory_t;
class func_t;
class fiber_t;
class bytecode_func_t;

struct stack_frame_t {
	instance_t*			instance;
	bytecode_func_t*	func;
	u08*				code;
	u08*				localsp;
};

struct vm_state_t : stack_frame_t {
	u08*				sp;
	int					condflags;
};

struct stop_state_t : vm_state_t {
	fiber_t*			fiber;
	bool				resume;
	int					err;
	istring				errtext;
};

END_MACHINE_NAMESPACE

// runtime specific material

#if VM_PROJECT

#include "lib_context_t.h"

BEGIN_RT_NAMESPACE
class thread_t;
END_RT_NAMESPACE

BEGIN_MACHINE_NAMESPACE

class fiber_t {
public:
						fiber_t(
							machine_t*			machine,
							size_t				stack_size );

	virtual				~fiber_t();

	machine_t*			machine() const;

	// ---------------------------------------------------------------------------

	int					get_cdext() const;
	
	void				set_cdext(
							int					ext );

	void				set_refcon(
							void*				refcon );
								
	void*				get_refcon() const;

	const stop_state_t&	state() const;

	// ---------------------------------------------------------------------------

	void				clear_stack();

	void				push_bool(
							bool				val );

	void				push_char(
							int					val );

	void				push_byte(
							int					val );

	void				push_short(
							int					val );

	void				push_int(
							long				val );
							
	void				push_long(
							s64					val );

	void				push_float(
							float				val );

	void				push_double(
							double				val );
	
	void				push_ptr(
							void*				ptr );
	
	void				push_block(
							void*				block,
							long				size );
	
	bool				pop_bool();
							
	int					pop_char();
	
	int					pop_byte();

	int					pop_short();
	
	long				pop_int();
	
	s64					pop_long();

	float				pop_float();
	
	double				pop_double();
	
	void*				pop_ptr();
	
	instance_t*			pop_instance();
	
	void*				pop_block(
							long				size );
							
	void*				alloc_stack(
							long				size );
							
	void*				free_stack(
							long				size );

	// ---------------------------------------------------------------------------
							
	void				check_access(
							const void*			ptr,
							long				size );

	bool				test_access(
							void*				ptr,
							long				size ) const;
							 
	void				fatal_error(
							int					err,
							const char*			text = 0 );

	void				resumable_error(
							int					err,
							const char*			text = 0 );

	void				stop_on_demand();
	
	void				execute_bytecode(
							instance_t*			self,
							bytecode_func_t*	func );
								
	void				stop();

	void				cancel_stop();
	
	void				call(
							instance_t*			self,
							func_t*				func );

	void				step(
							bool				stepinto );

	fiber_t*			next_fiber() const;

#ifdef VM_PROJECT
	lib_context_t		m_lib_context;
#endif

protected:	
	friend class machine_t;

	fiber_t*			m_next;

private:
	machine_t*			m_machine;
	memory_t*			m_memory;

	u08*				m_stack_base;
	u08*				m_stack_pointer;
	u32					m_stack_size;

	stop_state_t		m_state;
	bool				m_stop_flag;
	//bool				m_inside_native;

	int					m_cdext;
	void*				m_refcon;
};

inline machine_t*
fiber_t::machine() const
{
	return m_machine;
}

inline void
fiber_t::set_cdext(
	int					ext )
{
	m_cdext = ext;
}

inline void*
fiber_t::get_refcon() const
{
	return m_refcon;
}

inline int
fiber_t::get_cdext() const
{
	return m_cdext;
}

inline fiber_t*
fiber_t::next_fiber() const
{
	return m_next;
}

inline const stop_state_t&
fiber_t::state() const
{
	return m_state;
}

inline void
fiber_t::check_access(
	const void*			ptr,
	long				size )
{
#if CMA_MACHINE
	memory_t*			memory = m_memory;
	u08*				memlo = memory->m_memlo;
	u08*				memhi = memory->m_memhi;

	if( ( (u08*)ptr ) < memlo || ( (u08*)ptr ) + size > memhi )
		fatal_error( ERR_ILLEGAL_ACCESS );
#endif
}

inline bool
fiber_t::test_access(
	void*				ptr,
	long				size ) const
{
#if CMA_MACHINE
	memory_t*			memory = m_memory;
	u08*				memlo = memory->m_memlo;
	u08*				memhi = memory->m_memhi;

	if( ( (u08*)ptr ) < memlo || ( (u08*)ptr ) + size > memhi )
		return false;
#endif
	return true;
}

END_MACHINE_NAMESPACE

#endif

#endif

