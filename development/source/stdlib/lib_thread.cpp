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
//	lib_thread.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "machine_class_t.h"
#include "os_core.h"
#include "fiber_thread_t.h"
#include "lib_string.h"
#include "mutex_t.h"
#include "signal_t.h"
#include "queue_t.h"

BEGIN_MACHINE_NAMESPACE

struct vm_thread_t {
	fiber_thread_t*		thread;
};

STDLIB_CLASS_FUNCTION( thread_create, vm_thread_t )
	machine_t*			machine = f.machine();
	self->thread = new fiber_thread_t( machine, instptr );
	ithread_t::yield_to( self->thread );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( thread_destroy, vm_thread_t )
	f;
	self->thread->kill();
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( thread_wake, vm_thread_t )
	f;
	self->thread->wake();
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( thread_block, vm_thread_t )
	f;
	self->thread->block();
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( thread_sleep_millis, vm_thread_t )
	u64					millis = f.pop_long();
	f.stop_on_demand();
	self->thread->sleep( millis * 1000L );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( thread_sleep_micros, vm_thread_t )
	u64					micros = f.pop_long();
	f.stop_on_demand();
	self->thread->sleep( micros );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( thread_set_name, vm_thread_t )
	string_t*		s = pop_string( f );
	const char*		t = string_get_c_str( s );
	if( t )
		self->thread->set_name( t );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( thread_get_name, vm_thread_t )
	const char*		name = self->thread->name();
	machine_t&		m = *f.machine();

	u08				block[ STRING_CLASS_SIZE ];
	string_t*		s;
	
	init_string( block, m );
	s = INSTANCE_SELF( string_t, block );
	string_create( m, s, name, strlen( name ) );

	f.push_block( block, sizeof( block ) );	
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( thread_set_priority, vm_thread_t )
	self->thread->set_priority( f.pop_int() );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( thread_get_priority, vm_thread_t )
	f.push_int( self->thread->priority() );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

struct vm_mutex_t {
	mutex_t*			mutex;
};

STDLIB_CLASS_FUNCTION( mutex_create, vm_mutex_t )
	self->mutex = new mutex_t;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( mutex_destroy, vm_mutex_t )
	delete self->mutex;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( mutex_lock, vm_mutex_t )
	self->mutex->lock();
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( mutex_unlock, vm_mutex_t )
	self->mutex->unlock();
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

struct vm_signal_t {
	signal_t*			signal;
};

STDLIB_CLASS_FUNCTION( signal_create, vm_signal_t )
	self->signal = new signal_t;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( signal_destroy, vm_signal_t )
	delete self->signal;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( signal_signal, vm_signal_t )
	self->signal->signal();
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( signal_wait, vm_signal_t )
	self->signal->wait();
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( signal_wait_for, vm_signal_t )
	self->signal->wait_until( rt_micros() + f.pop_long() );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( signal_wait_until, vm_signal_t )
	self->signal->wait_until( f.pop_long() );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

struct vm_queue_t {
	queue_t*			queue;
};

static class_t*			s_queue_clss = 0;

STDLIB_FUNCTION( queue_make )
	long				size = f.pop_int();
	long				capacity = f.pop_int();
	instance_t*			instance;
	vm_queue_t*			self;

	instance = s_queue_clss->create_instance_array( &f, 1 );
	self = INSTANCE_SELF( vm_queue_t, instance );
	self->queue = new queue_t( size, capacity );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( queue_create, vm_queue_t )
	self->queue = new queue_t( sizeof( void* ), 0 );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( queue_destroy, vm_queue_t )
	delete self->queue;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( queue_put, vm_queue_t )
	self->queue->put( f.pop_ptr() );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( queue_get, vm_queue_t )
	self->queue->get( f.pop_ptr() );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( queue_put_cond, vm_queue_t )
	f.push_bool( self->queue->put_cond( f.pop_ptr() ) );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( queue_get_cond, vm_queue_t )
	f.push_bool( self->queue->get_cond( f.pop_ptr() ) );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( queue_put_until, vm_queue_t )
	void*				ptr = f.pop_ptr();
	u64					millis = f.pop_long();
	f.push_bool( self->queue->put_until( ptr, millis ) );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( queue_get_until, vm_queue_t )
	void*				ptr = f.pop_ptr();
	u64					millis = f.pop_long();
	f.push_bool( self->queue->get_until( ptr, millis ) );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( method_sleep_millis )
	u64					millis = f.pop_long();
	f.stop_on_demand();
	ithread_t::current()->sleep( millis * 1000L );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( method_sleep_micros )
	u64					micros = f.pop_long();
	f.stop_on_demand();
	ithread_t::current()->sleep( micros );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_threads(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "sleepMillis;l",
		method_sleep_millis, native_func_can_block );

	register_native_function(
		m, rootclss, "sleepMicros;l",
		method_sleep_micros, native_func_can_block );

	class_t*			clss;

	clss = rootclss->find_class( "Thread", 6 );
	if( !clss )
		throw_icarus_error( "Thread class not found" );

	register_native_function(
		m, clss, "create", thread_create );

	register_native_function(
		m, clss, "destroy", thread_destroy );

	register_native_function(
		m, clss, "wake", thread_wake );

	register_native_function(
		m, clss, "block", thread_block );

	register_native_function(
		m, clss, "sleepMillis;l",
		thread_sleep_millis, native_func_can_block );

	register_native_function(
		m, clss, "sleepMicros;l",
		thread_sleep_micros, native_func_can_block );

	register_native_function(
		m, clss, "setName;.String::&", thread_set_name );

	register_native_function(
		m, clss, "name", thread_get_name );
	
	register_native_function(
		m, clss, "setPriority;i", thread_set_priority );

	register_native_function(
		m, clss, "priority", thread_get_priority );

	clss = rootclss->find_class( "Mutex", 5 );
	if( !clss )
		throw_icarus_error( "Mutex class not found" );

	register_native_function(
		m, clss, "create", mutex_create );

	register_native_function(
		m, clss, "destroy", mutex_destroy );

	register_native_function(
		m, clss, "lock", mutex_lock );

	register_native_function(
		m, clss, "unlock", mutex_unlock );

	clss = rootclss->find_class( "Signal", 6 );
	if( !clss )
		throw_icarus_error( "Signal class not found" );

	register_native_function(
		m, clss, "create", signal_create );

	register_native_function(
		m, clss, "destroy", signal_destroy );

	register_native_function(
		m, clss, "signal", signal_signal );

	register_native_function(
		m, clss, "wait", signal_wait );

	register_native_function(
		m, clss, "waitFor;l", signal_wait_for );

	register_native_function(
		m, clss, "waitUntil;l", signal_wait_until );

	clss = rootclss->find_class( "Queue", 5 );
	if( !clss )
		throw_icarus_error( "Queue class not found" );

	s_queue_clss = clss;

	register_native_function(
		m, clss, "make;i;i", queue_make );

	register_native_function(
		m, clss, "create", queue_create );

	register_native_function(
		m, clss, "destroy", queue_destroy );

	register_native_function(
		m, clss, "put;.v*", queue_put );

	register_native_function(
		m, clss, "get;v*", queue_get );

	register_native_function(
		m, clss, "putCond;.v*", queue_put_cond );

	register_native_function(
		m, clss, "getCond;v*", queue_get_cond );

	register_native_function(
		m, clss, "putUntil;.v*;l", queue_put_until );

	register_native_function(
		m, clss, "getUntil;v*;l", queue_get_until );
}

END_MACHINE_NAMESPACE
