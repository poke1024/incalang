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
//	makecode.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_t.h"

BEGIN_COMPILER_NAMESPACE

struct binary_expr_t;

code_t*
make_var_code(
	variable_t*			var,
	class_t*			clss );

code_t*
make_arith_code(
	e_inst_code			iopcode,
	e_inst_code			fopcode,
	binary_expr_t*		expr );

code_t*
make_cmp_code(
	binary_expr_t*		expr );

struct expr;

code_t*
lval_to_rval_code(
	code_t*				code,
	type_t*				type );

code_t*
make_rval_code(
	expr_t*				expr );

code_t*
make_code(
	expr_t*				expr );

code_t*
promote_code_type(
	code_t*				code,
	type_t*				fromtype,
	type_t*				totype );

code_t*
promote_code_int_type(
	code_t*				code,
	e_operand_size		fromsize,
	e_operand_size		tosize,
	bool				is_signed );

code_t*
make_pass_parameter_code(
	code_t*				node,
	type_t*				fromtype,
	type_t*				totype,
	fileref_t*			filepos );

END_COMPILER_NAMESPACE
