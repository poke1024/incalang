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
//	screen_t.cpp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#define IMPLEMENT_FENCES 0

#define WINVER 0x0500
#define _WIN32_WINNT 0x0400
#define _WIN32_WINDOWS 0x0410 
#define _WIN32_IE 0x0400

#include <Windows.h>
#include <Windowsx.h>
#include <gl\glu.h>

#include "screen_t.h"
#include "thread_t.h"
#include "key_buffer_t.h"
#include "input_t.h"

#if IMPLEMENT_SGL
#include "sgl.h"
#endif

#if IMPLEMENT_FENCES
#define GLH_EXT_SINGLE_FILE
#include <glh_extensions.h>
#endif

extern HINSTANCE		gAppInstance;
extern machine::key_buffer_t*	g_key_buffer;

extern bool				g_quit_okay;
extern bool				g_quit_issued;

BEGIN_FIZBAN_NAMESPACE

// ---------------------------------------------------------------------------

LRESULT CALLBACK
screen_t::wnd_proc(
	HWND				hWnd,
	UINT				uMsg,
	WPARAM				wParam,
	LPARAM				lParam )
{
	screen_t*			screen;

	screen = (screen_t*)GetWindowLong( hWnd, GWL_USERDATA );
	if( screen == 0 )
		return DefWindowProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
		case WM_ACTIVATE:	
			if( !HIWORD( wParam ) )
				screen->m_active = true;
			else
				screen->m_active = false;

			return 0;

		case WM_SYSCOMMAND:
			switch( wParam )
			{
				// prevent screen saver or
				// monitor power save to switch on
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;

		case WM_CLOSE:
			if( g_quit_okay )
				g_quit_issued = true;
			else
				exit( EXIT_SUCCESS );
			return 0;

		case WM_CHAR:
			g_key_buffer->submit_key( wParam );
			return 0;

		case WM_KEYDOWN:
			g_key_buffer->submit_key( -wParam );
			//keys[wParam] = TRUE;
			return 0;

		case WM_KEYUP:
			//keys[wParam] = FALSE;
			return 0;

		case WM_SIZE:
			screen->resize_gl( LOWORD( lParam ),HIWORD( lParam ) );
			return 0;

		case WM_PAINT:
			//hdc = BeginPaint( hWnd, &ps );
			//EndPaint( hWnd, &ps );
			break;

		case WM_LBUTTONDOWN:
			screen->mouse_button_change( 0, true, lParam );
			break;

		case WM_RBUTTONDOWN:
			screen->mouse_button_change( 1, true, lParam );
			break;

		case WM_MBUTTONDOWN:
			screen->mouse_button_change( 2, true, lParam );
			break;

		case WM_LBUTTONUP:
			screen->mouse_button_change( 0, false, lParam );
			break;

		case WM_RBUTTONUP:
			screen->mouse_button_change( 1, false, lParam );
			break;

		case WM_MBUTTONUP:
			screen->mouse_button_change( 2, false, lParam );
			break;

		case WM_MOUSEMOVE:
			screen->m_mouse_location.h = GET_X_LPARAM( lParam ); 
			screen->m_mouse_location.v = GET_Y_LPARAM( lParam );
			break;

		case WM_MOUSEWHEEL:
			screen->m_mouse_z += GET_WHEEL_DELTA_WPARAM( wParam );
			break;

	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

// ---------------------------------------------------------------------------

static screen_t* s_current_screen = 0;

screen_t::screen_t()
{
	m_display_fullscreen = false;
	m_display_doublebuffer = true;
	m_title = "OpenGL View";

	m_display_width = 640;//800;
	m_display_height = 480;//600;
	m_display_bits = 32;
	m_display_frequency = 75;

	m_hWnd = NULL;
	m_active = true;
	m_resized = true;

	m_has_fence = false;
	m_2d_only = false;//true;

	m_mouse_location.h = -1;
	m_mouse_location.v = -1;
	m_mouse_z = 0;
	for( int i = 0; i < 8; i++ )
		m_mouse_button[ i ] = false;
}

screen_t::~screen_t()
{
	close();
	if( s_current_screen == this )
		s_current_screen = 0;
}

void
screen_t::open(
	monitor_t*			monitor )
{
	create_window( monitor );
	init_fences();

	if( m_2d_only )
		glDisable( GL_DEPTH_TEST );
}

void
screen_t::close()
{
	cleanup_fences();
	kill_window();
}

void
screen_t::clear()
{
	if( m_2d_only )
		glClear( GL_COLOR_BUFFER_BIT );
	else
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void
screen_t::swap()
{
	sgl_flush();

	// important - give OpenGL some time
	// to flush the pipeline and prepare
	// for the buffer swap

#if IMPLEMENT_FENCES
	glSetFenceNV( m_fence, GL_ALL_COMPLETED_NV );
	glFlush();
	
	while( true )
	{
		if( glTestFenceNV( m_fence ) )
			break;
		thread_t::current()->sleep( 50 );
	}
#endif

	// now swap buffers

	// SwapBuffers latency: 187 ms (peak), 37 ms (mean)
	// glFinish latency: 251 ms (peak), 92 ms (mean)
	// combined latency: 225 ms (peak), 129 ms (mean)

	if( m_gl_context.hdc() )
		SwapBuffers( m_gl_context.hdc() );

	glFinish();
}

void
screen_t::make_current()
{
	m_gl_context.make_current();
	s_current_screen = this;
}

screen_t*
screen_t::current()
{
	return s_current_screen;
}

void
screen_t::capture_mouse()
{
	SetCapture( m_hWnd );
	//ReleaseCapture();
}

void
screen_t::set_display_size(
	int					width,
	int					height )
{
	m_display_width = width;
	m_display_height = height;
}

void
screen_t::set_fullscreen(
	bool				fullscreen )
{
	m_display_fullscreen = fullscreen;
}

void
screen_t::set_doublebuffer(
	bool				doublebuffer )
{
	m_display_doublebuffer = doublebuffer;
}

void
screen_t::set_2d_only(
	bool				flag )
{
	m_2d_only = flag;
}

void
screen_t::set_title(
	const char*			title )
{
	m_title = title;
	if( m_hWnd )
		SetWindowText( m_hWnd, title );
}

HDC
screen_t::hdc() const
{
	return m_gl_context.hdc();
}

long
screen_t::width() const
{
	return m_display_width;
}

long
screen_t::height() const
{
	return m_display_height;
}

bool
screen_t::fetch_resize_flag()
{
	bool				flag = m_resized;
	m_resized = false;
	return flag;
}

long
screen_t::mouse_x() const
{
	return m_mouse_location.h;
}

long
screen_t::mouse_y() const
{
	return m_mouse_location.v;
}

long
screen_t::mouse_z() const
{
	return ( m_mouse_z / WHEEL_DELTA );
}

bool
screen_t::mouse_button(
	int					index ) const
{
	if( index < 0 || index > 8 )
		return false;
	return m_mouse_button[ index ];
}

void
screen_t::create_window(
	monitor_t*			monitor )
{
	GLuint				pixelFormat;
	DWORD				dwExStyle;
	DWORD				dwStyle;
	HINSTANCE			hInstance = gAppInstance;

	HDC					hDC;
	HGLRC				hRC;

	register_window_class();
	
	if( m_display_fullscreen )
	{
		DEVMODE			dmScreenSettings;

		memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) );
		dmScreenSettings.dmSize = sizeof( dmScreenSettings );
		dmScreenSettings.dmPelsWidth = m_display_width;
		dmScreenSettings.dmPelsHeight = m_display_height;
		dmScreenSettings.dmBitsPerPel = m_display_bits;
		dmScreenSettings.dmDisplayFrequency = m_display_frequency;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH |
			DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		if( !monitor->change_display_settings(
			&dmScreenSettings, CDS_FULLSCREEN ) )
		//if( ChangeDisplaySettings( &dmScreenSettings,
		//	CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
		{
			// fall back on windowed mode, if video card
			// does not support the requested mode
			m_display_fullscreen = false;
		}
	}

	RECT				windowRect;

	if( m_display_fullscreen )
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor( FALSE );
		windowRect = *monitor->display_rect();
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
		SetRect( &windowRect, 0, 0, m_display_width, m_display_height );
	}

	/*rt_log( "wrect %ld %ld %ld %ld\n",
		windowRect.left,
		windowRect.top,
		windowRect.right,
		windowRect.bottom );*/

	/*windowRect.left = (long)0;
	windowRect.right = (long)m_display_width;
	windowRect.top = (long)0;
	windowRect.bottom = (long)m_display_height;*/

	AdjustWindowRectEx( &windowRect,
		dwStyle, FALSE, dwExStyle );

	// Create The Window
	if( !( m_hWnd = CreateWindowEx(
						dwExStyle,							// Extended Style For The Window
						"OpenGL",							// Class Name
						m_title.c_str(),					// Window Title
						dwStyle |							// Defined Window Style
						WS_CLIPSIBLINGS |					// Required Window Style
						WS_CLIPCHILDREN,					// Required Window Style
						0, 0,								// Window Position
						windowRect.right - windowRect.left,	// Calculate Window Width
						windowRect.bottom - windowRect.top,	// Calculate Window Height
						NULL,								// No Parent Window
						NULL,								// No Menu
						hInstance,							// Instance
						NULL) ) )							// Dont Pass Anything To WM_CREATE
	{
		kill_window();
		throw_icarus_error( "gl window creation error" );
	}

	SetWindowLong( m_hWnd, GWL_USERDATA, (long)this );

	static PIXELFORMATDESCRIPTOR pfd = {			// pfd Tells Windows How We Want Things To Be
		sizeof( PIXELFORMATDESCRIPTOR ),			// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL,							// Format Must Support OpenGL
		PFD_TYPE_RGBA,								// Request An RGBA Format
		m_display_bits,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if( m_display_doublebuffer )
		pfd.dwFlags |= PFD_DOUBLEBUFFER;

	if( m_2d_only )
		pfd.cDepthBits = 0;

	if( !( hDC = GetDC( m_hWnd ) ) )
	{
		kill_window();
		throw_icarus_error( "cannot create device context" );
	}

	if( !( pixelFormat = ChoosePixelFormat( hDC, &pfd ) ) )
	{
		kill_window();
		throw_icarus_error( "cannot find suitable pixel format" );
	}

	if( !SetPixelFormat( hDC, pixelFormat, &pfd ) )
	{
		kill_window();
		throw_icarus_error( "cannot set pixel format" );
	}

	if( !( hRC = wglCreateContext( hDC ) ) )
	{
		kill_window();
		throw_icarus_error( "cannot create GL rendering context" );
	}

	m_gl_context.init( hDC, hRC );
	s_current_screen = this;

	ShowWindow( m_hWnd, SW_SHOW );
	SetForegroundWindow( m_hWnd );
	SetFocus( m_hWnd );
	resize_gl( m_display_width, m_display_height );

	rt_input_init( m_hWnd );
}

void
screen_t::kill_window()
{
	if( m_display_fullscreen )
	{
		ChangeDisplaySettings( NULL, 0 );
		ShowCursor( TRUE );
	}

	HDC					hDC = m_gl_context.hdc();

	m_gl_context.release();

	if( hDC && !ReleaseDC( m_hWnd, hDC ) )	
		throw_icarus_error( "release device context failed" );

	if( m_hWnd && !DestroyWindow( m_hWnd ) )
		throw_icarus_error( "could not release hWnd" );
	m_hWnd = NULL;
}

void
screen_t::register_window_class()
{
	static bool			init = false;

	if( init )
		return;

	HINSTANCE			hInstance = gAppInstance;
	WNDCLASS			wc;
	
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC)wnd_proc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon( NULL, IDI_WINLOGO );
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "OpenGL";

	if( !RegisterClass( &wc ) )
		throw_icarus_error( "failed to register window class" );

	init = true;
}

void
screen_t::resize_gl(
	GLsizei				width,
	GLsizei				height )
{
	m_gl_context.setup_viewport(
		width, height, m_2d_only );

	m_display_width = width;
	m_display_height = height;
	m_resized = true;
}

void
screen_t::init_fences()
{
#if IMPLEMENT_FENCES
	static bool			init_extension = false;

	if( !init_extension )
	{
		// we have to wait with the initialization of these extensions
		// until an OpenGL context was established in a window. before
		// that, the driver would not know, which video card we're
		// talking about.

		if( !glh_init_extensions( "GL_NV_fence " ) )
			rt_fatal( "GL_NV_fence extension not available" );

		if( !glh_init_extension( "GL_NV_fence" ) )
			rt_fatal( "could not initialize GL_NV_fence extension" );

		init_extension = true;
	}

	glGenFencesNV( 1, &m_fence );
	m_has_fence = true;
#endif
}

void
screen_t::cleanup_fences()
{
#if IMPLEMENT_FENCES
	if( m_has_fence )
	{
		glDeleteFencesNV( 1, &m_fence );
		m_has_fence = false;
	}
#endif
}

void
screen_t::mouse_button_change(
	int					index,
	bool				down,
	LPARAM				lParam )
{
	m_mouse_location.h = GET_X_LPARAM( lParam );
	m_mouse_location.v = GET_Y_LPARAM( lParam );
	m_mouse_button[ index ] = down;
}

END_FIZBAN_NAMESPACE
