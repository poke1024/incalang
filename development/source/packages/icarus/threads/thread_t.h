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
//	thread_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_THREAD_H
#define ICARUS_THREAD_H

//#pragma once

#include "rt.h"
#include "cpu_state_t.h"
#include "latency_t.h"

#if RT_WIN_FIBERS
#define WINVER 0x0400
#define _WIN32_WINNT 0x0400
#define _WIN32_WINDOWS 0x0410
#include <Windows.h>
#endif

BEGIN_RT_NAMESPACE

enum thread_state_t {
	thread_state_none,
	thread_state_ready,
	thread_state_running,
	thread_state_sleeping
};

enum {
	flag_NoUseOfFPU = 1
};

const int					msg_KillThread			= -1000;
const int					msg_ThreadError			= -1001;

const int					latency_AutoWake		= 0;
const int					latency_ForceWake		= 1;


class ithread_t {
public:
							ithread_t(
								const char*			name,
								long				priority = 0,
								size_t				stack_size = 0,
								int					flags = 0 );
	
	static void				yield();
	
	static void				yield_to(
								ithread_t*			thread );
	
	static ithread_t*		current();
	
	void					put_message(
								int					message,
								void*				param );

	void					sleep(
								u64					micros );

	void					sleep_until(
								u64					micros );

	void					block();
	
	void					wake();
	
	void					kill();

	virtual void			error(
								const char*			s );

	long					priority() const;

	void					set_priority(
								long				priority );

	const char*				name() const;

	void					set_name(
								const char*			name );

#if RT_MEASURE_THREAD_LATENCY
	const latency_t&		latency(
								int					type ) const;
#endif

protected:
	virtual void			run() = 0;

	virtual void			receive_message(
								int					message,
								void*				param );

// ---------------------------------------------------------------------------

	friend class			scheduler_t;
	friend class			cleanup_thread_t;
	
	void					init();

	void					check_stack_overflow();
	
	void					handle_messages();
	
	thread_state_t			state() const;
	
	void					set_state(
								thread_state_t		state );
								
	void					set_wake_cycles(
								u64					cycles );
								
	u64						wake_cycles() const;
	
	virtual					~ithread_t();

	virtual void			kill_self();

	virtual void			error_self();

	istring					m_error;

#if RT_WIN_FIBERS
	LPVOID					fiber() const
								{ return m_fiber; }
#endif

private:
	struct message_t {
		message_t*			next;
		int					message;
		void*				param;
	};

	char*					m_name;

#if RT_WIN_FIBERS
	LPVOID					m_fiber;
#else
	u08*					m_stack_top;
	u08*					m_stack_bottom;
	cpu_state_t				m_cpu_state;
#endif
	thread_state_t			m_state;
	s32						m_priority;
	u64						m_wake_cycles;
	message_t*				m_messages;

#if RT_MEASURE_LATENCY
	u64						m_wake_latency;
#endif

#if RT_MEASURE_THREAD_LATENCY
	latency_t				m_latencies[ 2 ];
#endif
};

class kill_thread_t {
};

END_RT_NAMESPACE

#endif

