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
//	fizban.h			 	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include <Windows.h>
#include "icarus.h"

#define USING_FIZBAN_NAMESPACE		using namespace fizban;

#define BEGIN_FIZBAN_NAMESPACE		namespace fizban {
#define END_FIZBAN_NAMESPACE		}

BEGIN_FIZBAN_NAMESPACE

USING_ICARUS_NAMESPACE

HDC
working_dc();

END_FIZBAN_NAMESPACE

