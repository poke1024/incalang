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
//	stmt_t.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "stmt_t.h"
#include "xalloc.h"
#include "parser.h"
#include "type_t.h"

BEGIN_COMPILER_NAMESPACE

static stmt_t*
alloc_stmt(
	size_t				size )
{
	stmt_t*				stmt = (stmt_t*)xalloc( size );
	
	get_filepos( &stmt->filepos );
	stmt->next = 0;
	
	return stmt;
}

stmt_t*
make_stmt(
	e_stmt_code			code )
{
	stmt_t*				stmt;
	
	stmt = (stmt_t*)alloc_stmt( sizeof( stmt_t ) );
	stmt->code = code;

	return stmt;
}

block_stmt_t*
make_block_stmt(
	stmt_t*				stmtlist )
{
	block_stmt_t*		stmt;
	
	stmt = (block_stmt_t*)alloc_stmt( sizeof( block_stmt_t ) );
	stmt->code = STMT_BLOCK;
	stmt->stmt = stmtlist;

	return stmt;
}

expr_stmt_t*
make_expr_stmt(
	e_stmt_code			code,
	expr_t*				expr )
{
	expr_stmt_t*		stmt;
	
	stmt = (expr_stmt_t*)alloc_stmt( sizeof( expr_stmt_t ) );
	stmt->code = code;
	stmt->expr = expr;

	return stmt;
}

ident_stmt_t*
make_ident_stmt(
	e_stmt_code			code,
	const char*			ident )
{
	ident_stmt_t*		stmt;
	
	stmt = (ident_stmt_t*)alloc_stmt( sizeof( ident_stmt_t ) );
	stmt->code = code;
	stmt->ident = ident;

	return stmt;
}

case_stmt_t*
make_case_stmt(
	expr_t*				expr )
{
	case_stmt_t*		stmt;
	
	stmt = (case_stmt_t*)alloc_stmt( sizeof( case_stmt_t ) );
	stmt->code = STMT_CASE;
	stmt->expr = expr;

	return stmt;
}

if_stmt_t*
make_if_stmt(
	expr_t*				expr,
	stmt_t*				stmt_true,
	stmt_t*				stmt_false )
{
	if_stmt_t*			stmt;
	
	stmt = (if_stmt_t*)alloc_stmt( sizeof( if_stmt_t ) );
	stmt->code = STMT_IF;
	stmt->expr = expr;
	stmt->stmt_true = stmt_true;
	stmt->stmt_false = stmt_false;

	return stmt;
}

cond_stmt_t*
make_cond_stmt(
	e_stmt_code			code,
	expr_t*				expr,
	stmt_t*				cond_stmt )
{
	cond_stmt_t*		stmt;
	
	stmt = (cond_stmt_t*)alloc_stmt( sizeof( cond_stmt_t ) );
	stmt->code = code;
	stmt->expr = expr;
	stmt->stmt = cond_stmt;

	return stmt;
}

for_stmt_t*
make_for_stmt(
	expr_t*				init,
	expr_t*				cond,
	expr_t*				iter,
	stmt_t*				for_stmt )
{
	for_stmt_t*			stmt;
	
	stmt = (for_stmt_t*)alloc_stmt( sizeof( for_stmt_t ) );
	stmt->code = STMT_FOR;
	stmt->init = init;
	stmt->iter = iter;
	stmt->cond = cond;
	stmt->stmt = for_stmt;

	return stmt;
}

decl_stmt_t*
make_decl_stmt(
	decl_t*				decl )
{
	decl_stmt_t*		stmt;
	
	stmt = (decl_stmt_t*)alloc_stmt( sizeof( decl_stmt_t ) );
	stmt->code = STMT_DECL;
	stmt->decl = decl;

	return stmt;
}

catch_stmt_t*
make_catch_stmt(
	decl_t*				decl,
	stmt_t*				stmtlist )
{
	catch_stmt_t*		stmt;
	
	stmt = (catch_stmt_t*)alloc_stmt( sizeof( catch_stmt_t ) );
	stmt->code = STMT_CATCH;
	stmt->decl = decl;
	stmt->stmt = stmtlist;

	return stmt;
}

try_stmt_t*
make_try_stmt(
	stmt_t*				stmtlist,
	catch_stmt_t*		catchstmt )
{
	try_stmt_t*			stmt;
	
	stmt = (try_stmt_t*)alloc_stmt( sizeof( try_stmt_t ) );
	stmt->code = STMT_TRY;
	stmt->stmt = stmtlist;
	stmt->catchstmt = catchstmt;

	return stmt;
}

stmt_t*
make_throw_stmt(
	expr_t*				expr )
{
	expr_stmt_t*		stmt;
	
	stmt = (expr_stmt_t*)alloc_stmt( sizeof( expr_stmt_t ) );
	stmt->code = STMT_THROW;
	stmt->expr = expr;

	return stmt;
}

END_COMPILER_NAMESPACE
