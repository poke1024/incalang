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
//	caret_blink_t.h		   	   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CARET_BLINK_H
#define CARET_BLINK_H

//#pragma once

#include "timer_t.h"
#include "aklabeth.h"

BEGIN_AKLABETH_NAMESPACE

class caret_view_t {
protected:
	friend class			caret_blink_t;

	virtual void			toggle_caret() = 0;
};

class caret_blink_t : public timer_t {
public:
							caret_blink_t(
								caret_view_t*		view );

protected:
	virtual void			tick();

	caret_view_t*			m_view;
};

END_AKLABETH_NAMESPACE

#endif
