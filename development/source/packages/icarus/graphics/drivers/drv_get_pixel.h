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
//	drv_get_pixel.h			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

if( x < 0 || x >= bmp->width || y < 0 || y >= bmp->height )
	return -1;
	
u08* pixel_ptr;

pixel_ptr = ( (u08*)bmp->pixels ) +
	x * DRV_BYTES_PER_PIXEL + y * (long)bmp->pitch;

return DRV_GET_PIXEL( pixel_ptr );