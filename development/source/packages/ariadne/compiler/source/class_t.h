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
//	class_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_CLASS_H
#define ARIADNE_CLASS_H

//#pragma once

#include "compiler.h"
#include "lexer.h"

BEGIN_COMPILER_NAMESPACE

struct scope_t;
struct qualname_t;
struct qualhead_t;
struct class_list_t;
struct resolve_t;
struct func_t;
struct type_t;
struct decl_t;
struct class_t;

enum {
	CONSTRUCTOR			= 0,
	DESTRUCTOR			= 1
};

struct clsstemp_map_t;

struct class_t {
	const char*			name;
	qualhead_t*			base;
	unsigned long		basecount;
	
	fileref_t			filepos;
	scope_t*			scope;
	unsigned long		size;
	unsigned long		partial_size;
	class_t*			embedding;
	
	func_t*				cd_func[ 2 ];
	bool				is_abstract;
	qualhead_t*			friendclss;
	
	class_t*			next;
	class_t*			child;

	clsstemp_map_t*		tmap;

	// static members from functions
	decl_t*				funcstat;
	
	// resolving
	union {
	
	struct {
		unsigned long	basecount;
		bool			traversing;
		bool			complete;
	} tmp1;
	
	struct {
		class_list_t*	dependclss;
		resolve_t*		dependvars;
	} tmp2;
		
	struct {
		class_t*		help;
	} tmp3;
	
	};
	
	// used in backend only
	void*				comp;
	void*				inst;
};

struct class_list_t {
	class_list_t*		next;
	class_t*			clss;
};

struct class_spec_t {
	const char*			name;
	u08					access;
};

class_spec_t*
make_class_spec(
	const char*			name,
	int					access );

void
init_class();

const char*
template_class_name();

class_t*
begin_class(
	class_spec_t*		spec,
	qualhead_t*			base );

void
end_class();

void
add_class_friend(
	qualname_t*			name );

bool
is_class_friend(
	class_t*			clss,
	class_t*			friendclss );

struct variable_t;
struct typedef_t;
struct func_list_t;
struct enum_t;

bool
class_find_inherited_variable(
	class_t*			clss,
	const char*			name,
	fileref_t*			filepos,
	findtag_t			findtag,
	variable_t**		outvar );

bool
class_find_inherited_type(
	class_t*			clss,
	const char*			name,
	fileref_t*			filepos,
	findtag_t			findtag,
	typedef_t**			outtype );
	
bool
class_find_inherited_funcs(
	class_t*			clss,
	const char*			name,
	fileref_t*			filepos,
	findtag_t			findtag,
	func_list_t*		outfunc );

bool
class_find_inherited_enum(
	class_t*			clss,
	const char*			name,
	fileref_t*			filepos,
	findtag_t			findtag,
	enum_t**			outenum );

unsigned long
get_class_size(
	class_t*			clss );
	
bool
is_class_size_known(
	class_t*			clss );

bool
is_class_abstract(
	scope_t*			instancescope,
	class_t*			clss );

bool
check_if_cd_access_valid(
	fileref_t*			fpos,
	func_t*				caller,
	type_t*				type,
	int					cd );

void
output_class_id(
	class_t*			clss );

void
output_func_id(
	func_t*				func );

enum found_t {
	NOT_FOUND,
	FOUND,
	ERROR,
	AMBIVALENT
};

found_t
is_base_class(
	class_t*			clss,
	class_t*			baseclss );
		
found_t
get_base_class_offset(
	class_t*			clss,
	class_t*			baseclss,
	unsigned long*		outoffset );

void
compile_classes(
	class_t*			clss );

void
export_classes(
	class_t*			clss );

void
compile_func(
	class_t*			clss,
	func_t*				func );

void
compile_template_func(
	class_t*			clss,
	func_t*				func );

void
compile_static_cd_func(
	class_t*			clss,
	int					index );

END_COMPILER_NAMESPACE

#endif

