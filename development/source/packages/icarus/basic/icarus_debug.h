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
//	icarus_debug.h		 	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_DEBUG_H
#define ICARUS_DEBUG_H

//#pragma once

#if ICARUS_DEBUG
#include <iostream>
#include <fstream>

extern std::fstream* s_debug_stream;
#define ICARUS_DEBUG_WRITE(x)\
	{ if( s_debug_stream ) { *s_debug_stream << x; } }
#define ICARUS_DEBUG_WRITELN(x)\
	{ if( s_debug_stream ) { *s_debug_stream << x << std::endl; } }
#define ICARUS_DEBUG_FLUSH\
	{ if( s_debug_stream ) s_debug_stream->flush(); }

void icarus_debug_disable( int level );
void icarus_debug_enable( int level );
#else
#include <stdlib.h>

#define ICARUS_DEBUG_WRITE(x)
#define ICARUS_DEBUG_WRITELN(x)

#define icarus_debug_disable( level )
#define icarus_debug_enable( level )
#endif

#endif

