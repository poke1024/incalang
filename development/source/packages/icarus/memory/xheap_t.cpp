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
//	xheap_t.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

// the whole idea of doing the memory management the way done here was
// inspired by source code I saw in a C compiler, written for the Amiga. the
// function was also called xalloc(). I'm not able to find the reference
// anymore. credits to the unknown writer!

#include "xheap_t.h"

BEGIN_ICARUS_NAMESPACE

//#define					USE_MALLOC_ONLY ICARUS_DEBUG
#define USE_MALLOC_ONLY	0

#define					BLOCK_SIZE 4096L
#if USE_MALLOC_ONLY
#define					ALLOC_LIMIT 0
#else
#define					ALLOC_LIMIT 256L
#endif

// ---------------------------------------------------------------------------

struct block_t {
	block_t*			next;
	u08					data[ 1 ];
};

struct destructor_t {
	param_func_t*		func;
	void*				param;
	destructor_t*		next;
};

static block_t*			s_block_cache = 0;

// ---------------------------------------------------------------------------

static block_t*
alloc_block()
{
	block_t*			block;

	if( s_block_cache )
	{
		block = s_block_cache;
		s_block_cache = block->next;
	}
	else
	{
		block = (block_t*)new u08[
			BLOCK_SIZE + sizeof( block_t ) ];
	}
	
	block->next = 0;
	
	return block;
}

static void
free_block(
	block_t*			block )
{
	block->next = s_block_cache;
	s_block_cache = block;
}

void
xframe_t::free_unused_memory()
{
	block_t*			block = s_block_cache;
	block_t*			next;

	while( block )
	{
		next = block->next;
		delete[] block;
		block = next;
	}
	
	s_block_cache = 0;
}

// ---------------------------------------------------------------------------

void
xframe_t::open()
{
	frame_t*			frame = &m_frame;
	
	frame->size = BLOCK_SIZE;
	frame->index = 0;
	frame->block = alloc_block();
	frame->custom = 0;
	frame->destructors = 0;
}

void
xframe_t::close()
{
	frame_t*			frame = &m_frame;

	destructor_t*		node = frame->destructors;
	
	while( node )
	{
		node->func( node->param );
		node = node->next;
	}
	
	block_t*			block = frame->block;
	block_t*			next;
	
	while( block )
	{
		next = block->next;
		free_block( block );
		block = next;
	}
	
	block = frame->custom;
	
	while( block )
	{
		next = block->next;
		delete[] block;
		block = next;
	}	
}

void*
xframe_t::alloc(
	isize_t				s )
{
	frame_t*			frame = &m_frame;

	if( s >= ALLOC_LIMIT )
	{
		block_t*		block;
		
		block = (block_t*)new u08[
			s + sizeof( block_t ) ];
	
		block->next = frame->custom;
		frame->custom = block;
		
		return block->data;
	}

	if( s & 3 )
		s += 4 - ( s & 3 );

	void*				p;

	if( frame->size >= s )
	{
		p = &frame->block->data[ frame->index ];
		frame->size -= s;
		frame->index += s;
	}
	else
	{
		block_t*		block = alloc_block();
		
		block->next = frame->block;
		frame->block = block;
		
		p = block->data;
		frame->size = BLOCK_SIZE - s;
		frame->index = s;
	}

	return p;
}

void
xframe_t::free(
	void*				p,
	isize_t				s )
{
	if( s < ALLOC_LIMIT )
		return;

	frame_t*			frame = &m_frame;
	
	block_t**			link = &frame->custom;
	block_t*			block = *link;
	
	while( block )
	{
		if( p == block->data )
		{
			*link = block->next;
			delete[] block;
			break;
		}
		
		link = &block->next;
		block = *link;
	}
}

void
xframe_t::destructor(
	param_func_t*		func,
	void*				param )
{
	frame_t*			frame = &m_frame;

	destructor_t*		node;
	
	node = (destructor_t*)alloc( sizeof( destructor_t ) );
	node->func = func;
	node->param = param;
	node->next = frame->destructors;
	frame->destructors = node;
}

// ---------------------------------------------------------------------------

xheap_t::xheap_t()
{
	open();
}

xheap_t::~xheap_t()
{
	close();
}

END_ICARUS_NAMESPACE
