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
//	view_t.cp			 	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "view_t.h"
#include "mouse_tracker_t.h"
#include "os_core.h"

BEGIN_OSWRAP_NAMESPACE

view_t::view_t() :

	m_click_count( 0 ),
	m_trackers( 0 ),
	m_view( 0 )
{
	m_last_click_millis = 0;
	m_click_where.h = -0xffff;
	m_click_where.v = -0xffff;
}

view_t::~view_t()
{
}

void
view_t::pinned_scroll_by(
	long				dx,
	long				dy )
{
	if( m_view == 0 )
		return;

	int					x;
	int					y;

	x = m_view->GetScrollPos( SB_HORZ );
	m_view->SetScrollPos( SB_HORZ, x + dx );

	y = m_view->GetScrollPos( SB_VERT );
	m_view->SetScrollPos( SB_VERT, y + dy );

	request_update();
}

void
view_t::global_to_frame( 
	point&					where )
{
	point					loc = image_location();
	where.h -= loc.h;
	where.v -= loc.v;
}

void
view_t::global_to_local( 
	point&					where )
{
}

const point&
view_t::image_location() const
{
	static point			where;

	if( m_view == 0 )
		where.h = where.v = 0;
	else
	{
		where.h = m_view->GetScrollPos( SB_HORZ );
		where.v = m_view->GetScrollPos( SB_VERT );
	}

	return where;
}

const dimension&
view_t::image_size() const
{
	static dimension		size;
	int						minpos;
	int						maxpos;
	RECT					r;

	if( m_view == 0 )
		size.width = size.height = 0;
	else
	{
		m_view->GetClientRect( &r );
	
		m_view->GetScrollRange( SB_HORZ, &minpos, &maxpos );
		size.width = maxpos - minpos + r.right - r.left;

		m_view->GetScrollRange( SB_VERT, &minpos, &maxpos );
		size.height = maxpos - minpos + r.bottom - r.top;
	}

	return size;
}

void
view_t::set_image_size(
	const dimension&	size )
{
	if( m_view == 0 )
		return;

	int						maxpos;
	RECT					r;
	SCROLLINFO				scroll_info;

	m_view->GetClientRect( &r );
	
	maxpos = size.width;// - ( r.right - r.left );
	if( maxpos < 0 )
		maxpos = 0;

	scroll_info.cbSize = sizeof( SCROLLINFO );
	scroll_info.fMask = SIF_PAGE + SIF_RANGE;
	scroll_info.nMin = 0;
	scroll_info.nMax = maxpos;
	scroll_info.nPage = r.right - r.left;
	
	m_view->SetScrollInfo( SB_HORZ, &scroll_info );

	maxpos = size.height;// - ( r.bottom - r.top );
	if( maxpos < 0 )
		maxpos = 0;

	scroll_info.cbSize = sizeof( SCROLLINFO );
	scroll_info.fMask = SIF_PAGE + SIF_RANGE;
	scroll_info.nMin = 0;
	scroll_info.nMax = maxpos;
	scroll_info.nPage = r.bottom - r.top;

	if( maxpos > 0 )
		maxpos = maxpos;

	m_view->SetScrollInfo( SB_VERT, &scroll_info );
}

void
view_t::set_mfc_view(
	CView*				view )
{
	m_view = view;
}

CView*
view_t::mfc_view()
{
	return m_view;
}

const rect&
view_t::bounds()
{
	static rect			r;
	RECT				wr;

	m_view->GetClientRect( &wr );
	r.left = wr.left;
	r.top = wr.top;
	r.right = wr.right;
	r.bottom = wr.bottom;

	return r;
}

void
view_t::request_update(
	const rect*			r )
{
	if( m_view == 0 )
		return;

	RECT				wr;

	if( r == 0 )
	{
		m_view->GetClientRect( &wr );
		m_view->InvalidateRect( &wr, FALSE );
	}
	else
	{
		wr.left = r->left;
		wr.top = r->top;
		wr.bottom = r->bottom + 1;
		wr.right = r->right + 1;
		m_view->InvalidateRect( &wr, FALSE );
	}
}

bool
view_t::is_activated() const
{
	return true;
}

bool
view_t::on_duty() const
{
	return CWnd::GetFocus() == (CWnd*)m_view;
	//return m_view->IsWindowEnabled();
	//return CWnd::GetActiveWindow() == (CWnd*)m_view;
}

void
view_t::set_click_count(
	int					click_count )
{
	m_click_count = click_count;
}

void
view_t::draw(
	grafport_t*			port )
{
	draw_self( port );
}

void
view_t::activate()
{
	activate_self();
}

void
view_t::mouse_down(
	long				x,
	long				y,
	mouse_button_t		button )
{
	u64					now_millis;

	now_millis = os_time_millis();

	if( abs( x - m_click_where.h ) < 4 &&
		abs( y - m_click_where.v ) < 4 &&
		now_millis - m_last_click_millis <
		os_double_click_millis() )
	{
		m_click_count += 1;
	}
	else
		m_click_count = 1;

	m_last_click_millis = now_millis;
	m_click_where.h = x;
	m_click_where.v = y;

	mouse_down_self( x, y, button );
}

void
view_t::mouse_up(
	long				x,
	long				y,
	mouse_button_t		button )
{
	mouse_tracker_t*	node = m_trackers;
	mouse_tracker_t*	next;
	
	while( node )
	{
		next = node->m_next;
		node->mouse_up( x, y, button );
		node = next;
	}
}

void
view_t::mouse_dragged(
	long				x,
	long				y )
{
	mouse_tracker_t*	node = m_trackers;
	mouse_tracker_t*	next;
	
	while( node )
	{
		next = node->m_next;
		node->mouse_moved( x, y );
		node = next;
	}
}

void
view_t::key_down(
	char				c,
	int					modifiers )
{
	key_down_self( c, modifiers );
}

bool
view_t::process_command(
	command_t			command,
	void*				io_param )
{
	return process_command_self( command, io_param );
}

bool
view_t::command_status(
	command_t			command,
	command_status_t&	status )
{
	return command_status_self( command, status );
}

void
view_t::add_mouse_tracker(
	mouse_tracker_t*	tracker )
{
	tracker->m_next = m_trackers;
	m_trackers = tracker;
}

void
view_t::remove_mouse_tracker(
	mouse_tracker_t*	tracker )
{
	mouse_tracker_t**	link = &m_trackers;
	mouse_tracker_t*	node = *link;

	while( node )
	{
		if( node == tracker )
		{
			*link = node->m_next;
			break;
		}
	
		link = &node->m_next;
		node = *link;
	}
}

int
view_t::click_count() const
{
	return m_click_count;
}

END_OSWRAP_NAMESPACE
