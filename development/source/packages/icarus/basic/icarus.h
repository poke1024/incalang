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
//	icarus.h			 	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_H
#define ICARUS_H

//#pragma once

#if __VISC__
#include <iso646.h>
#endif

#if __VISC__
#if NDEBUG
#include <new>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#endif
#endif

#if __GNUC__
#include <iso646.h>
#include <stdio.h>
#include <string.h>
#include <new>
#endif

#define USING_ICARUS_NAMESPACE		using namespace icarus;

#define BEGIN_ICARUS_NAMESPACE		namespace icarus {
#define END_ICARUS_NAMESPACE		}

#include "icarus_types.h"

BEGIN_ICARUS_NAMESPACE

#ifndef ICARUS_DEBUG
#define ICARUS_DEBUG				0
#else
#undef ICARUS_DEBUG
#define ICARUS_DEBUG				1
#endif

#ifndef ICARUS_GFX
#define ICARUS_GFX					0
#else
#undef ICARUS_GFX
#define ICARUS_GFX					1
#endif

#ifndef ICARUS_THREADS
#define ICARUS_THREADS				0
#else
#undef ICARUS_THREADS
#define ICARUS_THREADS				1
#endif

#ifndef ICARUS_RT
#define ICARUS_RT					0
#else
#undef ICARUS_RT
#define ICARUS_RT					1
#endif

#define ICARUS_RT_MEMORY			0

#define ICARUS_SAFE					ICARUS_DEBUG

#if macintosh
#define ICARUS_OS_MACOS 1
#define ICARUS_IS_BIG_ENDIAN 1
#elif WIN32
#define ICARUS_OS_WIN32 1
#define ICARUS_IS_BIG_ENDIAN 0
#else
#error unknown operating system
#endif

const double pi						= 3.1415926535898;

typedef s32 ifpos_t;
typedef u64 iptr_t;

#define ICARUS_PTR_SIZE				8

void
icarus_init();

void
icarus_cleanup();

END_ICARUS_NAMESPACE

#include "icarus_endian.h"
#include "icarus_stdfunc.h"

#include "icarus_error.h"
#include "icarus_debug.h"

#include "icarus_memory.h"
#include "icarus_string.h"

#endif

