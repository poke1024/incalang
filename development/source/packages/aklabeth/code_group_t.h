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
//	code_group_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_GROUP_H
#define CODE_GROUP_H

//#pragma once

#include "code.h"

#if __VISC__
#include "code_line_t.h"
#endif

BEGIN_CODE_NAMESPACE

struct line_t;

class group_t {
public:
	line_t*				line(
							long				number ) const;

	long				line_count() const;

	virtual void		update();

protected:
	line_t**			m_lines;
	s32					m_line_count;
};

inline line_t*
group_t::line(
	long				number ) const
{
	return m_lines[ number ];
}

inline long
group_t::line_count() const
{
	return m_line_count;
}

END_CODE_NAMESPACE

#endif

