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
//	machine.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef MACHINE_H
#define MACHINE_H

//#pragma once

#include "ariadne.h"
#include "icarus.h"
#include "rt.h"

#define BEGIN_MACHINE_NAMESPACE			namespace machine {
#define END_MACHINE_NAMESPACE			}

#ifdef VM_PROJECT
#define SAFE_MACHINE					1
#else
#define SAFE_MACHINE					0
#endif

// flag to toggle whether to check memory accesses
#define CMA_MACHINE						0

#define DEBUG_MACHINE					ICARUS_DEBUG
#define DEBUG_ABSTRACT_FUNCTIONS		1

BEGIN_MACHINE_NAMESPACE

using namespace ariadne;
using namespace icarus;
using namespace rt;

typedef long			message_t;

void*
obtain_buffer(
	isize_t				size );

void
print(
	const char*			text );

const s32								VK_OFFSET = 256;
	
END_MACHINE_NAMESPACE

#endif

