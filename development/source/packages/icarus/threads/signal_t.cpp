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
//	signal_t.cpp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "signal_t.h"
#include "thread_t.h"
#include "mutex_t.h"

BEGIN_RT_NAMESPACE

signal_t::signal_t()
{
	m_signal_flag = false;
	m_nodes = 0;
}

signal_t::~signal_t()
{
	if( m_signal_flag )
		icarus_fatal( "signal destroyed while signaling" );

	signal();
}

void
signal_t::signal()
{
	if( m_signal_flag )
		return;

	m_signal_flag = true;

	node_t*				node;

	node = m_nodes;
	m_nodes = 0;

	while( node )
	{
		node_t*			next;
		ithread_t*		thread;

		next = node->next;
		thread = node->thread;
		node->thread = 0;
		thread->wake();
		node = next;
	}

	m_signal_flag = false;
}

void
signal_t::wait()
{
	if( m_signal_flag )
		return;

	node_t*				node = new node_t;

	node->next = m_nodes;
	node->thread = ithread_t::current();
	m_nodes = node;

	do {
		ithread_t::current()->block();
	} while( node->thread );

	delete node;
}

bool
signal_t::wait_until(
	u64					micros )
{
	if( m_signal_flag )
		return true;

	node_t*				node = new node_t;

	node->next = m_nodes;
	node->thread = ithread_t::current();
	m_nodes = node;

	do {
		u64				now = rt_micros();

		if( now >= micros )
			return false;
		
		ithread_t::current()->sleep( now - micros );

	} while( node->thread );

	delete node;

	return true;
}

void
signal_t::wait(
	 mutex_t*			mutex )
{
	mutex->safe_unlock();
	wait();
	mutex->lock();
}

END_RT_NAMESPACE
