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
//	xalloc.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

// the whole idea of doing the memory management the way done here was
// inspired by source code I saw in a C compiler, written for the Amiga. the
// function was also called xalloc(). I'm not able to find the reference
// anymore. credits to the unknown writer!

#include "xalloc.h"
#include <cstring>
#include <cstdlib>
#include <new>
#include "st_mem_track.h"

BEGIN_ICARUS_NAMESPACE

using namespace std;

#define					DEBUG_XALLOC 1

int						s_frame_index = -1;
int						s_frame_max = 0;
xframe_t*				s_frames = 0;

#if DEBUG_XALLOC
int						s_closed_count = 0;
#endif

void
push_xalloc_frame()
{
#if DEBUG_XALLOC
	if( s_closed_count > 0 )
		throw_icarus_error( "illegal xalloc push" );
#endif

	int					index = s_frame_index;

	if( !s_frames )
	{
		st_mem_track	track( false );
	
		s_frame_max = 32;
		s_frames = (xframe_t*)imalloc(
			s_frame_max * sizeof( xframe_t ) );
		if( s_frames == 0 )
			throw std::bad_alloc();
	}

	if( ++index >= s_frame_max )
	{
		st_mem_track	track( false );

		s_frame_max *= 2;
		s_frames = (xframe_t*)irealloc(
			s_frames, s_frame_max * sizeof( xframe_t ) );
		if( s_frames == 0 )
			throw std::bad_alloc();
	}
	
	s_frame_index = index;
	
	xframe_t*			frame = &s_frames[ index ];
	
	frame->open();
}

void
pop_xalloc_frame()
{
	int					index = s_frame_index;
	xframe_t*			frame = &s_frames[ index ];

	frame->close();
	
	if( index-- < 0 )
	{
		throw_icarus_error( "xalloc frame underflow" );
		return;
	}

	s_frame_index = index;
}

void
open_xalloc_frame()
{
	s_frame_index++;

#if DEBUG_XALLOC
	s_closed_count--;
#endif
}

void
close_xalloc_frame()
{
	if( --s_frame_index < 0 )
	{
		s_frame_index = 0;
		throw_icarus_error( "xalloc frame underflow" );
	}

#if DEBUG_XALLOC
	s_closed_count++;
#endif
}

long
xalloc_frame_index()
{
	return s_frame_index;
}

void*
xalloc(
	isize_t				s )
{
#if DEBUG_XALLOC
	if( !s_frames || s_frame_index < 0 )
		throw_icarus_error( "illegal xalloc" );
#endif

	const int			index = s_frame_index;

	xframe_t*			frame = &s_frames[ index ];

	return frame->alloc( s );
}

void
xfree(
	void*				p,
	isize_t				s )
{
#if DEBUG_XALLOC
	if( !s_frames || s_frame_index < 0 )
		throw_icarus_error( "illegal xfree" );
#endif

	const int			index = s_frame_index;
	xframe_t*			frame = &s_frames[ index ];
	
	frame->free( p, s );
}

void
xdestructor(
	param_func_t*		func,
	void*				param )
{
#if DEBUG_XALLOC
	if( !s_frames )
		throw_icarus_error( "no xalloc frames allocated" );
#endif

	const int			index = s_frame_index;
	xframe_t*			frame = &s_frames[ index ];

	frame->destructor( func, param );
}

char*
xstrdup(
	const char*			s )
{
	const isize_t		length = ( istrlen( s ) + 1 ) * sizeof( char );

	char*				t = (char*)xalloc( length );

	imemcpy( t, s, length );
	
	return t;
}

END_ICARUS_NAMESPACE
