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
//	grafport_t.cp		 		   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "grafport_t.h"
#include "gfx_vtable.h"
#include "font_t.h"
#include <algorithm>

BEGIN_ICARUS_NAMESPACE

inline long min( long a, long b )
{
	return ( a < b ) ? a : b;
}

inline long max( long a, long b )
{
	return ( a > b ) ? a : b;
}

grafport_t::grafport_t(
	bitmap_t*			bitmap ) :
	
	m_bitmap( bitmap )
{
	bitmap->lock();
	
	m_data = bitmap->m_data;
	m_vtable = bitmap->m_vtable;
	
	init_clip();
}

grafport_t::~grafport_t()
{
	m_bitmap->unlock();
	cleanup_clip();
}

void
grafport_t::init_clip()
{
	m_clip = new clip_t;
	m_clip->next = 0;
	
	m_clip->rect_count = 1;
	m_clip->rect_parts.alloc = 8;
	m_clip->rect_parts.array = (rect*)icalloc( sizeof( rect ), 8 );
	if( !m_clip->rect_parts.array )
		throw_icarus_error( "out of memory" );

	rect&				clip = m_clip->rect_parts.array[ 0 ];

	clip.left = 0;
	clip.top = 0;
	clip.right = m_data->width;
	clip.bottom = m_data->height;
	
	m_clip->region.set_rect( clip );
}

void
grafport_t::cleanup_clip()
{
	clip_t*				clip = m_clip;
	clip_t*				next;

	while( clip )
	{
		next = clip->next;
		ifree( clip->rect_parts.array );
		delete clip;
		clip = next;
	}
}

grafport_t::clip_t*
grafport_t::alloc_clip()
{
	clip_t*				clip = new clip_t;

	clip->rect_count = 1;
	clip->rect_parts.alloc = 8;
	clip->rect_parts.array = (rect*)icalloc( sizeof( rect ), 8 );
	if( !clip->rect_parts.array )
		throw_icarus_error( "out of memory" );

	return clip;
}

void
grafport_t::free_clip(
	clip_t*				clip )
{
	ifree( clip->rect_parts.array );
	delete clip;
}

void
grafport_t::set_origin(
	const point&		o )
{
	m_data->origin = o;
}

void
grafport_t::get_origin(
	point&				o ) const
{
	o = m_data->origin;
}

void
grafport_t::offset_origin(
	long				dx,
	long				dy )
{
	point&				origin = m_data->origin;

	origin.h += dx;
	origin.v += dy;
}

void
grafport_t::set_clip_rect(
	const rect&			r )
{
	const point&		origin = m_data->origin;
	const long			ox = origin.h;
	const long			oy = origin.v;

	rect				clip;

	r.offset( ox, oy, clip );
	m_clip->region.set_rect( clip );

	clip.left = max( 0, r.left + ox );
	clip.top = max( 0, r.top + oy );
	clip.right = min( m_data->width, r.right + ox + 1 );
	clip.bottom = min( m_data->height, r.bottom + oy + 1 );

	m_clip->rect_count = 1;
	m_clip->rect_parts.array[ 0 ] = clip;
}

void
grafport_t::set_clip_region(
	const region_t&		rgn )
{
	const point&		origin = m_data->origin;
	const long			ox = origin.h;
	const long			oy = origin.v;

	region_t::offset(
		rgn, m_clip->region, ox, oy );
		
	region_t			r;
	
	region_t::copy(
		m_clip->region, r );

	m_clip->rect_count = r.to_rects(
		&m_clip->rect_parts );
}

void
grafport_t::get_clip_region(
	region_t&			region ) const
{
	const point&		origin = m_data->origin;
	const long			ox = origin.h;
	const long			oy = origin.v;

	region_t::offset(
		m_clip->region, region, -ox, -oy );
}

const region_t&
grafport_t::raw_clip_region() const
{
	return m_clip->region;
}

void
grafport_t::push_clip()
{
	clip_t*				clip;

	clip = alloc_clip();
	clip->next = m_clip;
	m_clip = clip;
}

void
grafport_t::pop_clip()
{
	clip_t*				clip = m_clip;		
	clip_t*				next = clip->next;

	if( !next )
		throw_icarus_error( "cannot pop clip" );

	m_clip = next;
	free_clip( clip );
}

void
grafport_t::put_pixel(
	long				x,
	long				y,
	u32					color ) const
{
	bitmap_data_t*		bmp = m_data;

	x += bmp->origin.h;
	y += bmp->origin.v;

	gfxdrv_fn_put_pixel	put_pixel = m_vtable->put_pixel;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		put_pixel( bmp, x, y, color );
	}
}

void
grafport_t::hline(
	long				x1,
	long				y1,
	long				x2,
	long				color ) const
{
	bitmap_data_t*		bmp = m_data;

	const long			xo = bmp->origin.h;
	const long			yo = bmp->origin.v;

	x1 += xo;
	y1 += yo;
	x2 += xo;

	gfxdrv_fn_hline		hline = m_vtable->hline;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		hline( bmp, x1, y1, x2, color );		
	}
}

void
grafport_t::vline(
	long				x1,
	long				y1,
	long				y2,
	long				color ) const
{
	bitmap_data_t*		bmp = m_data;

	const long			xo = bmp->origin.h;
	const long			yo = bmp->origin.v;

	x1 += xo;
	y1 += yo;
	y2 += yo;

	gfxdrv_fn_vline		vline = m_vtable->vline;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		vline( bmp, x1, y1, y2, color );	
	}
}

void
grafport_t::line(
	long				x1,
	long				y1,
	long				x2,
	long				y2,
	u32					color ) const
{
	bitmap_data_t*		bmp = m_data;

	const long			xo = bmp->origin.h;
	const long			yo = bmp->origin.v;

	x1 += xo;
	y1 += yo;
	x2 += xo;
	y2 += yo;

	gfxdrv_fn_line		line = m_vtable->line;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		line( bmp, x1, y1, x2, y2, color );
	}
}

void
grafport_t::shade_rect(
	long				x1,
	long				y1,
	long				x2,
	long				y2,
	u32					color ) const
{
	bitmap_data_t*		bmp = m_data;

	const long			xo = bmp->origin.h;
	const long			yo = bmp->origin.v;

	x1 += xo;
	y1 += yo;
	x2 += xo;
	y2 += yo;

	gfxdrv_fn_shade_rect	shade_rect = m_vtable->shade_rect;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		shade_rect( bmp, x1, y1, x2, y2, color );
	}
}

void
grafport_t::fill_rect(
	long				x1,
	long				y1,
	long				x2,
	long				y2,
	u32					color ) const
{
	bitmap_data_t*		bmp = m_data;

	const long			xo = bmp->origin.h;
	const long			yo = bmp->origin.v;

	x1 += xo;
	y1 += yo;
	x2 += xo;
	y2 += yo;

	gfxdrv_fn_fill_rect	fill_rect = m_vtable->fill_rect;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		fill_rect( bmp, x1, y1, x2, y2, color );
	}
}

void
grafport_t::frame_rect(
	long				x1,
	long				y1,
	long				x2,
	long				y2,
	u32					color ) const
{
	bitmap_data_t*		bmp = m_data;

	const long			xo = bmp->origin.h;
	const long			yo = bmp->origin.v;

	x1 += xo;
	y1 += yo;
	x2 += xo;
	y2 += yo;
	
	gfxdrv_fn_hline		hline = m_vtable->hline;
	gfxdrv_fn_hline		vline = m_vtable->vline;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		hline( bmp, x1, y1, x2, color );
		hline( bmp, x1, y2, x2, color );
		vline( bmp, x1, y1, y2, color );
		vline( bmp, x2, y1, y2, color );
	}
}

void
grafport_t::frame_oval(
	long				x1,
	long				y1,
	long				x2,
	long				y2,
	u32					color ) const
{
	bitmap_data_t*		bmp = m_data;

	const long			xo = bmp->origin.h;
	const long			yo = bmp->origin.v;

	x1 += xo;
	y1 += yo;
	x2 += xo;
	y2 += yo;

	gfxdrv_fn_frame_oval	frame_oval = m_vtable->frame_oval;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		frame_oval( bmp, x1, y1, x2, y2, color );
	}
}

void
grafport_t::fill_oval(
	long				x1,
	long				y1,
	long				x2,
	long				y2,
	u32					color ) const
{
	bitmap_data_t*		bmp = m_data;

	const long			xo = bmp->origin.h;
	const long			yo = bmp->origin.v;

	x1 += xo;
	y1 += yo;
	x2 += xo;
	y2 += yo;

	gfxdrv_fn_fill_oval	fill_oval = m_vtable->fill_oval;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		fill_oval( bmp, x1, y1, x2, y2, color );
	}
}

void
grafport_t::render_rgn(
	region_t*			rgn,
	const flt_rect&		dstrect,
	u32					color )
{
	bitmap_data_t*		bmp = m_data;
	flt_rect			r;

	const double		xo = bmp->origin.h;
	const double		yo = bmp->origin.v;

	double				left = dstrect.left;
	double				right = dstrect.right;
	double				bottom = dstrect.bottom;
	double				top = dstrect.top;

	if( right < left )
	{
		double h = left;
		left = right;
		right = h;
	}
	
	if( bottom < top )
	{
		double h = bottom;
		bottom = top;
		top = h;
	}

	r.left = left + xo;
	r.top = top + yo;
	r.right = right + xo;
	r.bottom = bottom + yo;

	gfxdrv_fn_render_rgn	render_rgn = m_vtable->render_rgn;
	long				count = m_clip->rect_count;
	const rect*			clip = m_clip->rect_parts.array;
	
	for( long i = 0; i < count; i++, clip++ )
	{
		bmp->clip = clip;
		render_rgn( bmp, rgn, r, color );
	}
}

void
grafport_t::blit(
	grafport_t&			srcport,
	grafport_t&			dstport,
	const rect&			srcrect,
	const rect&			dstrect )
{
	bitmap_data_t*		src_bmp = srcport.m_data;
	bitmap_data_t*		dst_bmp = dstport.m_data;

	long				def_src_x0, def_src_x1;
	long				def_src_y0, def_src_y1;
	long				def_dst_x0, def_dst_x1;
	long				def_dst_y0, def_dst_y1;

	{
		const long		xo = src_bmp->origin.h;
		const long		yo = src_bmp->origin.v;

		def_src_x0 = srcrect.left + xo;
		def_src_y0 = srcrect.top + yo;
		def_src_x1 = srcrect.right + xo;
		def_src_y1 = srcrect.bottom + yo;
	}
	
	{
		const long		xo = dst_bmp->origin.h;
		const long		yo = dst_bmp->origin.v;

		def_dst_x0 = dstrect.left + xo;
		def_dst_y0 = dstrect.top + yo;
		def_dst_x1 = dstrect.right + xo;
		def_dst_y1 = dstrect.bottom + yo;
	}
	
	// check out source and destination size

	if( def_src_x1 - def_src_x0 != def_dst_x1 - def_dst_x0 ||
		def_src_y1 - def_src_y0 != def_dst_y1 - def_dst_y0 )
	{
		throw_icarus_error( "scaled blit not supported" );
	}
	
	// check out pixel blit mode
	
	gfxdrv_blit_vtable_t*	vtab;
	
	vtab = get_blit_vtable( src_bmp->format, dst_bmp->format );
	if( !vtab )
		throw_icarus_error( "unsupported blit mode" );

	gfxdrv_fn_std_blit		std_blit = vtab->std_blit;
	
	if( !std_blit )
		throw_icarus_error( "std blit not supported" );
			
	// first clip the rectangles against the source bounds.

	long				x_min, y_min, x_max, y_max;
	
	x_min = 0;
	y_min = 0;
	x_max = src_bmp->width;
	y_max = src_bmp->height;
	
	if( def_src_x0 < x_min )
		{ def_dst_x0 -= def_src_x0 - x_min; def_src_x0 = x_min; }
	if( def_src_y0 < y_min )
		{ def_dst_y0 -= def_src_y0 - y_min; def_src_y0 = y_min; }
	if( def_src_x1 > x_max )
		{ def_dst_x1 -= def_src_x1 - x_max; def_src_x1 = x_max; }
	if( def_src_y1 > y_max )
		{ def_dst_y1 -= def_src_y1 - y_max; def_src_y1 = y_max; }

	long			count = dstport.m_clip->rect_count;
	const rect*		clip = dstport.m_clip->rect_parts.array;

	for( long i = 0; i < count; i++, clip++ )
	{
		// now clip the rectangles against the destination bounds.

		x_min = clip->left;
		y_min = clip->top;
		x_max = clip->right;
		y_max = clip->bottom;

		long		src_x0, src_x1, src_y0, src_y1;

		src_x0 = def_src_x0;
		src_y0 = def_src_y0;
		src_x1 = def_src_x1;
		src_y1 = def_src_y1;

		long		dst_x0, dst_x1, dst_y0, dst_y1;
		
		dst_x0 = def_dst_x0;
		dst_y0 = def_dst_y0;
		dst_x1 = def_dst_x1;
		dst_y1 = def_dst_y1;
	
		if( dst_x0 < x_min ) { src_x0 -= dst_x0 - x_min; dst_x0 = x_min; }
		if( dst_y0 < y_min ) { src_y0 -= dst_y0 - y_min; dst_y0 = y_min; }
		if( dst_x1 > x_max ) { src_x1 -= dst_x1 - x_max; dst_x1 = x_max; }
		if( dst_y1 > y_max ) { src_y1 -= dst_y1 - y_max; dst_y1 = y_max; }

		// check if we have valid rectangles left.

		if( src_x1 <= src_x0 || src_y1 <= src_y0 ||
			dst_x1 <= dst_x0 || dst_y1 <= dst_y0 )
			continue;
			
		// perform the blitting operation.
		long		src_width, src_height;

		src_width = src_x1 - src_x0;
		src_height = src_y1 - src_y0;

		dst_bmp->clip = clip;
		
		std_blit( src_bmp, dst_bmp, src_x0, src_y0,
			dst_x0, dst_y0, src_width, src_height );
	}
}

void
grafport_t::transform(
	grafport_t&			srcport,
	grafport_t&			dstport,
	const rect&			dstrect,
	const double*		matrix,
	u32					backcolor )
{
	bitmap_data_t*		src_bmp = srcport.m_data;
	bitmap_data_t*		dst_bmp = dstport.m_data;

	gfxdrv_blit_vtable_t*	vtab;
	
	vtab = get_blit_vtable( src_bmp->format, dst_bmp->format );
	if( !vtab )
		throw_icarus_error( "unsupported blit mode" );

	gfxdrv_fn_transform	tfm = vtab->transform;

	if( !tfm )
		throw_icarus_error( "transform not supported" );
	
	long				count = dstport.m_clip->rect_count;
	const rect*			clip = dstport.m_clip->rect_parts.array;

	for( long i = 0; i < count; i++, clip++ )
	{
		dst_bmp->clip = clip;
		tfm( src_bmp, dst_bmp, dstrect, matrix, backcolor );
	}
}

END_ICARUS_NAMESPACE
