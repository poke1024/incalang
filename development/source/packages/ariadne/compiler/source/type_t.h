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
//	type_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_TYPE_H
#define ARIADNE_TYPE_H

//#pragma once

#include "compiler.h"
#include <string>
#include "lexer.h"

#ifdef TYPE_BOOL
#undef TYPE_BOOL
#endif

BEGIN_COMPILER_NAMESPACE

enum e_type_code {
	TYPE_BYTE			= 0,
	TYPE_CHAR,
	TYPE_SHORT,
	TYPE_INT,
	TYPE_LONG,
	TYPE_WIDE,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_VOID,
	TYPE_BOOL,
	TYPE_POINTER,
	TYPE_REFERENCE,
	TYPE_CLASS,
	TYPE_ENUM,
	TYPE_UNRESOLVED,
	TYPE_TYPEDEF,
	TYPE_NULL,
	TYPE_TEMPLATE,
	TYPE_NONE
};

#define TYPE_CASE_INTEGER		\
	case TYPE_BYTE:				\
	case TYPE_CHAR:				\
	case TYPE_SHORT:			\
	case TYPE_INT:				\
	case TYPE_LONG:				\
	case TYPE_WIDE:

#define TYPE_CASE_ARITHMETIC	\
	case TYPE_BYTE:				\
	case TYPE_CHAR:				\
	case TYPE_SHORT:			\
	case TYPE_INT:				\
	case TYPE_LONG:				\
	case TYPE_WIDE:				\
	case TYPE_FLOAT:			\
	case TYPE_DOUBLE:
	
enum {
	TC_NONE				= 0,
	TC_CONST			= 1L << 0,
	TC_VOLATILE			= 1L << 1
};

enum {	
	DISTANCE_MAX		= 1L << 20
};

struct type_t {
	e_type_code			code;
	u08					quals;
};

struct func_t;
struct caster_t;

struct type_list_t {
	type_list_t*		next;
	type_t*				type;
	caster_t*			cast;
};

struct type_seq_t {
	type_seq_t*			next;
	type_t*				type;
};

type_list_t*
make_type_list(
	type_list_t*		next,
	type_t*				type );

type_seq_t*
make_type_seq(
	type_seq_t*			next,
	type_t*				type );

struct int_type_t : type_t {
	bool				is_signed;
};

struct expr_t;

struct ptr_type_t : type_t {
	type_t*				down;
	bool				is_array;
	expr_t*				array_expr;
};

struct ref_type_t : type_t {
	type_t*				down;
};

struct class_t;

struct class_type_t : type_t {
	class_t*			clss;
};

struct enum_type_t : type_t {
	const char*			name;
	class_t*			clss;
};

struct qualname_t;
struct scope_t;

struct unresolved_type_t : type_t {
	qualname_t*			qualname;
	fileref_t			filepos;
};

struct typedef_type_t : type_t {
	const char*			name;
	type_t*				type;
	type_seq_t*			tmpltype;
	class_t*			clss;
};

struct template_type_t : type_t {
	u64					fcid;
	type_t*				bind;
	const char*			name;
	u16					index;
};

union tt_type_t {
	int_type_t			_0;
	ptr_type_t			_1;
	ref_type_t			_2;
	class_type_t		_3;
	enum_type_t			_4;
	unresolved_type_t	_5;
	typedef_type_t		_6;
	template_type_t		_7;
};

type_t*
make_type(
	e_type_code			code );

int_type_t*
make_int_type(
	e_type_code			code,
	bool				is_signed );
	
ptr_type_t*
make_ptr_type(
	type_t*				base_type,
	int					quals );

ref_type_t*
make_ref_type(
	type_t*				base_type,
	int					quals );

class_type_t*
make_class_type(
	class_t*			clss );

enum_type_t*
make_enum_type(
	const char*			name,
	class_t*			clss );
	
unresolved_type_t*
make_unresolved_type(
	qualname_t*			qualname );
	
typedef_type_t*
make_typedef_type(
	const char*			name,
	type_t*				type,
	type_seq_t*			tmpltype,
	class_t*			clss );

template_type_t*
make_template_type(
	const char*			name,
	int					index );
	
bool
is_type_signed(
	type_t*				type );
	
bool
is_type_arithmetic(
	type_t*				type );

bool
is_type_floating(
	type_t*				type );

bool
is_type_integral(
	type_t*				type );

bool
is_type_boolean(
	type_t*				type );

bool
is_type_size_known(
	type_t*				type );
	
long
get_type_size(
	type_t*				type );
	
type_t*
common_type(
	type_t*				typeone,
	type_t*				typetwo );
	
bool
are_types_comparable_eq(
	type_t*				typeone,
	type_t*				typetwo );

bool
are_types_comparable_lg(
	type_t*				typeone,
	type_t*				typetwo );

bool
is_type_constant(
	type_t*				type );

bool
can_assign_type(
	type_t*				typeone,
	type_t*				typetwo );

bool
can_cast_type(
	type_t*				fromtype,
	type_t*				totype );
	
bool
is_same_type(
	type_t*				typeone,
	type_t*				typetwo,
	bool				ignorequals = false );

long
type_distance(
	type_t*				fromtype,
	type_t*				totype );
	
void
type_to_string(
	type_t*				type,
	istring&			str );
	
bool
is_type_resolved(
	type_t*				type );
	
bool
is_type_abstract(
	type_t*				type );
	
void
error_undeclared_type(
	type_t*				type );
	
inline type_t*
reduce_type(
	type_t*				type )
{
	if( type->code == TYPE_TYPEDEF )
		type = ( (typedef_type_t*)type )->type;

	return type;
}

inline type_t*
pure_type(
	type_t*				type )
{
	while( true )
	{
		switch( type->code )
		{
			case TYPE_REFERENCE:
				type = ( (ref_type_t*)type )->down;
				break;
				
			case TYPE_TYPEDEF:
				type = ( (typedef_type_t*)type )->type;
				break;
	
			default:
				goto done;
		}
	}

done:
	return type;	
}

type_t*
deep_reduce_type(
	type_t*				type );
	
void
output_type_id(
	type_t*				type,
	bool				reduce = false );
	
void
output_type_string(
	type_t*				type,
	bool				reduce );

type_t**
find_bottom_type(
	type_t**			link );

int
type_record_size(
	type_t*				type );

type_t*
copy_type_shallow(
	type_t*				type );

type_t*
stripped_type(
	type_t*				type );

template_type_t*
template_type(
	type_t*				type );

END_COMPILER_NAMESPACE

#endif

