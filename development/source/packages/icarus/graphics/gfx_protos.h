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
//	gfx_protos.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

BEGIN_ICARUS_NAMESPACE

void put_pixel_8( bitmap_data_t* bmp, long x, long y, u32 color );
u32 get_pixel_8( bitmap_data_t* bmp, long x, long y );
void hline_8( bitmap_data_t* bmp, long x1, long y, long x2, u32 color );
void vline_8( bitmap_data_t* bmp, long x, long y1, long y2, u32 color );
void line_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void shade_rect_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void fill_rect_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void frame_oval_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void fill_oval_8( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );

void put_pixel_16( bitmap_data_t* bmp, long x, long y, u32 color );
u32 get_pixel_16( bitmap_data_t* bmp, long x, long y );
void hline_16( bitmap_data_t* bmp, long x1, long y, long x2, u32 color );
void vline_16( bitmap_data_t* bmp, long x, long y1, long y2, u32 color );
void line_16( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void fill_rect_16( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void frame_oval_16( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void fill_oval_16( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );

void render_rgn_16_555( bitmap_data_t* bmp,
	region_t* rgn, const flt_rect& dest_rect, u32 draw_color );
void render_rgn_16_565( bitmap_data_t* bmp,
	region_t* rgn, const flt_rect& dest_rect, u32 draw_color );
void shade_rect_16_555( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void shade_rect_16_565( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );

void put_pixel_24( bitmap_data_t* bmp, long x, long y, u32 color );
u32 get_pixel_24( bitmap_data_t* bmp, long x, long y );
void hline_24( bitmap_data_t* bmp, long x1, long y, long x2, u32 color );
void vline_24( bitmap_data_t* bmp, long x, long y1, long y2, u32 color );
void line_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void shade_rect_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void fill_rect_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void render_rgn_24( bitmap_data_t* bmp,
	region_t* rgn, const flt_rect& dest_rect, u32 draw_color );
void frame_oval_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void fill_oval_24( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );

void put_pixel_32( bitmap_data_t* bmp, long x, long y, u32 color );
u32 get_pixel_32( bitmap_data_t* bmp, long x, long y );
void hline_32( bitmap_data_t* bmp, long x1, long y, long x2, u32 color );
void vline_32( bitmap_data_t* bmp, long x, long y1, long y2, u32 color );
void line_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void shade_rect_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void fill_rect_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void render_rgn_32( bitmap_data_t* bmp,
	region_t* rgn, const flt_rect& dest_rect, u32 draw_color );
void frame_oval_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );
void fill_oval_32( bitmap_data_t* bmp,
	long x1, long y1, long x2, long y2, u32 color );

// ---------------------------------------------------------------------------

STD_BLIT_PROTO( std_blit_8 );
STD_BLIT_PROTO( std_blit_16 );
STD_BLIT_PROTO( std_blit_24 );
STD_BLIT_PROTO( std_blit_32 );

STD_BLIT_PROTO( std_cross_blit );

// ---------------------------------------------------------------------------

void transform_24( bitmap_data_t* src_bmp,
	bitmap_data_t* dst_bmp, const rect& dst_rect,
	const double* matrix, u32 back_color );

END_ICARUS_NAMESPACE
