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
//	cpu_state_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_CPU_STATE_H
#define ICARUS_CPU_STATE_H

//#pragma once

#include "rt.h"

BEGIN_RT_NAMESPACE

#pragma pack(2)

struct cpu_state_t {
							// general purpose registers
	
	u32						esp;			// ESP
	u32						ebp;			// EBP
	u32						esi;			// ESI 
	u32						edi;			// EDI

	u32						eax;			// EAX
	u32						ebx;			// EBX
	u32						ecx;			// ECX
	u32						edx;			// EDX

							// segment registers

	u32						cs;				// CS
	u32						ds;				// DS
	u32						es;				// ES
	u32						fs;				// FS
	u32						gs;				// GS
	u32						ss;				// SS

							// program status and control register

	u32						eflags;			// EFLAGS
							
							// instruction pointer register
	
	u32						eip;			// EIP


							// fpu state

	u08						fpuenv[ 28 ];

							// fpu registers
		
	double					fp0;
	double					fp1;
	double					fp2;
	double					fp3;
	double					fp4;
	double					fp5;
	double					fp6;
	double					fp7;

							// parameter
	
	u32						param;
	u08						savefpu;

};

#pragma pack()

#if __VISC__
#define __CDECL _cdecl
#elif __GNUC__
#define __CDECL
#else
#error unknown compiler environment
#endif

u32 __CDECL
save_cpu_state(
	cpu_state_t*			state,
	u32						param );

u32 __CDECL
restore_cpu_state(
	cpu_state_t*			state );

#undef __CDECL

END_RT_NAMESPACE

#endif

