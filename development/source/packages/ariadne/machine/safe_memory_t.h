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
//	safe_memory_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef SAFE_MEMORY_H
#define SAFE_MEMORY_H

//#pragma once

#include "machine.h"

#if CMA_MACHINE

BEGIN_MACHINE_NAMESPACE

class memory_t {
public:
						memory_t(
							size_t				size );
						
						~memory_t();

	u08*				alloc(
							size_t				size );

	void				free(
							void*				block );

	u08*				m_memlo;
	u08*				m_memhi;
	
private:
	struct node_t {
		node_t*			next;
		size_t			size;
	};

	node_t*				m_free_list;
	u08*				m_base;
	u08*				m_mark;
};

END_MACHINE_NAMESPACE

#else

BEGIN_MACHINE_NAMESPACE

class memory_t {
public:
	u08*				alloc(
							size_t				size ) const;

	void				free(
							void*				block ) const;
};

inline u08*
memory_t::alloc(
	size_t				size ) const
{
	return (u08*)imalloc( size );
}

inline void
memory_t::free(
	void*				block ) const
{
	ifree( block );
}

END_MACHINE_NAMESPACE

#endif

#endif

