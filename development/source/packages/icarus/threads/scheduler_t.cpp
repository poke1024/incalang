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
//	scheduler_t.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "scheduler_t.h"
#include "cleanup_thread_t.h"
//#include "input_thread_t.h"
#include <Windows.h>
#include "vmbind.h"

BEGIN_RT_NAMESPACE

#pragma optimize("", off)

scheduler_t*			s_scheduler = 0;
static u64				s_one_second = 0;

// number of built-in threads
#define BIT_COUNT		1

bool
threaded_environment()
{
	return s_scheduler != 0;
}

void
yield_thread()
{
	if( s_scheduler )
		s_scheduler->yield();
}

void
rt_init_threads()
{
	if( s_scheduler == 0 )
		s_scheduler = new scheduler_t;

	s_one_second = rt_micros_to_cycles( 1000 );
}

void
rt_cleanup_threads()
{
	if( s_scheduler )
	{
		delete s_scheduler;
		s_scheduler = 0;
	}
}

void
rt_enter_threads()
{	
	scheduler_t::run();
}

latency_t&
scheduler_latency()
{
	return s_scheduler->latency();
}

// ---------------------------------------------------------------------------

scheduler_t::scheduler_t()
{
	m_run_thread = 0;
	m_cleanup_thread = 0;
	m_input_thread = 0;

#if RT_MEASURE_YIELD_LATENCY
	m_last_yield_cycles = 0;
#endif

	m_last_system_yield = 0;
}

scheduler_t::~scheduler_t()
{
	if( m_cleanup_thread )
		delete m_cleanup_thread;

	//if( m_input_thread )
	//	delete m_input_thread;
}

void
scheduler_t::run()
{
	if( s_scheduler->m_cleanup_thread == 0 )
		s_scheduler->m_cleanup_thread = new cleanup_thread_t;

	//if( s_scheduler->m_input_thread == 0 )
	//	s_scheduler->m_input_thread = new input_thread_t;

#if RT_WIN_FIBERS
	s_scheduler->m_entry_fiber =
		ConvertThreadToFiber( 0 );
#else
	if( save_cpu_state(
		&s_scheduler->m_entry_cpu_state, 1 ) )
	{	
		return;
	}
#endif

	s_scheduler->yield();
}

void
scheduler_t::yield_to(
	ithread_t*			thread )
{
	if( thread == 0 )
	{
		yield();
		return;
	}

	switch( thread->state() )
	{
		case thread_state_ready:
			break;
			
		case thread_state_sleeping:
			wake_thread( thread );
			break;
			
		default:
			return;
	}
		
	m_ready_queue.remove( thread );
	
	swap_context( thread );
}

void
scheduler_t::yield()
{
#if RT_MEASURE_YIELD_LATENCY
	measure_latency();
#endif

	ithread_t*			active = m_run_thread;
	ithread_t*			eligible;

	while( true )
	{
		check_sleepy();

		eligible = ready_thread( active );

		if( eligible )
			break;

		if( active )
		{
#if RT_MEASURE_YIELD_LATENCY
			m_last_yield_cycles = rt_cycles();
#endif
			return;
		}

		if( m_sleep_queue.count() <= BIT_COUNT )
		{
#if RT_WIN_FIBERS
			if( GetCurrentFiber() == m_entry_fiber )
				return;
			SwitchToFiber( m_entry_fiber );
#else
			restore_cpu_state( &m_entry_cpu_state );
#endif
		}
	}

#if RT_MEASURE_YIELD_LATENCY
	m_last_yield_cycles = rt_cycles();
#endif

	swap_context( eligible );

#if RT_WIN_FIBERS
	if( active )
		active->handle_messages();
#endif
}

#if RT_MEASURE_YIELD_LATENCY
void
scheduler_t::measure_latency()
{
	u64					last;

	if( ( last = m_last_yield_cycles ) == 0 )
		return;

	// if only cleanup thread and input thread
	// are running, then don't measure latency
	if( m_sleep_queue.count() +
		m_ready_queue.count() <= BIT_COUNT )
		return;

	u64					now = rt_cycles();
	u64					delta;

	delta = now - last;
	m_latency.add( delta );

#if RT_MAX_YIELD_LATENCY
	if( delta > RT_MAX_YIELD_LATENCY )
	{
		const char*		name = "unknown";

		if( m_run_thread )
			name = m_run_thread->name();

		SetPriorityClass( GetCurrentProcess(),
			IDLE_PRIORITY_CLASS );
		OutputDebugString( name );
		DebugBreak();
	}
#endif

	yield_to_system( now );
}
#endif

void
scheduler_t::yield_to_system(
	u64					now )
{
	if( machine::vm_in_real_time_mode() )
		return;

	if( now - m_last_system_yield > s_one_second )
	{
		Sleep( 1 );
		m_last_system_yield = rt_cycles();
	}
}

void
scheduler_t::swap_context(
	ithread_t*			eligible )
{
	ithread_t*			active = m_run_thread;

	m_run_thread = eligible;

#if RT_DEBUG
	if( m_ready_queue.contains( m_run_thread ) )
		icarus_trace( "ready queue contains running thread" );
#endif

	if( active )
	{	
		reschedule_ready( active );

#if !RT_WIN_FIBERS
		if( save_cpu_state(
			&active->m_cpu_state, 1 ) )
		{
			active->handle_messages();
		
			return;
		}
#endif

#if RT_DEBUG
		active->check_stack_overflow();
#endif
	}

#if RT_WIN_FIBERS
	SwitchToFiber( eligible->fiber() );
#else
	restore_cpu_state(
		&eligible->m_cpu_state );
#endif
}

void
scheduler_t::enter_thread(
	ithread_t*			thread )
{
	reschedule_ready( thread );
	
#if !RT_WIN_FIBERS
	run_thread( (ithread_t*)save_cpu_state(
		&thread->m_cpu_state, (u32)thread ) );
#endif

	thread->init();
}

void
scheduler_t::reschedule_thread(
	ithread_t*			thread )
{
	if( thread->state() != thread_state_ready )
		return;

	m_ready_queue.remove( thread );

	m_ready_queue.enqueue(
		thread->priority(), thread );
}

void
scheduler_t::sleep_thread(
	ithread_t*			thread,
	u64					micros )
{
	switch( thread->state() )
	{
		case thread_state_ready:
		case thread_state_running:
			break;
		
		default:
			return;
	}

	u64					wake_cycles;
	
	wake_cycles = rt_cycles() +
		rt_micros_to_cycles( micros );

	thread->set_state( thread_state_sleeping );
	thread->set_wake_cycles( wake_cycles );
	m_sleep_queue.enqueue( -(s64)wake_cycles, thread );
	
	if( thread == m_run_thread )
	{
		m_run_thread = 0;
	
#if !RT_WIN_FIBERS
		if( save_cpu_state(
			&thread->m_cpu_state, 1 ) )
		{
			thread->handle_messages();
		
			return;
		}
#endif
		
		yield();
	}
	else
	{
		m_ready_queue.remove( thread );
	}
}

void
scheduler_t::wake_thread(
	ithread_t*			thread )
{
	switch( thread->state() )
	{
		case thread_state_sleeping:
			break;
		
		default:
			return;
	}

	m_sleep_queue.remove( thread );
	reschedule_ready( thread );
}

void
scheduler_t::reschedule_ready(
	ithread_t*			thread )
{
#if RT_DEBUG
	if( thread == m_run_thread )
		icarus_trace( "rescheduling running thread" );
#endif

	thread->set_state(
		thread_state_ready );
	
	m_ready_queue.enqueue(
		thread->priority(), thread );
}

ithread_t*
scheduler_t::ready_thread(
	ithread_t*				active )
{
	ithread_t*				thread;
	
	thread = (ithread_t*)m_ready_queue.top();
	if( thread == 0 )
		return 0;

	if( active &&
		active->priority() >
		thread->priority() )
		return 0;

	m_ready_queue.dequeue();
		
	thread->set_state( thread_state_running );
	
	return thread;
}

void
scheduler_t::check_sleepy()
{
	ithread_t*				thread;
	u64						cycles = rt_cycles();

	while( true )
	{
		thread = (ithread_t*)m_sleep_queue.top();
		if( thread == 0 )
			break;
		
		if( thread->wake_cycles() > cycles )
			break;

		m_sleep_queue.dequeue();
		reschedule_ready( thread );
	}
}

ithread_t*
scheduler_t::current_thread() const
{
	return m_run_thread;
}

void
scheduler_t::run_thread(
	ithread_t*			thread )
{
	if( thread == 0 )
		return;

#if __VISC__ && !RT_WIN_FIBERS
	thread->run();
#else
	try
	{
		thread->run();
	}
	catch(...)
	{
	}
#endif

	s_scheduler->m_cleanup_thread->add( thread );
	
	s_scheduler->m_run_thread = 0;
	s_scheduler->yield();
	
	icarus_fatal( "should not end up here" );
}

#if RT_WIN_FIBERS
VOID CALLBACK
scheduler_t::FiberProc(
	PVOID				lpParameter )
{
	run_thread( (ithread_t*)lpParameter );
}
#endif

END_RT_NAMESPACE
