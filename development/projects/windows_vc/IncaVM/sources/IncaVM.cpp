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

// IncaVM.cpp

#include "stdafx.h"
#include "IncaVM.h"
#include "vmglobals.h"
#include "icarus.h"
#include "rt.h"
#include "vmbind.h"

#define MAX_LOADSTRING 100

// globals:
HINSTANCE hInst;								// current instance
HINSTANCE gAppInstance;
TCHAR szTitle[MAX_LOADSTRING];					// text in the window bar
TCHAR szWindowClass[MAX_LOADSTRING];			// name of main window

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void
enter_program();

#include <gl/gl.h>
static CreateDummyWindow()
{
	HWND				hWnd;

	DWORD				dwExStyle;
	DWORD				dwStyle;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW;

	hWnd = CreateWindowEx(
		dwExStyle,		// Extended Style For The Window
		"IncaVM",		// Class Name
		"IncaVM",		// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		1,	// Calculate Window Width
		1,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		gAppInstance,							// Instance
		NULL );							// Dont Pass Anything To WM_CREATE

	GLuint				pixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {			// pfd Tells Windows How We Want Things To Be
		sizeof( PIXELFORMATDESCRIPTOR ),			// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL,							// Format Must Support OpenGL
		PFD_TYPE_RGBA,								// Request An RGBA Format
		32,								// Select Our Color Depth
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
	
	HDC hDC = GetDC( hWnd );
	pixelFormat = ChoosePixelFormat( hDC, &pfd );
	if( pixelFormat == 0 )
		exit( 1 );
	SetPixelFormat( hDC, pixelFormat, &pfd );
	wglCreateContext( hDC );
	ReleaseDC( hWnd, hDC );

	AllocConsole();
	FreeConsole();

	DestroyWindow( hWnd );
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	gAppInstance = hInstance;

	CreateDummyWindow();

	rt::rt_time_init();

#if ICARUS_RT_MEMORY
	// run on a 4 MB partition
	icarus::imalloc_init( 4096L * 1024L );
#endif

	MSG				msg;
	HACCEL			hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_INCAVM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if( not IsProcessorFeaturePresent( PF_RDTSC_INSTRUCTION_AVAILABLE ) )
	{
		MessageBox( NULL,
			"You need a processor that supports the rdtsc command to run this program (for example a Pentium processor).",
			"Information", MB_OK );
		return 1;
	}

#if RUN_FROM_PIPE
	if( lpCmdLine == 0 || strcmp( lpCmdLine, "*run*" ) != 0 )
	{
		MessageBox( NULL,
			"This application is part of the Inca programming environment. It is started automatically by the Inca main application when needed. You don't have to start it manually. However, do not move or delete it.",
			"Information on IncaVM", MB_OK );
		return 1;
	}
#endif

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_INCAVM);

	enter_program();

	machine::vm_wait_for_quit();

	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_INCAVM);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_INCAVM;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance;

   /*hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);*/

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 

		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
