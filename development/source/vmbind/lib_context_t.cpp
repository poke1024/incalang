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
//	lib_context_t.cp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_context_t.h"
#include "runtime_object_t.h"
#include "g2world_t.h"
#include "g3world_t.h"

BEGIN_MACHINE_NAMESPACE

lib_context_t::lib_context_t()
{
	g3world = 0;
	g2world = 0;
	
	imemset( rt, 0, RTID_COUNT * sizeof( runtime_object_t* ) );
}

lib_context_t::~lib_context_t()
{
	for( long i = 0; i < RTID_COUNT; i++ )
	{
		while( rt[ i ] )
			delete rt[ i ];
	}
}

void
lib_context_t::pause()
{
	if( g3world )
		g3world->pause();
}

void
lib_context_t::stop()
{
	if( g3world )
		g3world->stop();
}

void
lib_context_t::resume()
{
	if( g3world )
		g3world->resume();
}

END_MACHINE_NAMESPACE
