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
//	bitmap_font_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "fizban.h"
#include <gl/gl.h>

BEGIN_FIZBAN_NAMESPACE

class bitmap_font_t {
public:
							bitmap_font_t(
								const char*			name,
								int					size );

							~bitmap_font_t();

	void					draw(
								int					x,
								int					y,
								const char*			text );

	float					text_width(
								const char*			text ) const;

	const char*				name() const;

	int						size() const;

	int						ascent() const;

	int						descent() const;

private:
	void					position_text( 
								int					x,
								int					y );

	enum {
		MAX_CHARS = 256
	};

	char*					m_name;
	GLuint					m_font_list_id;
	HFONT					m_font;
	int						m_size;
	INT						m_widths[ MAX_CHARS ];
	TEXTMETRIC				m_metrics;
};

inline const char*
bitmap_font_t::name() const
{
	return m_name;
}

END_FIZBAN_NAMESPACE
