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
//	input_t.cpp				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "input_t.h"
#include "rt.h"
#include "vmbind.h"

BEGIN_FIZBAN_NAMESPACE

#define rt_fatal throw_icarus_error

// ---------------------------------------------------------------------------

input_device_t::input_device_t(
	LPDIRECTINPUT8		lpdi ) :

	m_device( 0 )
{
}

input_device_t::~input_device_t()
{
	if( m_device )
	{
		m_device->Unacquire();
		m_device->Release();
		m_device = NULL;
	}
}

// ---------------------------------------------------------------------------

mouse_device_t::mouse_device_t(
	LPDIRECTINPUT8		lpdi,
	HWND				hWnd ) :
	input_device_t( lpdi )
{
	// initialize the mouse
	if( FAILED( lpdi->CreateDevice( GUID_SysMouse, &m_device, NULL ) ) )
		rt_fatal( "could not create mouse device" );
	if( FAILED( m_device->SetCooperativeLevel(
		hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE ) ) )
		rt_fatal( "could not set cooperative level for mouse" );
	if( FAILED( m_device->SetDataFormat( &c_dfDIMouse ) ) )
		rt_fatal( "could not set data format for mouse" );
	if( FAILED( m_device->Acquire() ) )
		rt_fatal( "could not acquire mouse" );

	memset( &m_state, 0, sizeof( m_state ) );
}

void
mouse_device_t::update()
{
	if( FAILED( m_device->GetDeviceState(
		sizeof( DIMOUSESTATE ), (LPVOID)&m_state ) ) )
	{
		rt_fatal( "could not update mouse status" );
	}
}

// ---------------------------------------------------------------------------

keyboard_device_t::keyboard_device_t(
	LPDIRECTINPUT8		lpdi,
	HWND				hWnd) :
	input_device_t( lpdi )
{
	// initialize the keyboard
	if( FAILED( lpdi->CreateDevice( GUID_SysKeyboard, &m_device, NULL) ) )
		rt_fatal( "could not create keyboard device" );
	if( FAILED( m_device->SetDataFormat( &c_dfDIKeyboard ) ) )
		rt_fatal( "could not set data format for keyboard" );
	if( FAILED( m_device->SetCooperativeLevel(
		hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE ) ) )
		rt_fatal( "could not set cooperative level for keyboard" );
	if( FAILED( m_device->Acquire() ) )
		rt_fatal( "could not acquire keyboard" );

	memset( m_state, 0, sizeof( m_state ) );
}

void
keyboard_device_t::update()
{
	if( FAILED( m_device->GetDeviceState(
		sizeof( UCHAR[ 256 ] ), (LPVOID)m_state ) ) )
	{
		rt_fatal( "could not update keyboard status" );
	}
}

void
keyboard_device_t::flush()
{
	const int			BUFFERCOUNT = 10;
	DIDEVICEOBJECTDATA	kbdBuffer[ BUFFERCOUNT ];  

	HRESULT				hres;
	DWORD				dwItems;

	while( true )
	{
		dwItems = BUFFERCOUNT;
		hres = m_device->GetDeviceData(
			sizeof( DIDEVICEOBJECTDATA ),
			kbdBuffer, &dwItems, 0 );

		if( hres != DI_BUFFEROVERFLOW )
			break;
	}
}

// ---------------------------------------------------------------------------

LPDIRECTINPUT8			s_lpdi = NULL;
mouse_device_t*			s_system_mouse = NULL;
keyboard_device_t*		s_system_keyboard = NULL;
long					s_input_enabled = 1;

void
rt_input_init(
	HWND				hWnd )
{
	if( s_lpdi == NULL )
	{
		// initialize interface
		if( FAILED( DirectInput8Create(
			GetModuleHandle( NULL ),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&s_lpdi, NULL ) ) )
		{
			rt_fatal( "couldn't create Direct Input Interface" );
		}

		s_system_mouse = new mouse_device_t( s_lpdi, hWnd );
		s_system_keyboard = new keyboard_device_t( s_lpdi, hWnd );
		s_input_enabled = 1;
	}
}

void
rt_input_cleanup()
{
	if( s_system_mouse )
	{
		delete s_system_mouse;
		s_system_mouse = NULL;
	}

	if( s_system_keyboard )
	{
		delete s_system_keyboard;
		s_system_keyboard = NULL;
	}

	if( s_lpdi )
	{
		s_lpdi->Release();
		s_lpdi = NULL;
	}
}

void
rt_input_enable()
{
	s_input_enabled += 1;
}

void
rt_input_disable()
{
	s_input_enabled -= 1;
}

void
rt_input_update()
{
	if( s_input_enabled < 1 )
		return;

	if( s_lpdi == 0 )
		return;

	bool				rt = machine::vm_in_real_time_mode();

	HANDLE				process;

	s_system_mouse->update();
	s_system_keyboard->update();

	if( rt )
	{
		process = GetCurrentProcess();

		SetPriorityClass( process,
			HIGH_PRIORITY_CLASS );
		SetPriorityClass( process,
			REALTIME_PRIORITY_CLASS );
	}
}

bool
rt_key_down(
	int					key )
{
	if( s_system_keyboard == 0 )
		return false;
	return s_system_keyboard->key_down( key );
}

long
rt_mouse_x()
{
	if( s_system_mouse == 0 )
		return 0;
	return s_system_mouse->x();
}

long
rt_mouse_y()
{
	if( s_system_mouse == 0 )
		return 0;
	return s_system_mouse->y();
}

long
rt_mouse_z()
{
	if( s_system_mouse == 0 )
		return 0;
	return s_system_mouse->z();
}

bool
rt_mouse_button(
	int					button )
{
	if( s_system_mouse == 0 )
		return false;
	return s_system_mouse->button( button );
}

END_FIZBAN_NAMESPACE
