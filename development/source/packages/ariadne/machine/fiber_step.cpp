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
//	fiber_step.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#if VM_PROJECT

#include "fiber_t.h"
#include "bytecode_func_t.h"

#include "bytecode.h"
#include "vmdefs.h"
#include "machine_class_t.h"

BEGIN_MACHINE_NAMESPACE

#define	SKIP_INDEX															\
	{ int x = *code++; if( x == 0xff ) { 									\
	x = *code++; x = ( x << 8 ) | ( *code++ ); }							\
	x = ( x << 8 ) | ( *code++ ); }
	
#define FETCH_UWORD(x)														\
	x = *code++; x = ( x << 8 ) | ( *code++ ); x = (u16)x;

void
fiber_t::step(
	bool				stepinto )
{
	bytecode_func_t*	func;
	
	func = m_state.func;
	
	u08*				nextcode;
	
	nextcode = m_state.code - 1;
	if( m_state.condflags & COND_BREAK )
		nextcode++;

	const u08*			breakcode;
	
	breakcode = func->find_break_location( nextcode );
	
	if( breakcode )
	{
		func->add_breakpoint(
			func->get_code_offset( breakcode ),
			BYTECODE_BREAK | OPS_BREAK_STEP );
	}
	else
	{
		breakcode = m_state.code + func->m_code_size;
	}

	u08*				code;

	code = m_state.code;

	if( m_state.condflags & COND_BREAK )
		code--;

	while( code < breakcode )
	{
		switch( *code++ )
		{
			case BYTECODE_ADD | OPS_BYTE:
			case BYTECODE_ADD | OPS_WORD:
			case BYTECODE_ADD | OPS_QUAD:
			case BYTECODE_ADD | OPS_OCTA:
			
			case BYTECODE_CMP | OPS_BYTE:
			case BYTECODE_CMP | OPS_WORD:
			case BYTECODE_CMP | OPS_QUAD:
			case BYTECODE_CMP | OPS_OCTA:

			case BYTECODE_LOAD | OPS_BYTE:
			case BYTECODE_LOAD | OPS_WORD:
			case BYTECODE_LOAD | OPS_QUAD:
			case BYTECODE_LOAD | OPS_OCTA:

			case BYTECODE_STORE | OPS_BYTE:
			case BYTECODE_STORE | OPS_WORD:
			case BYTECODE_STORE | OPS_QUAD:
			case BYTECODE_STORE | OPS_OCTA:

			case BYTECODE_PUSHLOCAL | OPS_PTR:
			case BYTECODE_PUSHSTACK | OPS_PTR:
			case BYTECODE_PUSHSELF | OPS_PTR:
				break;

			case BYTECODE_PUSHLOCAL | OPS_BYTE:
			case BYTECODE_PUSHSTACK | OPS_BYTE:
			case BYTECODE_LDI | OPS_BYTE:
			case BYTECODE_POP | OPS_BYTE:
				code++;
				break;

			case BYTECODE_PUSHLOCAL | OPS_WORD:
			case BYTECODE_PUSHSTACK | OPS_WORD:
			case BYTECODE_LDI | OPS_WORD:
			case BYTECODE_POP | OPS_WORD:
				code += 2;
				break;

			case BYTECODE_PUSHSTATIC | OPS_BYTE:
				code++;
				SKIP_INDEX;
				break;
				
			case BYTECODE_PUSHSTATIC | OPS_WORD:
				code += 2;
				SKIP_INDEX;
				break;

			case BYTECODE_PUSHSTATIC | OPS_PTR:
				SKIP_INDEX;
				break;

			case BYTECODE_CSF | OPS_WORD:
				if( stepinto )
				{
					unsigned long index; FETCH_UWORD( index );
					call_t* call = &func->m_call_table[ index ];
					index = call->funcid;
					class_t* clss = func->m_class_table[ call->clssindx ];
					unsigned long offset = 0;
					func_t* newfunc = clss->find_func_by_id( index, &offset );
					if( newfunc->get_type() == func_type_bytecode )
						( (bytecode_func_t*)newfunc )->add_breakpoint(
							0, BYTECODE_BREAK | OPS_BREAK_STEP );
				}
				else
					code += 2;
				break;
			
			case BYTECODE_CVF | OPS_WORD:
				if( stepinto )
				{
					func->add_breakpoint(
						func->get_code_offset( code ) - 1,
						BYTECODE_BREAK | OPS_BREAK_STEP_CVF );
				}
				code += 2;
				break;

			case BYTECODE_CRF | OPS_WORD:
				if( stepinto )
				{
					unsigned long index;
					FETCH_UWORD( index );
					call_t* call = &func->m_call_table[ index ];
					func_t* newfunc = call->regufunc;
					if( newfunc->get_type() == func_type_bytecode )
						( (bytecode_func_t*)newfunc )->add_breakpoint(
							0, BYTECODE_BREAK | OPS_BREAK_STEP );
				}
				else
					code += 2;
				break;
		
			case BYTECODE_LDI | OPS_QUAD:
			case BYTECODE_LDI | OPS_OCTA:
			case BYTECODE_POP | OPS_QUAD:
			case BYTECODE_SLDI | OPS_PTR:
			case BYTECODE_RTINFO | OPS_PTR:
				SKIP_INDEX
				break;
		
			case BYTECODE_BRA | OPS_WORD:
			case BYTECODE_BEQ | OPS_WORD:
			case BYTECODE_BNE | OPS_WORD:
			case BYTECODE_BGT | OPS_WORD:
			case BYTECODE_BLT | OPS_WORD:
			case BYTECODE_BGE | OPS_WORD:
			case BYTECODE_BLE | OPS_WORD:
			{
				int		offset;
				long	base;
				
				offset = code[ 0 ];	
				offset = ( offset << 8 ) | code[ 1 ];
				offset = (s16)offset;
				
				base = func->get_code_offset( code );
				
				func->add_breakpoint(
					base + offset, BYTECODE_BREAK | OPS_BREAK_STEP );
				func->add_breakpoint(
					base + 2, BYTECODE_BREAK | OPS_BREAK_STEP );
				code += 2;
				
				goto done;
			}
				
			default:
				goto done;
		}
	}

done:
	;
}

END_MACHINE_NAMESPACE

#endif // VM_PROJECT
