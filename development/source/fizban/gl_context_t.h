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
//	gl_context_t.h			   �2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "fizban.h"

#include <gl/gl.h>
#include "sgl.h"

BEGIN_FIZBAN_NAMESPACE

class gl_context_t {
public:
							gl_context_t();

							~gl_context_t();

	void					make_current();

	void					init(
								HDC					hDC,
								HGLRC				hRC );

	void					release();

	void					setup_viewport(
								long				width,
								long				height,
								bool				is_2d );

	HDC						hdc() const
								{ return m_hDC; }

	HGLRC					hrc() const
								{ return m_hRC; }

	sgl_context_t*			sgl_context()
								{ return &m_sgl_context; }

	long					height() const
								{ return m_height; }

	static gl_context_t*	current();

private:
	void					clear();

	HDC						m_hDC;
	HGLRC					m_hRC;
	sgl_context_t			m_sgl_context;
	long					m_width;
	long					m_height;
};

END_FIZBAN_NAMESPACE
