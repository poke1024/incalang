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
//	expr_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"
#include "lexer.h"

BEGIN_COMPILER_NAMESPACE

struct type_t;
struct variable_t;

enum e_expr_code {
	EXPR_NOP,

	EXPR_ICONST,
	EXPR_FCONST,
	EXPR_ID,
	EXPR_STR,
	EXPR_NULL,
	EXPR_THIS,
	EXPR_VARIABLE,
	
	EXPR_ACCESS_ARRAY,
	EXPR_ACCESS_DOT,
	EXPR_ACCESS_PTR,
	EXPR_CALL_FUNC,
	EXPR_NEW,
	EXPR_DTNEW,
	EXPR_DELETE,
			
	EXPR_ADDRESS_OF,
	EXPR_DEREFERENCE,
	EXPR_SIZEOF,
	EXPR_COMMA,
	EXPR_TYPECAST,

	EXPR_PREINC,
	EXPR_PREDEC,
	EXPR_POSTINC,
	EXPR_POSTDEC,
	
	EXPR_LOGICAL_NOT,
	EXPR_LOGICAL_AND,
	EXPR_LOGICAL_OR,
	
	EXPR_BITWISE_NOT,
	EXPR_BITWISE_AND,
	EXPR_BITWISE_XOR,
	EXPR_BITWISE_OR,
	
	EXPR_ADD,
	EXPR_SUB,
	EXPR_NEGATE,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_MOD,
	EXPR_SHL,
	EXPR_SHR,
	EXPR_LT,
	EXPR_GT,
	EXPR_LE,
	EXPR_GE,
	EXPR_EQ,
	EXPR_NE,
	
	EXPR_ASSIGN,
	EXPR_INITIALIZE,
	EXPR_MUL_ASSIGN,
	EXPR_DIV_ASSIGN,
	EXPR_MOD_ASSIGN,
	EXPR_ADD_ASSIGN,
	EXPR_SUB_ASSIGN,
	EXPR_SHL_ASSIGN,
	EXPR_SHR_ASSIGN,
	EXPR_AND_ASSIGN,
	EXPR_XOR_ASSIGN,
	EXPR_OR_ASSIGN,
	
	EXPR_PREUNA_OPERATOR,
	EXPR_POSTUNA_OPERATOR,
	EXPR_BIN_OPERATOR,
	EXPR_THROWN_OBJECT
};

typedef u64 intval_t;

struct expr_t {
	e_expr_code			code;
	type_t*				type;
	fileref_t			filepos;
};

struct iconst_expr_t : expr_t {
	intval_t			val;
};

struct fconst_expr_t : expr_t {
	fltval_t			val;
};

struct qualname_t;
struct scope_t;

struct id_expr_t : expr_t {
	qualname_t*			id;
	variable_t*			var;
	scope_t*			scope;
};

struct str_expr_t : expr_t {
	const char*			str;
};

struct unary_expr_t : expr_t {
	expr_t*				child;
};

struct binary_expr_t : expr_t {
	expr_t*				child[ 2 ];
};

struct new_expr_t : expr_t {
	type_t*				newtype;
	type_t*				elemtype;
};

struct dtnew_expr_t : new_expr_t {
	expr_t*				newname;
};

struct delete_expr_t : expr_t {
	expr_t*				delobject;
	type_t*				elemtype;
};

struct func_t;
struct decl_t;
struct type_seq_t;

struct func_expr_t : binary_expr_t {
	func_t*				func;
	type_seq_t*			tmpltype;
	expr_t*				qual;
	scope_t*			scope;
	bool				virtcall;
};

expr_t*
make_iconst_expr(
	intval_t			val,
	type_t*				type );

expr_t*
make_fconst_expr(
	fltval_t			val,
	type_t*				type );
	
expr_t*
make_id_expr(
	qualname_t*			id );
		
expr_t*
make_str_expr(
	const char*			str );
		
expr_t*
make_null_expr();

expr_t*
make_this_expr();
		
expr_t*
make_unary_expr(
	e_expr_code			code,
	expr_t*				child );

expr_t*
make_binary_expr(
	e_expr_code			code,
	expr_t*				left_child,
	expr_t*				right_child );
	
expr_t*
make_func_expr(
	func_t*				func,
	expr_t*				left_child,
	expr_t*				right_child );

expr_t*
make_tmpl_func_expr(
	func_t*				func,
	type_seq_t*			tmpltype,
	expr_t*				left_child,
	expr_t*				right_child );

expr_t*
make_new_expr(
	type_t*				type );

expr_t*
make_dtnew_expr(
	type_t*				type,
	expr_t*				name );

expr_t*
make_delete_expr(
	expr_t*				object );

expr_t*
make_variable_expr(
	variable_t*			var );
		
expr_t*
make_thrown_object_expr(
	type_t*				type );
		
expr_t*
eval_expr(
	expr_t*				expr );
		
bool
type_expr(
	expr_t*				expr );

// helpers

enum {
	NOT_CONST,
	INT_CONST,
	FLT_CONST
};

bool
get_expr_iconst(
	expr_t**			expr,
	intval_t&			value );

bool
get_expr_fconst(
	expr_t**			expr,
	fltval_t&			value );
		
struct type_list_t;

type_list_t*
cast_param_expr(
	expr_t**			expr,
	type_list_t*		node );
		
END_COMPILER_NAMESPACE
