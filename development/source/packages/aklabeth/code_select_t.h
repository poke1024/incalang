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
//	code_select_t.cp	  	   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "timer_t.h"
#include "mouse_tracker_t.h"
#include "code_view_t.h"

BEGIN_AKLABETH_NAMESPACE

enum {
	extend_mode_none,
	extend_mode_word,
	extend_mode_line
};

class code_select_t;

class autoscroller_t : public timer_t {
public:
						autoscroller_t(
							code_view_t*		view,
							code_select_t*		dragger );
							
	void				start(
							long				dx,
							long				dy,
							long				x,
							long				y );

protected:
	virtual void		tick();

	code_view_t*	m_view;
	code_select_t*		m_dragger;
	point				m_where;
	long				m_dx;
	long				m_dy;

	f32					m_speed;
	u64					m_last_millis;
};

class code_select_t : public mouse_tracker_t {
public:
						code_select_t(
							code_view_t*	view,
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
							
	void				update_selection(
							long				x,
							long				y );

protected:
	code_view_t*	m_view;
	codepos_t			m_left_anchor;
	codepos_t			m_right_anchor;
	autoscroller_t		m_autoscroller;
	mouse_button_t		m_button;
	int					m_extend_mode;
};

END_AKLABETH_NAMESPACE
