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
//	cpu_state_t.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "cpu_state_t.h"

#pragma optimize("", off)

BEGIN_RT_NAMESPACE

#if __VISC__

#if __VISC__
__declspec(naked) u32 _cdecl
#else
u32
#endif

save_cpu_state(
	cpu_state_t*			state,
	u32						param )
{
#if __VISC__
	_asm {
#else
    asm( "
#endif
	
		// load state into edi
		push edi
		mov edi,[esp+8]
		
		// save edi
		pop dword ptr [edi+12]
			
		// save eflags
		pushfd
		pop dword ptr [edi+56]
		
		// save general purpose registers
		mov [edi+0],esp	
		mov [edi+4],ebp	
		mov [edi+8],esi

		mov [edi+16],eax
		mov [edi+20],ebx
		mov [edi+24],ecx	
		mov [edi+28],edx

		// save segment registers
		//mov [edi+32],cs
		//mov [edi+36],ds
		mov [edi+40],es
		mov [edi+44],fs
		mov [edi+48],gs
		mov [edi+52],ss

		// check if fpu state needs save
		cmp byte ptr [edi+160],0
		je nofpu
		
		// clear floating-point exceptions
		fclex

		// store floating-point environment
		fnstenv [edi+64]

		// save fpu registers
		fst [edi+92]
		fld st(1)
		fstp [edi+100]
		fld st(2)
		fstp [edi+108]
		fld st(3)
		fstp [edi+116]
		fld st(4)
		fstp [edi+124]
		fld st(5)
		fstp [edi+132]
		fld st(6)
		fstp [edi+140]
		fld st(7)
		fstp [edi+148]

		// empty mmx state
		emms

nofpu:
		// save parameter
		mov eax,[esp+8]
		mov [edi+156],eax

		// save eip
		mov eax,[esp]
		mov [edi+60],eax

		// return
		mov edi,[edi+12]
		mov eax,0
		ret
		
#if __VISC__
	}
#else
    " );
#endif
}

#if __VISC__
__declspec(naked) u32 _cdecl
#else
u32
#endif

restore_cpu_state(
	cpu_state_t*			state )
{
#if __VISC__
	_asm {
#else
    asm( "
#endif
		// load state into edi
		mov edi,[esp+4]
	
		// restore general purpose registers
		mov esp,[edi+0]
		mov ebp,[edi+4]
		mov esi,[edi+8]

		mov eax,[edi+16]
		mov ebx,[edi+20]
		mov ecx,[edi+24]
		mov edx,[edi+28]
		
		// restore segment registers		
		mov es,[edi+40]
		mov fs,[edi+44]
		mov gs,[edi+48]
		mov ss,[edi+52]

		// check if fpu state needs save
		cmp byte ptr [edi+160],0
		je nofpu

		// restore floating-point registers
		fninit
		fld [edi+148]
		fld [edi+140]
		fld [edi+132]
		fld [edi+124]
		fld [edi+116]
		fld [edi+108]
		fld [edi+100]
		fld [edi+92]

		// restore floating-point environment
		fldenv [edi+64]

nofpu:
		// restore eflags
		push dword ptr [edi+56]
		popfd

		// load return address
		mov eax,[edi+60]
		mov [esp],eax

		// return
		mov eax,[edi+156]
		mov edi,[edi+12]
		ret
#if __VISC__
	}
#else
    " );
#endif
}

#endif

END_RT_NAMESPACE
