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
//	icarus_bmp.cpp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus_bmp.h"

#include "bitmap_t.h"
#include "grafport_t.h"
#include "input_stream_t.h"
#include "output_stream_t.h"
#include "file_stream_t.h"

#include <windows.h>

BEGIN_ICARUS_NAMESPACE

static void
write_truecolor_bmp(
	bitmap_t*			bitmap,
	output_stream_t*	stream,
	long				wpitch )
{
	const long			width = bitmap->width();
	const long			height = bitmap->height();

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
			if( alpha_bits )
			{
				color = ( (long)(*pixel_ptr++) ) << 24;
				color |= ( (long)(*pixel_ptr++) ) << 16;
				color |= ( (long)(*pixel_ptr++) ) << 8;
				color |= *pixel_ptr++;
			}
			else
			{
				color = ( (long)(*pixel_ptr++) ) << 16;
				color |= ( (long)(*pixel_ptr++) ) << 8;
				color |= ( (long)(*pixel_ptr++) ) << 0;
			}
				
			red = ( color >> red_shift ) & red_mask;
			red <<= 8 - red_bits;

			green = ( color >> green_shift ) & green_mask;
			green <<= 8 - green_bits;

			blue = ( color >> blue_shift ) & blue_mask;
			blue <<= 8 - blue_bits;

			alpha = ( color >> alpha_shift ) & alpha_mask;
			alpha <<= 8 - alpha_bits;

			if( alpha_bits )
				stream->write_u08( alpha );

			stream->write_u08( blue );
			stream->write_u08( green );
			stream->write_u08( red );
		}

		for( long i = pitch; i < wpitch; i++ )
			stream->write_u08( 0 );
		
		row_ptr += pitch;
	}
}

void
write_bmp(
	bitmap_t*			bitmap,
	output_stream_t*	stream )
{
	int					cClrBits;

	switch( bitmap->format() )
	{
		/*case pixel_format_8_indexed_gray:
		case pixel_format_8_indexed_palette:
			cClrBits = 8;
			break;*/

		case pixel_format_24_RGB:
		case pixel_format_24_GBR:
		case pixel_format_24_BRG:
		case pixel_format_24_BGR:
		case pixel_format_24_GRB:
		case pixel_format_24_RBG:
			cClrBits = 24;
			break;
			
		case pixel_format_32_RGBA:
			cClrBits = 32;
			break;

		default:
			throw_icarus_error(
				"unsupported bmp output format" );
	}

	BITMAPFILEHEADER	hdr;
    PBITMAPINFOHEADER	pbih;
	PBITMAPINFO			pbmi;
	long				pitch;
	
	if( cClrBits < 24 ) 
		pbmi = (PBITMAPINFO)new u08[ 
			sizeof( BITMAPINFOHEADER ) + 
			sizeof( RGBQUAD ) * ( 1L << cClrBits ) ]; 
	else 
		pbmi = (PBITMAPINFO)new u08[
			sizeof( BITMAPINFOHEADER ) ];

	memset( pbmi, 0, sizeof( BITMAPINFOHEADER ) );

    pbmi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER ); 
    pbmi->bmiHeader.biWidth = bitmap->width(); 
    pbmi->bmiHeader.biHeight = bitmap->height(); 
    pbmi->bmiHeader.biPlanes = 1; 
    pbmi->bmiHeader.biBitCount = cClrBits; 
    if( cClrBits < 24 ) 
        pbmi->bmiHeader.biClrUsed = ( 1 << cClrBits );
	else
		pbmi->bmiHeader.biClrUsed = 0;

	pbmi->bmiHeader.biCompression = BI_RGB; 

	pitch = ( ( pbmi->bmiHeader.biWidth *
		cClrBits + 31 ) & ~31 ) / 8;

	pbmi->bmiHeader.biSizeImage =
		pitch * pbmi->bmiHeader.biHeight; 
	
	pbmi->bmiHeader.biClrImportant = 0; 


	pbih = (PBITMAPINFOHEADER)pbmi; 

	hdr.bfType = 0x4d42;
	
	// Compute the size of the entire file. 
	hdr.bfSize = (DWORD)( sizeof( BITMAPFILEHEADER ) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof( RGBQUAD ) + pbih->biSizeImage );
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	
	// Compute the offset to the array of color indices.
	hdr.bfOffBits = (DWORD)sizeof( BITMAPFILEHEADER ) +
		pbih->biSize + pbih->biClrUsed * sizeof( RGBQUAD ); 
	
	// Copy the BITMAPFILEHEADER into the .BMP file.
	stream->write_bytes( &hdr, sizeof( BITMAPFILEHEADER ) );

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
	stream->write_bytes( pbih, sizeof( BITMAPINFOHEADER ) 
		+ pbih->biClrUsed * sizeof( RGBQUAD ) );

	delete[] pbmi;

    // Copy the array of color indices into the .BMP file.

	write_truecolor_bmp(
		bitmap, stream, pitch );
}

END_ICARUS_NAMESPACE
