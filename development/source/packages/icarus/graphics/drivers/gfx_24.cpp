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
//	gfx_24_RGB.cp			   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "gfx_driver.h"
#include "os_core.h"
#include "gfx_macro.h"

#include "region_t.h"
#include <math.h>
#include <algorithm>

BEGIN_ICARUS_NAMESPACE

#define DRV_BYTES_PER_PIXEL 3

inline void
DRV_PUT_PIXEL_24( u08* dstpix, u32 color )
{
	unsigned int r, g, b;
	r = EXTRACT_RED_24_RGB( color );
	g = EXTRACT_GREEN_24_RGB( color );
	b = EXTRACT_BLUE_24_RGB( color );
	
	*dstpix++ = r;
	*dstpix++ = g;
	*dstpix++ = b;
}

inline u32
DRV_GET_PIXEL_24( const u08* dstpix )
{
	unsigned int r, g, b;
	r = *dstpix++;
	g = *dstpix++;
	b = *dstpix++;
	
	return MAKE_24_RGB( r, g, b );
}

inline u08*
DRV_PUT_HLINE_24( u08* dstpix, long cnt, u32 color )
{
	if( !cnt )
		return dstpix;
		
	unsigned int r, g, b;
	r = EXTRACT_RED_24_RGB( color );
	g = EXTRACT_GREEN_24_RGB( color );
	b = EXTRACT_BLUE_24_RGB( color );
		
	do {
		*dstpix++ = r;
		*dstpix++ = g;
		*dstpix++ = b;
	} while( --cnt );
	
	return dstpix;
}

#define DRV_PUT_PIXEL DRV_PUT_PIXEL_24
#define DRV_GET_PIXEL DRV_GET_PIXEL_24
#define DRV_PUT_HLINE DRV_PUT_HLINE_24

// ---------------------------------------------------------------------------

void put_pixel_24( bitmap_data_t* bmp, long x, long y, u32 color )
{
	#include "drv_put_pixel.h"
}

u32 get_pixel_24( bitmap_data_t* bmp, long x, long y )
{
	#include "drv_get_pixel.h"
}

void hline_24( bitmap_data_t* bmp, long x1, long y, long x2, u32 color )
{
	#include "drv_hline.h"
}

void vline_24( bitmap_data_t* bmp, long x, long y1, long y2, u32 color )
{
	#include "drv_vline.h"
}

void line_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_line.h"
}

void fill_rect_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_rect.h"
}

void shade_rect_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	fill_rect_24( bmp, x1, y1, x2, y2, color );
}

void frame_oval_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_frame_oval.h"
}

void fill_oval_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color )
{
	#include "drv_fill_oval.h"
}

// ---------------------------------------------------------------------------

inline void DRV_MIX_PIXEL_COLORS( u08* pixel, u32 c2, int a1, int a0 )
{
	unsigned int r1, g1, b1;
	r1 = pixel[ 0 ];
	g1 = pixel[ 1 ];
	b1 = pixel[ 2 ];

	unsigned int r2, g2, b2;
	r2 = EXTRACT_RED_24_RGB( c2 );
	g2 = EXTRACT_GREEN_24_RGB( c2 );
	b2 = EXTRACT_BLUE_24_RGB( c2 );
	
	pixel[ 0 ] = ( ( r1 * a1 ) + ( r2 * a0 ) ) >> 8;
	pixel[ 1 ] = ( ( g1 * a1 ) + ( g2 * a0 ) ) >> 8;
	pixel[ 2 ] = ( ( b1 * a1 ) + ( b2 * a0 ) ) >> 8;
}

void render_rgn_24( bitmap_data_t* bmp,
	region_t* rgn, const flt_rect& dest_rect, u32 draw_color )
{
	#include "drv_render_rgn.h"
}

// ---------------------------------------------------------------------------

#include "drv_transform_helpers.h"

#define DRV_PUT_COMPONENT( pixel_ptr, comp )					\
	*( pixel_ptr ) = comp;
	
#define DRV_GET_COMPONENT( pixel_ptr )							\
	*( pixel_ptr )

#define COORD_TRUNC( x )										\
	( x >> 16 )

#define COORD_FRACT( x )										\
	( u32( x << 16 ) / u32( 0xffff ) )

static void
transform_row_24(
	s32						sx,
	s32						sy,
	bitmap_data_t*			src_bmp,
	const s32*				matrix,
	u08*					dst_pixel_ptr,
	const u08*				src_pixels,
	long					column_count,
	unsigned int			back_comp )
{
	#include "drv_transform.h"
}

#undef COORD_TYPE
#undef COORD_TRUNC
#undef COORD_FRACT
#undef DRV_GET_COMPONENT
#undef DRV_PUT_COMPONENT

void transform_24( bitmap_data_t* src_bmp,
	bitmap_data_t* dst_bmp, const rect& dst_rect,
	const double* matrix, u32 back_color )
{
	s32						fixmatrix[ 9 ];
	
	for( int i = 0; i < 9; i++ )
		fixmatrix[ i ] = matrix[ i ] * 65536.0;

	long					dst_left = dst_rect.left;
	long					dst_right = dst_rect.right;
	long					dst_top = dst_rect.top;
	long					dst_bottom = dst_rect.bottom;

	const int				flags = dst_bmp->flags;

	if( flags & bmp_draw_clipped )
	{
		const rect*			clip = dst_bmp->clip;
	
		const long clip_left = clip->left;
			
		if( dst_left < clip_left )
		{
			if( dst_right < clip_left )
				return;
			dst_left = clip_left;
		}
		
		const long clip_right = clip->right;

		if( dst_right >= clip_right )
		{
			if( dst_left >= clip_right )
				return;
			dst_right = clip_right - 1;
		}

		const long clip_top = clip->top;
			
		if( dst_top < clip_top )
		{
			if( dst_bottom < clip_top )
				return;
			dst_top = clip_top;
		}
		
		const long clip_bottom = clip->bottom;

		if( dst_bottom >= clip_bottom )
		{
			if( dst_top >= clip_bottom )
				return;
			dst_bottom = clip_bottom - 1;
		}
	}

	if( dst_right < dst_left )
		return;

	if( dst_bottom < dst_top )
		return;

	s32						sx, sy;
	
	{
		const s32			m00 = fixmatrix[ 3 * 0 + 0 ];
		const s32			m01 = fixmatrix[ 3 * 0 + 1 ];
		const s32			m10 = fixmatrix[ 3 * 1 + 0 ];
		const s32			m11 = fixmatrix[ 3 * 1 + 1 ];

		sx = m00 * dst_left + ( m00 >> 1 ) +
			m01 * dst_top + ( m01 >> 1 ) +
			fixmatrix[ 3 * 0 + 2 ];
			
		sy = m10 * dst_left + ( m10 >> 1 ) +
			m11 * dst_top + ( m11 >> 1 ) +
			fixmatrix[ 3 * 1 + 2 ];
	}

	const long				dst_pitch = dst_bmp->pitch;
	const long				dst_width = dst_right - dst_left + 1;

	u08*					dst_pixel_ptr;

	dst_pixel_ptr  = ( (u08*)dst_bmp->pixels ) +
		dst_left * DRV_BYTES_PER_PIXEL +
		dst_top * dst_pitch;

	long					row_count = dst_bottom - dst_top + 1;
		
	const u08*				src_pixels = (u08*)src_bmp->pixels;

	do {	
		transform_row_24( sx, sy, src_bmp, fixmatrix,
			dst_pixel_ptr, src_pixels,
			dst_width, EXTRACT_RED_24_RGB( back_color ) );

		transform_row_24( sx, sy, src_bmp, fixmatrix,
			dst_pixel_ptr + 1, src_pixels + 1,
			dst_width, EXTRACT_GREEN_24_RGB( back_color ) );

		transform_row_24( sx, sy, src_bmp, fixmatrix,
			dst_pixel_ptr + 2, src_pixels + 2,
			dst_width, EXTRACT_BLUE_24_RGB( back_color ) );
	
		dst_pixel_ptr += dst_pitch;

		sx += fixmatrix[ 3 * 0 + 1 ];
		sy += fixmatrix[ 3 * 1 + 1 ];

	} while( --row_count );
}

END_ICARUS_NAMESPACE
