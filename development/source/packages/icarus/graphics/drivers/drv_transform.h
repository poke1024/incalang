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
//	drv_transform.h			   ©1999-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

// the code in this file was inspired by the excellently written functions
// in the GIMP image transformation module

#define GET_PIXEL( r, x, y )									\
	if( ( (u32)( x ) ) < src_width &&							\
		( (u32)( y ) ) < src_height )							\
	{															\
		src_pixel_ptr = src_pixels +							\
			( x ) * DRV_BYTES_PER_PIXEL +						\
			( y ) * src_pitch;									\
		r = DRV_GET_COMPONENT( src_pixel_ptr );					\
	}															\
	else														\
		r = back_comp;

const u32			src_width = src_bmp->width;
const u32			src_height = src_bmp->height;
const long			src_pitch = src_bmp->pitch;

while( true )
{		
	const s32		ix = COORD_TRUNC( sx );
	const s32		iy = COORD_TRUNC( sy );

	if( ix < -2 || ix > src_width || iy < -2 || iy > src_height )
	{
		DRV_PUT_COMPONENT( dst_pixel_ptr, back_comp );
		dst_pixel_ptr += DRV_BYTES_PER_PIXEL;
		goto next_col;
	}

	const u08*		src_pixel_ptr;

	u32				vx0, vx1, vx2, vx3;
	s32				vy0, vy1, vy2, vy3;

	if( iy > 0 && iy <= src_height - 3  &&
		ix > 0 && ix <= src_width - 3 )
	{
		const s32	fract = COORD_FRACT( sx );

		src_pixel_ptr = src_pixels +
			( ix - 1 ) * DRV_BYTES_PER_PIXEL +
			( iy - 1 ) * src_pitch;
		
		vx0 = DRV_GET_COMPONENT( src_pixel_ptr + 0 * DRV_BYTES_PER_PIXEL );
		vx1 = DRV_GET_COMPONENT( src_pixel_ptr + 1 * DRV_BYTES_PER_PIXEL );
		vx2 = DRV_GET_COMPONENT( src_pixel_ptr + 2 * DRV_BYTES_PER_PIXEL );
		vx3 = DRV_GET_COMPONENT( src_pixel_ptr + 3 * DRV_BYTES_PER_PIXEL );
		vy0 = CUBIC_X( vx0, vx1, vx2, vx3, fract );
			
		src_pixel_ptr += src_pitch;
		vx0 = DRV_GET_COMPONENT( src_pixel_ptr + 0 * DRV_BYTES_PER_PIXEL );
		vx1 = DRV_GET_COMPONENT( src_pixel_ptr + 1 * DRV_BYTES_PER_PIXEL );
		vx2 = DRV_GET_COMPONENT( src_pixel_ptr + 2 * DRV_BYTES_PER_PIXEL );
		vx3 = DRV_GET_COMPONENT( src_pixel_ptr + 3 * DRV_BYTES_PER_PIXEL );
		vy1 = CUBIC_X( vx0, vx1, vx2, vx3, fract );

		src_pixel_ptr += src_pitch;
		vx0 = DRV_GET_COMPONENT( src_pixel_ptr + 0 * DRV_BYTES_PER_PIXEL );
		vx1 = DRV_GET_COMPONENT( src_pixel_ptr + 1 * DRV_BYTES_PER_PIXEL );
		vx2 = DRV_GET_COMPONENT( src_pixel_ptr + 2 * DRV_BYTES_PER_PIXEL );
		vx3 = DRV_GET_COMPONENT( src_pixel_ptr + 3 * DRV_BYTES_PER_PIXEL );
		vy2 = CUBIC_X( vx0, vx1, vx2, vx3, fract );

		src_pixel_ptr += src_pitch;
		vx0 = DRV_GET_COMPONENT( src_pixel_ptr + 0 * DRV_BYTES_PER_PIXEL );
		vx1 = DRV_GET_COMPONENT( src_pixel_ptr + 1 * DRV_BYTES_PER_PIXEL );
		vx2 = DRV_GET_COMPONENT( src_pixel_ptr + 2 * DRV_BYTES_PER_PIXEL );
		vx3 = DRV_GET_COMPONENT( src_pixel_ptr + 3 * DRV_BYTES_PER_PIXEL );
		vy3 = CUBIC_X( vx0, vx1, vx2, vx3, fract );
	}
	else
	{
		const s32	fract = COORD_FRACT( sx );
	
		GET_PIXEL( vx0, ix - 1,	iy - 1 );
		GET_PIXEL( vx1, ix + 0,	iy - 1 );
		GET_PIXEL( vx2, ix + 1,	iy - 1 );
		GET_PIXEL( vx3, ix + 2,	iy - 1 );
		vy0 = CUBIC_X( vx0, vx1, vx2, vx3, fract );
		
		GET_PIXEL( vx0, ix - 1,	iy + 0 );
		GET_PIXEL( vx1, ix + 0,	iy + 0 );
		GET_PIXEL( vx2, ix + 1,	iy + 0 );
		GET_PIXEL( vx3, ix + 2,	iy + 0 );
		vy1 = CUBIC_X( vx0, vx1, vx2, vx3, fract );
		
		GET_PIXEL( vx0, ix - 1,	iy + 1 );
		GET_PIXEL( vx1, ix + 0,	iy + 1 );
		GET_PIXEL( vx2, ix + 1,	iy + 1 );
		GET_PIXEL( vx3, ix + 2,	iy + 1 );
		vy2 = CUBIC_X( vx0, vx1, vx2, vx3, fract );
		
		GET_PIXEL( vx0, ix - 1,	iy + 2 );
		GET_PIXEL( vx1, ix + 0,	iy + 2 );
		GET_PIXEL( vx2, ix + 1,	iy + 2 );
		GET_PIXEL( vx3, ix + 2,	iy + 2 );
		vy3 = CUBIC_X( vx0, vx1, vx2, vx3, fract );
	}

	{
		const u32	fract = COORD_FRACT( sy );
		u32			gray;
	
		gray = CUBIC_Y( vy0, vy1, vy2, vy3, fract );
		DRV_PUT_COMPONENT( dst_pixel_ptr, gray );
		dst_pixel_ptr += DRV_BYTES_PER_PIXEL;
	}

next_col:
		if( --column_count == 0 )
			break;
			
	sx += matrix[ 3 * 0 + 0 ];
	sy += matrix[ 3 * 1 + 0 ];
}

#undef GET_PIXEL

