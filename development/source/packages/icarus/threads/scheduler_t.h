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
//	scheduler_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_SCHEDULER_H
#define ICARUS_SCHEDULER_H

//#pragma once

#include "rt.h"
#include "heap_t.h"
#include "thread_t.h"
#include "cpu_state_t.h"
#include "latency_t.h"

#if RT_WIN_FIBERS
#define WINVER 0x0400
#define _WIN32_WINNT 0x0400
#define _WIN32_WINDOWS 0x0410
#include <Windows.h>
#endif

BEGIN_RT_NAMESPACE

class cleanup_thread_t;

class scheduler_t {
public:
							scheduler_t();

	virtual					~scheduler_t();

	static void				run();

#if RT_MEASURE_LATENCY
	latency_t&				latency()
								{ return m_latency; }
#endif

#if RT_WIN_FIBERS
	static VOID CALLBACK	FiberProc(
								PVOID				lpParameter );
#endif

protected:
	friend class			ithread_t;	
	friend void				yield_thread();

	void					yield_to(
								ithread_t*			thread );
								
	void					yield();

	void					enter_thread(
								ithread_t*			thread );

	void					reschedule_thread(
								ithread_t*			thread );

	void					sleep_thread(
								ithread_t*			thread,
								u64					micros );

	void					wake_thread(
								ithread_t*			thread );

	void					reschedule_ready(
								ithread_t*			thread );

	ithread_t*				ready_thread(
								ithread_t*			active );

	void					check_sleepy();

	ithread_t*				current_thread() const;

private:
#if RT_MEASURE_YIELD_LATENCY
	void					measure_latency();
#endif

	void					yield_to_system(
								u64					now );

	void					swap_context(
								ithread_t*			eligible );

	static void				run_thread(
								ithread_t*			thread );
	
	heap_t					m_ready_queue;
	heap_t					m_sleep_queue;
	ithread_t*				m_run_thread;
	cleanup_thread_t*		m_cleanup_thread;
	ithread_t*				m_input_thread;

#if RT_WIN_FIBERS
	LPVOID					m_entry_fiber;
#else
	cpu_state_t				m_entry_cpu_state;
#endif

#if RT_MEASURE_LATENCY
	latency_t				m_latency;
#endif

#if RT_MEASURE_YIELD_LATENCY
	u64						m_last_yield_cycles;
#endif

	u64						m_last_system_yield;
};

extern scheduler_t*			s_scheduler;

void
rt_init_threads();

void
rt_cleanup_threads();

void
rt_enter_threads();

latency_t&
scheduler_latency();

END_RT_NAMESPACE

#endif

