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
//	code_select_t.cp	   	  	   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_select_t.h"
#include "code_view_t.h"
#include "os_core.h"

BEGIN_AKLABETH_NAMESPACE

autoscroller_t::autoscroller_t(
	code_view_t*		view,
	code_select_t*		dragger ) :

	m_view( view ),
	m_dragger( dragger )
{
	set_frequency( 50 );
	m_speed = 50;
}

void
autoscroller_t::start(
	long				dx,
	long				dy,
	long				x,
	long				y )
{
	m_where.h = x;
	m_where.v = y;
	m_dx = dx;
	m_dy = dy;
	timer_t::start();

	m_last_millis = os_time_millis();
	timer_t::start();
}

void
autoscroller_t::tick()
{
	u64					millis = os_time_millis();
	long				passed = millis - m_last_millis;
	float				m = ( m_speed * passed ) / 1000.0;

	m_view->pinned_scroll_by( m_dx * m, m_dy * m );
	
	m_dragger->update_selection( m_where.h, m_where.v );

	m_last_millis = millis;
}

// ---------------------------------------------------------------------------

code_select_t::code_select_t(
	code_view_t*		view,
	long				x,
	long				y,
	mouse_button_t		button ) :

	mouse_tracker_t( view ),
	m_view( view ),
	m_autoscroller( view, this ),
	m_button( button )
{
	const long			click_count = view->click_count();

	codepos_t			caret = m_view->point_to_offset( x, y );
	codepos_t			selfrom = caret;
	codepos_t			selto = caret;
		
	if( click_count == 2 )
		m_extend_mode = extend_mode_word;
	else if( click_count >= 3 )
		m_extend_mode = extend_mode_line;
	else
		m_extend_mode = extend_mode_none;
		
	if( x < m_view->left_margin() )
		m_extend_mode = extend_mode_line;
		
	m_view->extend_selection( selfrom, selto, m_extend_mode );

	m_view->set_selection( selfrom, selto );

	if( compare_codepos( selfrom, selto ) == 0 )
		m_view->caret_column_changed( selfrom );

	m_left_anchor = selfrom;
	m_right_anchor = selto;
}

bool
code_select_t::mouse_up(
	long				/*x*/,
	long				/*y*/,
	mouse_button_t		button )
{
	if( button != m_button )
		return false;

	delete this;
	
	return false;
}

bool
code_select_t::mouse_moved(
	long				x,
	long				y )
{
	update_selection( x, y );
	
	// auto scrolling
	point				where;
	
	where.set( x, y );
	m_view->global_to_frame( where );
	
	int					dx = 0;
	int					dy = 0;
	const rect&			r = m_view->bounds();
	
	const point&		dp = m_view->autoscroll_deltas();
	
	if( where.h < 0 )
		dx = -dp.h;
	else if( where.h > r.width() )
		dx = dp.h;
	if( where.v < 0 )
		dy = -dp.v;
	else if( where.v > r.height() )
		dy = dp.v;
	
	if( dx || dy )
		m_autoscroller.start( dx, dy, x, y );
	else
		m_autoscroller.stop();

	return false;
}

void
code_select_t::update_selection(
	long				x,
	long				y )
{
	point				where;
	codepos_t			offset;

	where.set( x, y );
	m_view->global_to_local( where );
	offset = m_view->point_to_offset( where.h, where.v );

	codepos_t			selfrom;
	codepos_t			selto;

	if( compare_codepos( m_left_anchor, offset ) < 0 )
		selfrom = m_left_anchor;
	else
		selfrom = offset;

	if( compare_codepos( m_right_anchor, offset ) > 0 )
		selto = m_right_anchor;
	else
		selto = offset;

	m_view->extend_selection(
		selfrom, selto, m_extend_mode );
		
	m_view->set_selection( selfrom, selto );

	if( compare_codepos( selfrom, selto ) == 0 )
		m_view->caret_column_changed( selfrom );
}

END_AKLABETH_NAMESPACE
