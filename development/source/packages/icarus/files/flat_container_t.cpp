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
//	flat_container_t.cp		   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "flat_container_t.h"
#include "icarus_decrypt.h"

BEGIN_ICARUS_NAMESPACE

inline u08
fetch_u08(
	const u08*&			ptr )
{
	return *ptr++;
}

inline u32
fetch_u32(
	const u08*&			ptr )
{
	u32					val;
	
	val = *(u32*)ptr;
	ptr += sizeof( u32 );

	return cond_swap_endian32( val );
}

inline u64
fetch_u64(
	const u08*&			ptr )
{
	u32					hi, lo;

	hi = fetch_u32( ptr );
	lo = fetch_u32( ptr );
	return ( ( (u64)hi ) << 32 ) | lo;
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

static int
calc_hash(
	const char*			text,
	int					textlength )
{
	const u08*			k = (const u08*)text;
	int					length = textlength * sizeof( char );
	
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

	return c & ( TOC_HASH_SIZE - 1 );
}

// ---------------------------------------------------------------------------

flat_container_t::flat_container_t(
	const char*			filename )
{
	for( int i = 0; i < TOC_HASH_SIZE; i++ )
		m_toc[ i ] = 0;

	m_file = os_fopen( filename, "rb" );
	if( !m_file )
		throw_icarus_error( "container file not found" );
		
	read_toc();
}

flat_container_t::~flat_container_t()
{
	os_fclose( m_file );

	for( int i = 0; i < TOC_HASH_SIZE; i++ )
	{
		toc_entry_t*	entry = m_toc[ i ];
		toc_entry_t*	next;
		
		while( entry )
		{
			next = entry->next;
			delete entry;
			entry = next;
		}
	}
}

toc_entry_t*
flat_container_t::lookup(
	const char*			filename )
{
	int					hash = calc_hash(
							filename, std::strlen( filename ) );
	toc_entry_t*		entry = m_toc[ hash ];

	while( entry )
	{
		if( not std::strcmp( entry->name.c_str(), filename ) )
			return entry;
			
		entry = entry->next;
	}
	
	return 0;
}

void
flat_container_t::read_toc()
{
	os_file_t			fp = m_file;
	u32					zero_offset;
		
	os_fread( &zero_offset, sizeof( zero_offset ), fp );
	decrypt( &zero_offset, sizeof( zero_offset ), 0 );
	zero_offset = cond_swap_endian32( zero_offset );
	
	u08*				tocdata = new u08[ zero_offset ];
	
	os_fread( tocdata, zero_offset, fp );
	decrypt( tocdata, zero_offset, 4 );
		
	const u08*			tocptr = tocdata;
	long				entry_count;
	
	entry_count = fetch_u32( tocptr );

	char*				buffer = new char[ 256 ];

	for( long i = 0; i < entry_count; i++ )
	{
		long			length;
		
		length = fetch_u32( tocptr );
		if( length >= 256 )
		{
			delete[] buffer;
			delete[] tocdata;
			throw_icarus_error( "flat file name too long" );
		}
		
		std::memcpy( buffer, tocptr, length * sizeof( char ) );
		tocptr += length * sizeof( char );
		buffer[ length ] = '\0';
		
		toc_entry_t*	entry;
		
		entry = new toc_entry_t;

		entry->name = buffer;
		entry->offset = fetch_u64( tocptr ) + zero_offset;
		entry->length = fetch_u64( tocptr );
		entry->flags = fetch_u08( tocptr );
		
		int				hash = calc_hash( buffer, length );
		
		entry->next = m_toc[ hash ];
		m_toc[ hash ] = entry;
	}

	delete[] buffer;
	delete[] tocdata;
}

END_ICARUS_NAMESPACE
