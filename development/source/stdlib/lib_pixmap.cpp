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
//	lib_pixmap.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "fiber_t.h"

#include "lib_protos.h"
#include "machine_t.h"
#include "vmutils.h"
#include "pixmap_t.h"
#include "lib_string.h"

#include "machine_class_t.h"
#include "machine_error.h"
#include "g2world_t.h"
#include "vmbind.h"

#include <Windows.h>
#include "gl_context_t.h"
using namespace fizban;

BEGIN_MACHINE_NAMESPACE

#include "vmperfdefs.h"

struct pixmap_wrap_t {
	pixmap_t*			pixmap;
};

#define PIXMAP_CLASS_SIZE ( sizeof( pixmap_wrap_t ) + CLASS_HEADER_SIZE )

u08*
create_pixmap_template(
	machine_t&			m )
{
	u08*				ptr = m.m_lib_static.pixmap_template;
	
	if( not ptr )
	{
		class_t*		rootclss = m.get_root_class();
		class_t*		point_class;

		point_class = rootclss->find_class( "Pixmap", 6 );
		
		ptr = m.get_memory()->alloc( PIXMAP_CLASS_SIZE );		
		point_class->init_instance( ptr );
		m.m_lib_static.pixmap_template = ptr;
	}
	
	return ptr;
}

#define INIT_PIXMAP( block, m )												\
	u08*				_template;											\
	if( ( _template = m.m_lib_static.pixmap_template ) == 0 )				\
		_template = create_pixmap_template( m );							\
	*( (void**)block ) = *(void**)_template;

// ---------------------------------------------------------------------------

class_t*				s_pixmap_class = 0;

#define PIXMAP_CLASS_FUNCTION( name, type ) \
	STDLIB_CLASS_FUNCTION( name, type ) \
	self = INSTANCE_SELF( type, dyncast( &f, instptr, s_pixmap_class ) );

#define PIXMAP_FUNCTION( name )												\
	PIXMAP_CLASS_FUNCTION( name, pixmap_wrap_t )							\
	pixmap_t* pixmap = self->pixmap;

#define PIXMAP_PORT_FUNCTION( name )										\
	PIXMAP_FUNCTION( name )													\
	grafport_t* port;														\
	if( ( port = pixmap->m_port ) == 0 )									\
		f.fatal_error( ERR_EMPTY_PIXMAP );

// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------

PIXMAP_CONTEXT_FUNCTION( pixmap_set_color_paletted )
	long				color = f.pop_int();

	pixmap->m_color = color;
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_set_color )
	long				r = f.pop_int();
	long				g = f.pop_int();
	long				b = f.pop_int();

	pixmap->m_color = make_color(
		pixmap->m_format, r, g, b, pixmap->m_default_alpha );
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_set_color_alpha )
	long				r = f.pop_int();
	long				g = f.pop_int();
	long				b = f.pop_int();
	long				a = f.pop_int();

	pixmap->m_color = make_color(
		pixmap->m_format, r, g, b, a );
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_put_pixel )
	long				x = f.pop_int();
	long				y = f.pop_int();

	port->put_pixel(
		x, y, pixmap->m_color );
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_get_pixel )
	long				x = f.pop_int();
	long				y = f.pop_int();

	f.push_int( port->get_pixel(
		x, y ) );
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_move_to )
	long				x = f.pop_int();
	long				y = f.pop_int();

	pixmap->m_cursor.h = x;
	pixmap->m_cursor.v = y;
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_line_to )
	long				x = f.pop_int();
	long				y = f.pop_int();
	
	port->line(
		pixmap->m_cursor.h,
		pixmap->m_cursor.v,
		x, y, pixmap->m_color );
		
	pixmap->m_cursor.h = x;
	pixmap->m_cursor.v = y;
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_frame_rect )
	long				x0 = f.pop_int();
	long				y0 = f.pop_int();
	long				x1 = f.pop_int();
	long				y1 = f.pop_int();

	port->frame_rect(
		x0, y0, x1, y1, pixmap->m_color );
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_fill_rect )
	long				x0 = f.pop_int();
	long				y0 = f.pop_int();
	long				x1 = f.pop_int();
	long				y1 = f.pop_int();

	port->fill_rect(
		x0, y0, x1, y1, pixmap->m_color );
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_frame_oval )
	long				x0 = f.pop_int();
	long				y0 = f.pop_int();
	long				x1 = f.pop_int();
	long				y1 = f.pop_int();

	port->frame_oval(
		x0, y0, x1, y1, pixmap->m_color );
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_fill_oval )
	long				x0 = f.pop_int();
	long				y0 = f.pop_int();
	long				x1 = f.pop_int();
	long				y1 = f.pop_int();

	port->fill_oval(
		x0, y0, x1, y1, pixmap->m_color );
END_STDLIB_FUNCTION

PIXMAP_DRAW_FUNCTION( pixmap_blit )
	instance_t*			instance = f.pop_instance();
	pixmap_t*			destination;
	grafport_t*			dstport;
	
	destination = INSTANCE_SELF(
		pixmap_wrap_t, instance )->pixmap;
	dstport = destination->m_port;
	if( not dstport )
		f.fatal_error( ERR_EMPTY_PIXMAP );

	long				sx = f.pop_int();
	long				sy = f.pop_int();
	long				dx = f.pop_int();
	long				dy = f.pop_int();
	long				w = f.pop_int();
	long				h = f.pop_int();

	rect				srcrect;
	rect				dstrect;
	
	srcrect.left = sx;
	srcrect.top = sy;
	srcrect.right = sx + w - 1;
	srcrect.bottom = sy + h - 1;
	
	dstrect.left = dx;
	dstrect.top = dy;
	dstrect.right = dx + w - 1;
	dstrect.bottom = dy + h - 1;

	grafport_t::blit(
		*port, *dstport, srcrect, dstrect );
		
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( set_port )
	instance_t*			instance = f.pop_instance();

	if( instance == 0 )
		f.m_lib_context.g2world = 0;
	else
		f.m_lib_context.g2world = INSTANCE_SELF(
			pixmap_wrap_t, instance )->pixmap;
END_STDLIB_FUNCTION

STDLIB_FUNCTION( get_port )
	pixmap_t*			pixmap;	

	pixmap = f.m_lib_context.g2world;
	if( pixmap )
		f.push_ptr( pixmap->m_instance );
	else
		f.push_ptr( 0 );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( pixmap_create, pixmap_wrap_t )
	self->pixmap = new pixmap_t( f.m_lib_context );
	self->pixmap->m_instance = instptr;
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_destroy )
	f;
	delete pixmap;
	self->pixmap = 0;
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_allocate )
	long				width = f.pop_int();
	long				height = f.pop_int();
	long				format = f.pop_int();
		
	if( format < 0 || format >= PIXEL_FORMAT_COUNT )
		f.fatal_error( ERR_ILLEGAL_PIXEL_FORMAT );
		
	pixmap->allocate( width, height,
		(pixel_format_t)format );
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_load )
	machine_t&			m = *f.machine();
	string_t*			string = pop_string( f );	
	istring				path;
	
	path.assign( string->text, string->length );
	
	path = m.file_path( path.c_str() );
	
	pixmap->load( path.c_str() );
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_save )
	machine_t&			m = *f.machine();
	string_t*			string = pop_string( f );	
	istring				path;
	
	path.assign( string->text, string->length );
	
	path = m.file_path( path.c_str() );
	
	pixmap->save( path.c_str() );
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_release )
	f;
	pixmap->release();
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_convert )
	long				format = f.pop_int();
		
	if( format < 0 || format >= PIXEL_FORMAT_COUNT )
		f.fatal_error( ERR_ILLEGAL_PIXEL_FORMAT );
		
	pixmap->convert( (pixel_format_t)format );
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_pixels )
	void*				pixels = 0;
	grafport_t*			port = pixmap->m_port;

	if( port )
		pixels = port->pixels();

	f.push_ptr( pixels );
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_width )
	long				width = 0;
	grafport_t*			port = pixmap->m_port;

	if( port )
		width = port->width();

	f.push_int( width );
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_height )
	long				height = 0;
	grafport_t*			port = pixmap->m_port;

	if( port )
		height = port->height();

	f.push_int( height );
END_STDLIB_FUNCTION

PIXMAP_FUNCTION( pixmap_pitch )
	long				pitch = 0;
	grafport_t*			port = pixmap->m_port;
	
	if( port )
		pitch = port->pitch();

	f.push_int( pitch );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

PIXMAP_CLASS_FUNCTION( g2pixmap_create, pixmap_wrap_t )
	self->pixmap = new pixmap_t( f.m_lib_context );
	self->pixmap->m_instance = instptr;
	self->pixmap->m_gl = 2;
	self->pixmap->m_depth_bits = 0;
END_STDLIB_FUNCTION

PIXMAP_CLASS_FUNCTION( g3pixmap_create, pixmap_wrap_t )
	self->pixmap = new pixmap_t( f.m_lib_context );
	self->pixmap->m_instance = instptr;
	self->pixmap->m_gl = 3;
	self->pixmap->m_depth_bits = 16;
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_pixmap(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "setColor;i", pixmap_set_color_paletted );

	register_native_function(
		m, rootclss, "setColor;i;i;i", pixmap_set_color );

	register_native_function(
		m, rootclss, "setColor;i;i;i;i", pixmap_set_color_alpha );

	register_native_function(
		m, rootclss, "putPixel;i;i", pixmap_put_pixel );

	register_native_function(
		m, rootclss, "getPixel;i;i", pixmap_get_pixel );

	register_native_function(
		m, rootclss, "moveTo;i;i", pixmap_move_to );

	register_native_function(
		m, rootclss, "lineTo;i;i", pixmap_line_to );

	register_native_function(
		m, rootclss, "frameRect;i;i;i;i", pixmap_frame_rect );

	register_native_function(
		m, rootclss, "fillRect;i;i;i;i", pixmap_fill_rect );

	register_native_function(
		m, rootclss, "frameOval;i;i;i;i", pixmap_frame_oval );

	register_native_function(
		m, rootclss, "fillOval;i;i;i;i", pixmap_fill_oval );

	register_native_function(
		m, rootclss, "blit;Pixmap::*;i;i;i;i;i;i", pixmap_blit  );



	register_native_function(
		m, rootclss, "setPixmapPort;Pixmap::*", set_port  );

	register_native_function(
		m, rootclss, "getPixmapPort", get_port );


	class_t*			clss;

	clss = rootclss->find_class( "Pixmap", 6 );
	if( !clss )
		throw_icarus_error( "Pixmap class not found" );

	s_pixmap_class = clss;
		
	register_native_function(
		m, clss, "create", pixmap_create );

	register_native_function(
		m, clss, "destroy", pixmap_destroy );

	register_native_function(
		m, clss, "allocate;i;i;PixelFormat::", pixmap_allocate );

	register_native_function(
		m, clss, "load;.String::&", pixmap_load );
		
	register_native_function(
		m, clss, "save;.String::&", pixmap_save );

	register_native_function(
		m, clss, "release", pixmap_release );

	register_native_function(
		m, clss, "convert;PixelFormat::", pixmap_convert );

	register_native_function(
		m, clss, "pixels", pixmap_pixels );

	register_native_function(
		m, clss, "width", pixmap_width );
		
	register_native_function(
		m, clss, "height", pixmap_height );		

	register_native_function(
		m, clss, "pitch", pixmap_pitch );

	clss = rootclss->find_class( "G2Pixmap", 8 );
	if( !clss )
		throw_icarus_error( "G2Pixmap class not found" );
		
	register_native_function(
		m, clss, "create", g2pixmap_create );

	clss = rootclss->find_class( "G3Pixmap", 8 );
	if( !clss )
		throw_icarus_error( "G3Pixmap class not found" );
		
	register_native_function(
		m, clss, "create", g3pixmap_create );
}

END_MACHINE_NAMESPACE
