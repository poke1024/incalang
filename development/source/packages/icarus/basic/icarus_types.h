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
//	icarus_types.h			    ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_TYPES_H
#define ICARUS_TYPES_H

//#pragma once

#if __MWERKS__
#include <size_t.h>
#endif

BEGIN_ICARUS_NAMESPACE

#if __MWERKS__
typedef std::size_t isize_t;
#elif __GNUC__
typedef unsigned long isize_t;
#else
typedef size_t isize_t;
#endif

#if __MWERKS__ || __GNUC__
typedef unsigned long long u64;
typedef signed long long s64;
#elif __VISC__
typedef unsigned __int64 u64;
typedef __int64 s64;
#else
#error unknown compiler environment
#endif

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u08;

typedef signed long s32;
typedef signed short s16;
typedef signed char s08;

typedef float f32;
typedef double f64;

typedef void* handle;

struct dimension {
	s32					width;
	s32					height;
};

struct point {
	s32					h;
	s32					v;
	
	void				set(
							s32					h_,
							s32					v_ );
};

inline void
point::set(
	s32					h_,
	s32					v_ )
{
	h = h_;
	v = v_;
}

struct rect {
	s32					left;
	s32					top;
	s32					right;
	s32					bottom;

	void				set(
							s32					left_,
							s32					top_,
							s32					right_,
							s32					bottom_ );
							
	void				offset(
							s32					dx,
							s32					dy,
							rect&				r ) const;
							
	long				width() const;
	
	long				height() const;
};

inline void
rect::set(
	s32					left_,
	s32					top_,
	s32					right_,
	s32					bottom_ )
{
	left = left_;
	top = top_;
	right = right_;
	bottom = bottom_;
}

inline void
rect::offset(
	s32					dx,
	s32					dy,
	rect&				r ) const
{
	r.left = left + dx;
	r.top = top + dy;
	r.right = right + dx;
	r.bottom = bottom + dy;
}

inline long
rect::width() const
{
	return right - left;
}
	
inline long
rect::height() const
{
	return bottom - top;
}

struct flt_point {
	f64					h;
	f64					v;
};

struct flt_rect {
	f64					left;
	f64					top;
	f64					right;
	f64					bottom;
};

END_ICARUS_NAMESPACE

#endif

