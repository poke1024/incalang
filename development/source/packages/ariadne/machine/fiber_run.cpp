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
//	fiber_run.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#if VM_PROJECT

#define YIELDED_MACHINE 1

// yield ten thousand times per second
#define YIELD_INTERVAL 100

// ---------------------------------------------------------------------------

#include "fiber_t.h"
#include "machine_t.h"
#include "machine_class_t.h"
#include <cstring>
#include <math.h>

#include <stdexcept>
#include <cstdio>

#include "bytecode_func_t.h"
#include "native_func_t.h"
#include "dll_func_t.h"
#include "safe_memory_t.h"
#include "machine_error.h"
#include "os_core.h"
#include "thread_t.h"
#include "ariadne_messages.h"

#if __MWERKS__
#pragma global_optimizer on
#endif

BEGIN_MACHINE_NAMESPACE

using namespace std;

// ---------------------------------------------------------------------------

#define VM_STORE_SP															\
	m_stack_pointer = sp;

#define VM_LOAD_SP															\
	sp = m_stack_pointer;

#define VM_ENTER_NATIVE( flags )											\
	if( flags & native_func_needs_context_save_mask )	{					\
		VM_SAVE_FIBER_STATE;												\
	}																		\
	VM_STORE_SP;															\

#define VM_LEAVE_NATIVE( flags )											\
	VM_LOAD_SP;
	
#define VM_DONE																\
	VM_STORE_SP;															\
	return;

#define VM_FIBER															\
	this

#define VM_MACHINE															\
	m_machine

#define VM_MEMORY															\
	m_memory

#include "bytecode.h"
#include "vmperfdefs.h"

// ---------------------------------------------------------------------------

#if YIELDED_MACHINE

#define VM_YIELD {															\
	if( rt_cycles() - last_yield_cycles >= yield_interval )	{				\
		ithread_t::yield();													\
		if( m_stop_flag ) {													\
			VM_SAVE_FIBER_STATE; stop_on_demand(); }						\
		last_yield_cycles = rt_cycles(); } }

#else

#define VM_YIELD

#endif

// ---------------------------------------------------------------------------

static void
invalid_opcode_error(
	machine_t*			machine,
	bytecode_func_t*	func,
	u08*				code,
	int					opcode )
{
	char				s[ 256 ];
				
	istring name( "unknown" );
		
	machine->func_id_to_name(
		func->get_id(), name );
		
	isnprintf( s, 255,
		"invalid opcode %ld/%ld at offset %.4x [function %s]",
		(long)( opcode & 63 ), (long)( opcode >> 6 ),
		(long)( code - func->m_code_data ),
		name.c_str() );
	throw_icarus_error( s );
}

void
fiber_t::execute_bytecode(
	instance_t*			self,
	bytecode_func_t*	func )
{
	u08*				sp = m_stack_pointer;
	u08*				code = func->m_code_data;
	int					condflags = 0;

#if CMA_MACHINE
	u08*				memlo = VM_MEMORY->m_memlo;
	u08*				memhi = VM_MEMORY->m_memhi;
#endif

#if YIELDED_MACHINE
	u64					last_yield_cycles = rt_cycles();
	u32					yield_interval = rt_micros_to_cycles(
							YIELD_INTERVAL );
#endif

	u08*				localsp = sp;

#if ICARUS_OS_WIN32
	__try {
#endif

	PUSH_PTR( 0 )
	PUSH_PTR( 0 )
	PUSH_PTR( 0 )
	PUSH_PTR( 0 )
	
	localsp = sp;
	sp += func->m_locals_size;
	
	while( true )
	{
		#include "vmperf.h"
	}

#if ICARUS_OS_WIN32
	}
	__except( ( GetExceptionCode() & 0xF0000000 ) == 0xE0000000 ?
		EXCEPTION_CONTINUE_SEARCH : EXCEPTION_EXECUTE_HANDLER )
	{
		VM_SAVE_FIBER_STATE;

		const char* t = "unknown exception";

		switch( GetExceptionCode() )
		{
			case EXCEPTION_ACCESS_VIOLATION:
				t = "access violation";
				break;

			case EXCEPTION_DATATYPE_MISALIGNMENT:
				t = "datatype misalignment";
				break;

			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				t = "array bounds exceeded";
				break;

			case EXCEPTION_FLT_DENORMAL_OPERAND:
				t = "denormal float operand";
				break;

			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				t = "float division by zero";
				break;

			case EXCEPTION_FLT_INEXACT_RESULT:
				t = "inexact float result";
				break;

			case EXCEPTION_FLT_INVALID_OPERATION:
				t = "invalid float operation";
				break;

			case EXCEPTION_FLT_OVERFLOW:
				t = "float overflow";
				break;

			case EXCEPTION_FLT_STACK_CHECK:
				t = "float stack overflow";
				break;

			case EXCEPTION_FLT_UNDERFLOW:
				t = "float underflow";
				break;

			case EXCEPTION_INT_DIVIDE_BY_ZERO:
				t = "integer division by zero";
				break;

			case EXCEPTION_INT_OVERFLOW:
				t = "integer overflow";
				break;

			case EXCEPTION_PRIV_INSTRUCTION:
				t = "privileged instruction";
				break;
		}

		fatal_error( ERR_EXCEPTION, t );
	}
#endif
}

// ---------------------------------------------------------------------------

#if SAFE_MACHINE
#undef MACHINE_RUNTIME_ERROR
#define MACHINE_RUNTIME_ERROR( error ) {									\
	m_lib_context.stop();													\
	VM_FIBER->m_state.resume = false;										\
	VM_FIBER->m_state.err = error;											\
	VM_MACHINE->broadcast_message( msg_MachineStopped, VM_FIBER );			\
	while( true ) { ithread_t::current()->block(); } }
#endif

// ---------------------------------------------------------------------------

void
fiber_t::clear_stack()
{
	m_stack_pointer = m_stack_base;
}

void
fiber_t::push_bool(
	bool				val )
{
	u08* sp = m_stack_pointer;
	PUSH_BYTE( val );
	m_stack_pointer = sp;
}

void
fiber_t::push_char(
	int					val )
{
	u08* sp = m_stack_pointer;
	PUSH_BYTE( val );
	m_stack_pointer = sp;
}

void
fiber_t::push_byte(
	int					val )
{
	u08* sp = m_stack_pointer;
	PUSH_BYTE( val );
	m_stack_pointer = sp;
}

void
fiber_t::push_short(
	int					val )
{
	u08* sp = m_stack_pointer;
	PUSH_WORD( val );
	m_stack_pointer = sp;
}

void
fiber_t::push_int(
	long				val )
{
	u08* sp = m_stack_pointer;
	PUSH_QUAD( val );
	m_stack_pointer = sp;
}

void
fiber_t::push_long(
	s64					val )
{
	u08* sp = m_stack_pointer;
	PUSH_OCTA( val );
	m_stack_pointer = sp;
}

void
fiber_t::push_float(
	float				val )
{
	u08* sp = m_stack_pointer;
	PUSH_FLOAT( val );
	m_stack_pointer = sp;
}

void
fiber_t::push_double(
	double				val )
{
	u08* sp = m_stack_pointer;
	PUSH_DOUBLE( val );
	m_stack_pointer = sp;
}

void
fiber_t::push_ptr(
	void*				ptr )
{
	u08* sp = m_stack_pointer;
	PUSH_PTR( ptr );
	m_stack_pointer = sp;
}

void
fiber_t::push_block(
	void*				block,
	long				size )
{
	u08* sp = m_stack_pointer;
	CHECK_STACK_OVERFLOW( size );
	imemcpy( sp, block, size );
	sp += size;
	m_stack_pointer = sp;
}

bool
fiber_t::pop_bool()
{
	int val;
	u08* sp = m_stack_pointer;
	POP_SBYTE( val );
	m_stack_pointer = sp;
	return val;
}

int
fiber_t::pop_char()
{
	int val;
	u08* sp = m_stack_pointer;
	POP_SBYTE( val );
	m_stack_pointer = sp;
	return val;
}

int
fiber_t::pop_byte()
{
	int val;
	u08* sp = m_stack_pointer;
	POP_SBYTE( val );
	m_stack_pointer = sp;
	return val;
}

int
fiber_t::pop_short()
{
	int val;
	u08* sp = m_stack_pointer;
	POP_SWORD( val );
	m_stack_pointer = sp;
	return val;
}

long
fiber_t::pop_int()
{
	long val;
	u08* sp = m_stack_pointer;
	POP_SQUAD( val );
	m_stack_pointer = sp;
	return val;
}

s64
fiber_t::pop_long()
{
	s64 val;
	u08* sp = m_stack_pointer;
	POP_SOCTA( val );
	m_stack_pointer = sp;
	return val;
}

float
fiber_t::pop_float()
{
	float val;
	u08* sp = m_stack_pointer;
	POP_FLOAT( val );
	m_stack_pointer = sp;
	return val;
}
	
double
fiber_t::pop_double()
{
	double val;
	u08* sp = m_stack_pointer;
	POP_DOUBLE( val );
	m_stack_pointer = sp;
	return val;
}

void*
fiber_t::pop_ptr()
{
	void* val;
	u08* sp = m_stack_pointer;
	POP_PTR( val );
	m_stack_pointer = sp;	
	return val;
}

instance_t*
fiber_t::pop_instance()
{
#if CMA_MACHINE
	u08* memlo = m_memory->m_memlo;
	u08* memhi = m_memory->m_memhi;
#endif

	void* val;
	u08* sp = m_stack_pointer;
	POP_PTR( val );
	m_stack_pointer = sp;
	CHECK_INSTANCE( val );
	return (instance_t*)val;
}

void*
fiber_t::pop_block(
	long				size )
{
	u08* sp = m_stack_pointer;
	sp -= size;
	m_stack_pointer = sp;
	
	return sp;
}

void*
fiber_t::alloc_stack(
	long				size )
{
	u08* base = m_stack_pointer;
	u08* sp = base;
	CHECK_STACK_OVERFLOW( size );
	sp += size;
	m_stack_pointer = sp;
	
	return base;
}

void*
fiber_t::free_stack(
	long				size )
{
	u08* sp = m_stack_pointer;
	sp -= size;
	m_stack_pointer = sp;
	return sp;
}

void
fiber_t::fatal_error(
	int					err,
	const char*			text )
{
	if( text )
		m_state.errtext = text;
	
	MACHINE_RUNTIME_ERROR( err );
}

void
fiber_t::resumable_error(
	int					err,
	const char*			text )
{
	if( text )
		m_state.errtext = text;
	
	m_lib_context.stop();
	VM_FIBER->m_state.resume = true;
	VM_FIBER->m_state.err = err;
	VM_MACHINE->broadcast_message( msg_MachineStopped, VM_FIBER );	
}

void
fiber_t::stop_on_demand()
{
	// should be called from native functions only

	if( not m_stop_flag )
		return;

	m_lib_context.stop();

	while( m_stop_flag )
	{		
		m_stop_flag = false;
		m_state.resume = true;
		m_machine->broadcast_message(
			msg_MachineStopped, this );
		//ithread_t::current()->block();
	}

	m_lib_context.resume();
}

END_MACHINE_NAMESPACE

#endif // VM_PROJECT
