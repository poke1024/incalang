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
//	mouse_tracker_t.h		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef OSWRAP_MOUSE_TRACKER_H
#define OSWRAP_MOUSE_TRACKER_H

//#pragma once

#include "oswrap.h"

BEGIN_OSWRAP_NAMESPACE

class view_t;

class mouse_tracker_t {
public:
						mouse_tracker_t(
							view_t*				view );
						
	virtual				~mouse_tracker_t();

	void				start();
	
	void				stop();

	virtual bool		mouse_down(
							long				x,
							long				y,
							mouse_button_t		button );

	virtual bool		mouse_up(
							long				x,
							long				y,
							mouse_button_t		button );

	virtual bool		mouse_moved(
							long				x,
							long				y );

	mouse_tracker_t*	m_next;

private:
	bool				m_running;
	view_t*				m_view;
};

END_OSWRAP_NAMESPACE

#endif

