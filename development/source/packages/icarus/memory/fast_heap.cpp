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
//	fast_heap.cp								    ©1999-2001 Bernhard Liebl.
// ===========================================================================

#include "fast_heap.h"
#include "st_mem_track.h"

BEGIN_ICARUS_NAMESPACE

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ fast_heap
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

fast_heap::fast_heap(
	unsigned int		chunk_size,
	long				initial_allocation,
	bool				debug ) :
	
	m_address( 0 ),
	m_chunk_size( chunk_size ),
	m_debug( debug )
{
	st_mem_track		track( false );

#if ICARUS_DEBUG
	chunk_size += 2 * sizeof( u32 );
	m_chunk_size = chunk_size;
#endif

	if( chunk_size < sizeof( void* ) )
		throw_icarus_error( "chunk size too small" );

	/*else if( chunk_size >= 256 )
		throw_icarus_error( "chunk size too big" );*/

	long				count;

	if( initial_allocation == 0 )
		count = 8192L / chunk_size;
	else
		count = initial_allocation;
		
	if( count < 1 )
		count = 1;
	
	m_address = new u08[ count * chunk_size + sizeof( void* ) ];

#if ICARUS_DEBUG
	std::memset( m_address, 0xfe,
		count * chunk_size + sizeof( void* ) );
#endif
	
	m_element_count = count;
	
	u08*				p = m_address;
	
	*(void**)p = 0;
	p += sizeof( void* );
	m_free_list = p;
	
	for( long i = count - 1; i; i-- )
	{
		u08*			q = p + chunk_size;
	
		*(void**)p = q;
		p = q;
	}
	
	*(void**)p = 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ ~fast_heap
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

fast_heap::~fast_heap()
{
	st_mem_track		track( false );

	u08*				p = m_address;
	u08*				next;

	do {
		next = *(u08**)p;
		delete[] p;
		p = next;
	} while( p );
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ expand
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

void*
fast_heap::expand()
{
	st_mem_track		track( false );

	long				old_count = m_element_count;
	long				new_count = old_count * 2;
	unsigned int		chunk_size = m_chunk_size;
	long				old_size = old_count * chunk_size;

	u08*				new_address;
	long				delta_count = new_count - old_count;

	new_address = new u08[ delta_count * chunk_size + sizeof( void* ) ];

#if ICARUS_DEBUG
	std::memset( new_address, 0xfe,
		delta_count * chunk_size + sizeof( void* ) );
#endif
		
	u08*				p;

	p = new_address;

	*(void**)p = m_address;
	p += sizeof( void* );
	
	u08*				free_list = p;
	
	for( long i = delta_count - 1; i; i-- )
	{
		u08*			q = p + chunk_size;
	
		*(void**)p = q;
		p = q;
	}

	*(void**)p = 0;

	m_address = new_address;
	m_element_count = new_count;

	return free_list;
}

END_ICARUS_NAMESPACE
