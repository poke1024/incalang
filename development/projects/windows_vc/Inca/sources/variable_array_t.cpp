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
//	variable_array_t.cpp	   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "variable_array_t.h"
#include "variable_t.h"
#include "ariadne_messages.h"

BEGIN_AKLABETH_NAMESPACE

variable_array_t::variable_array_t() :
	m_vars( sizeof( variable_t* ) )
{
	m_base = 0;
}

variable_array_t::~variable_array_t()
{
	cleanup();
}

void
variable_array_t::load_stack_frame(
	int					frame_index,
	random_stream_t*	stream )
{
	cleanup();

	stream->write_u32( msg_GetFrameVariables );
	stream->write_u32( frame_index );

	void*				instance;
	variable_t*			var;

	// "this" class variable

	instance = stream->read_ptr();

	var = new variable_t;
	var->init_as_variable(
		stream, (u08*)instance, 0 );
	append( var );

	// local variables

	void*				localsp;
	long				count;

	localsp = stream->read_ptr();
	count = stream->read_u32();

	for( long i = 0; i < count; i++ )
	{
		var = new variable_t;
		var->init_as_variable(
			stream, (u08*)localsp, 0 );
		append( var );
	}

	for( long i = m_vars.count() - 1; i >= 0; i-- )
		at( i )->setup( stream );

	m_base = localsp;
}

long
variable_array_t::count() const
{
	return m_vars.count();
}

variable_t*
variable_array_t::at(
	long				index )
{
	return *(variable_t**)m_vars[ index ];
}

void
variable_array_t::append(
	variable_t*			var )
{
	m_vars.append( &var );
}

void
variable_array_t::cleanup()
{
	for( int i = 0; i < m_vars.count(); i++ )
		delete at( i );
	m_vars.clear();
}

END_AKLABETH_NAMESPACE
