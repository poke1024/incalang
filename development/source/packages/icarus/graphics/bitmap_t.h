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
//	bitmap_t.h				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "icarus.h"
#include "gfx_driver.h"

BEGIN_ICARUS_NAMESPACE

class bitmap_t {
public:
						bitmap_t(
							pixel_format_t		format,
							long				width,
							long				height,
							int					flags );

						bitmap_t(
							bitmap_data_t*		data );

	virtual				~bitmap_t();

	pixel_format_t		format() const;

	long				width() const;
	
	long				height() const;
	
	int					flags() const;
	
	long				pitch() const;
	
	void*				native() const;
	
	void				set_palette_entry(
							int					index,
							const u08*			color );
							
	bitmap_t*			to_format(
							pixel_format_t		format );

protected:
	friend class		grafport_t;

	void				lock();
	
	void				unlock();

	gfxdrv_vtable_t*	m_vtable;
	bitmap_data_t*		m_data;
	long				m_lock_count;
	int					m_flags;
};

inline pixel_format_t
bitmap_t::format() const
{
	return m_data->format;
}

inline long
bitmap_t::width() const
{
	return m_data->width;
}
	
inline long
bitmap_t::height() const
{
	return m_data->height;
}

inline int
bitmap_t::flags() const
{
	return m_flags;
}

inline long
bitmap_t::pitch() const
{
	return m_data->pitch;
}

inline void*
bitmap_t::native() const
{
	return m_data->native;
}

END_ICARUS_NAMESPACE
