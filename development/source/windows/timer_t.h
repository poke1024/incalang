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
//	timer_t.h			 	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef OSWRAP_TIMER_H
#define OSWRAP_TIMER_H

//#pragma once

#if __VISC__
#include <afxwin.h>
#else
#include <Windows.h>
#endif

#include "oswrap.h"

BEGIN_OSWRAP_NAMESPACE

class timer_t {
public:
							timer_t();

	virtual					~timer_t();

	void					set_frequency(
								long				millis );

	long					frequency() const;

	void					start();
	
	void					stop();
	
	bool					running() const;

	virtual void			tick() = 0;

	void					restart();

	UINT					m_timer;

private:
	long					m_millis;
};

END_OSWRAP_NAMESPACE

#endif

