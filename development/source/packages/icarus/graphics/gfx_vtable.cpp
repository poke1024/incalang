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
//	gfx_vtable.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "gfx_vtable.h"
#include "gfx_protos.h"

BEGIN_ICARUS_NAMESPACE

gfxdrv_vtable_t gfxdrv_vtable[ PIXEL_FORMAT_COUNT ];

gfxdrv_blit_xvtable_t gfxdrv_blit_vtable[ PIXEL_FORMAT_COUNT ];

static void
init_gfx_vtables()
{
	gfxdrv_vtable_t*			table = gfxdrv_vtable;
	
	std::memset( table, 0,
		PIXEL_FORMAT_COUNT * sizeof( gfxdrv_vtable_t ) );
	
	for( int i = pixel_format_8_indexed_gray;
		i <= pixel_format_8_indexed; i++ )
	{
		table[ i ].put_pixel = put_pixel_8;
		table[ i ].get_pixel = get_pixel_8;
		table[ i ].hline = hline_8;
		table[ i ].vline = vline_8;
		table[ i ].line = line_8;
		table[ i ].shade_rect = shade_rect_8;
		table[ i ].fill_rect = fill_rect_8;
		table[ i ].frame_oval = frame_oval_8;
		table[ i ].fill_oval = fill_oval_8;
	}
	
	for( int i = pixel_format_16_555_RGB;
		i <= pixel_format_16_556_RBG; i++ )
	{
		table[ i ].put_pixel = put_pixel_16;
		table[ i ].get_pixel = get_pixel_16;
		table[ i ].hline = hline_16;
		table[ i ].vline = vline_16;
		table[ i ].line = line_16;
		table[ i ].fill_rect = fill_rect_16;
		table[ i ].frame_oval = frame_oval_16;
		table[ i ].fill_oval = fill_oval_16;
	}

	for( int i = pixel_format_16_555_RGB;
		i <= pixel_format_16_555_RBG; i++ )
	{
		table[ i ].shade_rect = shade_rect_16_555;
		table[ i ].render_rgn = render_rgn_16_555;
	}

	for( int i = pixel_format_16_565_RGB;
		i <= pixel_format_16_565_RBG; i++ )
	{
		table[ i ].shade_rect = shade_rect_16_565;
		table[ i ].render_rgn = render_rgn_16_565;
	}
	
	for( int i = pixel_format_24_RGB;
		i <= pixel_format_24_RBG; i++ )
	{
		table[ i ].put_pixel = put_pixel_24;
		table[ i ].get_pixel = get_pixel_24;
		table[ i ].hline = hline_24;
		table[ i ].vline = vline_24;
		table[ i ].line = line_24;
		table[ i ].shade_rect = shade_rect_24;
		table[ i ].fill_rect = fill_rect_24;
		table[ i ].frame_oval = frame_oval_24;
		table[ i ].fill_oval = fill_oval_24;
		table[ i ].render_rgn = render_rgn_24;
	}
	
	for( int i = pixel_format_32_RGBA;
		i <= pixel_format_32_RGBA; i++ )
	{
		table[ i ].put_pixel = put_pixel_32;
		table[ i ].get_pixel = get_pixel_32;
		table[ i ].hline = hline_32;
		table[ i ].vline = vline_32;
		table[ i ].line = line_32;
		table[ i ].shade_rect = shade_rect_32;
		table[ i ].fill_rect = fill_rect_32;
		table[ i ].frame_oval = frame_oval_32;
		table[ i ].fill_oval = fill_oval_32;
		table[ i ].render_rgn = render_rgn_32;
	}
}


static void
init_gfx_blit_vtables()
{
	gfxdrv_blit_xvtable_t*		table = gfxdrv_blit_vtable;
	
	std::memset( table, 0,
		PIXEL_FORMAT_COUNT * sizeof( gfxdrv_blit_xvtable_t ) );
		
	// standard blit methods
		
	table[ pixel_format_8_indexed_gray ]
		[ pixel_format_8_indexed_gray ].std_blit = std_blit_8;

	table[ pixel_format_8_indexed ]
		[ pixel_format_8_indexed ].std_blit = std_blit_8;
		
	for( int i = pixel_format_16_555_RGB;
		i <= pixel_format_16_556_RBG; i++ )
	{
		for( int j = pixel_format_16_555_RGB;
			j <= pixel_format_32_RGBA; j++ )
		{
			table[ i ][ j ].std_blit = std_cross_blit;
		}
	
		table[ i ][ i ].std_blit = std_blit_16;
	}
	
	for( int i = pixel_format_24_RGB;
		i <= pixel_format_24_RBG; i++ )
	{
		for( int j = pixel_format_16_555_RGB;
			j <= pixel_format_32_RGBA; j++ )
		{
			table[ i ][ j ].std_blit = std_cross_blit;
		}

		table[ i ][ i ].std_blit = std_blit_24;
	}
	
	for( int i = pixel_format_32_RGBA;
		i <= pixel_format_32_RGBA; i++ )
	{
		for( int j = pixel_format_16_555_RGB;
			j <= pixel_format_32_RGBA; j++ )
		{
			table[ i ][ j ].std_blit = std_cross_blit;
		}
	
		table[ i ][ i ].std_blit = std_blit_32;
	}

	// transform methods
	
	for( int i = pixel_format_24_RGB;
		i <= pixel_format_24_RBG; i++ )
	{
		table[ i ][ i ].transform = transform_24;
	}
}

void
init_gfx()
{
	init_gfx_vtables();
	init_gfx_blit_vtables();
}

END_ICARUS_NAMESPACE
