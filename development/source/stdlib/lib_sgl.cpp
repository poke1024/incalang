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
//	lib_sgl.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include <Windows.h>
#include "sgl.h"

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "machine_class_t.h"
#include "lib_point.h"
#include "lib_string.h"
#include "lib_color.h"

BEGIN_MACHINE_NAMESPACE

USING_FIZBAN_NAMESPACE

// ===========================================================================

static void
obtain_sgl_rect(
	fiber_t&			f,
	sgl_rect_t*			r )
{
	r->left = f.pop_float();
	r->top = f.pop_float();
	r->right = f.pop_float();
	r->bottom = f.pop_float();
}

// ===========================================================================

STDLIB_FUNCTION( lib_sgl_clear )
	GLfloat				color[ 4 ];
	glGetFloatv( GL_CURRENT_COLOR, color );
	glClearColor( color[ 0 ], color[ 1 ],
		color[ 2 ], color[ 3 ] );
	glClear( GL_COLOR_BUFFER_BIT );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_set_color_1 )
	point_t*			color = pop_point( f );
	glColor3f( color->x, color->y, color->z );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_set_color_2 )
	GLfloat				r = f.pop_float();
	GLfloat				g = f.pop_float();
	GLfloat				b = f.pop_float();
	glColor3f( r, g, b );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_set_color_3 )
	string_t*			s = pop_string( f );
	const char*			t = string_get_c_str( s );
	if( t )
	{
		u08				col[ 3 ];
		if( retrieve_color_by_name( t, col ) )
			glColor3ubv( col );
	}
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_set_indexed_color )
	long				index = f.pop_int();
	u08					col[ 3 ];
	retrieve_color_by_index( index, col );
	glColor3ubv( col );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_move_to )
	GLfloat				x = f.pop_float();
	GLfloat				y = f.pop_float();
	sgl_move_to( x, y );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_line_to )
	GLfloat				x = f.pop_float();
	GLfloat				y = f.pop_float();
	sgl_line_to( x, y );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_frame_rect )
	sgl_rect_t			r;
	obtain_sgl_rect( f, &r );
	sgl_frame_rect( &r );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_fill_rect )
	sgl_rect_t			r;
	obtain_sgl_rect( f, &r );
	sgl_fill_rect( &r );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_frame_oval )
	sgl_rect_t			r;
	obtain_sgl_rect( f, &r );
	sgl_frame_oval( &r );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_fill_oval )
	sgl_rect_t			r;
	obtain_sgl_rect( f, &r );
	sgl_fill_oval( &r );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_set_font )
	string_t*			s = pop_string( f );
	const char*			t = string_get_c_str( s );
	if( t )
		sgl_set_font( t );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_set_font_size )
	float				size = f.pop_float();
	sgl_set_font_size( size );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_get_font_size )
	f.push_float( sgl_get_font_size() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_get_font_ascent )
	f.push_float( sgl_get_font_ascent() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_get_font_descent )
	f.push_float( sgl_get_font_descent() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_text_width )
	string_t*			s = pop_string( f );
	const char*			t = string_get_c_str( s );
	if( t )
		f.push_float( sgl_text_width( t ) );
	else
		f.push_float( 0 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_draw_text )
	string_t*			s = pop_string( f );
	const char*			t = string_get_c_str( s );
	if( t )
		sgl_draw_text( t );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_load_bitmap_font )
	string_t*			s = pop_string( f );
	const char*			t = string_get_c_str( s );
	long				size = f.pop_int();
	if( t )
		sgl_load_bitmap_font( t, size );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_load_texture_font )
	string_t*			s = pop_string( f );
	const char*			t = string_get_c_str( s );
	if( t )
		sgl_load_texture_font( t );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( lib_sgl_flush )
	sgl_flush();
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

/*
native void sglClear();
native void sglSetColor( const Color& );
native void sglSetColor( float, float, float );
native void sglSetColor( const String& );
native void sglSetIndexedColor( int );
native void sglMoveTo( float, float );
native void sglLineTo( float, float );
native void sglFrameRect( float, float, float, float );
native void sglFillRect( float, float, float, float );
native void sglFillOval( float, float, float, float );
native void sglFrameOval( float, float, float, float );
native void sglSetFont( const String& );
native void sglSetFontSize( float );
native float sglGetFontSize();
native float sglGetFontAscent();
native float sglGetFontDescent();
native float sglTextWidth( const String& );
native void sglDrawText( const String& );
native void sglLoadBitmapFont( const String&, int );
native void sglLoadTextureFont( const String& );
native void sglFlush();
*/

void
init_lib_sgl(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "sglClear",
		lib_sgl_clear );

	register_native_function(
		m, rootclss, "sglSetColor;.Color::&",
		lib_sgl_set_color_1 );

	register_native_function(
		m, rootclss, "sglSetColor;f;f;f",
		lib_sgl_set_color_2 );

	register_native_function(
		m, rootclss, "sglSetColor;.String::&",
		lib_sgl_set_color_3 );

	register_native_function(
		m, rootclss, "sglSetIndexedColor;i",
		lib_sgl_set_indexed_color );

	register_native_function(
		m, rootclss, "sglMoveTo;f;f",
		lib_sgl_move_to );

	register_native_function(
		m, rootclss, "sglLineTo;f;f",
		lib_sgl_line_to );

	register_native_function(
		m, rootclss, "sglFrameRect;f;f;f;f",
		lib_sgl_frame_rect );

	register_native_function(
		m, rootclss, "sglFillRect;f;f;f;f",
		lib_sgl_fill_rect );

	register_native_function(
		m, rootclss, "sglFrameOval;f;f;f;f",
		lib_sgl_frame_oval );

	register_native_function(
		m, rootclss, "sglFillOval;f;f;f;f",
		lib_sgl_fill_oval );

	register_native_function(
		m, rootclss, "sglSetFont;.String::&",
		lib_sgl_set_font );

	register_native_function(
		m, rootclss, "sglSetFontSize;f",
		lib_sgl_set_font_size );
	
	register_native_function(
		m, rootclss, "sglGetFontSize",
		lib_sgl_get_font_size );

	register_native_function(
		m, rootclss, "sglGetFontAscent",
		lib_sgl_get_font_ascent );

	register_native_function(
		m, rootclss, "sglGetFontDescent",
		lib_sgl_get_font_descent );

	register_native_function(
		m, rootclss, "sglTextWidth;.String::&",
		lib_sgl_text_width );

	register_native_function(
		m, rootclss, "sglDrawText;.String::&",
		lib_sgl_draw_text );

	register_native_function(
		m, rootclss, "sglLoadBitmapFont;.String::&;i",
		lib_sgl_load_bitmap_font );

	register_native_function(
		m, rootclss, "sglLoadTextureFont;.String::&",
		lib_sgl_load_texture_font );

	register_native_function(
		m, rootclss, "sglFlush",
		lib_sgl_flush );
}

END_MACHINE_NAMESPACE
