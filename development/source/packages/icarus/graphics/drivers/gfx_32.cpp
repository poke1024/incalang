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
//	gfx_32.cp				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "gfx_driver.h"
#include "os_core.h"

#include "region_t.h"
#include <math.h>
#include <algorithm>

BEGIN_ICARUS_NAMESPACE

// ---------------------------------------------------------------------------

#define DRV_BYTES_PER_PIXEL 4

inline void
DRV_PUT_PIXEL_32( u08* dstpix, u32 color )
{
	*(u32*)dstpix = color;
}

inline u32
DRV_GET_PIXEL_32( const u08* dstpix )
{
	return *(u32*)dstpix;
}

inline u08*
DRV_PUT_HLINE_32( u08* dstpixb, long cnt, u32 color )
{
	u32* dstpix = (u32*)dstpixb;

	if( cnt )
	{
		do {
			*dstpix++ = color;
		} while( --cnt );
	}
	
	return (u08*)dstpix;	
}

#define DRV_PUT_PIXEL DRV_PUT_PIXEL_32
#define DRV_GET_PIXEL DRV_GET_PIXEL_32
#define DRV_PUT_HLINE DRV_PUT_HLINE_32

// ---------------------------------------------------------------------------

void put_pixel_32( bitmap_data_t* bmp, long x, long y, u32 color )
{
	#include "drv_put_pixel.h"
}

u32 get_pixel_32( bitmap_data_t* bmp, long x, long y )
{
	#include "drv_get_pixel.h"
}

void hline_32( bitmap_data_t* bmp,
	long x1, long y, long x2, u32 color )
{
	#include "drv_hline.h"
}

void vline_32( bitmap_data_t* bmp,
	long x, long y1, long y2, u32 color )
{
	#include "drv_vline.h"
}

void line_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_line.h"
}

void fill_rect_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_rect.h"
}

void shade_rect_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	fill_rect_32( bmp, x1, y1, x2, y2, color );
}

void frame_oval_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_frame_oval.h"
}

void fill_oval_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_oval.h"
}

// ---------------------------------------------------------------------------

inline void DRV_MIX_PIXEL_COLORS_32( u08* pixel, u32 c2, int a1, int a0 )
{
	u32 c1 = *( (u32*)pixel );

	unsigned int r1, g1, b1, q1;
	r1 = c1 & 0xff;
	g1 = ( c1 >> 8 ) & 0xff;
	b1 = ( c1 >> 16 ) & 0xff;
	q1 = ( c1 >> 24 ) & 0xff;

	unsigned int r2, g2, b2, q2;
	r2 = c2 & 0xff;
	g2 = ( c2 >> 8 ) & 0xff;
	b2 = ( c2 >> 16 ) & 0xff;
	q2 = ( c2 >> 24 ) & 0xff;
	
	u32 c3;
	c3 = ( ( ( r1 * a1 ) + ( r2 * a0 ) ) >> 8 ) & 0xff;
	c3 |= ( ( g1 * a1 ) + ( g2 * a0 ) ) & 0xff00;
	c3 |= ( ( ( b1 * a1 ) + ( b2 * a0 ) ) << 8 ) & 0xff0000;
	c3 |= ( ( ( q1 * a1 ) + ( q2 * a0 ) ) << 16 ) & 0xff000000;
	
	*( (u32*)pixel ) = c3;
}

#define DRV_MIX_PIXEL_COLORS DRV_MIX_PIXEL_COLORS_32

void render_rgn_32( bitmap_data_t* bmp,
	region_t* rgn, const flt_rect& dest_rect, u32 draw_color )
{
	#include "drv_render_rgn.h"
}

END_ICARUS_NAMESPACE
