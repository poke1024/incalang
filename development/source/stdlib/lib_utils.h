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
//	lib_utils.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

BEGIN_MACHINE_NAMESPACE

class st_stack_bytes {
public:
						st_stack_bytes(
							fiber_t&			fiber,
							long				count );

						~st_stack_bytes();

	u08*				operator()() const;

	fiber_t&			m_fiber;
	long				m_count;
	u08*				m_buffer;
};

inline st_stack_bytes::st_stack_bytes(
	fiber_t&			fiber,
	long				count ) :

	m_fiber( fiber ),
	m_count( count )
{
	m_buffer = (u08*)fiber.alloc_stack( count );
}

inline st_stack_bytes::~st_stack_bytes()
{
	m_fiber.free_stack( m_count );
}

inline u08*
st_stack_bytes::operator()() const
{
	return m_buffer;
}

// ---------------------------------------------------------------------------

class st_stack_chars {
public:
						st_stack_chars(
							fiber_t&			fiber,
							long				count );

						~st_stack_chars();

	char*				operator()() const;

	fiber_t&			m_fiber;
	long				m_count;
	char*				m_buffer;
};

inline st_stack_chars::st_stack_chars(
	fiber_t&			fiber,
	long				count ) :

	m_fiber( fiber ),
	m_count( count )
{
	m_buffer = (char*)fiber.alloc_stack(
		count * sizeof( char ) );
}

inline st_stack_chars::~st_stack_chars()
{
	m_fiber.free_stack( m_count * sizeof( char ) );
}

inline char*
st_stack_chars::operator()() const
{
	return m_buffer;
}

// ---------------------------------------------------------------------------

void
print_pointer(
	char*				buf,
	int					size,
	void*				ptr );

END_MACHINE_NAMESPACE
