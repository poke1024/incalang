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
//	icarus_memory.h			   ©2000-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_MEMORY_H
#define ICARUS_MEMORY_H

//#pragma once

BEGIN_ICARUS_NAMESPACE

extern u32							s_mem_alloc;
extern bool							s_mem_track;

void*
imalloc(
	size_t							size );

void*
icalloc(
	size_t							size,
	size_t							count );

void*
irealloc(
	 void*							ptr,
	 size_t							size );

void
ifree(
	void*							ptr );

char*
istrdup(
	const char*						s );

#if ICARUS_RT_MEMORY

// ===========================================================================

void
imalloc_init(
   size_t				size );

void
imalloc_cleanup();

size_t
imemfree();

// ===========================================================================

inline void*
malloc(
	size_t							size )
{
	throw_icarus_error( "please use imalloc instead of malloc" );
}

inline void*
calloc(
	size_t							size,
	size_t							count )
{
	throw_icarus_error( "please use icalloc instead of calloc" );
}

inline void*
realloc(
	 void*							ptr,
	 size_t							size )
{
	throw_icarus_error( "please use irealloc instead of realloc" );
}

inline void
free(
	void*							ptr )
{
	throw_icarus_error( "please use ifree instead of free" );
}

inline char*
strdup(
	const char*						s )
{
	throw_icarus_error( "please use istrdup instead of strdup" );
}

#endif

END_ICARUS_NAMESPACE

#endif

