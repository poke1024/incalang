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
//	lib_spixmap.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "vmutils.h"
#include "pixmap_t.h"

#include "machine_class_t.h"

#include "string_t.h"
#include "lib_string.h"

#include <math.h>

BEGIN_MACHINE_NAMESPACE

using namespace std;

#define PIXMAP_CONTEXT_FUNCTION( name )										\
	STDLIB_FUNCTION( name )													\
	g2world_t* pixmap;														\
	if( ( pixmap = f.m_lib_context.g2world ) == 0 )							\
		f.fatal_error( ERR_NO_G2WORLD_SET );

#define PIXMAP_DRAW_FUNCTION( name )										\
	PIXMAP_CONTEXT_FUNCTION( name )											\
	grafport_t* port;														\
	if( ( port = pixmap->m_port ) == 0 )									\
		f.fatal_error( ERR_EMPTY_PIXMAP );

#define PIXMAP_CG_DRAW_FUNCTION( name )										\
	PIXMAP_CONTEXT_FUNCTION( name )											\
	CGContextRef context;													\
	if( ( context = pixmap->m_cg_context ) == 0 )							\
		f.fatal_error( ERR_EMPTY_PIXMAP );

// ---------------------------------------------------------------------------

PIXMAP_CONTEXT_FUNCTION( pixmap_smooth_move_to )
	float				x = f.pop_float();
	float				y = f.pop_float();
	
	pixmap->m_scursor.h = x;
	pixmap->m_scursor.v = y;
END_STDLIB_FUNCTION

PIXMAP_CONTEXT_FUNCTION( pixmap_smooth_set_thickness_one )
	float				thickness = f.pop_float();

	if( thickness < 0 )
		thickness = 0;
	
	pixmap->m_sthickness.h = thickness;
	pixmap->m_sthickness.v = thickness;
END_STDLIB_FUNCTION

PIXMAP_CONTEXT_FUNCTION( pixmap_smooth_set_thickness_two )
	float				hthickness = f.pop_float();
	float				vthickness = f.pop_float();
	
	if( hthickness < 0 )
		hthickness = 0;
	if( vthickness < 0 )
		vthickness = 0;
	
	pixmap->m_sthickness.h = hthickness;
	pixmap->m_sthickness.v = vthickness;
END_STDLIB_FUNCTION

static void
build_line_polygon(
	float				x0,
	float				y0,
	float				x1,
	float				y1,
	const f32_point&	thick,
	flt_point*			v )
{
	float				dx, dy;
	float				angle;
	float				length;
	
	dx = x1 - x0;
	dy = y1 - y0;
	length = sqrt( dx * dx + dy * dy );
	
	if( length < 1e-4 )
		angle = 0;
	else
		angle = atan2( dy, dx );
		
	float				px, py;

	px = x0;
	py = y0;
	
	float				th = thick.h;
	float				tv = thick.v;
	float				thickness;
	
	if( th == tv )
		thickness = th;
	else
	{
		float			gamma = iabs( angle / ( icarus::pi * 0.5 ) );
	
		if( gamma > 1 )
			gamma = 2 - gamma;
	
		thickness = th * gamma + tv * ( 1.0 - gamma );
	}
		
	float				omega = angle + icarus::pi / 2;
	float				dhx, dhy;
	float				dvx, dvy;
	
	dhx = cos( omega ) * thickness * 0.5;
	dhy = sin( omega ) * thickness * 0.5;
	
	dvx = cos( angle ) * length;
	dvy = sin( angle ) * length;
	
	v[ 0 ].h = px - dhx;
	v[ 0 ].v = py - dhy;
	
	v[ 1 ].h = v[ 0 ].h + dvx;
	v[ 1 ].v = v[ 0 ].v + dvy;
	
	v[ 3 ].h = px + dhx;
	v[ 3 ].v = py + dhy;	

	v[ 2 ].h = v[ 3 ].h + dvx;
	v[ 2 ].v = v[ 3 ].v + dvy;
}

PIXMAP_DRAW_FUNCTION( pixmap_smooth_line_to )
	float				x1 = f.pop_float();
	float				y1 = f.pop_float();

	const float			scale = 4;

	float				x0 = pixmap->m_scursor.h;
	float				y0 = pixmap->m_scursor.v;

	region_t			rgn;
	flt_point			fpoints[ 4 ];
	
	build_line_polygon( x0, y0, x1, y1,
		pixmap->m_sthickness, fpoints );
		
	point				points[ 4 ];
	float				left, right;
	float				top, bottom;
	
	for( int i = 0; i < 4; i++ )
	{
		float			fx = fpoints[ i ].h;
		float			fy = fpoints[ i ].v;
	
		if( i == 0 )
		{
			left = right = fx;
			top = bottom = fy;
		}
		else
		{
			left = imin( left, fx );
			top = imin( top, fy );
			right = imax( right, fx );
			bottom = imax( bottom, fy );
		}
	
		points[ i ].h = fx * scale;
		points[ i ].v = fy * scale;
	}
	
	rgn.set_polygon( points, 4 );

	flt_rect			dstrect;
	
	dstrect.left = left;
	dstrect.top = top;
	dstrect.right = right;
	dstrect.bottom = bottom;

	port->render_rgn(
		&rgn, dstrect, pixmap->m_color );
		
	pixmap->m_scursor.h = x1;
	pixmap->m_scursor.v = y1;
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_smooth_frame_rect )
	float				x0 = f.pop_float();
	float				y0 = f.pop_float();
	float				x1 = f.pop_float();
	float				y1 = f.pop_float();
	
	if( x1 < x0 )
		swap( x0, x1 );
		
	if( y1 < y0 )
		swap( y0, y1 );

	float				hthickness = pixmap->m_sthickness.h;
	float				hthickhalf = 0.5 * hthickness;
	float				vthickness = pixmap->m_sthickness.v;
	float				vthickhalf = 0.5 * vthickness;
	const float			scale = 4;

	region_t			outrgn;
	region_t			inrgn;
	flt_rect			outrect;
	
	outrect.left = x0 - hthickhalf;
	outrect.top = y0 - vthickhalf;
	outrect.right = x1 + hthickhalf;
	outrect.bottom = y1 + vthickhalf;
	
	rect				outrecti;
	rect				inrecti;
	
	outrecti.left = outrect.left * scale;
	outrecti.top = outrect.top * scale;
	outrecti.right = outrect.right * scale;
	outrecti.bottom = outrect.bottom * scale;
	inrecti.left = ( x0 + hthickhalf ) * scale;
	inrecti.top = ( y0 + vthickhalf ) * scale;
	inrecti.right = ( x1 - hthickhalf ) * scale;
	inrecti.bottom = ( y1 - vthickhalf ) * scale;
	
	outrgn.set_rect( outrecti );
	
	if( inrecti.left < inrecti.right &&
		inrecti.top < inrecti.bottom )
	{
		region_t		tmprgn;
	
		inrgn.set_rect( inrecti );
		region_t::subtract( outrgn, inrgn, tmprgn );
	
		port->render_rgn(
			&tmprgn, outrect, pixmap->m_color );
	}
	else
	{
		port->render_rgn(
			&outrgn, outrect, pixmap->m_color );
	}
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_smooth_frame_oval )
	float				x0 = f.pop_float();
	float				y0 = f.pop_float();
	float				x1 = f.pop_float();
	float				y1 = f.pop_float();
	
	if( x1 < x0 )
		swap( x0, x1 );
		
	if( y1 < y0 )
		swap( y0, y1 );

	float				hthickness = pixmap->m_sthickness.h;
	float				hthickhalf = 0.5 * hthickness;
	float				vthickness = pixmap->m_sthickness.v;
	float				vthickhalf = 0.5 * vthickness;
	const float			scale = 4;

	region_t			outrgn;
	region_t			inrgn;
	flt_rect			outrect;
	
	outrect.left = x0 - hthickhalf;
	outrect.top = y0 - vthickhalf;
	outrect.right = x1 + hthickhalf;
	outrect.bottom = y1 + vthickhalf;
	
	rect				outrecti;
	rect				inrecti;
	
	outrecti.left = outrect.left * scale;
	outrecti.top = outrect.top * scale;
	outrecti.right = outrect.right * scale;
	outrecti.bottom = outrect.bottom * scale;
	inrecti.left = ( x0 + hthickhalf ) * scale;
	inrecti.top = ( y0 + vthickhalf ) * scale;
	inrecti.right = ( x1 - hthickhalf ) * scale;
	inrecti.bottom = ( y1 - vthickhalf ) * scale;
	
	outrgn.set_oval( outrecti );
	
	if( inrecti.left < inrecti.right &&
		inrecti.top < inrecti.bottom )
	{
		region_t		tmprgn;
	
		inrgn.set_oval( inrecti );
		region_t::subtract( outrgn, inrgn, tmprgn );
	
		port->render_rgn(
			&tmprgn, outrect, pixmap->m_color );
	}
	else
	{
		port->render_rgn(
			&outrgn, outrect, pixmap->m_color );
	}
END_STDLIB_FUNCTION

#if ICARUS_OS_MACOS

PIXMAP_CG_DRAW_FUNCTION( pixmap_cg_move_to_point )
	float				x = f.pop_float();
	float				y = f.pop_float();
	CGContextMoveToPoint( context, x, y );
END_STDLIB_FUNCTION

PIXMAP_CG_DRAW_FUNCTION( pixmap_cg_set_text_position )
	float				x = f.pop_float();
	float				y = f.pop_float();
	CGContextSetTextPosition( context, x, y );
END_STDLIB_FUNCTION

PIXMAP_CG_DRAW_FUNCTION( pixmap_cg_show_text )
	string_t*			string = pop_string( f );
	long				length = string->length;
	
	if( length < 1 )
		return;
		
	CGContextShowText( context, string->text,
		string->length );
END_STDLIB_FUNCTION

PIXMAP_CG_DRAW_FUNCTION( pixmap_cg_select_font )
	string_t*			string = pop_string( f );
	float				size = f.pop_float();
	istring				s;

	s.assign( string->text, string->length );
	CGContextSelectFont( context, s.c_str(), size, kCGEncodingMacRoman );
END_STDLIB_FUNCTION

#endif

// ---------------------------------------------------------------------------

void
init_spixmap(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();
		
	register_native_function(
		m, rootclss, "smoothThickness;f",
		pixmap_smooth_set_thickness_one );
		
	register_native_function(
		m, rootclss, "smoothThickness;f;f",
		pixmap_smooth_set_thickness_two );
	
	register_native_function(
		m, rootclss, "smoothMoveTo;f;f", pixmap_smooth_move_to );
	
	register_native_function(
		m, rootclss, "smoothLineTo;f;f", pixmap_smooth_line_to );

	register_native_function(
		m, rootclss, "smoothFrameRect;f;f;f;f", pixmap_smooth_frame_rect );

	register_native_function(
		m, rootclss, "smoothFrameOval;f;f;f;f", pixmap_smooth_frame_oval );


#if ICARUS_OS_MACOS

	register_native_function(
		m, rootclss, "cgMoveToPoint;f;f", pixmap_cg_move_to_point );

	register_native_function(
		m, rootclss, "cgSetTextPosition;f;f", pixmap_cg_set_text_position );

	register_native_function(
		m, rootclss, "cgShowText;.String::&", pixmap_cg_show_text );
		
	register_native_function(
		m, rootclss, "cgSelectFont;.String::&;f", pixmap_cg_select_font );

#endif
}

END_MACHINE_NAMESPACE
