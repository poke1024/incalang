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
//	compbreak.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compbreak.h"
#include "xalloc.h"

BEGIN_COMPILER_NAMESPACE

codepoint_t*			s_current_codepoint;

static breakpoint_t*	s_first_breakpoint;
static breakpoint_t**	s_breakpoint_link;

void
init_breakpoints()
{
#if EMIT_TRACEBACK_TABLES
	s_first_breakpoint = 0;
	s_breakpoint_link = &s_first_breakpoint;

	s_current_codepoint = 0;
#endif
}

codepoint_t*
mark_codepoint()
{
	codepoint_t*		codepoint = s_current_codepoint;

	if( codepoint )
		return codepoint;
		
	codepoint = (codepoint_t*)xalloc(
		sizeof( codepoint_t ) );
	
	codepoint->offset = illegal_offset;
	s_current_codepoint = codepoint;
	
	return codepoint;
}

void
mark_breakpoint(
	fileref_t*			filepos )
{
#if EMIT_TRACEBACK_TABLES
	mark_codepoint();

	breakpoint_t*		breakpoint;
	
	breakpoint = (breakpoint_t*)xalloc(
		sizeof( breakpoint_t ) );
	
	breakpoint->codepoint = mark_codepoint();
	breakpoint->filepos = *filepos;
	breakpoint->next = 0;
	
	*s_breakpoint_link = breakpoint;
	s_breakpoint_link = &breakpoint->next;
#else
	filepos;
#endif
}

void
default_breakpoint(
	fileref_t*			filepos )
{
	if( s_first_breakpoint == 0 )
		mark_breakpoint( filepos );
}

void
dump_breakpoints()
{
#if EMIT_TRACEBACK_TABLES
	pos_t				oldpos;
	pos_t				newpos;

	out_tell_pos( &oldpos );
	out_long( 0 );

	breakpoint_t*		breakpoint = s_first_breakpoint;
	long				count = 0;
	long				lastoffset = illegal_offset;
	
	while( breakpoint )
	{
		long			offset = breakpoint->codepoint->offset;
		
		if( offset != illegal_offset && offset != lastoffset )
		{
			out_long( offset );
			out_long( breakpoint->filepos.pos );
			count++;
			lastoffset = offset;
		}
		
		breakpoint = breakpoint->next;
	}
	
	out_tell_pos( &newpos );
	out_seek_pos( &oldpos );
	out_long( count );
	out_seek_pos( &newpos );
#else
	out_long( 0 );
#endif
}

END_COMPILER_NAMESPACE
