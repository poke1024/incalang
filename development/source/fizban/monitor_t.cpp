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
//	monitor_t.cpp		 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

// NOTE: it's important to #define WINVER 0x0500 in stdafx.h (for MFC apps)
// to make sure, that multi monitor support (like definition of MONITORINFO
// and related functions) is enabled.

#include "monitor_t.h"

BEGIN_FIZBAN_NAMESPACE

inline void
throw_directx(
	HRESULT				hr )
{
	if( hr != S_OK )
		throw_icarus_error( "DirectX error" );
}

// ---------------------------------------------------------------------------

monitor_t::monitor_t(
	info_t*				info ) :

	m_info( info )
{
	HRESULT				hr;

    // Create the DirectDraw object
    // The first NULL means use the active display driver
    // The last parameter must be NULL
	hr = DirectDrawCreate(
		info->primary ? NULL : &info->guid,
		&m_lpDD, NULL );
	throw_directx( hr );

	update_display_mode();
}

monitor_t::~monitor_t()
{
	if( m_lpDD )
	{
		( (LPDIRECTDRAW)m_lpDD )->Release();
		m_lpDD = NULL;
	}
}

long
monitor_t::scanline()
{
	HRESULT				hr;
	DWORD				scanline;

	hr = m_lpDD->GetScanLine( &scanline );
	if( hr == DDERR_VERTICALBLANKINPROGRESS )
		return -1;
	throw_directx( hr );

	return scanline;
}

long
monitor_t::frequency()
{
	HRESULT				hr;
	DWORD				freq;

	hr = m_lpDD->GetMonitorFrequency( &freq );
	throw_directx( hr );

	return freq;
}

long
monitor_t::width() const
{
	return m_surface_desc.dwWidth;
}

long
monitor_t::height() const
{
	return m_surface_desc.dwHeight;
}

LPCRECT
monitor_t::display_rect() const
{
	return &m_mi.rcMonitor; //rcWork?
}

bool
monitor_t::change_display_settings(
	LPDEVMODE			lpDevMode,
	DWORD				dwFlags )
{
	if( ChangeDisplaySettingsEx(
		m_info.device_name, lpDevMode,
		NULL, dwFlags, NULL ) != DISP_CHANGE_SUCCESSFUL )
	{
		return false;
	}

	update_display_mode();

	return true;
}

void
monitor_t::update_display_mode()
{
	HRESULT				hr;
	
	memset( &m_surface_desc, 0, sizeof( DDSURFACEDESC ) );
	m_surface_desc.dwSize = sizeof( DDSURFACEDESC );
	hr = m_lpDD->GetDisplayMode( &m_surface_desc );
	throw_directx( hr );

	memset( &m_mi, 0, sizeof( MONITORINFO ) );
	m_mi.cbSize = sizeof( MONITORINFO );
	if( !GetMonitorInfo( m_info.hm, &m_mi ) )
	{
		m_mi.dwFlags = MONITORINFOF_PRIMARY;
		SetRect( &m_mi.rcMonitor, 0, 0,
			GetSystemMetrics( SM_CXSCREEN ),
			GetSystemMetrics( SM_CYSCREEN ) );
	}
}

// ---------------------------------------------------------------------------

monitor_t::info_t::info_t() :

	driver_desc( 0 ),
	driver_name( 0 ),
	device_name( 0 )
{
}

monitor_t::info_t::info_t(
	info_t*				info )
{
	driver_desc = istrdup( info->driver_desc );
	driver_name = istrdup( info->driver_name );
	device_name = istrdup( info->device_name );
}

monitor_t::info_t::~info_t()
{
	ifree( driver_desc );
	ifree( driver_name );
	ifree( device_name );
}

// ---------------------------------------------------------------------------

typedef monitor_t::info_t info_t;

static BOOL WINAPI
MyDDEnumCallbackEx(
	GUID FAR*			lpGUID,    
	LPSTR				lpDriverDescription, 
	LPSTR				lpDriverName,        
	LPVOID				lpContext,           
	HMONITOR			hm )
{
	info_t**			link = (info_t**)lpContext;
	info_t*				info = new info_t;
	MONITORINFOEX		mix;

	memset( &mix, 0, sizeof( MONITORINFOEX ) );
	mix.cbSize = sizeof( MONITORINFOEX );
	if( !GetMonitorInfo( hm, &mix ) )
		info->device_name = NULL;
	else
		info->device_name = istrdup( mix.szDevice );

	info->hm = hm;

	if( lpGUID == NULL )
		info->primary = true;
	else
	{
		info->primary = false;
		info->guid = *lpGUID;
	}

	info->driver_desc = istrdup( lpDriverDescription );
	info->driver_name = istrdup( lpDriverName );

	info->next = *link;
	*link = info;

	return TRUE;
}

monitor_t::info_t*
monitor_t::enumerate()
{
	HRESULT				hr;
	info_t*				info = NULL;

	hr = DirectDrawEnumerateEx(
		MyDDEnumCallbackEx, &info,
		DDENUM_ATTACHEDSECONDARYDEVICES |
		DDENUM_DETACHEDSECONDARYDEVICES |
		DDENUM_NONDISPLAYDEVICES );
	throw_directx( S_OK );

	return info;
}

END_FIZBAN_NAMESPACE
