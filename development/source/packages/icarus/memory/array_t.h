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
//	array_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARRAY_H
#define ARRAY_H

//#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class array_t {
public:
							array_t(
								long				size,
								long				alloc = 0 );

	virtual					~array_t();
	
	long					count() const;
						
	const void*				operator[](
								long				index ) const;

	void					insert_at(
								long				index,
								const void*			item );
								
	void					append(
								const void*			item );
								
	void					remove(
								long				index );

	void					clear();

protected:
	void					grow();
	
	u32						m_elem_size;
	u08*					m_elems;
	u32						m_count;
	u32						m_alloc;
};

// ---------------------------------------------------------------------------

inline const void*
array_t::operator[](
	long				index ) const
{
	return &m_elems[ index * m_elem_size ];
}

inline long
array_t::count() const
{
	return m_count;
}

END_ICARUS_NAMESPACE

#endif

