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
//	bytecode.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef BYTECODE_H
#define BYTECODE_H

//#pragma once

enum {
	BYTECODE_ADD = 0,
	BYTECODE_SUB,
	BYTECODE_MUL,
	BYTECODE_DIV,
	BYTECODE_MOD,
	BYTECODE_NEG,
	BYTECODE_CMP,
	BYTECODE_CMPU,
	BYTECODE_FADD,
	BYTECODE_FSUB,
	BYTECODE_FMUL,
	BYTECODE_FDIV,
	BYTECODE_FMOD,
	BYTECODE_FNEG,
	BYTECODE_FCMP,
	
	BYTECODE_NOT,
	BYTECODE_AND,
	BYTECODE_OR,
	BYTECODE_XOR,
	BYTECODE_SHL,
	BYTECODE_SHR,
	BYTECODE_ASR,
	
	BYTECODE_BRA,
	BYTECODE_BEQ,
	BYTECODE_BNE,
	BYTECODE_BGT,
	BYTECODE_BLT,
	BYTECODE_BGE,
	BYTECODE_BLE,
	BYTECODE_RTS,
	
	BYTECODE_CSF,
	BYTECODE_CVF,
	BYTECODE_CRF,
	BYTECODE_RTINFO,
	
	BYTECODE_LDI,
	BYTECODE_FLDI,
	BYTECODE_SLDI,
	
	BYTECODE_STORE,
	BYTECODE_LOAD,
	BYTECODE_FSTORE,
	BYTECODE_FLOAD,
	BYTECODE_XSTORE,
	BYTECODE_XLOAD,
	BYTECODE_POP,
	
	BYTECODE_DYNCAST,
	BYTECODE_PUSHSELF,
	BYTECODE_PUSHLOCAL,
	BYTECODE_PUSHSTACK,
	BYTECODE_PUSHSTATIC,
	
	BYTECODE_EXTS,
	BYTECODE_EXTU,
	BYTECODE_RDC,
	BYTECODE_FEXT,
	BYTECODE_ITOF,
	BYTECODE_UTOF,
	BYTECODE_FTOI,
	BYTECODE_FTOU,
	BYTECODE_DUP,
	BYTECODE_FDUP,
	
	BYTECODE_NEW,
	BYTECODE_DELETE,
	BYTECODE_BREAK,
	BYTECODE_EXTRA,
	
	BYTECODE_LAST
};

enum {
	BYTECODE_SWITCH_LINEAR,
	BYTECODE_SWITCH_TABLE,
	BYTECODE_ENTER_TRY,
	BYTECODE_LEAVE_TRY,
	BYTECODE_ENTER_CATCH,
	BYTECODE_LEAVE_CATCH,
	BYTECODE_THROW,
	BYTECODE_DYNNEW,
	BYTECODE_TCSF, // tail calls
	BYTECODE_TCVF,
	BYTECODE_TCRF
};

#endif

