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
//	rt_time.cpp			 	   ©2000-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "rt.h"
#include <Windows.h>

#if RT_WINDOWS_TIME

BEGIN_RT_NAMESPACE

static u64				s_cycles_per_second = 0;

void
rt_time_init()
{
	LARGE_INTEGER		freq;

	if( QueryPerformanceFrequency( &freq ) )
		s_cycles_per_second = freq.QuadPart;
}

void
rt_time_cleanup()
{
}

u64
rt_cycles_per_second()
{
	return s_cycles_per_second;
}

u64
rt_micros_to_cycles(
	u64					micros )
{
	return ( micros * s_cycles_per_second ) / 1000000;
}

u64
rt_cycles_to_micros(
	u64					cycles )
{
	return ( cycles * 1000000 ) / s_cycles_per_second;
}

u64
rt_micros()
{
	return ( rt_cycles() * 1000000L ) / s_cycles_per_second;
}

u64
rt_nanos()
{
	return ( rt_cycles() * 1000000000L ) / s_cycles_per_second;
}

u64
rt_cycles()
{
	LARGE_INTEGER		count;

	if( !QueryPerformanceCounter( &count ) )
		return 0;

	return count.QuadPart;
}

END_RT_NAMESPACE

#else // not RT_WINDOWS_TIME

static const char*		g_app_name = "IncaVirtualMachine";
extern HINSTANCE		gAppInstance;
const int				IDD_MEASURECYCLES = 102;
const int				IDC_CLOCKRATE = 1000;

BEGIN_RT_NAMESPACE

void
setup_cycles_per_second();

u64						g_dialog_setup = 0;

BOOL CALLBACK
DialogProc(
	HWND				hwndDlg,
	UINT				message,
	WPARAM				wParam,
	LPARAM				lParam ) 
{ 
	switch( message ) 
	{ 
		case WM_ENTERIDLE:
			g_dialog_setup = GetTickCount();
			break;

		case WM_PAINT:
			break;
	}
	
	return FALSE; 
}

// ===========================================================================

static u64				s_cycles_per_second = 0;

u64
rt_cycles_per_second()
{
	return s_cycles_per_second;
}

u64
rt_micros_to_cycles(
	u64					micros )
{
	return ( micros * s_cycles_per_second ) / 1000000;
}

u64
rt_cycles_to_micros(
	u64					cycles )
{
	return ( cycles * 1000000 ) / s_cycles_per_second;
}

static u64
measure_cycles_per_second()
{
	HANDLE				current_process;
	HANDLE				current_thread;

	DWORD				old_class;
	DWORD				old_prio;

	current_process = GetCurrentProcess();
	current_thread = GetCurrentThread();

	old_class = GetPriorityClass( current_process );
	old_prio = GetThreadPriority( current_thread );

	SetPriorityClass( current_process, REALTIME_PRIORITY_CLASS );
	SetThreadPriority( current_thread, THREAD_PRIORITY_TIME_CRITICAL );

	u64					zero_rdtsc;
	LARGE_INTEGER		zero_counter;
	LARGE_INTEGER		counter;
	LARGE_INTEGER		frequency;
	u64					delay;

	QueryPerformanceFrequency( &frequency );
	delay = frequency.QuadPart;

	QueryPerformanceCounter( &zero_counter );
	zero_rdtsc = rt_cycles();

	do {
		QueryPerformanceCounter( &counter );
	} while( counter.QuadPart - zero_counter.QuadPart < delay );

	u64					ticks = rt_cycles() - zero_rdtsc;

	SetPriorityClass( current_process, old_class );
	SetThreadPriority( current_thread, old_prio );

	return ticks;
}

void
setup_cycles_per_second()
{
	char				s[ 128 ];

	u64					val0, val1;
	int					cnt = 0;

	val0 = measure_cycles_per_second();

	while( true )
	{
		val1 = measure_cycles_per_second();

		// leave, if the difference between two
		// samples is less than 1 ppm

		// this means that our cycle count might be
		// wrong by 0.0001%, indicating a worst case
		// time error of 1 second * 0.0001%, which is
		// about one microsecond.

		if( labs( val0 - val1 ) < val0 / 1000000L )
			break;
		
		iswap( val0, val1 );

		if( ++cnt > 10 )
			exit( 1 );
	}

	s_cycles_per_second = ( val0 + val1 ) / 2L;
}

void
rt_time_init()
{
	if( s_cycles_per_second )
		return;

	char				buf[ 64 + 1 ];

	GetProfileString( g_app_name,
		"CyclesPerSecond",
		"0", buf, 64 );

	s_cycles_per_second = atol( buf );

	if( GetAsyncKeyState( VK_SHIFT ) >> 15 )
		;
	else if( s_cycles_per_second > 0 )
		return;

	HWND				hWnd;

	g_dialog_setup = 0;
	hWnd = CreateDialog( gAppInstance,
		MAKEINTRESOURCE( IDD_MEASURECYCLES ),
		HWND_DESKTOP, (DLGPROC)DialogProc );
	SetDlgItemText( hWnd, IDC_CLOCKRATE, "" );
	ShowWindow( hWnd, SW_SHOW );
	PostMessage( hWnd, WM_ENTERIDLE, 0, 0 );

	MSG					msg;
	u64					done = 0;
	
	while( true ) 
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if( !IsWindow( hWnd )||
				!IsDialogMessage( hWnd, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}

		if( done == 0 && g_dialog_setup &&
			GetTickCount() - g_dialog_setup > 100 )
		{
			setup_cycles_per_second();

			_snprintf( buf, 64, "%.3lf MHz",
				(double)( s_cycles_per_second / (double)1000000.0 ) );
			SetDlgItemText( hWnd, IDC_CLOCKRATE, buf );
			done = GetTickCount();
		}

		if( done && ( GetTickCount() - done > 5000 ||
			GetAsyncKeyState( VK_LBUTTON ) ) )
		{
			break;
		}
	}

	DestroyWindow( hWnd );

	//s_cycles_per_second = 1675226965;

	sprintf( buf, "%I64d", s_cycles_per_second );
	WriteProfileString( g_app_name, "CyclesPerSecond", buf );
}

void
rt_time_cleanup()
{
}

u64
rt_micros()
{
	return ( rt_cycles() * 1000000L ) / s_cycles_per_second;
}

u64
rt_nanos()
{
	return ( rt_cycles() * 1000000000L ) / s_cycles_per_second;
}

END_RT_NAMESPACE

#endif // end RT_WINDOWS_TIME
