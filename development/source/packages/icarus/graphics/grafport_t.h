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
//	grafport_t.h		 	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "bitmap_t.h"
#include "region_t.h"

BEGIN_ICARUS_NAMESPACE

class grafport_t {
public:
						grafport_t(
							bitmap_t*			bitmap );
							
	virtual				~grafport_t();

	// ---------------------------------------------------------------------------

	void*				pixels() const;
	
	pixel_format_t		format() const;

	long				width() const;

	long				height() const;
	
	long				pitch() const;
	
	const point&		origin() const;
	
	bitmap_t*			bitmap() const;
	
	// ---------------------------------------------------------------------------

	void				set_origin(
							const point&		o );

	void				get_origin(
							point&				o ) const;

	void				offset_origin(
							long				dx,
							long				dy );
	
	void				set_clip_rect(
							const rect&			r );
	
	void				set_clip_region(
							const region_t&		rgn );
							
	void				get_clip_region(
							region_t&			region ) const;
							
	void				push_clip();
	
	void				pop_clip();
	
	// ---------------------------------------------------------------------------	
									
	void				put_pixel(
							long				x,
							long				y,
							u32					color ) const;

	u32					get_pixel(
							long				x,
							long				y ) const;

	void				hline(
							long				x1,
							long				y1,
							long				x2,
							long				color ) const;

	void				vline(
							long				x1,
							long				y1,
							long				y2,
							long				color ) const;

	void				line(
							long				x1,
							long				y1,
							long				x2,
							long				y2,
							u32					color ) const;

	void				shade_rect(
							long				x1,
							long				y1,
							long				x2,
							long				y2,
							u32					color ) const;
							
	void				fill_rect(
							long				x1,
							long				y1,
							long				x2,
							long				y2,
							u32					color ) const;

	void				frame_rect(
							long				x1,
							long				y1,
							long				x2,
							long				y2,
							u32					color ) const;

	void				frame_oval(
							long				x1,
							long				y1,
							long				x2,
							long				y2,
							u32					color ) const;

	void				fill_oval(
							long				x1,
							long				y1,
							long				x2,
							long				y2,
							u32					color ) const;

	void				render_rgn(
							region_t*			rgn,
							const flt_rect&		dstrect,
							u32					color );
							
	static void			blit(
							grafport_t&			srcport,
							grafport_t&			dstport,
							const rect&			srcrect,
							const rect&			dstrect );
							
	static void			transform(
							grafport_t&			srcport,
							grafport_t&			dstport,
							const rect&			dstrect,
							const double*		matrix,
							u32					backcolor );
	
	// ---------------------------------------------------------------------------
							
	f64					draw_text(
							f64					x,
							f64					y,
							const char*			text_buf,
							long				text_len,
							const typeface_t*	typeface,
							u32					color );
	
	const region_t&		raw_clip_region() const;
			
	long				raw_clip_count() const;
	
	const rect*			raw_clip_array() const;
		
	struct clip_t {
		clip_t*			next;
		region_t		region;
		u32				rect_count;
		region_rects_t	rect_parts;
	};

protected:
	void				init_clip();
	
	void				cleanup_clip();
	
	clip_t*				alloc_clip();
	
	void				free_clip(
							clip_t*				clip );

	bitmap_t*			m_bitmap;
	bitmap_data_t*		m_data;
	gfxdrv_vtable_t*	m_vtable;
	
	clip_t*				m_clip;
};

// ---------------------------------------------------------------------------

inline void*
grafport_t::pixels() const
{
	return m_data->pixels;
}

inline pixel_format_t
grafport_t::format() const
{
	return m_data->format;
}

inline long
grafport_t::width() const
{
	return m_data->width;
}

inline long
grafport_t::height() const
{
	return m_data->height;
}

inline long
grafport_t::pitch() const
{
	return m_data->pitch;
}

inline const point&
grafport_t::origin() const
{
	return m_data->origin;
}

inline bitmap_t*
grafport_t::bitmap() const
{
	return m_bitmap;
}

inline long
grafport_t::raw_clip_count() const
{
	return m_clip->rect_count;
}
	
inline const rect*
grafport_t::raw_clip_array() const
{
	return m_clip->rect_parts.array;
}

// ---------------------------------------------------------------------------

inline f64
grafport_t::draw_text(
	f64					x,
	f64					y,
	const char*			text_buf,
	long				text_len,
	const typeface_t*	typeface,
	u32					color )
{
	return typeface->render_text( this, x, y, text_buf,
		text_len, color );
}

// ---------------------------------------------------------------------------

inline u32
grafport_t::get_pixel(
	long				x,
	long				y ) const
{
	bitmap_data_t*		bmp = m_data;

	x += bmp->origin.h;
	y += bmp->origin.v;

	return m_vtable->get_pixel( bmp, x, y );
}

END_ICARUS_NAMESPACE
