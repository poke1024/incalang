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
//	font_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "fizban.h"
#include "TexFont.h"

BEGIN_FIZBAN_NAMESPACE

class texture_font_t {
public:
							texture_font_t(
								const char*			name );

							~texture_font_t();

	void					draw(
								const char*			text );

	float					text_width(
								const char*			text ) const;

	const char*				name() const;

	int						base_size() const;

	int						base_ascent() const;

	int						base_descent() const;

private:
	TexFont*				m_font;
	char*					m_name;
};

inline const char*
texture_font_t::name() const
{
	return m_name;
}

END_FIZBAN_NAMESPACE
