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
//	stmt_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_STMT_H
#define ARIADNE_STMT_H

//#pragma once

#include "compiler.h"
#include "lexer.h"

BEGIN_COMPILER_NAMESPACE

enum e_stmt_code {
	STMT_NOP,
	STMT_EXPR,
	STMT_LABEL,
	STMT_GOTO,
	STMT_CASE,
	STMT_DEFAULT,
	STMT_IF,
	STMT_SWITCH,
	STMT_WHILE,
	STMT_DO,
	STMT_FOR,
	STMT_CONTINUE,
	STMT_BREAK,
	STMT_RETURN,
	STMT_DECL,
	STMT_BLOCK,
	STMT_TRY,
	STMT_CATCH,
	STMT_THROW
};

struct expr_t;

struct stmt_t {
	e_stmt_code			code;
	stmt_t*				next;
	fileref_t			filepos;
};

stmt_t*
make_stmt(
	e_stmt_code			code );

struct block_stmt_t : stmt_t {
	stmt_t*				stmt;
};

block_stmt_t*
make_block_stmt(
	stmt_t*				stmt );

struct expr_stmt_t : stmt_t {
	expr_t*				expr;
};	
	
expr_stmt_t*
make_expr_stmt(
	e_stmt_code			code,
	expr_t*				expr );
	
struct ident_stmt_t : stmt_t {
	const char*			ident;
};
	
ident_stmt_t*
make_ident_stmt(
	e_stmt_code			code,
	const char*			ident );

struct case_stmt_t : stmt_t {
	expr_t*				expr;
};

case_stmt_t*
make_case_stmt(
	expr_t*				expr );

struct if_stmt_t : stmt_t {
	expr_t*				expr;
	stmt_t*				stmt_true;
	stmt_t*				stmt_false;
};

if_stmt_t*
make_if_stmt(
	expr_t*				expr,
	stmt_t*				stmt_true,
	stmt_t*				stmt_false );

struct cond_stmt_t : stmt_t {
	expr_t*				expr;
	stmt_t*				stmt;
};

cond_stmt_t*
make_cond_stmt(
	e_stmt_code			code,
	expr_t*				expr,
	stmt_t*				stmt );

struct for_stmt_t : stmt_t {
	expr_t*				init;
	expr_t*				cond;
	expr_t*				iter;
	stmt_t*				stmt;
};

for_stmt_t*
make_for_stmt(
	expr_t*				init,
	expr_t*				cond,
	expr_t*				iter,
	stmt_t*				stmt );

struct decl_t;

struct decl_stmt_t : stmt_t {
	decl_t*				decl;
};

decl_stmt_t*
make_decl_stmt(
	decl_t*				decl );

struct catch_stmt_t : stmt_t {
	decl_t*				decl;
	stmt_t*				stmt;
};

catch_stmt_t*
make_catch_stmt(
	decl_t*				decl,
	stmt_t*				stmt );
	
struct try_stmt_t : stmt_t {
	stmt_t*				stmt;
	catch_stmt_t*		catchstmt;
};

try_stmt_t*
make_try_stmt(
	stmt_t*				stmt,
	catch_stmt_t*		catchstmt );

stmt_t*
make_throw_stmt(
	expr_t*				expr );

END_COMPILER_NAMESPACE

#endif

