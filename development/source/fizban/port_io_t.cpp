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

// this file is heavily based on code by Graham Wideman:

// Functions for interacting with gwiopm I/O-permissions-map "driver",
// in order to give cpu I/O instructions permission to operate
// via the I/O permissions mechanism under Win NT.

// Revisions
// ---------
// 98-05-23 GW original

// Copyright Graham Wideman
// ------------------------
// This module is distributed as freeware, and may be freely used for any purpose.
// I would appreciate a credit notice if this is useful in your work. Thanks.

// Note that this work was greatly aided by demo code from:
// Dale Roberts      (giveio.sys)
// Paula Tomlinson   (LOADDRV)

// ===========================================================================
//	port_io_t.cpp
// ===========================================================================

#include "port_io_t.h"
#include <WinSvc.h>
#include <direct.h>

#define DEVICE_NAME_STRING		"gwiopm"

#define	IOPMD_TYPE				0xF100

// Manipulate driver's local IOPM (LIOPM)
#define IOCMD_IOPMD_SET_LIOPM	0x911
#define IOCMD_IOPMD_GET_LIOPMB	0x912

// Interact with kernel IOPM (KIOPM)
#define IOCMD_IOPMD_ACTIVATE_KIOPM		0x920
#define IOCMD_IOPMD_DEACTIVATE_KIOPM	0x921

#define GWIO_PARAMCOUNT			3
#define GWIO_BYTES_IN			( GWIO_PARAMCOUNT * 4 )
#define GWIO_BYTES_OUT			( GWIO_PARAMCOUNT * 4 )

typedef long TGWIO_PARAMS[ GWIO_PARAMCOUNT ];

BEGIN_FIZBAN_NAMESPACE

port_io_t::port_io_t()
{
	//char buffer[ _MAX_PATH ];
	//_getcwd( buffer, _MAX_PATH );

	m_hSCManager = NULL;
	m_hDevice = INVALID_HANDLE_VALUE;
	//m_home_dir = buffer;
	m_driver_name = DEVICE_NAME_STRING;
	// default driver name needed by stop/remove if install
	// wasn't executed this run (ie: driver already installed)
}

void
port_io_t::init()
{
	open_scm();

	for( int i = 0; i < 2; i++ )
	{
		if( start_driver() )
		{
			if( open_device() )
				break;
		}
		if( i == 0 )
			install_driver();
		else
			icarus_fatal( "couldn't open gwiopm.sys device" );
				//(%ld)", (long)GetLastError() );
	}
}

void
port_io_t::open_scm()
{
	m_hSCManager = OpenSCManager(
		NULL, NULL, SC_MANAGER_ALL_ACCESS );
	
	if( m_hSCManager == 0 )
		icarus_fatal( "couldn't open SC Manager" );
}

void
port_io_t::close_scm()
{
	CloseServiceHandle( m_hSCManager );
	m_hSCManager = 0;
}

void
port_io_t::install_driver()
{
	SC_HANDLE			hService;
	std::string			driver_path;

	char				buffer[ _MAX_PATH ];
	
	/*if( _getcwd( buffer, _MAX_PATH ) == NULL )
		icarus_fatal( "_getcwd error" );
	
	driver_path = buffer;*/

	if( GetWindowsDirectory( buffer, _MAX_PATH ) == 0 )
		icarus_fatal( "could not get Windows directory" );

	driver_path = buffer;
	driver_path += "\\";
	driver_path += m_driver_name.c_str();
	driver_path += ".sys";

	// add to service control manager's database
	hService = CreateService(
		m_hSCManager,
		m_driver_name.c_str(),
		m_driver_name.c_str(),
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		driver_path.c_str(),
		NULL, NULL, NULL, NULL, NULL );

	if( hService == 0 )
		icarus_fatal( "couldn't create service" );
			//(%ld)", (long)GetLastError() );

	CloseServiceHandle( hService );
}

bool
port_io_t::start_driver()
{
	LPCTSTR				lpServiceArgVectors = NULL;
	SC_HANDLE			hService;

	// get a handle to the service
	hService = OpenService(
		m_hSCManager, m_driver_name.c_str(),
		SERVICE_ALL_ACCESS );

	if( hService == 0 )
		return false;

	StartService(
		hService, 0, &lpServiceArgVectors );
	//icarus_fatal( "could not start service" );

	if( hService != NULL )
		CloseServiceHandle( hService );

	return true;
}

bool
port_io_t::open_device()
{
	if( m_hDevice != INVALID_HANDLE_VALUE )
		return false;

	std::string			name;

	name = "\\\\.\\";
	name += DEVICE_NAME_STRING;

	m_hDevice = CreateFile(
		name.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0 );

	if( m_hDevice == INVALID_HANDLE_VALUE )
		return false;

	return true;
}

DWORD
port_io_t::ioctl_iopmd_misc1(
	int					cmd )
{
	TGWIO_PARAMS		in_buffer;
	TGWIO_PARAMS		out_buffer;
	DWORD				control_code;
	DWORD				bytes_returned;

	in_buffer[ 0 ] = 0;
	in_buffer[ 1 ] = 0;
	in_buffer[ 2 ] = 0;
	
	if( m_hDevice == INVALID_HANDLE_VALUE )
		icarus_fatal( "no valid device handle" );

    control_code = CTL_CODE(
		IOPMD_TYPE, cmd, METHOD_BUFFERED, FILE_ANY_ACCESS );

    bytes_returned = 0;
	if( !DeviceIoControl(
		m_hDevice, control_code ,
		in_buffer, GWIO_BYTES_IN,
		out_buffer, GWIO_BYTES_OUT,
		&bytes_returned, NULL ) )
	{
		icarus_fatal( "DeviceIoControl failed" );
	}

	return out_buffer[ 0 ];
}

void
port_io_t::ioctl_iopmd_activate_kiopm()
{
	DWORD				ret;

	ret = ioctl_iopmd_misc1( IOCMD_IOPMD_ACTIVATE_KIOPM );
	if( ret != ERROR_SUCCESS )
		icarus_fatal( "activate kiopm failed" );
}

void
port_io_t::ioctl_iopmd_get_set_liopm(
	u16					addr,
	u08&				b,
	int					cmd )
{
	if( m_hDevice == INVALID_HANDLE_VALUE )
		icarus_fatal( "no device open" );

	DWORD				control_code;
	TGWIO_PARAMS		in_buffer;
	TGWIO_PARAMS		out_buffer;
	DWORD				bytes_returned;

	control_code =
		CTL_CODE( IOPMD_TYPE, cmd,
		METHOD_BUFFERED, FILE_ANY_ACCESS );

    in_buffer[ 0 ] = addr;
    in_buffer[ 1 ] = b;

	bytes_returned = 0;
	
	if( !DeviceIoControl(
		m_hDevice, control_code ,
		in_buffer,  GWIO_BYTES_IN,
		out_buffer, GWIO_BYTES_OUT,
		&bytes_returned, NULL ) )
	{
		icarus_fatal( "DeviceIoControl did not succeed" );
	}

	b = out_buffer[ 1 ];
}

u08
port_io_t::ioctl_iopmd_get_liopmb(
	u16					addr )
{
	u08					b;

	ioctl_iopmd_get_set_liopm(
		addr, b, IOCMD_IOPMD_GET_LIOPMB );

	return b;
}

void
port_io_t::ioctl_iopmd_set_liopm(
	u16					addr,
	u08					b )
{
	ioctl_iopmd_get_set_liopm(
		addr, b, IOCMD_IOPMD_SET_LIOPM );
}

void
port_io_t::set_ports(
	u16					begin_port,
	u16					end_port,
	bool				enable )
{
	int					IOPM_index;
	int					IOPM_bit_num;
	u08					IOPM_byte = 0xff;
	u08					mask_byte;

	for( u16 port_num = begin_port;
		port_num <= end_port; port_num++ )
	{
		IOPM_index = port_num >> 3;
		IOPM_bit_num = port_num & 7;
		mask_byte = 1 << IOPM_bit_num;

		if( port_num == begin_port ||
			IOPM_bit_num == 0 )
		{
			IOPM_byte = ioctl_iopmd_get_liopmb(
				IOPM_index );
		}
	
		if( enable )
			IOPM_byte &= 0xff ^ mask_byte;
		else
			IOPM_byte |= mask_byte;

		if( port_num == end_port ||
			IOPM_bit_num == 7 )
		{
			ioctl_iopmd_set_liopm(
				IOPM_index, IOPM_byte );
		}
	}
}

// ---------------------------------------------------------------------------

static port_io_t*		s_io = NULL;

void
port_io_init()
{
	if( !s_io )
	{
		s_io = new port_io_t;
		s_io->init();

		//enable_ports( 0x42, 0x43 );
		//enable_ports( 0x61, 0x61 );
		activate_kiopm();
	}
}

void
port_io_cleanup()
{
}

void
enable_ports(
	long			from,
	long			to )
{
	if( s_io )
		s_io->set_ports( from, to, true );
}

void
disable_ports(
	long			from,
	long			to )
{
	if( s_io )
		s_io->set_ports( from, to, false );
}

void
activate_kiopm()
{
	if( s_io )
		s_io->ioctl_iopmd_activate_kiopm();
}

// ---------------------------------------------------------------------------

u08
port_in(
	u16				port )
{
	u08				a;

	_asm {
		push dx
		mov dx,port
		in al,dx
		pop dx
		mov a,al
	}
	return a;
}

void
port_out(
	u16				port,
	u08				a )
{
	_asm {
		push dx
		push ax
		mov dx,port
		mov al,a
		out dx,al
		pop ax
		pop dx
	}
}

END_FIZBAN_NAMESPACE
