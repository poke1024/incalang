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
//	drv_hline.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

if( x2 < x1 )
{
	long h = x2;
	x2 = x1;
	x1 = h;
}

const int flags = bmp->flags;

if( flags & bmp_draw_clipped )
{
	const rect* clip = bmp->clip;

	if( y < clip->top || y >= clip->bottom )
		return;

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
}

u08* pixel_ptr;

pixel_ptr = ( (u08*)bmp->pixels ) +
	x1 * DRV_BYTES_PER_PIXEL + y * (long)bmp->pitch;

DRV_PUT_HLINE( pixel_ptr, x2 - x1 + 1, color );
