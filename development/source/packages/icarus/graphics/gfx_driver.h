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
//	gfx_driver.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "icarus.h"
#include "pixel_format_t.h"
#include "typeface_t.h"
#include <string>

BEGIN_ICARUS_NAMESPACE

enum {
	bmp_create_native	= ( 1L << 0 )
};

enum {
	bmp_draw_clipped	= ( 1L << 0 )
};

class font_t;

struct bitmap_data_t {
	pixel_format_t		format;
	void*				pixels;
	void*				native;
	u32					width;
	u32					height;
	u32					pitch;
	u08					flags;
	point				origin;
	const rect*			clip;
	u08*				palette;
};

bitmap_data_t* create_bitmap(
	pixel_format_t format, long width, long height, int flags );

void delete_bitmap( bitmap_data_t* bmp );

void set_palette_entry( bitmap_data_t* bmp, int index, const u08* rgb );

class region_t;
	
typedef void (*gfxdrv_fn_put_pixel)(
	bitmap_data_t* bmp, long x, long y, u32 color );

typedef u32 (*gfxdrv_fn_get_pixel)(
	bitmap_data_t* bmp, long x, long y );

typedef void (*gfxdrv_fn_hline)(
	bitmap_data_t* bmp, long x1, long y, long x2, u32 color );

typedef void (*gfxdrv_fn_vline)(
	bitmap_data_t* bmp, long x, long y1, long y2, u32 color );

typedef void (*gfxdrv_fn_line)(
	bitmap_data_t* bmp, long x1, long y1, long x2, long y2, u32 color );

typedef void (*gfxdrv_fn_shade_rect)(
	bitmap_data_t* bmp, long x1, long y1, long x2, long y2, u32 color );

typedef void (*gfxdrv_fn_fill_rect)(
	bitmap_data_t* bmp, long x1, long y1, long x2, long y2, u32 color );

typedef void (*gfxdrv_fn_frame_oval)(
	bitmap_data_t* bmp, long x1, long y1, long x2, long y2, u32 color );

typedef void (*gfxdrv_fn_fill_oval)(
	bitmap_data_t* bmp, long x1, long y1, long x2, long y2, u32 color );

typedef void (*gfxdrv_fn_render_rgn)(
	bitmap_data_t* bmp, region_t* rgn, const flt_rect& r, u32 color );

struct gfxdrv_vtable_t {
	gfxdrv_fn_put_pixel							put_pixel;
	gfxdrv_fn_get_pixel							get_pixel;
	gfxdrv_fn_hline								hline;
	gfxdrv_fn_vline								vline;
	gfxdrv_fn_line								line;
	gfxdrv_fn_shade_rect						shade_rect;
	gfxdrv_fn_fill_rect							fill_rect;
	gfxdrv_fn_frame_oval						frame_oval;
	gfxdrv_fn_fill_oval							fill_oval;
	gfxdrv_fn_render_rgn						render_rgn;
};

typedef void (*gfxdrv_fn_std_blit)(
	bitmap_data_t* src_bmp, bitmap_data_t* dst_bmp,
	long src_x, long src_y, long dst_x, long dst_y,
	long width, long height );

typedef void (*gfxdrv_fn_transform)(
	bitmap_data_t* src_bmp, bitmap_data_t* dst_bmp,
	const rect& dst_rect, const double* matrix,
	u32 back_color );

struct gfxdrv_blit_vtable_t {
	gfxdrv_fn_std_blit							std_blit;
	gfxdrv_fn_transform							transform;
};

typedef gfxdrv_blit_vtable_t gfxdrv_blit_xvtable_t[ PIXEL_FORMAT_COUNT ];

END_ICARUS_NAMESPACE
