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
//	region_t.h			 	   ©1997-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "icarus.h"

#if ICARUS_OS_MACOS
#include <Quickdraw.h>
#endif

BEGIN_ICARUS_NAMESPACE

class input_stream_t;
class output_stream_t;

class bitmap_t;
struct bitmap_data_t;

struct region_rects_t {
	rect*				array;
	long				alloc;
};

class region_t {
public:
						region_t();
						
	virtual				~region_t();
	
	void				serialize(
							output_stream_t&	stream );

	void				deserialize(
							input_stream_t&		stream );
	
	bool				is_empty() const;
	
	const rect&			bounds() const;
	
	void				set_rect(
							const rect&			r );
							
	void				set_circle(
							const point&		center,
							unsigned long		r );

	void				set_oval(
							const rect& 		r );

	void				set_polygon(
							const point*		points,
							long				count );
		
	void				set_pie(
							const point&		center,
							unsigned long		r,
							float				alpha,
							float				beta );
		
	bool				contains(
							const point&		p ) const;

	static void			copy(
							const region_t&		a,
							region_t&			r );

	static void			offset(
							const region_t&		a,
							region_t&			r,
							long				dx,
							long				dy );

	static void			intersect(
							const region_t&		a,
							const region_t&		b,
							region_t&			r );
							
	static void			subtract(
							const region_t&		a,
							const region_t&		b,
							region_t&			r );

	static void			unite(
							const region_t&		a,
							const region_t&		b,
							region_t&			r );

	static void			exclusive_or(
							const region_t&		a,
							const region_t&		b,
							region_t&			r );

	static bool			intersect(
							const region_t&		a,
							const region_t&		b );
	
	static void			intersect_rect(
							const region_t&		a,
							const rect&			area,
							region_t&			r );

	static bool			intersect_rect(
							const region_t&		a,
							const rect&			area );
							
	long				to_rects(
							region_rects_t*		rects );
	
	void				from_bitmap(
							bitmap_t*			bitmap,
							u32					color,
							const rect&			r );
	
	void				clear();
	
#if ICARUS_OS_MACOS
	void				to_mac_rgn(
							RgnHandle			rgn ) const;
#endif
						
	static void			release_heaps();

	struct scanline_t {
		scanline_t*		next;
		s32				start;
		s32				end;
	};

private:
	friend void render_rgn_16_555( bitmap_data_t* bmp,
		region_t* rgn, const flt_rect& dest_rect, u32 draw_color );

	friend void render_rgn_16_565( bitmap_data_t* bmp,
		region_t* rgn, const flt_rect& dest_rect, u32 draw_color );

	friend void render_rgn_24( bitmap_data_t* bmp,
		region_t* rgn, const flt_rect& dest_rect, u32 draw_color );

	friend void render_rgn_32( bitmap_data_t* bmp,
		region_t* rgn, const flt_rect& dest_rect, u32 draw_color );
		
	friend class polygon_rasterizer_t;

	scanline_t**		allocate(
							long				height );
	
	void				shrink_row_table();
	
	void				import(
							long				dy,
							const long*			coords,
							long				count );
	
	rect				m_bounds;

	scanline_t**		m_row_table_block;
	long				m_row_table_block_size;
	
	scanline_t**		m_row_table;
	long				m_row_table_size;
};
	
inline bool
region_t::is_empty() const
{
	return m_row_table == 0;
}

inline const rect&
region_t::bounds() const
{
	return m_bounds;
}

u32*
get_region_render_tables(
	u32					width,
	float				bx,
	float				fx );

END_ICARUS_NAMESPACE
