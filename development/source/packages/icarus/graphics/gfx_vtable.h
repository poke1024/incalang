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
//	gfx_vtable.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "gfx_driver.h"

BEGIN_ICARUS_NAMESPACE

extern gfxdrv_vtable_t gfxdrv_vtable[ PIXEL_FORMAT_COUNT ];
	
#define STD_BLIT_PROTO(name)											\
	void name( bitmap_data_t* src_bmp, bitmap_data_t* dst_bmp,			\
	long src_x, long src_y, long dst_x, long dst_y,						\
	long width, long height )

extern gfxdrv_blit_xvtable_t gfxdrv_blit_vtable[ PIXEL_FORMAT_COUNT ];

inline gfxdrv_blit_vtable_t*
get_blit_vtable(
	pixel_format_t		srcformat,
	pixel_format_t		dstformat )
{
	return &gfxdrv_blit_vtable[ srcformat ][ dstformat ];
}

void
init_gfx();
	
END_ICARUS_NAMESPACE
