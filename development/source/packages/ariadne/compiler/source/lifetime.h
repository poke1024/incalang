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
//	lifetime.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"

BEGIN_COMPILER_NAMESPACE

struct codepoint_t;
struct type_t;

struct lifetime_t {
	lifetime_t*			next;
	const char*			name;
	type_t*				type;
	s32					offset;
	codepoint_t*		create;
	codepoint_t*		destroy;
};

void
init_lifetimes();

lifetime_t*
variable_created(
	const char*			name,
	type_t*				type,
	s32					offset );

void
variable_destroyed(
	lifetime_t*			lifetime );

void
dump_lifetimes();


END_COMPILER_NAMESPACE
