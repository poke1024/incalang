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
//	drv_vline.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

if( y2 < y1 )
{
	long h = y2;
	y2 = y1;
	y1 = h;
}

const int flags = bmp->flags;

if( flags & bmp_draw_clipped )
{
	const rect* clip = bmp->clip;

	if( x < clip->left || x >= bmp->clip->right )
		return;

	const long clip_top = clip->top;
		
	if( y1 < clip_top )
	{
		if( y2 < clip_top )
			return;
		y1 = clip_top;
	}
	
	const long clip_bottom = clip->bottom;

	if( y2 >= clip_bottom )
	{
		if( y1 >= clip_bottom )
			return;
		y2 = clip_bottom - 1;
	}
}

u08* pixel_ptr;
const long pitch = bmp->pitch;
long ycount;

if( !( ycount = y2 - y1 + 1 ) )
	return;

pixel_ptr = ( (u08*)bmp->pixels ) + x * DRV_BYTES_PER_PIXEL
	+ y1 * (long)pitch;

do {
	DRV_PUT_PIXEL( pixel_ptr, color );
	pixel_ptr += pitch;
} while( --ycount );