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
//	lib_noise.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "vmutils.h"
#include "fiber_t.h"
#include "lib_point.h"
#include "lib_string.h"

BEGIN_MACHINE_NAMESPACE

#include "gnoise.h"
#include "gnoise_tables.h"

#include "hash.h"

// ---------------------------------------------------------------------------

inline float snoise( float val )
	{	float h = _gradient_noise( val, _gradient_perm1 ); return h + h; }

inline float noise( float val )
	{	return snoise( val ) * 0.5 + 0.5; }

inline float snoise( float u, float v )
	{	float h = _gradient_noise( u, v, _gradient_perm1 ); return h + h; }
	
inline float noise( float u, float v )
	{	return snoise( u, v ) * 0.5 + 0.5; }

inline float snoise( float u, float v, float w )
	{	float h = _gradient_noise(
		u, v, w, _gradient_perm1 ); return h + h; }

inline float noise( float u, float v, float w )
	{	return snoise( u, v, w ) * 0.5 + 0.5; }

// ---------------------------------------------------------------------------

inline float
fBm(
	const point_t*		p,
	float				lacunarity,
	float				gain,
	float				cutoff )
{
	float				value = 0;
	float				fade;
	float				amp = 1;
	float				f;
	
	float				qx = p->x;
	float				qy = p->y;
	float				qz = p->z;
	float				limit = 0.5 * cutoff;
	
	for( f = 1; f < limit; f *= lacunarity, amp *= gain )
	{
		value += amp * snoise( qx, qy, qz );
		qx *= lacunarity;
		qy *= lacunarity;
		qz *= lacunarity;
	}
		
	fade = 2. * ( cutoff - f ) / cutoff;
	fade = imin( fade, 1.f );
	
	if( fade > 0 )
		value += fade * amp * snoise( qx, qy, qz );
	
	return value;
}

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( noise_snoise_1d )
	float val = f.pop_float();
	f.push_float( snoise( val ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( noise_noise_1d )
	float val = f.pop_float();
	f.push_float( noise( val ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( noise_snoise_2d )
	float u = f.pop_float();
	float v = f.pop_float();
	f.push_float( snoise( u, v ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( noise_noise_2d )
	float u = f.pop_float();
	float v = f.pop_float();
	f.push_float( noise( u, v ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( noise_snoise_3d )
	point_t*			p = pop_point( f );
	f.push_float( snoise( p->x, p->y, p->z ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( noise_noise_3d )
	point_t*			p = pop_point( f );
	f.push_float( noise( p->x, p->y, p->z ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( noise_point_snoise )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			r = INSTANCE_SELF( point_t, block );
	
	float				px = a->x;
	float				py = a->y;;
	float				pz = a->z;

	r->x = _gradient_noise( px, py, pz, _gradient_perm1 ) * 2;
	r->y = _gradient_noise( px, py, pz, _gradient_perm2 ) * 2;
	r->z = _gradient_noise( px, py, pz, _gradient_perm3 ) * 2;
	
	f.push_block( block, POINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( noise_point_noise )
	machine_t&			m = *f.machine();
	point_t*			a = pop_point( f );
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			r = INSTANCE_SELF( point_t, block );
	
	float				px = a->x;
	float				py = a->y;;
	float				pz = a->z;

	r->x = _gradient_noise( px, py, pz, _gradient_perm1 ) + 0.5;
	r->y = _gradient_noise( px, py, pz, _gradient_perm2 ) + 0.5;
	r->z = _gradient_noise( px, py, pz, _gradient_perm3 ) + 0.5;
	
	f.push_block( block, POINT_CLASS_SIZE );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( noise_fbm )
	point_t*			p = pop_point( f );
	float				lacunarity = f.pop_float();
	float				gain = f.pop_float();
	float				cutoff = f.pop_float();

	f.push_float( fBm( p, lacunarity, gain, cutoff ) );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( hash_hash_1d_float )
	float x = f.pop_float();
	f.push_int( hash1d( x ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( hash_hash_2d_float )
	float x = f.pop_float();
	float y = f.pop_float();
	f.push_int( hash2d( x, y ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( hash_hash_3d_float )
	point_t*			a = pop_point( f );
	f.push_int( hash3d( a->x, a->y, a->z ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( hash_hash_2d_int )
	long x = f.pop_int();
	long y = f.pop_int();
	f.push_int( hash2d( x, y ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( hash_hash_3d_int )
	long x = f.pop_int();
	long y = f.pop_int();
	long z = f.pop_int();
	f.push_int( hash3d( x, y, z ) );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

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

STDLIB_FUNCTION( hash_block )
	const u08* p = (u08*)f.pop_ptr();
	long size = f.pop_int();
	f.push_int( lookup( p, size ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( hash_string )
	string_t* s = pop_string( f );
	f.push_int( lookup( (u08*)s->text, s->length * sizeof( char ) ) );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

/*
native float snoise( float );
native float snoise( float, float );
native float snoise( const Point& );
native Point psnoise( const Point& );

native float noise( float );
native float noise( float, float );
native float noise( const Point& );
native Point pnoise( const Point& );

native float fBm( const Point&, float, float, float );

native int hash( float );
native int hash( float, float );
native int hash( const Point& );
native int hash( int, int );
native int hash( int, int, int );
native int hash( const void*, int );
native int hash( const String& );
*/

void
init_noise(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function( m, rootclss,
		"snoise;f", noise_snoise_1d, 0 );
	register_native_function( m, rootclss,
		"snoise;f;f", noise_snoise_2d, 0 );
	register_native_function( m, rootclss,
		"snoise;.Point::&", noise_snoise_3d, 0 );
	register_native_function( m, rootclss,
		"psnoise;.Point::&", noise_point_snoise, 0 );

	register_native_function( m, rootclss,
		"noise;f", noise_noise_1d, 0 );
	register_native_function( m, rootclss,
		"noise;f;f", noise_noise_2d, 0 );
	register_native_function( m, rootclss,
		"noise;.Point::&", noise_noise_3d, 0 );
	register_native_function( m, rootclss,
		"pnoise;.Point::&", noise_point_noise, 0 );

	register_native_function( m, rootclss,
		"fBm;.Point::&;f;f;f", noise_fbm, 0 );
		
	register_native_function( m, rootclss,
		"hash;f", hash_hash_1d_float, 0 );
	register_native_function( m, rootclss,
		"hash;f;f", hash_hash_2d_float, 0 );
	register_native_function( m, rootclss,
		"hash;.Point::&", hash_hash_3d_float, 0 );

	register_native_function( m, rootclss,
		"hash;i;i", hash_hash_2d_int, 0 );
	register_native_function( m, rootclss,
		"hash;i;i;i", hash_hash_3d_int, 0 );	

	register_native_function( m, rootclss,
		"hash;.v*;i", hash_block, 0 );	
	register_native_function( m, rootclss,
		"hash;.String::&", hash_string, 0 );	
}

END_MACHINE_NAMESPACE
