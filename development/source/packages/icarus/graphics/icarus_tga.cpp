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
//	icarus_tga.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus_tga.h"

#include "bitmap_t.h"
#include "grafport_t.h"
#include "input_stream_t.h"
#include "output_stream_t.h"
#include "file_stream_t.h"

BEGIN_ICARUS_NAMESPACE

bitmap_t*
read_tga(
	const char*			path,
	int					flags )
{
	file_stream_t		stream( path, "rb" );
	
	return read_tga( &stream, flags );
}

bitmap_t*
read_tga(
	input_stream_t*		stream,
	int					flags )
{
	bitmap_t*			bitmap;

	static u08			image_id[ 256 ];
	static u08			image_palette[ 256 ][ 3 ];

	int					id_length;
	int					palette_type;
	int					image_type;
	unsigned int		first_color;
	unsigned int		palette_colors;
	int					palette_entry_size;
	unsigned int		left;
	unsigned int		top;
	unsigned int		image_width;
	unsigned int		image_height;
	int					bpp;
	int					descriptor_bits;

	id_length = stream->read_u08();
	palette_type = stream->read_u08();
	image_type = stream->read_u08();
	first_color = swap_endian16( stream->read_u16() );
	palette_colors  = swap_endian16( stream->read_u16() );
	palette_entry_size = stream->read_u08();
	left = swap_endian16( stream->read_u16() );
	top = swap_endian16( stream->read_u16() );
	image_width = swap_endian16( stream->read_u16() );
	image_height = swap_endian16( stream->read_u16() );
	bpp = stream->read_u08();
	descriptor_bits = stream->read_u08();
	
	stream->read_bytes( image_id, id_length );
	stream->read_bytes( image_palette, palette_colors * 3 );

	// Image type:
	// 0 = no image data
	// 1 = uncompressed color mapped
	// 2 = uncompressed true color
	// 3 = grayscale
	// 9 = RLE color mapped
	// 10 = RLE true color
	// 11 = RLE grayscale

	if( image_type & 8 )
		throw_icarus_error( "tga compression not supported" );
	
	image_type &= 7;

	if( image_type < 1 || image_type > 3 )
		throw_icarus_error( "unsupported tga image type" );
		
	switch( image_type )
	{
		case 1:
			// paletted image
			if( palette_type != 1 || bpp != 8 )
				throw_icarus_error( "unsupported tga format" );

			bitmap = new bitmap_t( pixel_format_8_indexed,
				image_width, image_height, flags );

			for( long i = 0; i < palette_colors; i++ )
			{
				bitmap->set_palette_entry(
					i, image_palette[ i ] );
			}
			break;
		
		case 2:
		{
			// truecolor image
			if( palette_type != 0 )
				throw_icarus_error( "unsupported tga format" );
	
			pixel_format_t	format;
	
			if( bpp == 24 )
				format = pixel_format_24_RGB;
			else if( bpp == 32 )
				format = pixel_format_32_RGBA;
			else
				throw_icarus_error( "unsupported tga format" );				
			
			bitmap = new bitmap_t( format,
				image_width, image_height, flags );
			break;
		}
			
		 case 3:
			// grayscale image
			if( palette_type != 0 || bpp != 8 )
				throw_icarus_error( "unsupported tga format" );

			bitmap = new bitmap_t( pixel_format_8_indexed_gray,
				image_width, image_height, flags );
			break;
			
		default:
			throw_icarus_error( "unsupported tga format" );
			break;
	}
	
	grafport_t			port( bitmap );
	void*				pixels = port.pixels();
	
	for( int y = image_height; y; y-- )
	{
		int 			yc;
		u08*			row_ptr;
		
		yc = ( descriptor_bits & 0x20 ) ? image_height - y : y - 1;
		
		row_ptr = (u08*)pixels + yc * (long)bitmap->pitch();
		
		switch( image_type )
		{
			case 1:
			case 3:
				stream->read_bytes( row_ptr, image_width );
				break;
				
			case 2:
				if( bpp == 24 )
				{
					stream->read_bytes( row_ptr, image_width * 3 );
					
					u08*	pixel_ptr = row_ptr;

					for( int x = 0; x < image_width; x++ )
					{
						const int h = pixel_ptr[ 0 ];
						pixel_ptr[ 0 ] = pixel_ptr[ 2 ];
						pixel_ptr[ 2 ] = h;
						pixel_ptr += 3;
					}
				}
				else
				{
					stream->read_bytes( row_ptr, image_width * 4 );
					
					u08*	pixel_ptr = row_ptr;

					for( int x = 0; x < image_width; x++ )
					{
						const int h = pixel_ptr[ 0 ];
						pixel_ptr[ 0 ] = pixel_ptr[ 2 ];
						pixel_ptr[ 2 ] = h;
						pixel_ptr += 4;
					}
				}

				break;
		}
	}

	return bitmap;
}

static void
write_tga_8_indexed_gray(
	bitmap_t*			bitmap,
	output_stream_t*		stream )
{
	const long			width = bitmap->width();
	const long			height = bitmap->height();

	stream->write_u08( 0 );			// id_length
	stream->write_u08( 0 );			// palette_type
	stream->write_u08( 3 );			// image_type
	stream->write_u16( 0 );			// first_color
	stream->write_u16( 0 );			// palette_colors
	stream->write_u08( 0 );			// palette_entry_size
	stream->write_u16( 0 );			// left
	stream->write_u16( 0 );			// top
	stream->write_u16( swap_endian16( width ) );
	stream->write_u16( swap_endian16( height ) );
	stream->write_u08( 8 );			// bpp
	stream->write_u08( 0x20 );		// descriptor_bits
	
	grafport_t			port( bitmap );
	
	const u08*			row_ptr = (u08*)port.pixels();
	const long			pitch = port.pitch();
	
	for( long y = 0; y < height; y++ )
	{
		stream->write_bytes( row_ptr, width );		
		row_ptr += pitch;
	}
}

static void
write_tga_24(
	bitmap_t*			bitmap,
	output_stream_t*		stream )
{
	const long			width = bitmap->width();
	const long			height = bitmap->height();

	stream->write_u08( 0 );			// id_length
	stream->write_u08( 0 );			// palette_type
	stream->write_u08( 2 );			// image_type
	stream->write_u16( 0 );			// first_color
	stream->write_u16( 0 );			// palette_colors
	stream->write_u08( 0 );			// palette_entry_size
	stream->write_u16( 0 );			// left
	stream->write_u16( 0 );			// top
	stream->write_u16( swap_endian16( width ) );
	stream->write_u16( swap_endian16( height ) );
	stream->write_u08( 24 );		// bpp
	stream->write_u08( 0x20 );		// descriptor_bits
	
	grafport_t			port( bitmap );
	
	const u08*			row_ptr = (u08*)port.pixels();
	const long			pitch = port.pitch();
	
	const pixel_profile_t*	profile;
	
	profile = get_pixel_format_profile(
		bitmap->format() );
	
	int					red_shift, green_shift, blue_shift;
	int					red_bits, green_bits, blue_bits;
	unsigned long		red_mask, green_mask, blue_mask;
	
	red_shift = profile->red_shift;
	green_shift = profile->green_shift;
	blue_shift = profile->blue_shift;

	red_bits = profile->red_bits;
	green_bits = profile->green_bits;
	blue_bits = profile->blue_bits;
	
	red_mask = ( 1L << red_bits ) - 1L;
	green_mask = ( 1L << green_bits ) - 1L;
	blue_mask = ( 1L << blue_bits ) - 1L;

	for( long y = 0; y < height; y++ )
	{
		const u08*		pixel_ptr = row_ptr;
	
		unsigned long	color;
		unsigned int	red, green, blue;
	
		for( long x = 0; x < width; x++ )
		{
			color = ( (long)(*pixel_ptr++) ) << 16;
			color |= ( (long)(*pixel_ptr++) ) << 8;
			color |= *pixel_ptr++;
				
			red = ( color >> red_shift ) & red_mask;
			red <<= 8 - red_bits;

			green = ( color >> green_shift ) & green_mask;
			green <<= 8 - green_bits;

			blue = ( color >> blue_shift ) & blue_mask;
			blue <<= 8 - blue_bits;
		
			stream->write_u08( blue );
			stream->write_u08( green );
			stream->write_u08( red );
		}
		
		row_ptr += pitch;
	}
}

static void
write_tga_32(
	bitmap_t*			bitmap,
	output_stream_t*	stream )
{
	const long			width = bitmap->width();
	const long			height = bitmap->height();

	stream->write_u08( 0 );			// id_length
	stream->write_u08( 0 );			// palette_type
	stream->write_u08( 2 );			// image_type
	stream->write_u16( 0 );			// first_color
	stream->write_u16( 0 );			// palette_colors
	stream->write_u08( 0 );			// palette_entry_size
	stream->write_u16( 0 );			// left
	stream->write_u16( 0 );			// top
	stream->write_u16( swap_endian16( width ) );
	stream->write_u16( swap_endian16( height ) );
	stream->write_u08( 32 );		// bpp
	stream->write_u08( 0x20 );		// descriptor_bits
	
	grafport_t			port( bitmap );
	
	const u08*			row_ptr = (u08*)port.pixels();
	const long			pitch = port.pitch();
	
	const pixel_profile_t*	profile;
	
	profile = get_pixel_format_profile(
		bitmap->format() );
	
	int					red_shift, green_shift, blue_shift, alpha_shift;
	int					red_bits, green_bits, blue_bits, alpha_bits;
	unsigned long		red_mask, green_mask, blue_mask, alpha_mask;
	
	red_shift = profile->red_shift;
	green_shift = profile->green_shift;
	blue_shift = profile->blue_shift;
	alpha_shift = profile->alpha_shift;

	red_bits = profile->red_bits;
	green_bits = profile->green_bits;
	blue_bits = profile->blue_bits;
	alpha_bits = profile->alpha_bits;
	
	red_mask = ( 1L << red_bits ) - 1L;
	green_mask = ( 1L << green_bits ) - 1L;
	blue_mask = ( 1L << blue_bits ) - 1L;
	alpha_mask = ( 1L << alpha_bits ) - 1L;

	for( long y = 0; y < height; y++ )
	{
		const u08*		pixel_ptr = row_ptr;
	
		unsigned long	color;
		unsigned int	red, green, blue, alpha;
	
		for( long x = 0; x < width; x++ )
		{
			color = ( (long)(*pixel_ptr++) ) << 24;
			color |= ( (long)(*pixel_ptr++) ) << 16;
			color |= ( (long)(*pixel_ptr++) ) << 8;
			color |= *pixel_ptr++;
				
			red = ( color >> red_shift ) & red_mask;
			red <<= 8 - red_bits;

			green = ( color >> green_shift ) & green_mask;
			green <<= 8 - green_bits;

			blue = ( color >> blue_shift ) & blue_mask;
			blue <<= 8 - blue_bits;

			alpha = ( color >> alpha_shift ) & alpha_mask;
			alpha <<= 8 - alpha_bits;

			stream->write_u08( blue );
			stream->write_u08( green );
			stream->write_u08( red );
			stream->write_u08( alpha );
		}
		
		row_ptr += pitch;
	}
}

void
write_tga(
	bitmap_t*			bitmap,
	output_stream_t*	stream )
{
	switch( bitmap->format() )
	{
		case pixel_format_8_indexed_gray:
			write_tga_8_indexed_gray( bitmap, stream );
			break;
	
		case pixel_format_24_RGB:
		case pixel_format_24_GBR:
		case pixel_format_24_BRG:
		case pixel_format_24_BGR:
		case pixel_format_24_GRB:
		case pixel_format_24_RBG:
			write_tga_24( bitmap, stream );
			break;
			
		case pixel_format_32_RGBA:
			write_tga_32( bitmap, stream );
			break;

		default:
			throw_icarus_error( "unsupported tga output format" );
	}
}

END_ICARUS_NAMESPACE
