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

// this file heavily based is on code by Graham Wideman:

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
//	port_io_t.h
// ===========================================================================

#include "fizban.h"
#include <Windows.h>
#include <string>

BEGIN_FIZBAN_NAMESPACE

class port_io_t {
public:

							port_io_t();

	void					init();

	void					set_ports(
								u16					begin_port,
								u16					end_port,
								bool				enable );

	void					ioctl_iopmd_activate_kiopm();

private:
	void					open_scm();

	void					close_scm();

	bool					start_driver();

	void					install_driver();

	bool					open_device();

	DWORD					ioctl_iopmd_misc1(
								int					cmd );

	void					ioctl_iopmd_get_set_liopm(
								u16					addr,
								u08&				b,
								int					cmd );

	u08						ioctl_iopmd_get_liopmb(
								u16					addr );

	void					ioctl_iopmd_set_liopm(
								u16					addr,
								u08					b );
	
	SC_HANDLE				m_hSCManager;
	HANDLE					m_hDevice;
	//std::string				m_home_dir;

	std::string				m_driver_dir;
	std::string				m_driver_name;
	std::string				m_driver_path;
};

void
port_io_init();

void
port_io_cleanup();

void
enable_ports(
	long			from,
	long			to );

void
disable_ports(
	long			from,
	long			to );

void
activate_kiopm();

u08
port_in(
	u16				port );

void
port_out(
	u16				port,
	u08				a );

END_FIZBAN_NAMESPACE
