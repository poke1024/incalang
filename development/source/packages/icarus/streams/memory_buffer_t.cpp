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
//	memory_buffer_t.cp		   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "memory_buffer_t.h"

BEGIN_ICARUS_NAMESPACE

memory_buffer_t::memory_buffer_t()
{
	m_alloc = 8;
	m_size = 0;
	m_mark = 0;
	m_data = new u08[ m_alloc ];
}

memory_buffer_t::~memory_buffer_t()
{
	delete[] m_data;
}

void
memory_buffer_t::seek(
	fpos_t				where,
	seek_mode			mode )
{
	fpos_t				mark;

	switch( mode )
	{
		case seek_mode_begin:
			mark = where;
			break;
		
		case seek_mode_relative:
			mark = m_mark + where;
			break;
	
		case seek_mode_end:
			mark = m_size - where;
			break;
	}
	
	if( mark < 0 )
		mark = 0;
	else if( mark > m_size )
		mark = m_size;
		
	m_mark = mark;
}
							
fpos_t
memory_buffer_t::tell()
{
	return m_mark;
}

fpos_t
memory_buffer_t::length()
{
	return m_size;
}

fpos_t
memory_buffer_t::read_some_bytes(
	void*				buffer,
	fpos_t				count )
{
	fpos_t				end_count = m_size - m_mark;
	
	if( count > end_count )
		count = end_count;

	std::memcpy( buffer, &m_data[ m_mark ], count );
	m_mark += count;

	return count;
}

fpos_t
memory_buffer_t::write_some_bytes(
	const void*			buffer,
	fpos_t				count )
{
	if( m_mark + count > m_size )
		expand( m_mark + count );
		
	std::memcpy( &m_data[ m_mark ], buffer, count );
	m_mark += count;
		
	return count;
}

void
memory_buffer_t::flush()
{
}

void
memory_buffer_t::clear()
{
	m_size = 0;
}

void
memory_buffer_t::shrink()
{
	fpos_t				alloc = 8;
	const fpos_t		size = m_size;
	
	while( alloc < size )
		alloc *= 2;
		
	u08*				data;
			
	data = new u08[ alloc ];
	std::memcpy( data, m_data, size );

	delete[] m_data;
	m_data = data;
	m_alloc = alloc;
}

u08*
memory_buffer_t::access()
{
	return &m_data[ m_mark ];
}

void
memory_buffer_t::expand(
	fpos_t				new_size )
{
	fpos_t				alloc = m_alloc;

	if( new_size > alloc )
	{
		const fpos_t	old_size = m_size;

		while( alloc < new_size )
		{
			alloc <<= 1;
			if( !alloc )
				throw_icarus_error( "memory buffer overflow" );
		}
			
		u08*			data;
			
		data = new u08[ alloc ];
		std::memcpy( data, m_data, old_size );
	
		delete[] m_data;
		m_data = data;
		m_alloc = alloc;
	}
	
	m_size = new_size;
}

void
memory_buffer_t::reserve(
	fpos_t				size )
{
	fpos_t				alloc = m_alloc;
	
	if( size < alloc )
		return;

	while( alloc < size )
	{
		alloc <<= 1;
		if( alloc == 0 )
			throw_icarus_error( "memory buffer overflow" );
	}
			
	u08*				data;
			
	data = new u08[ alloc ];
	memcpy( data, m_data, m_size );

	delete[] m_data;
	m_data = data;
	m_alloc = alloc;
}

END_ICARUS_NAMESPACE

