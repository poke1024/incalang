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
//	gfx_8.cp				   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "gfx_driver.h"
#include <math.h>

BEGIN_ICARUS_NAMESPACE

// ---------------------------------------------------------------------------

#define DRV_BYTES_PER_PIXEL 1

inline void
DRV_PUT_PIXEL_8( u08* dstpix, u32 color )
{
	*dstpix = color;
}

inline u32
DRV_GET_PIXEL_8( const u08* dstpix )
{
	return *dstpix;
}

inline u08*
DRV_PUT_HLINE_8( u08* dstpix, long cnt, u32 color )
{
	switch( ( (u64)dstpix ) & 3 )
	{
		case 0:
			break;
			
		case 1:
			*dstpix++ = color;
			if( !--cnt ) return dstpix;

		case 2:
			*dstpix++ = color;
			if( !--cnt ) return dstpix;

		case 3:
			*dstpix++ = color;
			if( !--cnt ) return dstpix;
	}
	
	if( cnt >= 4 )
	{
		u32 lcolor = color;
		lcolor |= lcolor << 8;
		lcolor |= lcolor << 16;		
		
		u32* ldstpix = (u32*)dstpix;
		long lcnt = cnt >> 2;
		do {
			*ldstpix++ = lcolor;
		} while( --lcnt );
		cnt &= 3;
		dstpix = (u08*)ldstpix;
	}

	switch( cnt )
	{
		case 0:
			break;
			
		case 3:
			*dstpix++ = color;
			
		case 2:
			*dstpix++ = color;
			
		case 1:
			*dstpix++ = color;
			break;
	}
	
	return dstpix;
}

#define DRV_PUT_PIXEL DRV_PUT_PIXEL_8
#define DRV_GET_PIXEL DRV_GET_PIXEL_8
#define DRV_PUT_HLINE DRV_PUT_HLINE_8

// ---------------------------------------------------------------------------

void put_pixel_8( bitmap_data_t* bmp, long x, long y, u32 color )
{
	#include "drv_put_pixel.h"
}

u32 get_pixel_8( bitmap_data_t* bmp, long x, long y )
{
	#include "drv_get_pixel.h"
}

void hline_8( bitmap_data_t* bmp,
	long x1, long y, long x2, u32 color )
{
	#include "drv_hline.h"
}

void vline_8( bitmap_data_t* bmp,
	long x, long y1, long y2, u32 color )
{
	#include "drv_vline.h"
}

void line_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_line.h"
}

void fill_rect_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_rect.h"
}

void shade_rect_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	fill_rect_8( bmp, x1, y1, x2, y2, color );
}

void frame_oval_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_frame_oval.h"
}

void fill_oval_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_oval.h"
}

END_ICARUS_NAMESPACE
