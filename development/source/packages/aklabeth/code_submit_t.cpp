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
//	code_submit_t.cp	 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_submit_t.h"

BEGIN_AKLABETH_NAMESPACE

code_submit_t::code_submit_t(
	code_view_t*		view ) :
	
	m_view( view )
{
	m_view->get_selection(
		m_selfrom, m_selto );

	codepos_t			pos;
	
	pos.offset = 0;
	pos.line = (u32)-1;

	m_view->set_selection(
		pos, pos );
		
	m_view->submit_focus();
}

code_submit_t::~code_submit_t()
{
	release();
}

void
code_submit_t::release()
{
	if( m_view )
		m_view->set_selection(
			m_selfrom, m_selto );

	m_view = 0;
}

void
code_submit_t::patch(
	const codepos_t&	from,
	const codepos_t&	to )
{
	m_selfrom = from;
	m_selto = to;
}

END_AKLABETH_NAMESPACE
