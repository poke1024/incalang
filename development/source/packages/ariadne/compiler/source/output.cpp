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
//	output.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

// CAUTION: out_flush must NOT be called prior to the final write out of the
// data, or references obtained via out_tell will be invalid!

#include "output.h"
#include <cstdio>
#include <cstring>
#include "file_stream_t.h"

BEGIN_COMPILER_NAMESPACE

using namespace std;

#define BLOCK_SIZE		1024
#define POOL_SIZE		64

struct block_t {
	u16					size;
	block_t*			next;
	unsigned long		offset;
	u08					data[ 0 ];
};

static output_stream_t*	s_stream;
static istring*			s_string;
static pos_t			s_pos = { 0, 0 };
static block_t*			s_first_block = 0;

static block_t*			s_free_block = 0;

static void
alloc_block_pool()
{
	long				node_size;
	u08*				pool;
	
	node_size = BLOCK_SIZE + sizeof( block_t );

	pool = new u08[ node_size * POOL_SIZE ];

	u08*				node = pool;

	for( long i = 0; i < POOL_SIZE; i++ )
	{
		block_t*		block;

		block = (block_t*)node;
		block->next = s_free_block;
		s_free_block = block;
		node += node_size;
	}
}

static block_t*
alloc_block(
	block_t*			oldblock )
{
	block_t*			block;

	while( true )
	{
		if( ( block = s_free_block ) )
		{
			s_free_block = block->next;
			break;
		}
		else
			alloc_block_pool();
	}
	
	block->next = 0;
	block->size = 0;
	
	if( oldblock )
	{
		oldblock->next = block;
		block->offset = oldblock->offset + BLOCK_SIZE;
	}
	else
		block->offset = 0;
	
	return block;
}

static void
free_block(
	block_t*			block )
{
	block->next = s_free_block;
	s_free_block = block;
}

static void
adjust_block_size()
{
	block_t*			block = (block_t*)s_pos.block;
	
	if( block->next == 0 )
	{
		const size_t	block_size = block->size;
		const size_t	offset = s_pos.offset;
		
		if( offset > block_size )
			block->size = offset;
	}
}

static void
out_flush()
{		
	adjust_block_size();

	output_stream_t*	stream = s_stream;
	block_t*			block = s_first_block;
	
	while( block )
	{
		block_t*		next = block->next;
		
		long			size;
		
		if( next )
			size = BLOCK_SIZE;
		else
			size = block->size;
			
		if( stream )
			stream->write_bytes( block->data, size );
		else
			s_string->append( (char*)block->data, size );
			
		free_block( block );
		
		block = next;
	}
	
	block = alloc_block( 0 );
	s_first_block = block;
	
	s_pos.block = block;
	s_pos.offset = 0;
}

// ===========================================================================

void
out_free_unused_memory()
{
}

void
out_open_inline()
{
	block_t*			block = alloc_block( 0 );
	
	s_first_block = block;
	
	s_pos.block = block;
	s_pos.offset = 0;
}

void*
out_close_inline()
{
	adjust_block_size();
	return s_first_block;
}

void
out_open_stream(
	output_stream_t*	stream )
{
	s_stream = stream;
	out_open_inline();
}

void
out_close_stream()
{
	s_stream->reserve( out_tell() );

	out_flush();
	s_stream = 0;
	free_block( s_first_block );
}

void
out_open_string(
	istring&			string )
{
	s_string = &string;
	out_open_inline();
}

void
out_close_string()
{
	out_flush();
	s_string = 0;
	free_block( s_first_block );
}

void
out_byte(
	int					val )
{
	pos_t*				pos = &s_pos;
	
	block_t*			block = (block_t*)pos->block;
	long				offset = pos->offset;

	if( offset >= BLOCK_SIZE )
	{
		block_t*		oldblock = block;
		
		block = oldblock->next;
		if( !block )
			block = alloc_block( oldblock );

		pos->block = block;
		offset = 0;
	}
	
#if ICARUS_SAFE
	if( offset < 0 || offset >= BLOCK_SIZE )
		throw_icarus_error( "invalid block access" );
#endif
	
	block->data[ offset ] = val;
	pos->offset = offset + 1;
}

void
out_word(
	unsigned int		val )
{
	out_byte( val >> 8 );
	out_byte( val );
}

void
out_long(
	unsigned long		val )
{
	out_byte( val >> 24 );
	out_byte( val >> 16 );
	out_byte( val >> 8 );
	out_byte( val );
}

void
out_wide(
	u64					val )
{
	out_long( val >> 32 );
	out_long( val & 0xffffffffL );
}

void
out_float(
	float				val )
{
	out_long( ( (u32*)&val )[ 0 ] );
}

void
out_double(
	double				val )
{
	out_wide( *(u64*)&val );
}

void
out_string(
	const char*			s )
{
	unsigned int		len = strlen( s );

	out_word( len );

	out_block( (u08*)s, len );
}

void
out_block(
	const u08*			data,
	size_t				size )
{
	pos_t*				pos = &s_pos;
	
	block_t*			block = (block_t*)pos->block;
	long				offset = pos->offset;

	if( offset + size > BLOCK_SIZE )
	{	
		size_t			partsize = BLOCK_SIZE - offset;

#if ICARUS_SAFE
		if( offset < 0 || partsize < 0 ||
			partsize + offset > BLOCK_SIZE )
			throw_icarus_error( "invalid block access" );
#endif
	
		if( partsize )
			memmove( &block->data[ offset ], data, partsize );
		
		data += partsize;
		size -= partsize;

		if( size > BLOCK_SIZE )
			throw_icarus_error( "output block too large" );

		block_t*		oldblock = block;
	
		block = oldblock->next;
		if( !block )
			block = alloc_block( oldblock );
		
		pos->block = block;

#if ICARUS_SAFE
		if( size < 0 || size > BLOCK_SIZE )
			throw_icarus_error( "invalid block access" );
#endif
		
		if( size )
			memmove( &block->data[ 0 ], data, size );
	
		pos->offset = size;
	}
	else
	{
#if ICARUS_SAFE
		if( offset < 0 || size < 0 ||
			size + offset > BLOCK_SIZE )
			throw_icarus_error( "invalid block access" );
#endif

		memmove( &block->data[ offset ], data, size );
		pos->offset = offset + size;
	}
}

void
out_inline(
	void*				data )
{
	// write and release an inlined block of data

	block_t*			block = (block_t*)data;

	while( block )
	{
		block_t*		next = block->next;

		if( next )
			out_block( block->data, BLOCK_SIZE );
		else
			out_block( block->data, block->size );

		free_block( block );
		block = next;
	}
}

unsigned long
out_tell()
{
	pos_t*				pos = &s_pos;
	
	block_t*			block = (block_t*)pos->block;
			
	return block->offset + pos->offset;
}

void
out_tell_pos(
	pos_t*				pos )
{
	*pos = s_pos;
}

void
out_seek_pos(
	const pos_t*		pos )
{
	adjust_block_size();
	
	s_pos = *pos;
}

END_COMPILER_NAMESPACE
