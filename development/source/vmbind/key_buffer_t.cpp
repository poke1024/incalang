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
//	key_buffer_t.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "key_buffer_t.h"
#include "os_core.h"

BEGIN_MACHINE_NAMESPACE

const long				expire_millis = 1000;
const int				max_buffer_size = 16;

key_buffer_t::key_buffer_t() :

	m_nodes( 0 ),
	m_count( 0 )
{
	m_link = &m_nodes;
}

key_buffer_t::~key_buffer_t()
{
	while( m_nodes )
		pop();
}

void
key_buffer_t::submit_key(
	s32					c )
{
	u64					timestamp;

	timestamp = os_time_millis();
	remove_expired( timestamp );
	
	if( m_count >= max_buffer_size )
		return;
		
	node_t*				node = new node_t;
	
	node->next = 0;
	node->timestamp = timestamp + expire_millis;
	node->c = c;
	
	*m_link = node;
	m_link = &node->next;
	
	m_count += 1;
}

s32
key_buffer_t::fetch_key()
{
	u64					timestamp;

	timestamp = os_time_millis();
	remove_expired( timestamp );

	node_t*				node = m_nodes;
	
	if( not node )
		return 0;

	s32					c = node->c;
	
	pop();
		
	return c;
}

void
key_buffer_t::remove_expired(
	u64					timestamp )
{
	while( m_nodes )
	{
		if( timestamp <= m_nodes->timestamp )
			break;

		pop();
	}
}

void
key_buffer_t::pop()
{
	node_t*				node = m_nodes;

	if( not node )
		return;
	
	node_t*				next = node->next;

	m_nodes = next;
	delete node;
		
	if( next == 0 )
		m_link = &m_nodes;
		
	m_count -= 1;
}

END_MACHINE_NAMESPACE
