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
//	parser.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_PARSER_H
#define ARIADNE_PARSER_H

//#pragma once

#include "compiler.h"
#include "lexer.h"
#include "stmt_t.h"
#include <string>

BEGIN_COMPILER_NAMESPACE

struct expr_t;
struct type_t;
struct stmt_t;
struct variable_t;
	
struct decl_init_t {
	expr_t*				expr;
	decl_init_t*		list;
	decl_init_t*		next;
	fileref_t			filepos;
};

decl_init_t*
make_decl_init();

struct decl_spec_t {
	type_t*				type;
	int					modifiers;
	fileref_t			filepos;
};

decl_spec_t*
make_decl_spec(
	type_t*				type,
	int					modifiers );

struct decl_func_t {
	decl_t*				param;
};

decl_func_t*
make_decl_func(
	decl_t*				param );

struct type_seq_t;

struct decl_t {
	const char*			name;
	decl_t*				next;
	
	decl_spec_t*		spec;
	type_t*				prefix;
	type_t*				type;
		
	decl_init_t*		init;
	decl_func_t*		func;
	type_seq_t*			tmpltype;
		
	union {
		type_t*			postfix;
		variable_t*		var;
	};
};

decl_t*
make_decl(
	const char*			name );
	
struct array_decl_t {
	expr_t*				expr;
	array_decl_t*		up;
	array_decl_t*		down;
	array_decl_t*		tail;
};

array_decl_t*
make_array_decl(
	expr_t*				expr );

decl_t*
combine_array_decl(
	decl_t*				decl,
	array_decl_t*		array );

struct enum_node_t {
	enum_node_t*		next;
	const char*			name;
	expr_t*				expr;
	enumval_t			value;
};

struct enum_list_t {
	enum_node_t*		first;
	enum_node_t**		link;
	enumval_t			value;
};

enum_list_t*
make_enum_list();

void
append_enumerator(
	enum_list_t*		list,
	const char*			name,
	expr_t*				expr );

struct typename_list_t {
	typename_list_t*	next;
	const char*			name;
};

typename_list_t*
make_typename_list(
	typename_list_t*	next,
	const char*			name );

// ---------------------------------------------------------------------------

bool
finish_decl(
	decl_t*				decl,
	bool				resolve );

void
decl_to_string(
	decl_t*				decl,
	istring&			str );

int
union_flags(
	int					flags,
	int					added );

bool
are_params_equal(
	decl_t*				paramone,
	decl_t*				paramtwo );

stmt_t*
parse_if_stmt(
	expr_t*				expr,
	stmt_t*				stmt_true,
	stmt_t*				stmt_false );

stmt_t*
parse_cond_stmt(
	e_stmt_code			code,
	expr_t*				expr,
	stmt_t*				stmt );

stmt_t*
parse_expr_for_stmt(
	expr_t*				init,
	expr_t*				cond,
	expr_t*				iter,
	stmt_t*				stmt );

stmt_t*
parse_decl_for_stmt(
	decl_t*				init,
	expr_t*				cond,
	expr_t*				iter,
	stmt_t*				stmt );
	
void
parse_declaration(
	decl_t*				decl,
	int					amod,
	int					imod,
	bool				resolve );

void
parse_function(
	decl_t*				decl,
	stmt_t*				stmt );
	
void
make_type_name(
	istring&			name );
	
void
make_variable_name(
	istring&			name );

inline bool
is_type_name(
	const char*			name )
{
	char				c = *name++;

	if( islower( c ) )
		return false;
		
	if( c == '\0' )
		return false;
		
	if( *name == '\0' )
		return true;
	
	while( ( c = *name++ ) != '\0' )
	{
		if( islower( c ) )
			return true;
	}

	// only upper letters -> definition like GL_BUFFER_BIT
	return false;
}

END_COMPILER_NAMESPACE

#endif

