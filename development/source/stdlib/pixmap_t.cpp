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
//	pixmap_t.cp				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "pixmap_t.h"
#include "icarus_tga.h"
#include "icarus_bmp.h"
#include "file_stream_t.h"

#include <windows.h>
#include <gl/gl.h>
#include "os_core.h"

BEGIN_MACHINE_NAMESPACE

pixmap_t::pixmap_t()
{
	init();
}

pixmap_t::pixmap_t(
	lib_context_t&		context ) :
	
	runtime_object_t( context, RTID_PIXMAP )
{
	init();
}

pixmap_t::~pixmap_t()
{
	release();
}

void
pixmap_t::allocate(
	long				width,
	long				height,
	pixel_format_t		format )
{
	release();

	m_format = format;
	
	m_bitmap = new bitmap_t(
		format, width, height,
		bitmap_flags() );
		
	m_port = new grafport_t(
		m_bitmap );

	allocate_gl();
}

void
pixmap_t::load(
	const char*			path )
{
	release();
	
	m_bitmap = read_tga( path, bitmap_flags() );
	m_format = m_bitmap->format();
	m_port = new grafport_t( m_bitmap );

	allocate_gl();
}

void
pixmap_t::save(
	const char*			path )
{
	if( not m_bitmap )
		return;

	istring				name = path;

	const char*			ext = strrchr( path, '.' );
	long				format = 0;

	if( ext )
	{
		if( strcmp( ext, ".tga" ) == 0 )
			format = 'tga';
		else if( strcmp( ext, ".bmp" ) == 0 )
			format = 'bmp';
	}

	if( format == 0 )
	{
		name.append( ".bmp" );
		format = 'bmp';
	}

	file_stream_t		stream( name.c_str(), "wb" );

	if( format == 'tga' )
		write_tga( m_bitmap, &stream );
	else if( format == 'bmp' )
		write_bmp( m_bitmap, &stream );
}

void
pixmap_t::release()
{
	release_gl();

	if( m_port )
		delete m_port;
		
	if( m_bitmap )
		delete m_bitmap;

	m_port = 0;
	m_bitmap = 0;
}

void
pixmap_t::convert(
	pixel_format_t		format )
{
	if( not m_bitmap )
		return;

	if( format == m_bitmap->format() )
		return;
		
	release_gl();

	delete m_port;
	m_bitmap = m_bitmap->to_format( format );
	m_port = new grafport_t( m_bitmap );
	m_format = format;

	allocate_gl();
}

void
pixmap_t::allocate_gl()
{
	if( not m_gl )
		return;

	void*				native = m_bitmap->native();
	HGLRC				hRC;

	HDC					hDC;
	HBITMAP				hBitmap;

	hDC = (HDC)os_win_bitmap_hdc( native );
	hBitmap = (HBITMAP)os_win_bitmap_hbitmap( native );

	m_old_bitmap = SelectObject( hDC, hBitmap );

	BITMAP				bmp;
	int					cClrBits;

    if( !GetObject( hBitmap, sizeof( BITMAP ), (LPSTR)&bmp ) ) 
        throw_icarus_error( "GetObject failed" );

    cClrBits = (WORD)( bmp.bmPlanes * bmp.bmBitsPixel ); 

	PIXELFORMATDESCRIPTOR	pfd;

	memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_BITMAP |
		PFD_SUPPORT_OPENGL |
		PFD_SUPPORT_GDI;
	pfd.iPixelType = PFD_TYPE_RGBA; 
	pfd.cColorBits = (BYTE)cClrBits;
	pfd.cDepthBits = m_depth_bits;
	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int					nPixelFormat;
	BOOL				bResult;
	
	nPixelFormat = ChoosePixelFormat( hDC, &pfd );
	if( nPixelFormat == 0 )
		throw_icarus_error(
			"ChoosePixelFormat failed (%d)",
			GetLastError() );

	bResult = SetPixelFormat( hDC, nPixelFormat, &pfd );
	if( !bResult )
		throw_icarus_error(
			"SetPixelFormat failed (%d)",
			GetLastError() );
	
	hRC = wglCreateContext( hDC );
	if( !hRC )
		throw_icarus_error(
			"wglCreateContext failed (%d)",
			GetLastError() );

	const int			width = m_bitmap->width();
	const int			height = m_bitmap->height();

	m_gl_context = new fizban::gl_context_t;

	m_gl_context->init( hDC, hRC );
	m_gl_context->setup_viewport(
		width, height, m_gl == 2 );
}

void
pixmap_t::release_gl()
{
	if( m_gl_context )
	{
		delete m_gl_context;
		m_gl_context = 0;
	}
}

void
pixmap_t::init()
{
	m_format = pixel_format_24_RGB;
	m_bitmap = 0;
	m_port = 0;
	m_instance = 0;

	m_color = 0;
	m_cursor.h = 0;
	m_cursor.v = 0;
	m_scursor.h = 0;
	m_scursor.v = 0;
	m_sthickness.h = 1;
	m_sthickness.v = 1;
	m_default_alpha = 255;

	m_gl = 0;
	m_depth_bits = 0;
	m_gl_context = 0;
}

int
pixmap_t::bitmap_flags()
{
	return m_gl ? bmp_create_native : 0;
}

END_MACHINE_NAMESPACE
