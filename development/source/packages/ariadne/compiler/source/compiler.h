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
//	compiler.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_COMPILER_H
#define ARIADNE_COMPILER_H

//#pragma once

#include "ariadne.h"
#include "icarus.h"
#include "compiler_config.h"

#define BEGIN_COMPILER_NAMESPACE		namespace compiler {
#define END_COMPILER_NAMESPACE			}

BEGIN_ICARUS_NAMESPACE
struct xhtab_t;
END_ICARUS_NAMESPACE

BEGIN_COMPILER_NAMESPACE

using namespace ariadne;
USING_ICARUS_NAMESPACE

//typedef std::size_t size_t;
enum err_t {
	ERR_INTERNAL,
	ERR_INVALID_QUALIFIERS,
	ERR_NUMBER_TOO_BIG,
	ERR_INVALID_NUMBER,
	ERR_TYPE_MISMATCH,
	ERR_ILLEGAL_TYPECAST,
	ERR_ILLEGAL_TYPENAME,
	ERR_ILLEGAL_VARNAME,
	ERR_EXPECTED_POINTER,
	ERR_ILLEGAL_TYPE,
	ERR_EXPECTED_LVALUE,
	ERR_EXPECTED_CONSTANT,
	ERR_EXPECTED_INTEGER_CONSTANT,
	ERR_SYNTAX_ERROR,
	ERR_INCOMPLETE_TYPE,
	ERR_UNDECLARED_TYPE,
	ERR_UNDECLARED_QUALNAME,
	ERR_ILLEGAL_MODIFIER,
	ERR_UNDECLARED_VARIABLE,
	ERR_EXPECTED_CLASS,
	ERR_EXPECTED_NAMESPACE,
	ERR_VARIABLE_REDEFINED,
	ERR_TYPE_REDEFINED,
	ERR_FUNCTION_REDEFINED,
	ERR_FUNCTION_AMBIVALENT,
	ERR_FUNCTION_UNDEFINED,
	ERR_EXPECTED_RETURN_VALUE,
	ERR_VARIABLE_NOT_STATIC,
	ERR_FUNCTION_NOT_STATIC,
	ERR_AMBIGOUS_VARIABLE,
	ERR_AMBIGOUS_TYPE,
	ERR_AMBIGOUS_ENUM,
	ERR_ABSTRACT_CLASS,
	ERR_ABSTRACT_VARIABLE,
	ERR_ILLEGAL_ACCESS,
	ERR_ILLEGAL_CONSTRUCTOR_ACCESS,
	ERR_ILLEGAL_DESTRUCTOR_ACCESS,
	ERR_CONSTANT_ALTERED,
	ERR_TOO_MANY_INITIALIZERS,
	ERR_UNINITIALIZED_REFERENCE,
	ERR_CASE_WITHOUT_SWITCH,
	ERR_INTEGER_CONSTANT_TOO_BIG,
	ERR_TOO_MANY_CASES,
	ERR_CASE_DEFINED_TWICE,
	ERR_CONTINUE_IN_SWITCH_BLOCK,
	ERR_DEFAULT_REDEFINED,
	ERR_CASE_EXPECTED,
	ERR_ENUM_CONSTANT_DEFINED_TWICE,
	ERR_ILLEGAL_OPERATOR_DECLARATION,
	ERR_TOO_MANY_PARAMETERS,
	ERR_TOO_MANY_TEMPLATE_PARAMETERS,
	ERR_USING_NAMESPACE_CYCLE,
	ERR_TEMPLATE_CYCLE,
	ERR_ILLEGAL_TEMPLATE_RETURN_TYPE,
	ERR_UNDETERMINED_TEMPLATE_TYPE,
	ERR_TEMPLATE_INSTANCE_FOR_NON_TEMPLATE_TYPE,
	ERR_WRONG_TYPE_COUNT_FOR_TEMPLATE_INSTANCE,
	
	ERR_BRANCH_TOO_LONG,
	ERR_TOO_MANY_FUNCTIONS,
	ERR_TOO_MANY_VARIABLES,
	ERR_CYCLE_IN_BASECLASS_GRAPH,
	ERR_NOT_IMPLEMENTED,
	ERR_DIV_BY_ZERO,
	
	ERR_FILE_OPEN_FAILED,
	ERR_LINE_TOO_LONG,
	ERR_ILLEGAL_DIRECTIVE,
	ERR_UNTERMINATED_COMMENT,
	ERR_INCLUDE_FILE_NOT_FOUND,
	ERR_ILLEGAL_COMPILER_SETTING
};

typedef u32 fileid_t;

enum {
	fileid_invalid		= 0
};

struct fileref_t {
	fileid_t			id;
	u32					pos;
};

void
compile_error(
	const fileref_t*	fpos,
	err_t				err,
	const char*			data_one = 0,
	const char*			data_two = 0 );

void
compile_setting(
	const char*			setting );

typedef u64 intval_t;
typedef double fltval_t;
typedef long caseval_t;
typedef s64 enumval_t;

typedef s32 findtag_t;

enum {
	INTVAL_BITS = 64,
	CLASS_HEADER_SIZE = 4
};

#if __VISC__
const intval_t INTVAL_MAX = (u64)0xffffffffffffffff;
#else
const intval_t INTVAL_MAX = 0xffffffffffffffffULL;
#endif

#define EMIT_TRACEBACK_TABLES				1

END_COMPILER_NAMESPACE

#endif

