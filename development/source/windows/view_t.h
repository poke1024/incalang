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
//	view_t.h			 	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef OSWRAP_VIEW_H
#define OSWRAP_VIEW_H

//#pragma once

#include "oswrap.h"
#include "qd_grafport_t.h"
#include "array_t.h"

#if __VISC__
#include <afxwin.h>
#else
#include <Windows.h>
#endif

BEGIN_OSWRAP_NAMESPACE

class view_t {
public:
							view_t();
	
	virtual					~view_t();

	const point&			image_location() const;
	
	void					pinned_scroll_by(
								long				dx,
								long				dy );

	void					scroll_to(
								long				dx,
								long				dy );

	const dimension&		image_size() const;

	void					set_image_size(
								const dimension&	size );

	void					global_to_frame( 
								point&					where );

	void					global_to_local(
								point&					where );

							// === pane methods ===
								
	const rect&				bounds();
	
	void					request_update(
								const rect*			r = 0 );	

	bool					is_activated() const;

	bool					on_duty() const;

							// == misc methods ==

	void					draw(
								grafport_t*			port );

	void					mouse_down(
								long				x,
								long				y,
								mouse_button_t		button );

	void					mouse_up(
								long				x,
								long				y,
								mouse_button_t		button );

	void					mouse_dragged(
								long				x,
								long				y );

	void					key_down(
								char				c,
								int					modifiers );

	bool					process_command(
								command_t			command,
								void*				io_param );

	bool					command_status(
								command_t			command,
								command_status_t&	status );

	int						click_count() const;

	void					activate();

							// == internal methods ==

	void					set_mfc_view(
								CView*				view );

	CView*					mfc_view();

	void					set_click_count(
								int					click_count );

protected:
	virtual void			draw_self(
								grafport_t*			port ) = 0;

	virtual bool			mouse_down_self(
								long				x,
								long				y,
								mouse_button_t		button ) = 0;

	virtual bool			key_down_self(
								char				c,
								int					modifiers ) = 0;

	virtual bool			process_command_self(
								command_t			command,
								void*				io_param ) = 0;

	virtual bool			command_status_self(
								command_t			command,
								command_status_t&	status ) = 0;

	virtual void			activate_self() = 0;

	friend class			mouse_tracker_t;
	
	void					add_mouse_tracker(
								mouse_tracker_t*	tracker );

	void					remove_mouse_tracker(
								mouse_tracker_t*	tracker );

private:
	CView*					m_view;
	mouse_tracker_t*		m_trackers;

	int						m_click_count;
	u64						m_last_click_millis;
	point					m_click_where;
};

END_OSWRAP_NAMESPACE

#endif

