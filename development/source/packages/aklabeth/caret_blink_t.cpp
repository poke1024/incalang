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
//	caret_blink_t.cp		   	   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "caret_blink_t.h"

BEGIN_AKLABETH_NAMESPACE

const u32					kCaretFrequency = 500;

caret_blink_t::caret_blink_t(
	caret_view_t*			view ) :
	m_view( view )
{
	set_frequency( kCaretFrequency );
}

void
caret_blink_t::tick()
{
	m_view->toggle_caret();
}

END_AKLABETH_NAMESPACE
