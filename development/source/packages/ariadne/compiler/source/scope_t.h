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
//	scope_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"
#include "lexer.h"

BEGIN_COMPILER_NAMESPACE

struct type_t;

enum {
	MOD_TYPEDEF			= 1L << 0,
	MOD_STATIC			= 1L << 2,
	MOD_CONST			= 1L << 3,
	MOD_NATIVE			= 1L << 4,
	MOD_ABSTRACT		= 1L << 5,
	MOD_TEMPLATE		= 1L << 6,
	MOD_DLL				= 1L << 7,
	
	MOD_LOCAL			= 1L << 8,
	MOD_TEMP			= 1L << 9,
	MOD_PARAM			= 1L << 10
};

enum {
	ACC_PUBLIC			= 0,
	ACC_PRIVATE			= 1,
	ACC_PROTECTED		= 2
};

enum {
	SCF_NAMESPACE		= 1 << 0,
	SCF_TEMPLATE		= 1 << 1,
	SCF_COMPILE			= 1 << 2,
	SCF_TTCLASS			= 1 << 3,
	SCF_DLL				= 1 << 4
};

enum {
	SCOPE_EMBEDDING		= 1L << 0,
	SCOPE_INHERITED		= 1L << 1,
	SCOPE_USING			= 1L << 2,
	SCOPE_ALL			= SCOPE_EMBEDDING + SCOPE_INHERITED + SCOPE_USING
};

struct scope_t;
struct decl_t;
struct lifetime_t;

struct variable_t {
	decl_t*				decl;
	short				modifiers;
	short				access;
	u32					offset;
	scope_t*			scope;
	union {
		lifetime_t*		lifetime;
		variable_t*		help;
	};
};

struct typedef_t {
	const char*			name;
	type_t*				type;
};

struct stmt_t;
struct scope_t;
struct functemp_map_t;

struct func_t {
	scope_t*			scope;
	decl_t*				decl;
	short				access;
	stmt_t*				stmt;
	func_t*				next;
	func_t*				same;

	// shortcuts
	decl_t*				param;

	// templates
	functemp_map_t*		tmap;

	// for backend only
	func_t*				comp;
	void*				call;

	void*				code;
};

struct enum_node_t;
struct enum_list_t;

struct enum_t {
	type_t*				type;
	enum_node_t*		node;
};

struct caster_t {
	caster_t*			next;
	type_t*				from;
	type_t*				to;
	func_t*				func;
};

struct func_list_t;

struct resolve_t {
	scope_t*			scope;
	resolve_t*			next;
	decl_t*				decl;
	type_t*				type;
	short				modifiers;
	short				access;
	fileref_t			filepos;
};

struct scope_t;

struct using_t {
	using_t*			next;
	scope_t*			scope;
};

struct qualname_t;

struct unresolved_using_t {
	unresolved_using_t*	next;
	qualname_t*			name;
	fileref_t			filepos;
};

struct class_t;

struct scope_t {
	scope_t*			up;
	xhtab_t*			variables;
	xhtab_t*			typedefs;
	xhtab_t*			functions;
	xhtab_t*			enums;
	caster_t*			casters;
	u32					funccount;
	u32					offset;
	u32					static_offset;
	class_t*			clss;
	u08					access;
	u08					flags;
	findtag_t			findtag;
	resolve_t*			vuresolve;
	resolve_t*			vrresolve;
	resolve_t*			vpresolve;
	unsigned long		depcount;
	void*				init_code;
	using_t*			usings;
	unresolved_using_t*	urusings;
};

void
init_scopes();

scope_t*
make_scope();

void
scope_export_variables(
	scope_t*			scope );

void
scope_enter_variable(
	scope_t*			scope,
	decl_t*				decl,
	int					modifiers,
	int					access,
	fileref_t*			filepos );

void
scope_enter_type(
	scope_t*			scope,
	const char*			name,
	type_t*				type,
	fileref_t*			filepos );

void
scope_declare_variable(
	scope_t*			scope,
	decl_t*				decl,
	int					modifiers,
	int					access,
	fileref_t*			filepos );

void
scope_declare_type(
	scope_t*			scope,
	const char*			name,
	type_t*				type,
	fileref_t*			filepos );

func_t*
scope_declare_function(
	scope_t*			scope,
	decl_t*				decl,
	int					access,
	stmt_t*				stmt );

void
scope_declare_enum(
	scope_t*			scope,
	const char*			name,
	enum_list_t*		list,
	fileref_t*			filepos );

bool
scope_find_variable(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	variable_t**		outvar );

bool
scope_find_variable_recursive(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	findtag_t			findtag,
	variable_t**		outvar );

bool
scope_find_type_recursive(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	findtag_t			findtag,
	typedef_t**			outtype );

bool
scope_find_functions_recursive(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	findtag_t			findtag,
	func_list_t*		outfunc );

bool
scope_find_enum_recursive(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	findtag_t			findtag,
	enum_t**			outenum );

bool
scope_find_type(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	typedef_t**			outtype );

bool
scope_find_functions(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	func_list_t*		outfunc );

bool
scope_find_enum(
	scope_t*			scope,
	const char*			name,
	fileref_t*			filepos,
	int					flags,
	enum_t**			outenum );

bool
scope_find_caster(
	scope_t*			scope,
	type_t*				fromtype,
	type_t*				totype,
	fileref_t*			filepos,
	int					flags,
	caster_t**			outcaster );

unsigned long
get_scope_size(
	scope_t*			scope );

unsigned long
get_static_scope_size(
	scope_t*			scope );

bool
is_scope_size_known(
	scope_t*			scope );

struct qualname_t {
	const char*			name;
	qualname_t*			down;
	qualname_t*			next;
	scope_t*			scope;
	fileref_t			filepos;
};

struct qualhead_t {
	qualname_t*			name;
	qualhead_t*			next;
	type_t*				type;
	fileref_t			filepos;
};

union identifier_t {
	variable_t*			variable;
	enum_t*				enumerator;
};

enum {
	identifier_none = 0,
	identifier_variable,
	identifier_enumerator
};

qualname_t*
make_qualname(
	const char*			name );

qualhead_t*
make_qualhead(
	qualname_t*			name );

type_t*
scope_find_qualname_type(
	qualname_t*			qualname );
	
int
scope_find_qualname_identifier(
	qualname_t*			qualname,
	identifier_t*		identifier );
	
func_t*
scope_find_qualname_functions(
	qualname_t*			qualname );
	
// ---------------------------------------------------------------------------

void
scope_to_string(
	scope_t*			scope,
	istring&			name );

void
qualname_to_string(
	qualname_t*			qualname,
	istring&			name );
		
// ---------------------------------------------------------------------------

type_t*
resolve_type(
	type_t*				type );

bool
scope_resolve(
	scope_t*			scope );

// ---------------------------------------------------------------------------

struct typename_list_t;

void
scope_make_namespace(
	scope_t*			scope );

void
scope_make_template(
	scope_t*			scope,
	typename_list_t*	list );

void
scope_make_dll(
	scope_t*			scope );

void
scope_make_global(
	scope_t*			scope );

void
scope_add_unresolved_using(
	scope_t*			scope,
	qualname_t*			name,
	fileref_t*			filepos );

void
scope_add_using(
	scope_t*			scope,
	scope_t*			what );

// ---------------------------------------------------------------------------

scope_t*
push_scope();

void
pop_scope();

void
undo_pop_scope();

scope_t*
cur_scope();

scope_t*
global_scope();

void
declare_scope_access(
	int					access );

void
declare_variable(
	decl_t*				decl,
	int					modifiers,
	fileref_t*			filepos );
	
void
declare_type(
	const char*			name,
	type_t*				type,
	fileref_t*			filepos );

void
declare_function(
	decl_t*				decl,
	stmt_t*				stmt );

void
declare_enum(
	const char*			name,
	enum_list_t*		list,
	fileref_t*			filepos );

END_COMPILER_NAMESPACE
