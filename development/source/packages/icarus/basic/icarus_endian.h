// ===========================================================================
//	icarus_endian.h		 	   ©2000-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_ENDIAN_H
#define ICARUS_ENDIAN_H

//#pragma once

BEGIN_ICARUS_NAMESPACE

inline u32 swap_endian32( u32 x )
{
	return ( ( ( ( x >> 8 ) & 0xff ) | ( ( x << 8 ) & 0xff00 ) ) << 16 ) |
		( ( ( x >> 24 ) & 0xff ) | ( ( ( x >> 16 ) << 8 ) & 0xff00 ) );
}

inline u16 swap_endian16( u16 x )
{
	return ( ( x >> 8 ) & 0xff ) | ( ( x << 8 ) & 0xff00 );
}

inline u64 swap_endian64( u64 x )
{
	u32 x0 = swap_endian32( (u32)x );
	u32 x1 = swap_endian32( (u32)( x >> 32 ) );
	return ( ( (u64)x0 ) << 32 ) | x1;
}

inline u64 cond_swap_endian64( u64 x )
{
#if ICARUS_IS_BIG_ENDIAN
	return x;
#else
	return swap_endian64( x );
#endif
}

inline u32 cond_swap_endian32( u32 x )
{
#if ICARUS_IS_BIG_ENDIAN
	return x;
#else
	return swap_endian32( x );
#endif
}

inline u16 cond_swap_endian16( u16 x )
{
#if ICARUS_IS_BIG_ENDIAN
	return x;
#else
	return swap_endian16( x );
#endif
}

END_ICARUS_NAMESPACE

#endif

