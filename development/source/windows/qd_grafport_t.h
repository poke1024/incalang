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
//	grafport_t.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_GRAFPORT_H
#define ICARUS_GRAFPORT_H

//#pragma once

#if __VISC__
#include <afxwin.h>
#else
#include <Windows.h>
#endif
#include "icarus.h"
#include "qd_typeface_t.h"

BEGIN_ICARUS_NAMESPACE

class grafport_t {
public:
						grafport_t(
							CDC*				hdc,
							const rect&			clip );
							
						~grafport_t();
							
	u32					selection_color() const;
							
	void				fill_rect(
							long				x0,
							long				y0,
							long				x1,
							long				y1,
							u32					color );

	void				fill_oval(
							long				x0,
							long				y0,
							long				x1,
							long				y1,
							u32					color );

	void				vline(
							long				x,
							long				y0,
							long				y1,
							u32					color );

	f64					draw_text(
							f64					x,
							f64					y,
							const char*			text,
							long				length,
							const typeface_t*	typeface,
							u32					color );

	void				set_origin(
							const point&		p );

	int					format() const;

	const rect&			raw_clip_rect();

	CDC*				hdc() const;

	void				set_hilite_text(
							bool				hilite );

private:
	CDC*				m_hdc;
	CBrush*				m_brush;
	rect				m_raw_clip_rect;
	point				m_origin;
	bool				m_hilite_text;
};

inline int
grafport_t::format() const
{
	return 0;
}

inline const rect&
grafport_t::raw_clip_rect()
{
	return m_raw_clip_rect;
}

inline u32
make_color(
	int					/*format*/,
	int					r,
	int					g,
	int					b )
{
	return (u32)r |
		( ( (u32)g ) << 8 ) |
		( ( (u32)b ) << 16 );
}

END_ICARUS_NAMESPACE

#endif

