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
//	icarus_memory.cpp		   ©2000-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

// a memory unit capable of real-time constant time memory allocation

#include "icarus.h"
#include <cstdlib>
#include <cstring>
#include <Windows.h>
#include <new>

BEGIN_ICARUS_NAMESPACE

#if ICARUS_RT_MEMORY

bool					s_rt_malloc = false;

static u08**			s_buckets;
static int				s_bucket_count;

static u08*				s_mem_top;
static u08*				s_mem_current;
static size_t			s_mem_size;
static size_t			s_mem_left;

// ---------------------------------------------------------------------------

struct header_t {
	size_t				alloc;
	size_t				used;
};

inline header_t*
rt_malloc_header(
	 void*				p )
{
	u08*				q = (u08*)p;
	q -= sizeof( header_t );
	return (header_t*)q;
}

static int
ilog2(
  size_t				size )
{
	int					bits = 0;
	size_t				val = 1;

	while( size > val )
	{
		bits++;
		val <<= 1;
	}

	return bits;
}

// ---------------------------------------------------------------------------

static void*
core_alloc(
	size_t				size )
{
	LPBYTE				p;

	p = (LPBYTE)LocalAlloc( LMEM_FIXED, size );
	if( p == 0 )
		icarus_fatal( "could not allocate core memory" );
	return p;
}

static void
core_free(
	void*				p )
{
	LocalFree( p );
}

void
imalloc_init(
   size_t				size )
{
	s_mem_size = size;
	s_mem_top = (u08*)core_alloc( size );

	s_bucket_count = ilog2( size );
	s_buckets = (u08**)core_alloc(
		s_bucket_count * sizeof( u08* ) );
	memset( s_buckets, 0,
		sizeof( u08* ) * s_bucket_count );

	s_mem_current = s_mem_top;
	s_mem_left = size;

	VirtualLock( s_mem_top, size );
	VirtualLock( s_buckets,
		sizeof( u08* ) * s_bucket_count );

	s_rt_malloc = true;
}

void
imalloc_cleanup()
{
	s_rt_malloc = false;

	VirtualUnlock( s_mem_top, s_mem_size );
	VirtualUnlock( s_buckets,
		sizeof( u08* ) * s_bucket_count );

	core_free( s_buckets );
	core_free( s_mem_top );
}

// ---------------------------------------------------------------------------

#if ICARUS_DEBUG
#define tag_memory( p, size )\
	memset( p, 0xfe, size )
#define mem_balance( amount )\
    if( s_mem_track ) s_mem_alloc += ( amount );
#else
#define tag_memory( p, size )
#define mem_balance( amount )
#endif

// ---------------------------------------------------------------------------

void*
imalloc(
	size_t				size )
{
	if( size < 1 )
		return NULL;

	u08					index = ilog2( size );

	if( index >= s_bucket_count )
		icarus_fatal( "not enough memory" );

	u08*				p = s_buckets[ index ];

	if( p )
	{
		rt_malloc_header( p )->used = size;
		s_buckets[ index ] = *(u08**)p;
		tag_memory( p, size );
		mem_balance( size );
		return p;
	}

	size_t				allocsize = 1L << index;
	
	if( allocsize < sizeof( void* ) )
		allocsize = sizeof( void* );
	
	size_t				fullsize = allocsize + sizeof( header_t );

	if( s_mem_left < fullsize )
		icarus_fatal( "out of memory" );

	p = s_mem_current;
	s_mem_left -= fullsize;
	s_mem_current += fullsize;

	header_t*			header = (header_t*)p;

	header->alloc = allocsize;
	header->used = size;
	p += sizeof( header_t );

	tag_memory( p, size );
	mem_balance( size );
	return p;
}

void
ifree(
	void*				p )
{
	if( !p )
		return;

	header_t*			header = rt_malloc_header( p );
	int					index = ilog2( header->alloc );

	tag_memory( p, header->alloc );
	mem_balance( -header->used );

	*(void**)p = s_buckets[ index ];
	s_buckets[ index ] = (u08*)p;
}

void*
icalloc(
	size_t				size,
	size_t				count )
{
	size_t				bytes = size * count;
	void*				p = imalloc( bytes );
	memset( p, 0, bytes );
	return p;
}

void*
irealloc(
	void*				p,
	size_t				qsize )
{
	if( !p )
		return imalloc( qsize );

	header_t*			header = rt_malloc_header( p );
	size_t				psize = header->used;
	void*				q = imalloc( qsize );
	
	memcpy( q, p, ( psize < qsize ) ? psize : qsize );
	ifree( p );

	return q;
}

char*
istrdup(
	const char*		s )
{
	char*			t;

	if( !s )
		return 0;

	t = (char*)imalloc(
		sizeof( char ) * ( strlen( s ) + 1 ) );
	strcpy( t, s );

	return t;
}

size_t
imemfree()
{
	return s_mem_left;
}

#endif

END_ICARUS_NAMESPACE
