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
//	drv_fillrect.h			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

if( x2 < x1 )
{
	long h = x2;
	x2 = x1;
	x1 = h;
}

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

	const long clip_left = clip->left;
		
	if( x1 < clip_left )
	{
		if( x2 < clip_left )
			return;
		x1 = clip_left;
	}
	
	const long clip_right = clip->right;

	if( x2 >= clip_right )
	{
		if( x1 >= clip_right )
			return;
		x2 = clip_right - 1;
	}

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
const long xcount = x2 - x1 + 1;
long ycount;

if( ( ycount = y2 - y1 + 1 ) == 0 )
	return;

pixel_ptr = ( (u08*)bmp->pixels ) +
	x1 * DRV_BYTES_PER_PIXEL + y1 * (long)pitch;

do {
	DRV_PUT_HLINE( pixel_ptr, xcount, color );
	pixel_ptr += pitch;
} while( --ycount );

