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
//	mutex_t.cpp				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "mutex_t.h"
#include "thread_t.h"

#define PREFER_HIGH_PRIO_THREADS 1

BEGIN_RT_NAMESPACE

mutex_t::mutex_t()
{
	m_sem = 0;
	m_wait = 0;
}

mutex_t::~mutex_t()
{
	while( m_sem > 0 )
		unlock();
}

void
mutex_t::lock()
{
	if( ++m_sem < 0 )
		icarus_fatal( "mutex semaphore count exceeded" );

	if( m_sem == 1 )
		return;

	node_t*				node = new node_t;

	node->next = m_wait;
	node->thread = ithread_t::current();
	m_wait = node;

	do {
		ithread_t::current()->block();
	} while( node->thread );

	delete node;
}

void
mutex_t::unlock()
{
	safe_unlock();
	ithread_t::yield();
}

void
mutex_t::safe_unlock()
{
	// this unlock guarantees that
	// no thread switch takes place

	if( --m_sem < 0 )
		icarus_fatal( "mutex semaphore unbalanced unlock" );

	if( m_sem == 0 )
		return;

	node_t**			link = &m_wait;
	node_t*				node = *link;

	long				bestprio = node->thread->priority();
	node_t**			bestlink = link;

#if PREFER_HIGH_PRIO_THREADS
	// chooses highest priority thread to unblock - takes O(n)
	// to find this thread. starving of low priority threads
	// quite possible.

	while( true )
	{
		link = &node->next;
		if( ( node = *link ) == 0 )
			break;

		long			prio = node->thread->priority();

		if( prio > bestprio )
		{		
			bestlink = link;
			prio = bestprio;
		}
	}
#endif

	node = *bestlink;
	*bestlink = node->next;

	ithread_t*			thread;

	thread = node->thread;
	node->thread = 0;

	thread->wake();
}

END_RT_NAMESPACE
