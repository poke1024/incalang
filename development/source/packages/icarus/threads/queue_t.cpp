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
//	queue_t.cpp				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "queue_t.h"
#include <string.h>

BEGIN_RT_NAMESPACE

queue_t::queue_t(
	size_t				data_size,
	unsigned long		capacity )
{
	m_nodes = 0;
	m_link = &m_nodes;
	m_count = 0;
	
	m_data_size = data_size;
	m_node_size = sizeof( node_t ) - 1 + data_size;
	m_capacity = capacity;
}

queue_t::~queue_t()
{
	node_t*				node = m_nodes;
	node_t*				next;

	while( node )
	{
		next = node->next;
		ifree( node );
		node = next;
	}
}

bool
queue_t::put_cond(
   void*				data )
{
	if( m_capacity == 0 ||  m_count < m_capacity )
	{
		put_prim( data );
		return true;
	}

	return false;
}

void
queue_t::put(
	void*				data )
{
	if( m_capacity > 0 )
	{
		while( m_count == m_capacity )
			m_put_signal.wait();
	}

	put_prim( data );
}

bool
queue_t::put_until(
	void*				data,
	u64					micros )
{
	if( m_capacity > 0 )
	{
		if( m_count == m_capacity )
		{
			while( true )
			{
				m_put_signal.wait_until( micros );
				if( m_count < m_capacity )
					break;
				if( rt_micros() >= micros )
					return false;
			}
		}
	}

	put_prim( data );
	return true;
}

bool
queue_t::get_cond(
	void*				data )
{
	node_t*				node;

	if( ( node = m_nodes ) == 0 )
		return false;

	if( ( m_nodes = node->next ) == 0 )
		m_link = &m_nodes;

	m_count--;
	m_put_signal.signal();

	memcpy( data, node->data, m_data_size );

	ifree( node );

	return true;
}

void
queue_t::get(
	void*				data )
{
	while( m_count < 1 )
		m_get_signal.wait();

	if( !get_cond( data ) )
		icarus_fatal( "internal error" );
}

bool
queue_t::get_until(
	void*				data,
	u64					micros )
{
	if( m_count < 1 )
	{
		while( true )
		{
			m_get_signal.wait_until( micros );
			if( m_count >= 1 )
				break;
			if( rt_micros() >= micros )
				return false;
		}
	}

	if( !get_cond( data ) )
		icarus_fatal( "internal error" );

	return true;
}

void
queue_t::put_prim(
   void*				data )
{
	node_t*				node;

	node = (node_t*)imalloc( m_node_size );
	memcpy( node->data, data, m_data_size );
	
	node->next = 0;
	*m_link = node;

	m_count++;
	m_get_signal.signal();
}

END_RT_NAMESPACE
