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
//	icarus_debug.cp		 	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus.h"

#if ICARUS_DEBUG

#if __MWERKS__
#define ICARUS_SPOTLIGHT 1
#endif

#if ICARUS_SPOTLIGHT
#include "SpotlightAPI.h"
#endif

static const int debugLevel = 1;
static int debugEnableCount = 1;

BEGIN_ICARUS_NAMESPACE

void icarus_debug_init()
{
#if ICARUS_SPOTLIGHT
	SLInit();
#endif
}

END_ICARUS_NAMESPACE

void icarus_debug_disable( int level )
{
#if ICARUS_SPOTLIGHT
	if( level < debugLevel )
	{
		debugEnableCount--;
		if( debugEnableCount == 0 )
			SLDisable();
	}
#endif
}

void icarus_debug_enable( int level )
{
#if ICARUS_SPOTLIGHT
	if( level < debugLevel )
	{
		debugEnableCount++;
		if( debugEnableCount == 1 )
			SLEnable();
	}
#endif
}

#endif
