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
//	fiber_t.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#if VM_PROJECT

#include "fiber_t.h"
#include "machine_t.h"
#include "safe_memory_t.h"

#include "machine_error.h"
#include "native_func_t.h"

BEGIN_MACHINE_NAMESPACE

long s_fiber_count		= 0;

fiber_t::fiber_t(
	machine_t*			machine,
	size_t				stack_size ) :
	
	m_machine( machine )
{
	m_memory = machine->get_memory();

	m_stack_base = m_memory->alloc( stack_size );
	m_stack_pointer = m_stack_base;
	m_stack_size = stack_size;
	
	m_stop_flag = false;
	
	m_cdext = 0;
	m_refcon = 0;
	
	machine->register_fiber( this );

	m_state.func = 0;
	m_state.fiber = this;
	m_state.resume = true;
	m_state.err = ERR_NONE;
	
	s_fiber_count += 1;
}

fiber_t::~fiber_t()
{
	s_fiber_count -= 1;

	m_memory->free( m_stack_base );

	m_machine->unregister_fiber( this );
}

void
fiber_t::set_refcon(
	void*				refcon )
{
	m_refcon = refcon;
}

void
fiber_t::stop()
{
	m_stop_flag = true;
}

void
fiber_t::cancel_stop()
{
	m_stop_flag = false;
}

void
fiber_t::call(
	instance_t*			self,
	func_t*				func )
{
	switch( func->get_type() )
	{
		case func_type_bytecode:
			execute_bytecode(
				self, (bytecode_func_t*)func );
			break;

		case func_type_native:
			( (native_func_t*)func )->call(
				*this, self );
			break;
			
#if DEBUG_ABSTRACT_FUNCTIONS
		case func_type_abstract:
			throw_icarus_error( "call of abstract function" );
			break;
#endif
	}
}

END_MACHINE_NAMESPACE

#endif // VM_PROJECT
