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
//	fast_heap.h									    ©1999-2001 Bernhard Liebl.
// ===========================================================================

#ifndef FAST_HEAP_H
#define FAST_HEAP_H

//#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class fast_heap {
public:
						fast_heap(
							unsigned int		chunk_size,
							long				initial_allocation = 0,
							bool				debug = false );
							
						~fast_heap();

	void*				alloc();
	
	void				free(
							void*				block );

protected:
	void*				expand();

	u08*				m_address;
	void*				m_free_list;
	
	u16					m_chunk_size;
	u32					m_element_count;
	
	bool				m_debug;
};

#if ICARUS_DEBUG
const u32				kBlockGuard = 0xAF9361C3;
#endif

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ alloc
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

inline void*
fast_heap::alloc()
{
#if ICARUS_DEBUG
	void*					free_entry;

	if( m_debug )
		free_entry = new u08[ m_chunk_size ];
	else
	{
		free_entry = m_free_list;
		if( !free_entry )
			free_entry = expand();
		m_free_list = *(void**)free_entry;
	}
	
	if( s_mem_track )
		s_mem_alloc += m_chunk_size;
#else
	void*					free_entry = m_free_list;
	
	if( !free_entry )
		free_entry = expand();
		
	m_free_list = *(void**)free_entry;
#endif

#if ICARUS_DEBUG
	std::memset( free_entry, 0xfe, m_chunk_size );

	*(u32*)free_entry = kBlockGuard;
	u08* ptr = (u08*)free_entry;
	ptr += sizeof( u32 );
	free_entry = ptr;
	ptr += m_chunk_size - sizeof( u32 ) * 2;
	*(u32*)ptr = kBlockGuard;
#endif

	return free_entry;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ Delete
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

inline void
fast_heap::free(
	void*					block )
{
#if ICARUS_DEBUG
	block = ( (u08*)block ) - sizeof( u32 );
	if( *(u32*)block != kBlockGuard )
		throw_icarus_error( "fast heap block corrupt" );
	u08* ptr = (u08*)block;
	ptr += m_chunk_size - sizeof( u32 );
	if( *(u32*)ptr != kBlockGuard )
		throw_icarus_error( "fast heap block corrupt" );
#endif

#if ICARUS_DEBUG
	std::memset( block, 0xfe, m_chunk_size );
#endif

#if ICARUS_DEBUG
	if( m_debug )
		delete[] block;
	else
	{
		*(void**)block = m_free_list;
		m_free_list = block;
	}
	
	if( s_mem_track )
		s_mem_alloc -= m_chunk_size;
#else
	*(void**)block = m_free_list;
	
	m_free_list = block;
#endif
}

END_ICARUS_NAMESPACE

#endif

