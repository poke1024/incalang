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
//	inst_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_INST_H
#define ARIADNE_INST_H

//#pragma once

#include "compiler.h"
#include "type_t.h"

BEGIN_COMPILER_NAMESPACE

struct code_t;
struct type_t;
struct variable_t;
struct codepoint_t;

enum e_inst_code {
	INST_ADD,
	INST_SUB,
	INST_MUL,
	INST_DIV,
	INST_MOD,
	INST_NEG,
	INST_CMP,
	INST_CMPU,
	INST_FADD,
	INST_FSUB,
	INST_FMUL,
	INST_FDIV,
	INST_FMOD,
	INST_FNEG,
	INST_FCMP,

	INST_NOT,
	INST_AND,
	INST_OR,
	INST_XOR,
	INST_SHL,
	INST_SHR,
	INST_ASR,

	INST_BRA,
	INST_BEQ,
	INST_BNE,
	INST_BGT,
	INST_BLT,
	INST_BGE,
	INST_BLE,
	
	INST_RTS,
	INST_CSF,
	INST_CVF,
	INST_CRF,
	INST_TCSF,
	INST_TCVF,
	INST_TCRF,
	INST_RTINFO,
	
	INST_LDI,
	INST_FLDI,
	INST_STORE,
	INST_LOAD,
	INST_FSTORE,
	INST_FLOAD,
	INST_XLOAD,
	INST_XSTORE,
	
	INST_EXTS,
	INST_EXTU,
	INST_RDC,
	INST_FEXT,
	INST_ITOF,
	INST_UTOF,
	INST_FTOI,
	INST_FTOU,
	INST_DUP,
	INST_FDUP,
		
	INST_DYNCAST,
	INST_NEW,
	INST_DTNEW,
	INST_DELETE,
	INST_SWITCH,
	
	INST_ENTER_TRY,
	INST_LEAVE_TRY,
	INST_ENTER_CATCH,
	INST_LEAVE_CATCH,
	INST_THROW,
	
	INST_TREE,
	CODE_STR,
	CODE_REG,
	CODE_RET,
	CODE_SEQ,
	CODE_SEQ_NEUTRAL,
	CODE_SEQ_POSTOP,
	CODE_POP,
	CODE_PARAM,
	CODE_DESTROY,

	CODE_TVAR,
	CODE_STACKHINT
};

enum e_operand_size {
	OPS_NONE,
	OPS_BYTE,
	OPS_WORD,
	OPS_QUAD,
	OPS_OCTA,
	OPS_WIDE,
	OPS_SINGLE,
	OPS_DOUBLE,
	
	OPS_PTR = OPS_QUAD
};
	
void
init_inst();

void
finish_inst();
	
e_operand_size
get_type_operand_size(
	type_t*				type );
	
bool
is_type_int_operand(
	type_t*				type );

size_t
get_int_stack_size(
	e_operand_size		size );

size_t
get_flt_stack_size(
	e_operand_size		size );

size_t
get_type_stack_size(
	type_t*				type );

size_t
get_type_array_size(
	type_t*				type );

struct inst_t {
	e_inst_code			code;
	inst_t*				next;
};

struct code_inst_t : inst_t {
	code_t*				tree;
	codepoint_t*		codepoint;
};

void
inst_code(
	code_t*				tree );

struct block_t {
	inst_t*				inst;
	block_t*			next;
	
	long				outoffset;
	
	// for traversing
	long				visit;
};

struct label_t {
	block_t*			block;
};

struct bra_inst_t : inst_t {
	label_t*			target;
};
	
void
inst_bra(
	e_inst_code			code,
	label_t*			target );

void
inst_rts();

label_t*
make_label();

void
put_label(
	label_t*			label );

// ---------------------------------------------------------------------------

struct expr_t;

void
inst_logical_expr(
	expr_t*				expr,
	label_t*			labelfalse,
	bool				negate );

struct variable_t;

variable_t*
inst_arith_expr(
	expr_t*				expr );

void
inst_variable_cd_code(
	variable_t*			var,
	int					index );
	
struct class_t;
	
void
inst_inherited_cd_code(
	class_t*			clss,
	class_t*			baseclss,
	int					index );
	
// ---------------------------------------------------------------------------

inline size_t
get_stack_size(
	size_t				size )
{
	if( size & 3 )
		size += 4 - ( size & 3 );
		
	return size;
}

inline size_t
get_type_stack_size(
	type_t*				type )
{
	return get_stack_size( get_type_size( type ) );
}

inline size_t
get_type_array_size(
	type_t*				type )
{
	return get_type_size( type );
}
	
END_COMPILER_NAMESPACE

#endif

