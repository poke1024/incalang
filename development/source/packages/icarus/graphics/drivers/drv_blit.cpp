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
//	gfx_blit.cp		 		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "gfx_vtable.h"
#include "gfx_macro.h"

BEGIN_ICARUS_NAMESPACE

inline void
copy_bits(
	const u08* src, u08* dst, long cnt )
{
	int srcal = ( (u64)src ) & 3;
	int dstal = ( (u64)dst ) & 3;

	if( srcal != dstal )
	{
		do {
			*dst++ = *src++;
		} while( --cnt );
		return;
	}

	switch( srcal )
	{
		case 0:
			break;

		case 1:
			*dst++ = *src++; 
			if( !--cnt ) return;
		
		case 2:
			*dst++ = *src++;
			if( !--cnt ) return;

		case 3:
			*dst++ = *src++;
			if( !--cnt ) return;
			break;
	}
	
	if( cnt >= 4 )
	{
		const u32* lsrc = (const u32*)src;
		u32* ldst = (u32*)dst;
		long lcnt = cnt >> 2;
		do {
			*ldst++ = *lsrc++;
		} while( --lcnt );
		cnt &= 3;
		src = (const u08*)lsrc;
		dst = (u08*)ldst;
	}

	switch( cnt )
	{
		case 0:
			break;
			
		case 3:
			*dst++ = *src++;
			
		case 2:
			*dst++ = *src++;
			
		case 1:
			*dst++ = *src++;
			break;
	}
}

STD_BLIT_PROTO( std_blit_8 )
{
	const long srcpitch = src_bmp->pitch;
	const long dstpitch = dst_bmp->pitch;

	const u08* srcrow = (const u08*)src_bmp->pixels;
	srcrow += srcpitch * src_y + src_x;
	
	u08* dstrow = (u08*)dst_bmp->pixels;
	dstrow += dstpitch * dst_y + dst_x;

	long ycount = height;
	do {
		copy_bits( srcrow, dstrow, width );
		srcrow += srcpitch;
		dstrow += dstpitch;
	} while( --ycount );
}

STD_BLIT_PROTO( std_blit_16 )
{
	const long srcpitch = src_bmp->pitch;
	const long dstpitch = dst_bmp->pitch;

	const u08* srcrow = (const u08*)src_bmp->pixels;
	srcrow += srcpitch * src_y + ( src_x << 1 );
	
	u08* dstrow = (u08*)dst_bmp->pixels;
	dstrow += dstpitch * dst_y + ( dst_x << 1 );

	const long width2 = width << 1;
	long ycount = height;
	do {
		copy_bits( srcrow, dstrow, width2 );
		srcrow += srcpitch;
		dstrow += dstpitch;
	} while( --ycount );
}

STD_BLIT_PROTO( std_blit_24 )
{
	const long srcpitch = src_bmp->pitch;
	const long dstpitch = dst_bmp->pitch;

	const u08* srcrow = (const u08*)src_bmp->pixels;
	srcrow += srcpitch * src_y + src_x * 3;
	
	u08* dstrow = (u08*)dst_bmp->pixels;
	dstrow += dstpitch * dst_y + dst_x * 3;

	const long width3 = width * 3;
	long ycount = height;
	do {
		copy_bits( srcrow, dstrow, width3 );
		srcrow += srcpitch;
		dstrow += dstpitch;
	} while( --ycount );
}

STD_BLIT_PROTO( std_blit_32 )
{
	const long srcpitch = src_bmp->pitch;
	const long dstpitch = dst_bmp->pitch;

	const u08* srcrow = (const u08*)src_bmp->pixels;
	srcrow += srcpitch * src_y + ( src_x << 2 );
	
	u08* dstrow = (u08*)dst_bmp->pixels;
	dstrow += dstpitch * dst_y + ( dst_x << 2 );

	const long width4 = width << 2;
	long ycount = height;
	do {
		copy_bits( srcrow, dstrow, width4 );
		srcrow += srcpitch;
		dstrow += dstpitch;
	} while( --ycount );
}

// ---------------------------------------------------------------------------

STD_BLIT_PROTO( std_cross_blit )
{
	pixel_format_t srcformat = src_bmp->format;
	pixel_format_t dstformat = dst_bmp->format;

	const long srcpitch = src_bmp->pitch;
	const long dstpitch = dst_bmp->pitch;

	const u08* srcrow = (const u08*)src_bmp->pixels;
	srcrow += srcpitch * src_y +
		src_x * ( get_pixel_format_profile( srcformat )->depth >> 3 );
	
	u08* dstrow = (u08*)dst_bmp->pixels;
	dstrow += dstpitch * dst_y +
		dst_x * ( get_pixel_format_profile( dstformat )->depth >> 3 );

	std_blit_cross_color( srcformat, dstformat, 
		srcrow, dstrow, srcpitch, dstpitch,
		width, height );
}

END_ICARUS_NAMESPACE
