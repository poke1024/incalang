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
//	array_t.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "array_t.h"
#include <cstring>

BEGIN_ICARUS_NAMESPACE

using namespace std;

array_t::array_t(
	long				size,
	long				alloc )
{
	m_elem_size = size;
	m_count = 0;
	m_alloc = alloc;
	
	if( m_alloc )
		m_elems = new u08[ m_alloc * m_elem_size ];
	else
		m_elems = 0;
}

array_t::~array_t()
{
	if( m_elems )
		delete[] m_elems;
}

void
array_t::insert_at(
	long				index,
	const void*			item )
{
	const u32			count = m_count;

	if( index < 0 )
		index = 0;
	else if( index > count )
		index = count;
	
	if( count == m_alloc )
		grow();
		
	u08*				elem = m_elems;
	const u32			elem_size = m_elem_size;
	u08*				e0 = &elem[ index * elem_size ];
		
	memmove(
		e0 + elem_size,
		e0,	( count - index ) * elem_size );
		
	memcpy(
		e0, item, elem_size );
	
	m_count = count + 1;
}

void
array_t::append(
	const void*			item )
{
	insert_at( count(), item );
}

void
array_t::remove(
	long				index )
{
	const u32			count = m_count;

	if( index < 0 || index >= count )
		return;

	u08*				elem = m_elems;
	const u32			elem_size = m_elem_size;
	u08*				e0 = &elem[ index * elem_size ];

	memmove(
		e0,
		e0 + elem_size,
		( count - index - 1 ) * elem_size );
	
	m_count = count - 1;
}

void
array_t::clear()
{
	m_count = 0;
}

void
array_t::grow()
{
	u08*				elem;
	const u32			elem_size = m_elem_size;

	if( m_alloc == 0 )
		m_alloc = 1;
	else
		m_alloc *= 2;
		
	elem = new u08[ m_alloc * elem_size ];
	if( m_elems )
	{
		memcpy( elem, m_elems, m_count * elem_size );
		delete[] m_elems;
	}
	m_elems = elem;
}

END_ICARUS_NAMESPACE
