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
//	thread_t.cpp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "thread_t.h"
#include "scheduler_t.h"

BEGIN_RT_NAMESPACE

#if __VISC__
const u64				block_micros = 3153600000000000000;
#else
const u64               block_micros = 3153600000000000000ULL;
#endif
						// 100 years in microseconds:
						// 1000L * 1000L * 60L * 60L * 24L * 365L * 1000L

// latency_AutoWake measures the time that was needed for a sleeping thread
// to get active automatically after the sleep quantum was over
// latency_ForceWake gives the time that was needed for a blocked thread to
// get active after it has been woke up by force

ithread_t::ithread_t(
    const char*			name,
	long				priority,
	size_t				stack_size,
	int					flags )
{
	if( stack_size == 0 )
		stack_size = RT_DEFAULT_STACK_SIZE;

	m_state = thread_state_ready;
	m_messages = 0;

	m_name = istrdup( name );
	m_priority = priority;

#if !RT_WIN_FIBERS
	m_stack_top = new u08[ stack_size ];
	m_stack_bottom = m_stack_top + stack_size;
	
	if( flags & flag_NoUseOfFPU )
		m_cpu_state.savefpu = 0;
	else
		m_cpu_state.savefpu = 1;
#endif
	
	scheduler_t*		scheduler = s_scheduler;
	
	if( !scheduler )
		icarus_fatal( "threads not initialized" );
	
	scheduler->enter_thread( this );

#if RT_WIN_FIBERS
	m_fiber = CreateFiber( stack_size,
		scheduler_t::FiberProc, this );
#endif
}

ithread_t::~ithread_t()
{
#if RT_WIN_FIBERS
	DeleteFiber( m_fiber );
#else
	delete[] m_stack_top;
#endif

	message_t*			node = m_messages;
	message_t*			next;
	
	while( node )
	{
		next = node->next;
		delete node;
		node = next;
	}

	ifree( m_name );
	m_name = 0;
}

void
ithread_t::yield()
{
	s_scheduler->yield();
}

void
ithread_t::yield_to(
	ithread_t*			thread )
{
	s_scheduler->yield_to( thread );
}

ithread_t*
ithread_t::current()
{
	scheduler_t*		scheduler;

	if( ( scheduler = s_scheduler ) == 0 )
		return 0;

	return scheduler->current_thread();
}

void
ithread_t::put_message(
	int					message,
	void*				param )
{
	message_t*			node;

	node = new message_t;
	node->message = message;
	node->param = param;

	node->next = m_messages;
	m_messages = node;
	
	wake();
}

void
ithread_t::receive_message(
	int					message,
	void*				/*param*/ )
{
	switch( message )
	{
		case msg_KillThread:
			kill_self();
			break;

		case msg_ThreadError:
			error_self();
			break;
	}
}

void
ithread_t::sleep(
	u64					micros )
{
#if RT_MEASURE_LATENCY
	u64					now = rt_cycles();
	m_wake_latency = 0;
#endif

	s_scheduler->sleep_thread( this, micros );

#if RT_MEASURE_LATENCY
	u64					latency;

	if( m_wake_latency )
	{
		latency = rt_cycles() - m_wake_latency;
#if RT_MEASURE_THREAD_LATENCY
		m_latencies[ latency_ForceWake ].add( latency );
#endif
		scheduler_latency().add( latency );
	}
	else
	{
		latency = rt_cycles() - now -
			rt_micros_to_cycles( micros );
#if RT_MEASURE_THREAD_LATENCY
		m_latencies[ latency_AutoWake ].add( latency );
#endif
		scheduler_latency().add( latency );
	}
#endif
}

void
ithread_t::sleep_until(
	u64					micros )
{
	u64					now = rt_micros();

	if( micros <= now )
		return;

	sleep( micros - now );
}

void
ithread_t::block()
{
	s_scheduler->sleep_thread( this, block_micros );

#if RT_MEASURE_LATENCY
	u64					latency;
	
	latency = rt_cycles() - m_wake_latency;
#if RT_MEASURE_THREAD_LATENCY
	m_latencies[ latency_ForceWake ].add( latency );
#endif
	scheduler_latency().add( latency );
#endif
}

void
ithread_t::wake()
{
#if RT_MEASURE_LATENCY
	if( m_state == thread_state_sleeping )
		m_wake_latency = rt_cycles();
#endif

	s_scheduler->wake_thread( this );
}

void
ithread_t::kill()
{
	put_message( msg_KillThread, 0 );
}

void
ithread_t::error(
	const char*			s )
{
	if( m_error.length() > 0 )
		return;
	
	m_error = s;

	if( ithread_t::current() == this )
		error_self();
	else
		put_message( msg_ThreadError, 0 );
}

void
ithread_t::init()
{
#if !RT_WIN_FIBERS
	m_cpu_state.esp =
		(u32)m_stack_bottom - 128;	
#endif
}

void
ithread_t::check_stack_overflow()
{
#if RT_DEBUG && !RT_WIN_FIBERS
	if( m_cpu_state.esp < (u32)m_stack_top + 1024 )
		icarus_fatal( "thread stack overflow" );
#endif
}

void
ithread_t::handle_messages()
{
	message_t*			node = m_messages;
	
	while( node )
	{
		m_messages = node->next;
		receive_message( node->message, node->param );
		delete node;
		node = m_messages;
	}
}

thread_state_t
ithread_t::state() const
{
	return m_state;
}

void
ithread_t::set_state(
	thread_state_t		state )
{
	m_state = state;
}

void
ithread_t::set_wake_cycles(
	u64					cycles )
{
	m_wake_cycles = cycles;
}								

u64
ithread_t::wake_cycles() const
{
	return m_wake_cycles;
}

const char*
ithread_t::name() const
{
	return m_name;
}

void
ithread_t::set_name(
	const char*			name )
{
	ifree( m_name );
	m_name = 0;
	m_name = istrdup( name );
}

long
ithread_t::priority() const
{
	return m_priority;
}

void
ithread_t::set_priority(
	long				priority )
{
	if( m_priority == priority )
		return;

	m_priority = priority;
	s_scheduler->reschedule_thread( this );
}

#if RT_MEASURE_THREAD_LATENCY
const latency_t&
ithread_t::latency(
	int					type ) const
{
	return m_latencies[ type ];
}
#endif

void
ithread_t::kill_self()
{
	throw kill_thread_t();
}

void
ithread_t::error_self()
{
	istring				buf;
	buf = m_error.c_str();
	m_error.clear();
	icarus_error( buf.c_str() );
}

END_RT_NAMESPACE
