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
//	compbreak.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"
#include "lexer.h"
#include "output.h"

BEGIN_COMPILER_NAMESPACE

enum {
	illegal_offset		= -1
};

struct codepoint_t {
	s32					offset;
};

struct breakpoint_t {
	codepoint_t*		codepoint;
	breakpoint_t*		next;
	fileref_t			filepos;
};

extern unsigned long	s_code_zero_offset;
extern codepoint_t*		s_current_codepoint;

void
init_breakpoints();

codepoint_t*
mark_codepoint();

void
mark_breakpoint(
	fileref_t*			filepos );

void
default_breakpoint(
	fileref_t*			filepos );

void
dump_breakpoints();

// ---------------------------------------------------------------------------

inline codepoint_t*
current_codepoint()
{
	codepoint_t*		codepoint = s_current_codepoint;

	if( codepoint == 0 )
		codepoint = mark_codepoint();

	return codepoint;
}

inline codepoint_t*
fetch_codepoint()
{
	codepoint_t*		codepoint = s_current_codepoint;

	s_current_codepoint = 0;
	return codepoint;
}

inline void
update_codepoint(
	codepoint_t*		codepoint )
{
	if( codepoint )
		codepoint->offset = out_tell() - s_code_zero_offset;
}

END_COMPILER_NAMESPACE
