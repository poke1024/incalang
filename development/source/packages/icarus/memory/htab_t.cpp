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
//	htab_t.cp
// ===========================================================================

// this whole file is a variation of hashtab.c, written by Bob Jenkins, 1996
// and placed in the Public Domain.

// the hash functions in this file are taken from the lookup2.c, written by
// Bob Jenkins, December 1996, Public Domain.
// see http://burtleburtle.net/bob/hash/.

#include "htab_t.h"
#include <cstring>
#include "st_mem_track.h"

BEGIN_ICARUS_NAMESPACE

struct cache_t {
	cache_t*			next;
};

inline hitem_t**
cache_to_table(
	cache_t*			node )
{
	u08*				ptr = (u08*)node;
	ptr += sizeof( cache_t* );
	return (hitem_t**)ptr;
}

inline cache_t*
table_to_cache(
	hitem_t**			table )
{
	u08*				ptr = (u08*)table;
	ptr -= sizeof( cache_t* );
	return (cache_t*)ptr;
}

static cache_t**	s_table_cache = 0;

static hitem_t**
alloc_table(
	int					logsize )
{
	if( !s_table_cache )
	{
		st_mem_track	track( false );
	
		s_table_cache = (cache_t**)icalloc( sizeof( cache_t* ), 14 );
		if( !s_table_cache )
			throw_icarus_error( "out of memory" );
	}

#if ICARUS_SAFE
	if( logsize == 0 )
		throw_icarus_error( "invalid size" );
#endif

	if( logsize <= 14 )
	{
		cache_t**		link = &s_table_cache[ logsize - 1 ];
		cache_t*		node = *link;
		
		if( node )
		{
			*link = node->next;
			return cache_to_table( node );
		}
	}
	
	const isize_t		size = 1L << logsize;	
	st_mem_track		track( logsize > 14 );
	cache_t*			node;
	
	node = (cache_t*)new u08[
		sizeof( cache_t* ) + sizeof( hitem_t* ) * size ];
	
	return cache_to_table( node );
}

static void
dispose_table(
	hitem_t**			table,
	int					logsize )
{
	cache_t*			node = table_to_cache( table );

	if( logsize <= 14 )
	{
		cache_t**		link = &s_table_cache[ logsize - 1 ];
	
		node->next = *link;
		*link = node;
		
		return;
	}
	
	delete[] (u08*)table;
}

// ---------------------------------------------------------------------------

static void
hgrow(
	htab_t*			t )
{
	const int			oldlogsize = t->logsize;
	const int			newlogsize = ++t->logsize;

	const long			oldsize = 1L << oldlogsize;
	const long			newsize = 1L << newlogsize;
	const long			newmask = newsize - 1L;
	hitem_t**			oldtab = t->table;

	// make sure newtab is cleared
	hitem_t**			newtab;
	
	newtab = alloc_table( newlogsize );
	imemset( newtab, 0, sizeof( hitem_t* ) * newsize );
	
	// walk through old table putting entries in new table
	for( long i = oldsize; i--; )
	{		
		for( hitem_t* item = oldtab[ i ]; item; )
		{
			hitem_t*	that = item;
			
			item = item->next;
			
			hitem_t**	newplace;
			
			newplace = &newtab[ ( that->hval & newmask ) ];
			that->next = *newplace;
			*newplace = that;
		}
	}

	// free the old array
	dispose_table( oldtab, oldlogsize );
	
	t->table = newtab;
	t->mask = newmask;
}

#define mix(a,b,c) {				\
	a -= b; a -= c; a ^= (c>>13);	\
	b -= c; b -= a; b^= (a<<8);		\
	c -= a; c -= b; c ^= (b>>13);	\
	a -= b; a -= c; a ^= (c>>12);	\
	b -= c; b -= a; b ^= (a<<16);	\
	c -= a; c -= b; c ^= (b>>5);	\
	a -= b; a -= c; a ^= (c>>3);	\
	b -= c; b -= a; b ^= (a<<10);	\
	c -= a; c -= b; c ^= (b>>15);	}

static u32
lookup(
	const u08*			k,
	long				length )
{
	u32					a = 0, b = 0, c = 0;

	while( length >= 12 )
	{
		a += ( k[ 0 ] +( (u32)k[ 1 ]<<8)+((u32)k[ 2 ]<<16) +((u32)k[ 3 ]<<24));
		b += ( k[ 4 ] +( (u32)k[ 5 ]<<8)+((u32)k[ 6 ]<<16) +((u32)k[ 7 ]<<24));
		c += ( k[ 8 ] +( (u32)k[ 9 ]<<8)+((u32)k[ 10 ]<<16)+((u32)k[ 11 ]<<24));
		mix( a, b, c );
		k += 12;
		length -= 12;
	}

	c += length;
	switch( length )
	{
		// all the case statements fall through
		case 11:		c += ( (u32)k[ 10 ] << 24 );
		case 10:		c += ( (u32)k[ 9 ] << 16 );
		case 9:			c += ( (u32)k[ 8 ] << 8 );
		// the first byte of c is reserved for the length
		case 8:			b += ( (u32)k[ 7 ] << 24 );
		case 7:			b += ( (u32)k[ 6 ] << 16 );
		case 6:			b += ( (u32)k[ 5 ] << 8 );
		case 5:			b += k[ 4 ];
		case 4:			a += ( (u32)k[ 3 ]<< 24 );
		case 3:			a += ( (u32)k[ 2 ]<< 16 );
		case 2:			a += ( (u32)k[ 1 ]<< 8 );
		case 1:			a += k[ 0 ];
		// case 0: nothing left to add
	}
	mix( a, b, c );

	return c;
}

// hcreate - create a hash table initially of size power( 2, logsize )
htab_t*
hcreate(
	int					logsize )
{
	htab_t*			t = new htab_t;
	
	const long			len = 1L << logsize;

	t->table = alloc_table( logsize );
	imemset( t->table, 0, sizeof( hitem_t* ) * len );
	
	t->logsize = logsize;
	t->mask = len - 1;
	t->count = 0;
	t->apos = 0;
	t->ipos = 0;
	t->bcount = 0;
	
	t->fkeyl = 0;
		
	return t;
}

// dispose a hash table
void
hdispose(
	htab_t*			t )
{
	if( hfirst( t ) )
	{
		hitem_t*		item;
		bool			next;
	
		do {
			item = t->ipos;
			next = hnext( t );
			delete item;
		} while( next );
	}

	delete t;
}

inline u32
hfastlookup(
	htab_t*			t,
	const u08*			key,
	long				keyl )
{
	u32					x;
	
	if( keyl == t->fkeyl && !imemcmp( key, t->fkey, keyl )  )
		x = t->flookup;
	else
	{
		x = lookup( key, keyl );
		
		if( keyl <= CACHED_KEY_SIZE )
		{
			imemcpy( t->fkey, key, keyl );
			t->fkeyl = keyl;
			t->flookup = x;
		}
	}
	
	return x;
}

bool
hfind(
	htab_t*				t,
	const u08*			key,
	long				keyl )
{
	u32					x = hfastlookup( t, key, keyl );
	const u32			y = x & t->mask;	
	
	for( hitem_t* h = t->table[ y ]; h; h = h->next )
	{
		if( ( x == h->hval ) && ( keyl == h->keyl ) &&
			!imemcmp( key, h->key, keyl ) )
		{
			t->apos = y;
			t->ipos = h;
			return true;
		}
	}
	
	return false;
}

bool
hfindnext(
	htab_t*			t,
	const u08*			key,
	long				keyl )
{
	const u32			x = t->flookup;

	for( hitem_t* h = t->ipos->next; h; h = h->next )
	{
		if( ( x == h->hval ) && ( keyl == h->keyl ) &&
			!imemcmp( key, h->key, keyl ) )
		{
			t->ipos = h;
			return true;
		}
	}

	return false;
}

void
hadd(
	htab_t* 			t,
	const u08*			key,
	long				keyl,
	void*				stuff )
{
	u32					x = hfastlookup( t, key, keyl );
	u32					y = x & t->mask;
	
	// find space for a new item
	hitem_t*			h = new hitem_t;
	
	// make the hash table bigger if it is getting full
	if( ++t->count > ( 1L << t->logsize ) )
	{
		hgrow( t );
		y = x & t->mask;
	}
	
	// add the new key to the table
	h->key = key;
	h->keyl = keyl;
	h->stuff = stuff;
	h->hval = x;
	
	hitem_t**			hp = &t->table[ y ];
	h->next = *hp;
	*hp = h;
	t->ipos = h;
	t->apos = y;
}

bool
hfirst(
	htab_t*			t )
{
	t->apos = t->mask;
	hnbucket( t );
	return t->ipos != 0;
}

bool
hnbucket(
	htab_t*			t )
{
	hitem_t**			table = t->table;

	const u32			oldapos = t->apos;
	const u32			len = 1L << t->logsize;
	
	// see if the element can be found without wrapping around
	for( u32 i = oldapos + 1; i < len; i++ )
	{
		hitem_t*		item;
	
		if( ( item = table[ i ] ) != 0 )
		{
			t->apos = i;
			t->ipos = item;
			return true;
		}
	}

	// must have to wrap around to find the last element
	
	for( u32 i = 0; i <= oldapos; i++ )
	{
		hitem_t*		item;

		if( ( item = table[ i ] ) != 0 )
		{
			t->apos = i;
			t->ipos = item;
			return false;
		}
	}
	
	return false;
}

END_ICARUS_NAMESPACE
