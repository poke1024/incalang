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
//	heap_pool_t.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "heap_pool_t.h"

BEGIN_MACHINE_NAMESPACE

inline int
find_bucket(
	size_t				length )
{
	size_t				value = length;
	int					i = 0;
	int					r = 0;
	
	while( value != 1 )
	{
		r |= value & 1;
		value >>= 1;
		i++;
	}
	
	return i + r;
}

heap_pool_t::heap_pool_t(
	const size_t*		distribution,
	int					dimensions,
	memory_t*			memory ) :
	
	m_memory( memory )
{
	m_buckets = new u08*[ dimensions ];
	m_count = new size_t[ dimensions ];
	m_distribution = new size_t[ dimensions ];
	m_dimensions = dimensions;

	imemset( m_buckets, 0, dimensions * sizeof( void* ) );
	imemset( m_count, 0, dimensions * sizeof( size_t ) );
	imemcpy( m_distribution, distribution,
		dimensions * sizeof( size_t ) );
}

heap_pool_t::~heap_pool_t()
{
	delete[] m_buckets;
	delete[] m_count;
	delete[] m_distribution;
}

void*
heap_pool_t::alloc(
	long				length )
{
	if( length <= 0 )
		return 0;

	int					bucket = find_bucket( length );

	if( bucket >= m_dimensions )
		return m_memory->alloc( length );
		
	u08**				link = &m_buckets[ bucket ];
	u08*				block = *link;
	
	if( block )
	{
		m_count[ bucket ] -= 1;

		*link = *(u08**)block;
		return block;
	}

	long				size;
	
	size = 1L << bucket;
	if( size < sizeof( void* ) )
		size = sizeof( void* );
	
	return m_memory->alloc( size );
}

void
heap_pool_t::free(
	void*				block,
	long				length )
{
	if( length <= 0 )
		return;

	int					bucket = find_bucket( length );

	if( bucket >= m_dimensions )
	{
		m_memory->free( block );
		return;
	}
	
	long				count = m_count[ bucket ];
	
	if( count == m_distribution[ bucket ] )
	{
		m_memory->free( block );
		return;
	}
	
	u08**				link = &m_buckets[ bucket ];

	*(u08**)block = *link;
	*link = (u08*)block;
	
	m_count[ bucket ] = count + 1;
}

END_MACHINE_NAMESPACE
