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
//	safe_memory_t.cp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine.h"
#include "safe_memory_t.h"

BEGIN_MACHINE_NAMESPACE

#if CMA_MACHINE

const int				GUARD_SIZE = 64;

memory_t::memory_t(
	size_t				size )
{
	m_base = new u08[ size + 2 * GUARD_SIZE ];

	m_memlo = m_base + GUARD_SIZE;
	m_memhi = m_memlo + size;
	m_mark = m_memlo;
	m_free_list = 0;
}

memory_t::~memory_t()
{
	delete[] m_base;
}

u08*
memory_t::alloc(
	size_t				size )
{
	if( size < 1 )
		return 0;

	u08*				memlo = m_memlo;
	u08*				memhi = m_memhi;

	node_t**			link = &m_free_list;
	node_t*				node = *link;
	
	while( node )
	{
		unsigned long	nodesize = node->size;
	
		if( (u08*)node < memlo || (u08*)node + nodesize > memhi )
			throw_icarus_error( "heap corrupt" );
	
		if( nodesize >= size )
		{			
			long		left = nodesize - size;
			u08*		p = (u08*)node;
			
			if( left > sizeof( node_t ) )
			{
				node->size = left - sizeof( node_t );
				p += left;
				( (node_t*)p )->size = size;
			}			
			else
				*link = node->next;
			
			return p + sizeof( node_t );
		}
	
		link = &node->next;
		node = *link;
	}

	u08*				lo;
	u08*				hi;
	
	lo = m_mark;
	hi = lo + size + sizeof( node_t );
	
	if( hi > memhi )
		throw std::bad_alloc();
		
	m_mark = hi;
	
	( (node_t*)lo )->size = size;

	return lo + sizeof( node_t );
}

void
memory_t::free(
	void*				block )
{
	if( block == 0 )
		return;
		
	u08*				memlo = m_memlo;
	u08*				memhi = m_memhi;

	u08*				p = (u08*)block;
	
	p -= sizeof( node_t );

	if( p < memlo || p + sizeof( node_t ) > memhi )
		throw_icarus_error( "heap corrupt" );

	node_t*				node = (node_t*)p;
	long				size = node->size;

	if( p + size > memhi || size < 1 )
		throw_icarus_error( "heap corrupt" );
		
	node->next = m_free_list;
	m_free_list = node;
}

#endif

END_MACHINE_NAMESPACE
