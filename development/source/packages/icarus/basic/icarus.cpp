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
//	icarus.cp			 	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus.h"

#if ICARUS_GFX
#include "gfx_vtable.h"
#include "region_t.h"
#endif

#if ICARUS_RT
#include "rt.h"
#include "scheduler_t.h"
#endif

#if __VISC__
#include <math.h>
#endif

#if ICARUS_DEBUG
std::fstream *s_debug_stream = 0;
#endif

BEGIN_ICARUS_NAMESPACE

u32									s_mem_alloc = 0;
bool								s_mem_track = true;
	
void
icarus_debug_init();

void
icarus_init()
{
#if ICARUS_RT
	rt::rt_time_init();
	rt::rt_init_threads();
#endif

	s_mem_alloc = 0;
	s_mem_track = true;

#if ICARUS_GFX
	init_gfx();
#endif

#if ICARUS_DEBUG
	/*if( !s_debug_stream )
	{
		s_debug_stream = new std::fstream;
		s_debug_stream->open( "icarus debug log", std::ios_base::out );
	}*/
	icarus_debug_init();
#endif
}

void
icarus_cleanup()
{
#if ICARUS_GFX
	region_t::release_heaps();
#endif

#if ICARUS_DEBUG
	if( s_debug_stream )
	{
		delete s_debug_stream;
		s_debug_stream = 0;
	}
#endif
}

#if __VISC__
float roundf( float x )
{
	return (float)round( x );
}

double round( double x )
{
	if( x >= 0 )
	{
		double y = floor( x );
		if( ( x - y ) >= 0.5 )
			return y + 1;
		else
			return y;
	}

	return -round( -x );
}
#endif

END_ICARUS_NAMESPACE
