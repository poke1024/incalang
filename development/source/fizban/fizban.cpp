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
//	fizban.cpp			 	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "fizban.h"
#include "screen_t.h"

BEGIN_FIZBAN_NAMESPACE

HDC
working_dc()
{
	static HDC			hDC = 0;

	if( hDC == 0 )
	{
		HDC				hdcScreen;

		hdcScreen = CreateDC( "DISPLAY", NULL, NULL, NULL ); 
		hDC = CreateCompatibleDC( hdcScreen );
		DeleteDC ( hdcScreen );

#if 0
		HBITMAP			bitmap;
		
		//bitmap = CreateCompatibleBitmap( hDC, 0, 0 );
		BITMAPINFOHEADER
						BIH;
		int				iSize = sizeof( BITMAPINFOHEADER );
		void*			pBits;

		memset( &BIH, 0, iSize );

		BIH.biSize = iSize;
		BIH.biWidth = 10;
		BIH.biHeight = 10;
		BIH.biPlanes = 1;
		BIH.biBitCount = 32;
		BIH.biCompression = BI_RGB;

		bitmap = CreateDIBSection( hDC,
			(BITMAPINFO*) &BIH,
			DIB_RGB_COLORS,
			&pBits,
			NULL,
			0 );
		
		SelectObject( hDC, bitmap );
#endif
	}

	return hDC;
}

#if 0
void
working_wgl_context()
{
	if( screen_t::current() != 0 )
		return;

	HDC					hDC = working_dc();
	static HGLRC		hRC = 0;

	PIXELFORMATDESCRIPTOR
						pfd;

	memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR ); 
	pfd.nVersion = 1; 
	pfd.dwFlags =  PFD_DRAW_TO_BITMAP |
		PFD_SUPPORT_OPENGL |
		PFD_SUPPORT_GDI;
	pfd.iPixelType = PFD_TYPE_RGBA; // PFD_TYPE_COLORINDEX
	pfd.cColorBits = 32; //(BYTE)bmInfo.bmBitsPixel;
	pfd.cDepthBits = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;

	GLuint				pixelFormat;

	if( !( pixelFormat = ChoosePixelFormat( hDC, &pfd ) ) )
		throw_icarus_error( "cannot find suitable pixel format" );

	if( !SetPixelFormat( hDC, pixelFormat, &pfd ) )
		throw_icarus_error( "cannot set pixel format" );

	if( hRC == 0 )
	{
		if( !( hRC = wglCreateContext( hDC ) ) )
			throw_icarus_error( "cannot create GL rendering context" );
	}

	if( !wglMakeCurrent( hDC, hRC ) )
		throw_icarus_error( "cannot activate gl rendering context" );
}
#endif

END_FIZBAN_NAMESPACE
