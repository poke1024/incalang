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
//	runtime_object_t.cp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "runtime_object_t.h"

BEGIN_MACHINE_NAMESPACE

runtime_object_t::runtime_object_t() :

	m_context( 0 )
{
}

runtime_object_t::runtime_object_t(
	lib_context_t&		context,
	int					index ) :
	
	m_context( &context ),
	m_rtindex( index )
{
	m_next = m_context->rt[ index ];
	m_context->rt[ index ] = this;
}

runtime_object_t::~runtime_object_t()
{
	if( not m_context )
		return;

	runtime_object_t*	node;
	runtime_object_t**	link;

	link = &m_context->rt[ m_rtindex ];
	
	while( true )
	{
		node = *link;
		
/*#if ICARUS_SAFE
		if( node == 0 )
			Debugger();
#endif*/
		
		if( node == this )
		{
			*link = m_next;
			break;
		}
		
		link = &node->m_next;
	}

}

END_MACHINE_NAMESPACE
