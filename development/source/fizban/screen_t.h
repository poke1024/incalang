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
//	screen_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "monitor_t.h"
#include "gl_context_t.h"

BEGIN_FIZBAN_NAMESPACE

class font_t;

class screen_t {
public:
							screen_t();

	virtual					~screen_t();

	void					open(
								monitor_t*			monitor );

	void					close();

	void					clear();

	void					swap();

	void					make_current();

	void					capture_mouse();

	void					set_display_size(
								int					width,
								int					height );

	void					set_fullscreen(
								bool				fullscreen );

	void					set_doublebuffer(
								bool				doublebuffer );

	void					set_2d_only(
								bool				flag );

	void					set_title(
								const char*			title );

	HDC						hdc() const;

	long					width() const;

	long					height() const;

	bool					fetch_resize_flag();

	long					mouse_x() const;

	long					mouse_y() const;

	long					mouse_z() const;

	bool					mouse_button(
								int					index ) const;

	static screen_t*		current();

protected:
	virtual void			resize_gl(
								GLsizei				width,
								GLsizei				height );
	
	virtual void			create_window(
								monitor_t*			monitor );

	virtual void			kill_window();

	void					init_fences();

	void					cleanup_fences();

	static void				register_window_class();

	static LRESULT CALLBACK wnd_proc(
								HWND				hWnd,
								UINT				uMsg,
								WPARAM				wParam,
								LPARAM				lParam );

	void					position_text(
								float				x,
								float				y );

	void					mouse_button_change(
								int					index,
								bool				down,
								LPARAM				lParam );

	gl_context_t			m_gl_context;
	HWND					m_hWnd;
	bool					m_active;
	bool					m_2d_only;

	istring					m_title;
	int						m_display_width;
	int						m_display_height;
	int						m_display_bits;
	bool					m_resized;

	bool					m_display_fullscreen;
	bool					m_display_doublebuffer;
	int						m_display_frequency;

	point					m_mouse_location;
	s32						m_mouse_z;
	bool					m_mouse_button[ 8 ];

	GLuint					m_fence;
	bool					m_has_fence;
};

END_FIZBAN_NAMESPACE
