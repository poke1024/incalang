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
//	disassembler_t.h		   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_DISASSEMBLER_H
#define ARIADNE_DISASSEMBLER_H

//#pragma once

#include "fiber_t.h"
#include <string>

BEGIN_MACHINE_NAMESPACE

union instance_t;
class bytecode_func_t;
class machine_t;
class class_t;
struct call_t;

class disassembler_t {
public:
						disassembler_t(
							machine_t*			machine );
						
	virtual				~disassembler_t();
	
	void				dall();
	
	void				dclass(
							class_t*			clss );

	void				df(
							bytecode_func_t*	func );

	void				dcode(
							vm_state_t*			state );
							
	const u08*			disassemble_call(
							const char*			name,
							const u08*			code,
							call_t*				call_table,
							u32					call_table_size,
							class_t**			class_table,
							bool				tail_call );
							
	virtual void		print(
							const char*			text );

protected:
	machine_t*			m_machine;

	void				dump_lifetimes(
							bytecode_func_t*	func );

private:
	void				print_opcode(
							const char*			name,
							int					opcode,
							bool				integer );
							
	const u08*			print_branch(
							const char*			name,
							unsigned short		offset,
							const u08*			code );

	const u08*			disassemble_rtinfo(
							const u08*			code,
							class_t**			class_table );

	const u08*			disassemble_dyncast(
							const u08*			code,
							class_t**			class_table,
							unsigned long*		long_table );
	
	const u08*			disassemble_newinstance(
							const u08*			code,
							class_t**			class_table );
	
	const u08*			disassemble_newblock(
							const u08*			code,
							unsigned long*		long_table );
	
	const u08*			disassemble_newinstancedyn(
							const u08*			code,
							class_t**			class_table );
	
	const u08*			disassemble_delete(
							int					opcode,
							const u08*			code );
	
	const u08*			disassemble_pushstatic(
							int					opcode,
							const u08*			code,
							class_t**			class_table );
	
	const u08*			print_rts(
							int					opcode,
							const u08*			code,
							const u32*			table );
	
	const u08*			print_ldi(
							int					opcode,
							const u08*			code,
							const u32*			qtable,
							const u64*			otable );
							
	const u08*			print_fldi(
							int					opcode,
							const u08*			code,
							const float*		ftable,
							const double*		dtable );
	
	const u08*			print_sldi(
							int					opcode,
							const u08*			code,
							const u32*			m_string_table,
							const char*			m_string_data );
	
	const u08*			print_ldv(
							const char*			name,
							int					opcode,
							const u08*			code,
							const u32*			table );
							
	const u08*			print_ival_opcode(
							const char*			name,
							int					opcode,
							const u08*			code,
							const u32*			table );
							
	const u08*			print_pushreg(
							const char*			name,
							int					opcode,
							const u08*			code );
							
	const u08*			disassemble_switch_linear(
							vm_state_t*			state,
							const u08*			code );
							
	const u08*			disassemble_enter_try(
							vm_state_t*			state,
							const u08*			code );
};

END_MACHINE_NAMESPACE

#endif
