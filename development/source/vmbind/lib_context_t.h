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
//	lib_context_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"

BEGIN_MACHINE_NAMESPACE

enum {
	RTID_PIXMAP,
	RTID_FILE,
	RTID_BIGUINT,
	RTID_COUNT
};

class pixmap_t;
typedef pixmap_t g2world_t;
class g3world_t;

class runtime_object_t;

struct lib_context_t {
						lib_context_t();

						~lib_context_t();

	void				pause();

	void				stop();

	void				resume();

	g2world_t*			g2world;
	g3world_t*			g3world;
	
	runtime_object_t*	rt[ RTID_COUNT ];
};

END_MACHINE_NAMESPACE
