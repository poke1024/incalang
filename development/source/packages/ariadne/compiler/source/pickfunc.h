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
//	pickfunc.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"

BEGIN_COMPILER_NAMESPACE

struct scope_t;
struct func_t;
struct template_map_t;
struct type_list_t;
struct type_seq_t;
struct fileref_t;

enum {
	PICKFUNC_QUIET		= 1 << 0
};

const char*
user_func_name(
	const char*			name );

func_t*
pick_function(
	scope_t*			scope,
	func_t*				func,
	type_list_t*		typelist,
	type_seq_t*			tmpltype,
	fileref_t*			filepos,
	int					flags );

END_COMPILER_NAMESPACE
