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
//	gfx_macro.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#define MAKE_24_RGB(r,g,b)\
	((((long)r)<<16)|(((long)g)<<8)|((long)b))

#define EXTRACT_RED_24_RGB(c)\
	(((c)>>16)&0xff)

#define EXTRACT_GREEN_24_RGB(c)\
	(((c)>>8)&0xff)
	
#define EXTRACT_BLUE_24_RGB(c)\
	((c)&0xff)
	
#if 0

// robust

#define MIX_PIXEL_COLORS_555_RGB( c1, c2, alpha, invAlpha, result )			\
	{ unsigned int r, g, b;													\
	b = ( c1 & 31 ) * alpha + ( c2 & 31 ) * invAlpha;						\
	g = ( ( c1 >> 5 ) & 31 ) * alpha + ( ( c2 >> 5 ) & 31 ) * invAlpha;		\
	r = ( ( c1 >> 10 ) & 31 ) * alpha + ( ( c2 >> 10 ) & 31 ) * invAlpha;	\
	result = ( b >> 8 ) | ( ( g >> 8 ) << 5 ) | ( ( r >> 8 ) << 10 ); }

#define MIX_PIXEL_COLORS_565_RGB( c1, c2, alpha, invAlpha, result )			\
	{ unsigned int r, g, b;													\
	b = ( c1 & 31 ) * alpha + ( c2 & 31 ) * invAlpha;						\
	g = ( ( c1 >> 5 ) & 63 ) * alpha + ( ( c2 >> 5 ) & 63 ) * invAlpha;		\
	r = ( ( c1 >> 11 ) & 31 ) * alpha + ( ( c2 >> 11 ) & 31 ) * invAlpha;	\
	result = ( b >> 8 ) | ( ( g >> 8 ) << 5 ) | ( ( r >> 8 ) << 11 ); }

#else

// faster

#define MIX_PIXEL_COLORS_555_RGB( c1, c2, alpha, invAlpha, result )			\
	{ unsigned long m1, m2, h; unsigned int r;								\
	m1 = ( c1 & 31 ) | ( ( ( c1 >> 5 ) & 31 ) << 16 );						\
	m2 = ( c2 & 31 ) | ( ( ( c2 >> 5 ) & 31 ) << 16 );						\
	h = m1 * alpha + m2 * invAlpha;											\
	r = ( ( c1 >> 10 ) & 31 ) * alpha + ( ( c2 >> 10 ) & 31 ) * invAlpha;	\
	result = ( ( h >> 8 ) & 31 ) | ( ( ( h >> 24 ) & 31 ) << 5 ) |			\
		( r >> 8 ) << 10; }

#define MIX_PIXEL_COLORS_565_RGB( c1, c2, alpha, invAlpha, result )			\
	{ unsigned long m1, m2, h; unsigned int r;								\
	m1 = ( c1 & 31 ) | ( ( ( c1 >> 5 ) & 63 ) << 16 );						\
	m2 = ( c2 & 31 ) | ( ( ( c2 >> 5 ) & 63 ) << 16 );						\
	h = m1 * alpha + m2 * invAlpha;											\
	r = ( ( c1 >> 11 ) & 31 ) * alpha + ( ( c2 >> 11 ) & 31 ) * invAlpha;	\
	result = ( ( h >> 8 ) & 31 ) | ( ( ( h >> 24 ) & 63 ) << 5 ) |			\
		( r >> 8 ) << 11; }

#endif
