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
//	os_core_win.cpp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus.h"
#include "os_core.h"

#include <string>
#include <cstring>
#include <windows.h>

BEGIN_ICARUS_NAMESPACE

#if ICARUS_GFX

struct wbitmap_t {
	HDC					hDC;
	HBITMAP				hBitmap;
	void*				bits;
	long				pitch;
};

void
os_lock_bitmap(
	void*				bmp )
{
}

void
os_unlock_bitmap(
	void*				bmp )
{
}

int
os_get_bitmap_pitch(
	void*				bmp )
{
	return ( (wbitmap_t*)bmp )->pitch;
}

void*
os_get_bitmap_pixels(
	void*				bmp )
{
	return ( (wbitmap_t*)bmp )->bits;
}

void*
os_create_bitmap(
	pixel_format_t		format,
	long				width,
	long				height,
	int					/*flags*/ )
{
	int					depth;

	switch( format )
	{
		case pixel_format_24_RGB:
			depth = 24;
			break;

		case pixel_format_32_RGBA:
			depth = 32;
			break;

		default:
			throw_icarus_error( "unsupported native pixel format" );
	}

	BITMAPINFO			bi;

	bi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = depth;
	bi.bmiHeader.biCompression = BI_RGB;

	wbitmap_t*			bitmap;

	bitmap = new wbitmap_t;

	bitmap->hDC = CreateCompatibleDC( NULL );
	//hdcScreen = CreateDC( "DISPLAY", NULL, NULL, NULL );
	//DeleteObject( hdcScreen );

	bitmap->bits = NULL;
	bitmap->hBitmap = CreateDIBSection(
		bitmap->hDC,
		&bi,
		DIB_RGB_COLORS,
		&bitmap->bits,
		NULL, 0 );

	bitmap->pitch = ( depth * width ) / 8;
	if( bitmap->pitch & 3 )
		bitmap->pitch += 4 - ( bitmap->pitch & 3 );

	return bitmap;
}

void
os_destroy_bitmap(
	void*				bmp )
{
	wbitmap_t*			bitmap;
	
	bitmap = (wbitmap_t*)bmp;

	DeleteObject( bitmap->hBitmap );
	DeleteDC( bitmap->hDC );
}

void*
os_win_bitmap_hdc(
	void*				bmp )
{
	return ( (wbitmap_t*)bmp )->hDC;
}

void*
os_win_bitmap_hbitmap(
	void*				bmp )
{
	return ( (wbitmap_t*)bmp )->hBitmap;
}

#endif

// ---------------------------------------------------------------------------

u64
os_time_millis()
{
	return GetTickCount();
}

int
os_double_click_millis()
{
	return GetDoubleClickTime();
}

END_ICARUS_NAMESPACE
