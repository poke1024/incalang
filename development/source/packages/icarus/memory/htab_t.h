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
//	htab_t.h
// ===========================================================================

// this whole file is a variation of hashtab.c, written by Bob Jenkins, 1996
// and placed in the Public Domain.

#ifndef HTAB_H
#define HTAB_H

//#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

enum {
	CACHED_KEY_SIZE		= 32
};

struct hitem_t {
	const u08*			key;		// key that is hashed
	u32					keyl;		// length of key
	void*				stuff;		// stuff stored in this hitem
	u32					hval;		// hash value
	hitem_t*			next;		// next hitem in list
};

struct htab_iterator_t {
	u32					apos;
	hitem_t*			ipos;
};

struct htab_t {
	hitem_t**			table;		// hash table, array of size 2^logsize
	u16					logsize;	// log of size of table
	size_t				mask;		// (hashval & mask) is position in table
	u32					count;		// how many items in this hash table so far?
	u32					apos;		// position in the array
	hitem_t*			ipos;		// current item in the array
	u32					bcount;		// # hitems useable in current block
	
	u08					fkey[ CACHED_KEY_SIZE ];	// key last last searched
	u32					fkeyl;		// length of key last searched
	u32					flookup;	// lookup value of key last searched
};

htab_t*
hcreate(
	int					logsize );
	
void
hdispose(
	htab_t*				t );

bool
hfind(
	htab_t*				t,
	const u08*			key,
	long				keyl );

bool
hfindnext(
	htab_t*				t,
	const u08*			key,
	long				keyl );

void
hadd(
	htab_t* 			t,
	const u08*			key,
	long				keyl,
	void*				stuff );

inline const u08*
hkey(
	htab_t*				t )
{
	return t->ipos->key;
}

inline long
hkeyl(
	htab_t*				t )
{
	return t->ipos->keyl;
}

inline hitem_t*
hitem(
	htab_t*				t )
{
	return t->ipos;
}

inline void*
hstuff(
	htab_t*				t )
{
	return t->ipos->stuff;
}

bool
hfirst(
	htab_t*				t );

bool
hnbucket(
	htab_t*				t );

inline bool
hnext(
	htab_t*				t )
{
	hitem_t*			ipos;

	if( ( ipos = t->ipos ) == 0 )
		return false;
		
	if( ( t->ipos = ipos->next ) != 0 )
		return true;
		
	return hnbucket( t );
}

inline void
hgetiterator(
	htab_t*				t,
	htab_iterator_t*	it )
{
	it->apos = t->apos;
	it->ipos = t->ipos;
}

inline void
hsetiterator(
	htab_t*				t,
	htab_iterator_t*	it )
{
	t->apos = it->apos;
	t->ipos = it->ipos;
}

END_ICARUS_NAMESPACE

#endif

