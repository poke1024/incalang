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
//	typeface_t.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_TYPEFACE_H
#define ICARUS_TYPEFACE_H

//#pragma once

#if __VISC__
#include <afxwin.h>
#else
#include <Windows.h>
#endif
#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class typeface_t {
public:
						typeface_t();
							
						~typeface_t();
							
	void				set(
							const typeface_t*	typeface );
							
	f64					height() const;
	
	f64					ascent() const;

	f64					descent() const;

	f64					leading() const;
	
	f64					text_width(
							const char*			text_buf,
							long				text_len ) const;

	long				pixel_to_char(
							const char*			text_buf,
							long				text_len,
							f64*				offset_ptr ) const;

	void				init(
							HDC					hdc );

private:
	f64					m_height;
	f64					m_ascent;
	f64					m_descent;
	f64					m_leading;

	FLOAT				m_width[ 256 ];
};

// ---------------------------------------------------------------------------

inline f64
typeface_t::height() const
{
	return m_height;
}
	
inline f64
typeface_t::ascent() const
{
	return m_ascent;
}

inline f64
typeface_t::descent() const
{
	return m_descent;
}

inline f64
typeface_t::leading() const
{
	return m_leading;
}

END_ICARUS_NAMESPACE

#endif
