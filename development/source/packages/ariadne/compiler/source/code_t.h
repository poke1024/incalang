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
//	code_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_CODE_H
#define ARIADNE_CODE_H

//#pragma once

#include "inst_t.h"

BEGIN_COMPILER_NAMESPACE

enum {
	REG_LOCAL = -1,
	REG_STATIC = -2,
	REG_SELF = -3,
	REG_STACK = -4
};

struct code_t {
	e_inst_code			inst;
	e_operand_size		size;
	bool				addr;
};

struct unary_code_t : code_t {
	code_t*				child;
};

code_t*
make_unary_code(
	e_inst_code			inst,
	e_operand_size		size,
	code_t*				child );

code_t*
make_ret_code(
	code_t*				child );

struct binary_code_t : code_t {
	code_t*				child[ 2 ];
};

code_t*
make_binary_code(
	e_inst_code			inst,
	e_operand_size		size,
	code_t*				left,
	code_t*				right );

code_t*
make_param_code(
	code_t*				left,
	code_t*				right );

code_t*
make_seq_code(
	code_t*				left,
	code_t*				right );

code_t*
make_seq_code(
	e_inst_code			code,
	code_t*				left,
	code_t*				right );

code_t*
make_comma_code(
	code_t*				left,
	code_t*				right );

code_t*
make_destroy_code(
	code_t*				left,
	code_t*				right );

code_t*
make_hint_code(
	e_inst_code			code,
	code_t*				node,
	code_t*				param );

struct iconst_code_t : code_t {
	bool				is_signed;
	intval_t			val;
};

iconst_code_t*
make_iconst_code(
	e_operand_size		size,
	bool				is_signed,
	intval_t			val );

struct fconst_code_t : code_t {
	fltval_t			val;
};

fconst_code_t*
make_fconst_code(
	e_operand_size		size,
	fltval_t			val );

struct str_code_t : code_t {
	const char*			str;
};

str_code_t*
make_str_code(
	const char*			str );

struct tvar_code_t : code_t {
	variable_t*			tvar;
};

tvar_code_t*
make_tvar_code(
	variable_t*			var );

struct rtinfo_code_t : code_t {
	code_t*				instance;
	class_t*			clss;
};

rtinfo_code_t*
make_rtinfo_code(
	code_t*				instance,
	class_t*			clss );

struct func_t;
struct decl_t;

struct func_code_t : code_t {
	func_t*				func;
	code_t*				instance;
	code_t*				param;
};

code_t*
make_func_code(
	e_inst_code			inst,
	func_t*				func,
	code_t*				addr,
	code_t*				param );

struct reg_code_t : code_t {
	short				reg;
	void*				data;
};

reg_code_t*
make_reg_code(
	e_operand_size		size,
	short				reg,
	void*				data );

struct pop_code_t : public unary_code_t {
	size_t				amount;
};

code_t*
make_pop_code(
	code_t*				child,
	size_t				amount );

struct xstore_code_t : binary_code_t {
	size_t				bytecount;
};

struct class_type_t;

code_t*
make_xstore_code(
	class_type_t*		type,
	code_t*				left,
	code_t*				right );

struct xload_code_t : unary_code_t {
	size_t				bytecount;
};

code_t*
make_xload_code(
	class_type_t*		type,
	code_t*				child );

struct class_t;

struct dyncast_code_t : unary_code_t {
	class_t*			toclass;
	unsigned long		offset;
};

code_t*
make_dynamic_cast_code(
	class_t*			toclass,
	unsigned long		offset,
	code_t*				child );

struct new_code_t : code_t {
	type_t*				newtype;
	type_t*				elemtype;
	code_t*				arraysize;
};

new_code_t*
make_new_code(
	type_t*				newtype,
	type_t*				elemtype );

struct dtnew_code_t : new_code_t {
	code_t*				newname;
};

dtnew_code_t*
make_dtnew_code(
	type_t*				newtype,
	type_t*				elemtype,
	expr_t*				newname );

struct delete_code_t : code_t {
	type_t*				elemtype;
	code_t*				delobject;
};

delete_code_t*
make_delete_code(
	type_t*				newtype,
	expr_t*				delexpr );
	
struct switch_node_t {
	switch_node_t*		next;
	label_t*			label;
	caseval_t			value;
};
	
struct switch_code_t : code_t {
	code_t*				selector;
	label_t*			defaultlabel;
	switch_node_t*		nodes;
	caseval_t			minval;
	caseval_t			maxval;
	long				count;
};

switch_code_t*
make_switch_code(
	code_t*				selector,
	type_t*				type );

bool
add_switch_node(
	switch_code_t*		code,
	label_t*			label,
	caseval_t			value,
	fileref_t*			filepos );

variable_t*
code_tmp_var(
	code_t**			link,
	type_t*				type );

struct catch_entry_t {
	catch_entry_t*		next;
	label_t*			label;
	class_t*			clss;
};

struct try_code_t : code_t {
	catch_entry_t*		entries;
	catch_entry_t**		link;
	long				count;
};

try_code_t*
make_try_code();

void
add_try_catch(
	try_code_t*			code,
	label_t*			label,
	class_t*			clss );

END_COMPILER_NAMESPACE

#endif

