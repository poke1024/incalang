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
//	rt.h					   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

// icarus real-time layer

#ifndef ICARUS_RT_H
#define ICARUS_RT_H

// use the following define to toggle between fast
// and correct rtdsc time, and slower, but more
// compatible Windows time
#define RT_WINDOWS_TIME				1

//#pragma once

#include "icarus.h"

#define RT_DEBUG					ICARUS_DEBUG
#define RT_MEASURE_LATENCY			1
#define RT_DEFAULT_STACK_SIZE		(16L*1024L)
//#define RT_REALTIME					1
#define RT_WIN_FIBERS				1

#define RT_MEASURE_YIELD_LATENCY	0
#define RT_MAX_YIELD_LATENCY		0
#define RT_MEASURE_THREAD_LATENCY	0

#define USING_RT_NAMESPACE			using namespace rt;

#define BEGIN_RT_NAMESPACE			namespace rt {
#define END_RT_NAMESPACE			}

#include "icarus.h"

BEGIN_RT_NAMESPACE
USING_ICARUS_NAMESPACE

// timing functions

void
rt_time_init();

void
rt_time_cleanup();

u64
rt_micros();

u64
rt_nanos();

u64
rt_cycles_per_second();

u64
rt_micros_to_cycles(
	u64					micros );

u64
rt_cycles_to_micros(
	u64					cycles );

#if ICARUS_OS_WIN32

#if RT_WINDOWS_TIME

u64
rt_cycles();

#else // RT_WINDOWS_TIME

#if __VISC__
inline __declspec(naked) u64
rt_cycles()
{
	_asm {
		rdtsc
		ret
	}
}
#elif __GNUC__
inline u64 rt_cycles()
{
    asm( "rdtsc
        ret" );
}
#else
#error unknown compiler environment
#endif

#endif // RT_WINDOWS_TIME

#endif

END_RT_NAMESPACE

#endif

