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
//	monitor_t.h		 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include <ddraw.h>
#include "fizban.h"

BEGIN_FIZBAN_NAMESPACE

class monitor_t {
public:
	struct info_t {
								info_t();

								info_t(
									info_t*				info );

								~info_t();

		info_t*					next;
		bool					primary;
		GUID					guid;
		HMONITOR				hm;
		char*					driver_desc;
		char*					driver_name;
		char*					device_name;
	};

								monitor_t(
									info_t*				info );

								~monitor_t();

	long						scanline();

	long						frequency();

	long						width() const;

	long						height() const;

	bool						change_display_settings(
									LPDEVMODE			lpDevMode,
									DWORD				dwFlags );

	LPCRECT						display_rect() const;

	static info_t*				enumerate();

private:
	void						update_display_mode();

	LPDIRECTDRAW				m_lpDD;
	DDSURFACEDESC				m_surface_desc;
	info_t						m_info;
	MONITORINFO					m_mi;
};

END_FIZBAN_NAMESPACE
