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
//	mouse_tracker_t.cp		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "mouse_tracker_t.h"
#include "view_t.h"

BEGIN_OSWRAP_NAMESPACE

mouse_tracker_t::mouse_tracker_t(
	view_t*				view ) :
	
	m_view( view ),
	m_running( false )
{
}

mouse_tracker_t::~mouse_tracker_t()
{
	stop();
}

void
mouse_tracker_t::start()
{
	stop();
	
	m_view->add_mouse_tracker( this );
	
	m_running = true;
}

void
mouse_tracker_t::stop()
{
	if( not m_running )
		return;
		
	m_view->remove_mouse_tracker( this );
		
	m_running = false;
}

bool
mouse_tracker_t::mouse_down(
	long				/*x*/,
	long				/*y*/,
	mouse_button_t		/*button*/ )
{
	return false;
}

bool
mouse_tracker_t::mouse_up(
	long				/*x*/,
	long				/*y*/,
	mouse_button_t		/*button*/ )
{
	return false;
}

bool
mouse_tracker_t::mouse_moved(
	long				/*x*/,
	long				/*y*/ )
{
	return false;
}

END_OSWRAP_NAMESPACE
