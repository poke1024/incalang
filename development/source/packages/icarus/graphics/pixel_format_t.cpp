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

#include "pixel_format_t.h"

BEGIN_ICARUS_NAMESPACE

pixel_profile_t s_profiles[ PIXEL_FORMAT_COUNT ] = {
	{ 8,  0, 0, 0, 0, 0,  0,  0,  0  },		// pixel_format_8_indexed_gray
	{ 8,  0, 0, 0, 0, 0,  0,  0,  0  },		// pixel_format_8_indexed

	{ 16, 5, 5, 5, 0, 10, 5,  0,  0  },		// pixel_format_16_555_RGB
	{ 16, 5, 5, 5, 0, 0,  10, 5,  0  },		// pixel_format_16_555_GBR
	{ 16, 5, 5, 5, 0, 5,  0,  10, 0  },		// pixel_format_16_555_BRG
	{ 16, 5, 5, 5, 0, 0,  5,  10, 0  },		// pixel_format_16_555_BGR
	{ 16, 5, 5, 5, 0, 5,  10, 0,  0  },		// pixel_format_16_555_GRB
	{ 16, 5, 5, 5, 0, 10, 0,  5,  0  },		// pixel_format_16_555_RBG
	
	{ 16, 5, 6, 5, 0, 11, 5,  0,  0  },		// pixel_format_16_565_RGB
	{ 16, 5, 5, 6, 0, 0,  11, 5,  0  },		// pixel_format_16_565_GBR
	{ 16, 6, 5, 5, 0, 5,  0,  11, 0  },		// pixel_format_16_565_BRG
	{ 16, 5, 6, 5, 0, 0,  5,  11, 0  },		// pixel_format_16_565_BGR
	{ 16, 6, 5, 5, 0, 5,  11, 0,  0  }, 	// pixel_format_16_565_GRB
	{ 16, 5, 5, 6, 0, 11, 0,  5,  0  },		// pixel_format_16_565_RBG

	{ 16, 6, 5, 5, 0, 10, 5,  0,  0  },		// pixel_format_16_655_RGB
	{ 16, 5, 6, 5, 0, 0,  10, 5,  0  },		// pixel_format_16_655_GBR
	{ 16, 5, 5, 6, 0, 5,  0,  10, 0  },		// pixel_format_16_655_BRG
	{ 16, 5, 5, 6, 0, 0,  5,  10, 0  },		// pixel_format_16_655_BGR
	{ 16, 5, 6, 5, 0, 5,  10, 0,  0  }, 	// pixel_format_16_655_GRB
	{ 16, 6, 5, 5, 0, 10, 0,  5,  0  },		// pixel_format_16_655_RBG

	{ 16, 5, 5, 6, 0, 11, 6,  0,  0  },		// pixel_format_16_556_RGB
	{ 16, 6, 5, 5, 0, 0,  11, 6,  0  },		// pixel_format_16_556_GBR
	{ 16, 5, 6, 5, 0, 6,  0,  11, 0  },		// pixel_format_16_556_BRG
	{ 16, 6, 5, 5, 0, 0,  6,  11, 0  },		// pixel_format_16_556_BGR
	{ 16, 5, 5, 6, 0, 6,  11, 0,  0  }, 	// pixel_format_16_556_GRB
	{ 16, 5, 6, 5, 0, 11, 0,  6,  0  },		// pixel_format_16_556_RBG

	{ 24, 8, 8, 8, 0, 16, 8,  0,  0  },		// pixel_format_24_RGB
	{ 24, 8, 8, 8, 0, 0,  16, 8,  0  },		// pixel_format_24_GBR
	{ 24, 8, 8, 8, 0, 8,  0,  16, 0  },		// pixel_format_24_BRG
	{ 24, 8, 8, 8, 0, 0,  8,  16, 0  },		// pixel_format_24_BGR
	{ 24, 8, 8, 8, 0, 8,  16, 0,  0  }, 	// pixel_format_24_GRB
	{ 24, 8, 8, 8, 0, 16, 0,  8,  0  },		// pixel_format_24_RBG

#if ICARUS_BIG_ENDIAN
	{ 32, 8, 8, 8, 8, 24, 16, 8,  0  }		// pixel_format_32_RGBA
#else
	{ 32, 8, 8, 8, 8, 0, 8, 16,  24  }		// pixel_format_32_RGBA
#endif
};

u32
make_color(
	pixel_format_t			format,
	int						r,
	int						g,
	int						b )
{
	const pixel_profile_t*	profile;

	profile = &s_profiles[ format ];
	
	unsigned long			red, green, blue;
	
	red = ( ( (long)r ) >> ( 8 - profile->red_bits ) ) << profile->red_shift;
	green = ( ( (long)g ) >> ( 8 - profile->green_bits ) ) << profile->green_shift;
	blue = ( ( (long)b ) >> ( 8 - profile->blue_bits ) ) << profile->blue_shift;

	return red | green | blue;
}

u32
make_color(
	pixel_format_t			format,
	int						r,
	int						g,
	int						b,
	int						a )
{
	const pixel_profile_t*	profile;

	profile = &s_profiles[ format ];
	
	unsigned long			red, green, blue, alpha;
	
	red = ( ( (long)r ) >> ( 8 - profile->red_bits ) ) << profile->red_shift;
	green = ( ( (long)g ) >> ( 8 - profile->green_bits ) ) << profile->green_shift;
	blue = ( ( (long)b ) >> ( 8 - profile->blue_bits ) ) << profile->blue_shift;
	alpha = ( ( (long)a ) >> ( 8 - profile->alpha_bits ) ) << profile->alpha_shift;

	return red | green | blue | alpha;
}

inline u32
extract_component(
	u32						color,
	int						shift,
	int						bits )
{
	return ( ( color >> shift ) << ( 8 - bits ) ) & 0xff;
}

u32
convert_color(
	pixel_format_t			from_format,
	pixel_format_t			into_format,
	u32						color )
{
	const pixel_profile_t*	profile;

	profile = &s_profiles[ from_format ];
	
	u32						r, g, b;

	r = extract_component( color,
		profile->red_shift, profile->red_bits );
	g = extract_component( color,
		profile->green_shift, profile->green_bits );
	b = extract_component( color,
		profile->blue_shift, profile->blue_bits );
	
	return make_color( into_format, r, g, b );
}

inline u32
blend_component(
	u32						from_color,
	u32						into_color,
	int						from_alpha,
	int						into_alpha,
	int						bits,
	int						shift )
{
	const int				inv_bits = 8 - bits;

	u32						from_comp, into_comp;

	from_comp = ( ( from_color >> shift ) << inv_bits ) & 0xff;
	into_comp = ( ( into_color >> shift ) << inv_bits ) & 0xff;
	
	u32						comp;

	comp = ( from_comp * from_alpha + into_comp * into_alpha ) >> 8;
	
	return ( comp >> inv_bits ) << shift;
}

u32
blend_color(
	pixel_format_t			format,
	u32						from_color,
	u32						into_color,
	u32						alpha )
{
	const pixel_profile_t*	profile;

	profile = &s_profiles[ format ];

	const u32				from_alpha = 0xff - alpha;
	unsigned long			red, green, blue;

	red = blend_component( from_color, into_color,
		from_alpha, alpha, profile->red_bits, profile->red_shift );
		
	green = blend_component( from_color, into_color,
		from_alpha, alpha, profile->green_bits, profile->green_shift );
	
	blue = blend_component( from_color, into_color,
		from_alpha, alpha, profile->blue_bits, profile->blue_shift );
	
	return red | green | blue;
}

void
std_blit_cross_color(
	pixel_format_t			from_format,
	pixel_format_t			into_format,
	const u08*				from_addr,
	u08*					into_addr,
	long					from_pitch,
	long					into_pitch,
	long					width,
	long					height )
{
	const pixel_profile_t*	from_profile;
	const pixel_profile_t*	into_profile;

	from_profile = &s_profiles[ from_format ];
	into_profile = &s_profiles[ into_format ];

	int						from_red_shift;
	int						from_green_shift;
	int						from_blue_shift;
	int						from_alpha_shift;
	int						from_red_bits;
	int						from_green_bits;
	int						from_blue_bits;
	int						from_alpha_bits;
	int						from_red_mask;
	int						from_green_mask;
	int						from_blue_mask;
	int						from_alpha_mask;
	
	from_red_shift = from_profile->red_shift;
	from_green_shift = from_profile->green_shift;
	from_blue_shift = from_profile->blue_shift;
	from_alpha_shift = from_profile->alpha_shift;
	
	from_red_bits = from_profile->red_bits;
	from_green_bits = from_profile->green_bits;
	from_blue_bits = from_profile->blue_bits;
	from_alpha_bits = from_profile->alpha_bits;
	
	from_red_mask = ( 1L << from_red_bits ) - 1L;
	from_green_mask = ( 1L << from_green_bits ) - 1L;
	from_blue_mask = ( 1L << from_blue_bits ) - 1L;
	from_alpha_mask = ( 1L << from_alpha_bits ) - 1L;
	
	int						into_red_shift;
	int						into_green_shift;
	int						into_blue_shift;
	int						into_alpha_shift;
	int						into_red_bits;
	int						into_green_bits;
	int						into_blue_bits;
	int						into_alpha_bits;
	
	into_red_shift = into_profile->red_shift;
	into_green_shift = into_profile->green_shift;
	into_blue_shift = into_profile->blue_shift;
	into_alpha_shift = into_profile->alpha_shift;
	
	into_red_bits = into_profile->red_bits;
	into_green_bits = into_profile->green_bits;
	into_blue_bits = into_profile->blue_bits;
	into_alpha_bits = into_profile->alpha_bits;
	
	int						from_depth = from_profile->depth;
	int						into_depth = into_profile->depth;
	
	for( long j = 0; j < height; j++ )
	{
		const u08*			from_pixel = from_addr;
		u08*				into_pixel = into_addr;
	
		for( long i = 0; i < width; i++ )
		{
			unsigned long	color;
		
			if( from_depth == 16 )
			{
				color = *((u16*)from_pixel);
				from_pixel += 2;
			}
			else if( from_depth == 24 )
			{
				color = ( (long)(*from_pixel++) ) << 16;
				color |= ( (long)(*from_pixel++) ) << 8;
				color |= *from_pixel++;
			}
			else if( from_depth == 32 )
			{
				color = *((u32*)from_pixel);
				from_pixel += 4;
			}
			
			unsigned long	red, green, blue, alpha;
			
			red = ( color >> from_red_shift ) & from_red_mask;
			if( into_red_bits > from_red_bits )
				red <<= into_red_bits - from_red_bits + into_red_shift;
			else
			{
				red >>= from_red_bits - into_red_bits;
				red <<= into_red_shift;
			}

			green = ( color >> from_green_shift ) & from_green_mask;
			if( into_green_bits > from_green_bits )
				green <<= into_green_bits - from_green_bits + into_green_shift;
			else
			{
				green >>= from_green_bits - into_green_bits;
				green <<= into_green_shift;
			}

			blue = ( color >> from_blue_shift ) & from_blue_mask;
			if( into_blue_bits > from_blue_bits )
				blue <<= into_blue_bits - from_blue_bits + into_blue_shift;
			else
			{
				blue >>= from_blue_bits - into_blue_bits;
				blue <<= into_blue_shift;
			}

			alpha = ( color >> from_alpha_shift ) & from_alpha_mask;
			if( into_alpha_bits > from_alpha_bits )
				alpha <<= into_alpha_bits - from_alpha_bits + into_alpha_shift;
			else
			{
				alpha >>= from_alpha_bits - into_alpha_bits;
				alpha <<= into_alpha_shift;
			}
			
			color = red | green | blue | alpha;
			
			if( into_depth == 16 )
			{
				*((u16*)into_pixel) = color;
				into_pixel += 2;
			}
			else if( into_depth == 24 )
			{
				*into_pixel++ = color >> 16;
				*into_pixel++ = color >> 8;
				*into_pixel++ = color & 0xff;
			}
			else if( into_depth == 32 )
			{
				*((u32*)into_pixel) = color;
				into_pixel += 4;
			}
		}
		
		from_addr += from_pitch;
		into_addr += into_pitch;
	}
}

END_ICARUS_NAMESPACE
