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
//	pixel_format.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

enum pixel_format_t {
	pixel_format_8_indexed_gray			= 0,
	pixel_format_8_indexed				= 1,
	pixel_format_16_555_RGB				= 2,
	pixel_format_16_555_GBR				= 3,
	pixel_format_16_555_BRG				= 4,
	pixel_format_16_555_BGR				= 5,
	pixel_format_16_555_GRB				= 6,
	pixel_format_16_555_RBG				= 7,
	pixel_format_16_565_RGB				= 8,
	pixel_format_16_565_GBR				= 9,
	pixel_format_16_565_BRG				= 10,
	pixel_format_16_565_BGR				= 11,
	pixel_format_16_565_GRB				= 12,
	pixel_format_16_565_RBG				= 13,
	pixel_format_16_655_RGB				= 14,
	pixel_format_16_655_GBR				= 15,
	pixel_format_16_655_BRG				= 16,
	pixel_format_16_655_BGR				= 17,
	pixel_format_16_655_GRB				= 18,
	pixel_format_16_655_RBG				= 19,
	pixel_format_16_556_RGB				= 20,
	pixel_format_16_556_GBR				= 21,
	pixel_format_16_556_BRG				= 22,
	pixel_format_16_556_BGR				= 23,
	pixel_format_16_556_GRB				= 24,
	pixel_format_16_556_RBG				= 25,
	pixel_format_24_RGB					= 26,
	pixel_format_24_GBR					= 27,
	pixel_format_24_BRG					= 28,
	pixel_format_24_BGR					= 29,
	pixel_format_24_GRB					= 30,
	pixel_format_24_RBG					= 31,
	pixel_format_32_RGBA				= 32,
	PIXEL_FORMAT_COUNT
};

struct pixel_profile_t {
	u08						depth;
	u08						red_bits;
	u08						green_bits;
	u08						blue_bits;
	u08						alpha_bits;
	u08						red_shift;
	u08						green_shift;
	u08						blue_shift;
	u08						alpha_shift;
};

extern pixel_profile_t s_profiles[ PIXEL_FORMAT_COUNT ];

u32
make_color(
	pixel_format_t			format,
	int						r,
	int						g,
	int						b );

u32
make_color(
	pixel_format_t			format,
	int						r,
	int						g,
	int						b,
	int						a );

void
std_blit_cross_color(
	pixel_format_t			from_format,
	pixel_format_t			into_format,
	const u08*				from_addr,
	u08*					into_addr,
	long					from_pitch,
	long					into_pitch,
	long					width,
	long					height );

u32
convert_color(
	pixel_format_t			from_format,
	pixel_format_t			into_format,
	u32						color );

u32
blend_color(
	pixel_format_t			format,
	u32						from_color,
	u32						into_color,
	u32						alpha );

inline const pixel_profile_t*
get_pixel_format_profile(
	pixel_format_t			format )
{
	return &s_profiles[ format ];
}

END_ICARUS_NAMESPACE
