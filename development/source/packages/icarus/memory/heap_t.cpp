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
//	heap_t.cp				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

// this heap class and related code is heavily
// based on a heap implementation by John Tal.

#include "heap_t.h"
#include <new>
#include <algorithm>

BEGIN_ICARUS_NAMESPACE

#define iswap std::swap

enum {
	heap_empty			= -1,
	heap_top			= 0
};

heap_t::heap_t(
	long				alloc ) :
	
	m_alloc( alloc )
{
	node_t*				heap;

	heap = (node_t*)imalloc(
		alloc * sizeof( node_t ) );
	m_heap = heap;
		
	m_bottom = heap_empty;
}

heap_t::~heap_t()
{
	node_t*				heap = m_heap;

	if( heap )
		ifree( heap );
}

void
heap_t::enqueue(
	priority_t			priority,
	void*				data )
{
	long				bottom = m_bottom + 1;

	if( bottom >= m_alloc )
		expand();
	
	m_bottom = bottom;
	
	node_t*				node = &m_heap[ bottom ];

	node->priority = priority;
	node->data = data;

	up();
}

void*
heap_t::dequeue()
{
	long				bottom = m_bottom;

	if( bottom == heap_empty )
		return 0;

	node_t*				heap = m_heap;
	node_t*				top = &m_heap[ heap_top ];
	void*				data = top->data;
		
	*top = heap[ bottom ];
	m_bottom = --bottom;
	
	down();
	
	return data;
}

void*
heap_t::top()
{
	long				bottom = m_bottom;

	if( bottom == heap_empty )
		return 0;
		
	return m_heap[ heap_top ].data;
}

void
heap_t::remove(
	void*				data )
{
	long				bottom = m_bottom;
	node_t*				heap = m_heap;

	for( long i = heap_top; i <= bottom; i++ )
	{
		if( heap[ i ].data == data )
		{
			if( i < bottom )
				iswap( heap[ i ], heap[ bottom ] );

			m_bottom = --bottom;
			
			if( i == heap_top )
				down();
				
			break;
		}
	}
}

#if ICARUS_SAFE || ICARUS_DEBUG
bool
heap_t::contains(
	void*				data )
{
	long				bottom = m_bottom;
	node_t*				heap = m_heap;

	for( long i = heap_top; i <= bottom; i++ )
	{
		if( heap[ i ].data == data )
			return true;
	}
	
	return false;
}
#endif


long
heap_t::count() const
{
	return m_bottom + 1;
}

void
heap_t::down()
{
	node_t*				heap = m_heap;
	long				bottom = m_bottom;

	long				root2;
	long				max_child;
	long				root = heap_top;
		
	root2 = root << 1;

	//  Process until the root value is in its correct place
	
	while( root2 <= bottom )
	{
		//  Calculate index of the child with the larger value
		
		if( root2 == bottom )
			// only one child 
			max_child = root2;
		else
		{
			//  Select the greater of the 2 children
			if( heap[ root2 ].priority >			// left child 
				heap[ root2 + 1 ].priority )		// right child
				max_child = root2;
			else
				max_child = root2 + 1;
		}
	
		//  If heap property violated, swap values
		
		node_t*			r = &heap[ root ];
		node_t*			c = &heap[ max_child ];
		
		if( r->priority < c->priority )
		{
			iswap( *r, *c );
			
			root = max_child;
			root2 = root << 1;
		}
		else
			break;
	}
}

void
heap_t::up()
{
	long				current;
	long				parent;
	node_t*				heap = m_heap;

	current = m_bottom;
	parent = current >> 1;

	//  move last element up in heap till in correct place

	while( current > ( heap_empty + 1 ) )
	{
		//  compare current and parent

		node_t*			p = &heap[ parent ];
		node_t*			c = &heap[ current ];

		if( p->priority >= c->priority )
			break;
		else
		{   
			iswap( *p, *c );
		
			current = parent;
			parent = parent >> 1;
		}
	}
}

void
heap_t::expand()
{
	long				alloc = m_alloc;
	
	if( alloc < 1 )
		alloc = 2;
	else
		alloc <<= 1;
		
	node_t*				heap = m_heap;
	
	if( heap == 0 )
		heap = (node_t*)imalloc(
			alloc * sizeof( node_t* ) );
	else
		heap = (node_t*)irealloc( heap,
			alloc * sizeof( node_t ) );
	
	if( heap == 0 )
		throw std::bad_alloc();

	m_heap = heap;
	m_alloc = alloc;
}

END_ICARUS_NAMESPACE
