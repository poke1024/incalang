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
//	gfx_driver.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "gfx_driver.h"
#include "os_core.h"
#include "font_t.h"

BEGIN_ICARUS_NAMESPACE

bitmap_data_t* create_bitmap(
	pixel_format_t format, long width, long height, int flags )
{
	bitmap_data_t* bmp = new bitmap_data_t;

	try {
		if( flags & bmp_create_native )
		{
			bmp->native = os_create_bitmap(
				format, width, height, flags );
			bmp->pitch = os_get_bitmap_pitch( bmp->native );
			bmp->pixels = 0;
		}
		else
		{
			bmp->pitch = width *
				( get_pixel_format_profile( format )->depth >> 3 );
			bmp->native = 0;
			bmp->pixels = new u08[ (long)bmp->pitch * height ];
		}
	} catch(... ) {
		delete bmp;
		throw;
	}
	
	bmp->width = width;
	bmp->height = height;
	bmp->origin.set( 0, 0 );
	
	bmp->flags = bmp_draw_clipped;
	bmp->clip = 0;
	
	if( format == pixel_format_8_indexed )
		bmp->palette = new u08[ 256 * 3 ];
	else
		bmp->palette = 0;
	
	bmp->format = format;
	
	return bmp;
}

void delete_bitmap( bitmap_data_t* bmp )
{
	if( bmp->native )
		os_destroy_bitmap( bmp->native );
	else
		delete[] bmp->pixels;
		
	if( bmp->palette )
		delete[] bmp->palette;
	
	delete bmp;
}

END_ICARUS_NAMESPACE
