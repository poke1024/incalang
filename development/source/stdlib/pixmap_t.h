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
//	pixmap_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"
#include "bitmap_t.h"
#include "grafport_t.h"
#include "runtime_object_t.h"
#include "gl_context_t.h"

BEGIN_MACHINE_NAMESPACE

struct f32_point {
	float				h;
	float				v;
};

class pixmap_t : public runtime_object_t {
public:
						pixmap_t();

						pixmap_t(
							lib_context_t&		context );

						~pixmap_t();

	void				allocate(
							long				width,
							long				height,
							pixel_format_t		format );

	void				load(
							const char*			path );

	void				save(
							const char*			path );

	void				release();
	
	void				convert(
							pixel_format_t		format );

	u32					m_color;
	point				m_cursor;
	f32_point			m_scursor;
	f32_point			m_sthickness;
	u08					m_default_alpha;

	pixel_format_t		m_format;
	bitmap_t*			m_bitmap;
	grafport_t*			m_port;
	void*				m_instance;

	int					m_gl;
	int					m_depth_bits;
	void*				m_old_bitmap;
	fizban::gl_context_t*	m_gl_context;

private:
	void				init();

	void				allocate_gl();

	void				release_gl();

	int					bitmap_flags();
};

END_MACHINE_NAMESPACE
