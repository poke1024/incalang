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
//	gfx_16.cp				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "gfx_driver.h"
#include "os_core.h"
#include "gfx_macro.h"

#include "region_t.h"
#include <math.h>
#include <algorithm>

BEGIN_ICARUS_NAMESPACE

// ---------------------------------------------------------------------------

#define DRV_BYTES_PER_PIXEL 2

inline void
DRV_PUT_PIXEL_16( u08* dstpix, u32 color )
{
	*(u16*)dstpix = color;
}

inline u32
DRV_GET_PIXEL_16( const u08* dstpix )
{
	return *(u16*)dstpix;
}

inline u08*
DRV_PUT_HLINE_16( u08* dstpixb, long cnt, u32 color )
{
	u16* dstpix = (u16*)dstpixb;

	if( ( (u64)dstpix ) & 2 )
	{
		*dstpix++ = color;
		if( !--cnt ) return (u08*)dstpix;
	}
	
	if( cnt >= 2 )
	{
		u32 lcolor = color;
		lcolor |= lcolor << 16;		
		
		u32* ldstpix = (u32*)dstpix;
		long lcnt = cnt >> 1;
		do {
			*ldstpix++ = lcolor;
		} while( --lcnt );
		cnt &= 1;
		dstpix = (u16*)ldstpix;
	}

	if( cnt )
		*dstpix++ = color;
	
	return (u08*)dstpix;	
}

#define DRV_PUT_PIXEL DRV_PUT_PIXEL_16
#define DRV_GET_PIXEL DRV_GET_PIXEL_16
#define DRV_PUT_HLINE DRV_PUT_HLINE_16

// ---------------------------------------------------------------------------

void put_pixel_16( bitmap_data_t* bmp, long x, long y, u32 color )
{
	#include "drv_put_pixel.h"
}

u32 get_pixel_16( bitmap_data_t* bmp, long x, long y )
{
	#include "drv_get_pixel.h"
}

void hline_16( bitmap_data_t* bmp,
	long x1, long y, long x2, u32 color )
{
	#include "drv_hline.h"
}

void vline_16( bitmap_data_t* bmp,
	long x, long y1, long y2, u32 color )
{
	#include "drv_vline.h"
}

void line_16( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_line.h"
}

void fill_rect_16( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_rect.h"
}

void frame_oval_16( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_frame_oval.h"
}

void fill_oval_16( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_oval.h"
}

// ---------------------------------------------------------------------------

#define DRV_MIX_PIXEL_COLORS( pixel, c2, alpha, invAlpha )		\
	{ long c1 = *(u16*)(pixel);	long c3;						\
	MIX_PIXEL_COLORS_555_RGB( c1, c2, invAlpha, alpha, c3 );	\
	*(u16*)(pixel) = c3; }

void render_rgn_16_555( bitmap_data_t* bmp,
	region_t* rgn, const flt_rect& dest_rect, u32 draw_color )
{
	#include "drv_render_rgn.h"
}

#undef DRV_MIX_PIXEL_COLORS

#define DRV_MIX_PIXEL_COLORS( pixel, c2, alpha, invAlpha )		\
	{ long c1 = *(u16*)(pixel);	long c3;						\
	MIX_PIXEL_COLORS_565_RGB( c1, c2, invAlpha, alpha, c3 );	\
	*(u16*)(pixel) = c3; }

void render_rgn_16_565( bitmap_data_t* bmp,
	region_t* rgn, const flt_rect& dest_rect, u32 draw_color )
{
	#include "drv_render_rgn.h"
}

#undef DRV_MIX_PIXEL_COLORS

// ---------------------------------------------------------------------------

#undef DRV_PUT_HLINE

inline u08*
DRV_SHADE_HLINE_16_555( u08* dstpixb, long cnt, u32 color )
{
	u16* dstpix = (u16*)dstpixb;
	unsigned int pix;

	if( cnt )
	{
		int r = color & 31;
		int g = ( color >> 5 ) & 31;
		int b = ( color >> 10 ) & 31;
		dstpix--;

		do
		{
			unsigned int pr, pg, pb;
		
			pix = *++dstpix;
			pr = ( ( pix & 31 ) + r ) >> 1;
			pg = ( ( ( pix >> 5 ) & 31 ) + g ) >> 1;
			pb = ( ( ( pix >> 10 ) & 31 ) + b ) >> 1;
			*dstpix = pr | ( pg << 5 ) | ( pb << 10 );

		} while( --cnt );
	}
	
	return (u08*)dstpix;	
}

#define DRV_PUT_HLINE DRV_SHADE_HLINE_16_555

void shade_rect_16_555( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_rect.h"
}

// ---------------------------------------------------------------------------

#undef DRV_PUT_HLINE

inline u08*
DRV_SHADE_HLINE_16_565( u08* dstpixb, long cnt, u32 color )
{
	u16* dstpix = (u16*)dstpixb;
	unsigned int pix;

	if( cnt )
	{
		int r = color & 31;
		int g = ( color >> 5 ) & 63;
		int b = ( color >> 11 ) & 31;
		dstpix--;

		do
		{
			unsigned int pr, pg, pb;
		
			pix = *++dstpix;
			pr = ( ( pix & 31 ) + r ) >> 1;
			pg = ( ( ( pix >> 5 ) & 63 ) + g ) >> 1;
			pb = ( ( ( pix >> 11 ) & 31 ) + b ) >> 1;
			*dstpix = pr | ( pg << 5 ) | ( pb << 11 );

		} while( --cnt );
	}
	
	return (u08*)dstpix;	
}

#define DRV_PUT_HLINE DRV_SHADE_HLINE_16_565

void shade_rect_16_565( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_rect.h"
}

END_ICARUS_NAMESPACE
