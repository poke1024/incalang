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
//	native_func_t.h			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef NATIVE_FUNC_H
#define NATIVE_FUNC_H

//#pragma once

#include "machine_func_t.h"
#include "machine_t.h"

BEGIN_MACHINE_NAMESPACE

class fiber_t;
union instance_t;

typedef void 			( *native_proc_ptr_t )(
							fiber_t&			fiber,
							instance_t*			self );

// ---------------------------------------------------------------------------

// blocking implies the ability to throw here

enum {
	native_func_can_block						= 1 << 0,
	native_func_can_throw						= 1 << 1,
	
	native_func_needs_context_save_mask			= 3
};

// ---------------------------------------------------------------------------

class native_func_t : public func_t {
public:
						native_func_t(
							func_id_t			id,
							class_t*			owner,
							machine_t*			machine );
	
	void				call(
							fiber_t&			fiber,
							instance_t*			self ) const;
							
	void				set_proc_ptr(
							native_proc_ptr_t	proc_ptr );
	
	void				set_flags(
							int					flags );
	
	int					flags() const;
							
protected:
	native_proc_ptr_t	m_proc_ptr;
	u08					m_flags;

#if ICARUS_SAFE
	machine_t*			m_machine;
#endif
};

// ---------------------------------------------------------------------------

inline void
native_func_t::call(
	fiber_t&			fiber,
	instance_t*			self ) const
{
	native_proc_ptr_t	proc_ptr = m_proc_ptr;

	if( !proc_ptr )
	{
#if ICARUS_SAFE
		istring			name;
		istring			s;
		
		m_machine->func_id_to_name( m_id, name );
		
		s = "unresolved native function \"";
		s.append( name );
		s.append( "\"" );
		throw_icarus_error( s.c_str() );
#else
		throw_icarus_error( "unresolved native function" );
#endif
	}

	( *proc_ptr )( fiber, self );
}

inline int
native_func_t::flags() const
{
	return m_flags;
}

inline void
native_func_t::set_proc_ptr(
	native_proc_ptr_t	proc_ptr )
{
	m_proc_ptr = proc_ptr;
}

inline void
native_func_t::set_flags(
	int					flags )
{
	m_flags = flags;
}

END_MACHINE_NAMESPACE

#endif

