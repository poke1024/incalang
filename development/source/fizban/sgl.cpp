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
//	sgl.cpp					   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

// simple graphics layer

// note: if you mix sgl and OpenGL calls, don't forget to call sgl_flush()
// before calling OpenGL, or strange things might happen.

#include <Windows.h>
#include "sgl.h"
#include <math.h>
#include "gl_context_t.h"
#include "font_t.h"
#include "bitmap_font_t.h"
#include "texture_font_t.h"
#include <stdio.h>

BEGIN_FIZBAN_NAMESPACE

const double			M_PI = pi;
const double			M_2PI = 2 * pi;

// ===========================================================================

void
sgl_context_clear(
	sgl_context_t*		context )
{
	memset( context, 0, sizeof( sgl_context_t ) );
}

void
sgl_context_init(
	sgl_context_t*		context )
{
	context->x = 0;
	context->y = 0;
	context->z = 0;

	context->line_strip = false;
	context->text_mode = false;

	context->font_angle = 0;
	context->font_size = 12;
	memset( context->font_name, 0, 64 );
	strcpy( context->font_name, "Helvetica" );
	context->font_dirty = true;

	context->bitmap_font = 0;
	context->texture_font = 0;

	context->bitmap_font_alloc = 8;
	context->bitmap_font_count = 0;
	context->bitmap_font_cache = (bitmap_font_t**)imalloc(
		context->bitmap_font_alloc * sizeof( bitmap_font_t* ) );

	context->texture_font_alloc = 8;
	context->texture_font_count = 0;
	context->texture_font_cache = (texture_font_t**)imalloc(
		context->texture_font_alloc * sizeof( texture_font_t* ) );

	sgl_load_bitmap_font( "Helvetica", 12 );
}

void
sgl_context_cleanup(
	sgl_context_t*		context )
{
	for( int i = 0; i < context->bitmap_font_count; i++ )
	{
		delete context->bitmap_font_cache[ i ];
		context->bitmap_font_cache[ i ] = 0;
	}

	ifree( context->bitmap_font_cache );
	context->bitmap_font_cache = 0;

	for( int i = 0; i < context->texture_font_count; i++ )
	{
		delete context->texture_font_cache[ i ];
		context->texture_font_cache[ i ] = 0;
	}

	ifree( context->texture_font_cache );
	context->texture_font_cache = 0;
}

inline sgl_context_t*
obtain_sgl_context()
{
	return gl_context_t::current()->sgl_context();
}

bool
operator==(
	const sgl_color_t&	col1,
	const sgl_color_t&	col2 )
{
	if( col1.type != col2.type )
		return false;

	if( col1.type == SGL_COLOR_INDEXED )
		return col1.index == col2.index;

	for( int i = 0; i < 4; i++ )
		if( col1.rgba[ i ] != col2.rgba[ i ] )
			return false;

	return true;
}

// ===========================================================================

void
sgl_set_rect(
	sgl_rect_t*			r,
	GLfloat				left,
	GLfloat				top,
	GLfloat				right,
	GLfloat				bottom )
{
	r->left = left;
	r->top = top;
	r->right = right;
	r->bottom = bottom;
}

void
sgl_frame_rect(
	const sgl_rect_t*	r )
{
	sgl_flush();
	
	glBegin( GL_LINE_STRIP );
	glVertex2f( r->left, r->top );
	glVertex2f( r->right, r->top );
	glVertex2f( r->right, r->bottom );
	glVertex2f( r->left, r->bottom );
	glVertex2f( r->left, r->top );
	glEnd();
}

void
sgl_fill_rect(
	const sgl_rect_t*	r )
{
	sgl_flush();
	
	glBegin( GL_QUADS );
	glVertex2f( r->left, r->top );
	glVertex2f( r->right, r->top );
	glVertex2f( r->right, r->bottom );
	glVertex2f( r->left, r->bottom );
	glEnd();
}

static void
sgl_regular_polygon(
	const sgl_rect_t*	r,
	int					n,
	bool				close )
{
	const float			cx = 0.5 * ( r->left + r->right );
	const float			cy = 0.5 * ( r->bottom + r->top );
	const float			sx = 0.5 * ( r->right - r->left );
	const float			sy = 0.5 * ( r->bottom - r->top );

	const float			x0 = cx + sx;
	const float			y0 = cy;

	glVertex2f( x0, y0 );

	for( int i = 1; i < n; i++ )
	{
		float			angle;
		float			x, y;

		angle = ( i * M_2PI ) / n;
		x = cos( angle );
		y = sin( angle );
		glVertex2f( sx * x + cx, sy * y + cy );
	}

	if( close )
		glVertex2f( x0, y0 );
}

static void
sgl_regular_arc(
	const sgl_rect_t*	r,
	int					n,
	bool				close,
	float				alpha = 0,
	float				beta = M_2PI )
{
	const float			cx = 0.5 * ( r->left + r->right );
	const float			cy = 0.5 * ( r->bottom + r->top );
	const float			sx = 0.5 * ( r->right - r->left );
	const float			sy = 0.5 * ( r->bottom - r->top );

	glVertex2f( cx, cy );

	for( int i = 0; i < n; i++ )
	{
		float			angle;
		float			x, y;

		angle = ( i * beta ) / n + alpha;
		x = cos( angle );
		y = sin( angle );
		glVertex2f( sx * x + cx, sy * y + cy );
	}

	if( close )
		glVertex2f( cx, cy );
}

inline long
sqr(
	long				x )
{
	return x * x;
}

static long
sgl_estimate_oval(
	const sgl_rect_t*	r,
	float				f = 1.0 )
{
	const float			sx = 0.5 * ( r->right - r->left );
	const float			sy = 0.5 * ( r->bottom - r->top );
	float				a = max( sx, sy );
	float				b = min( sx, sy );
	float				u = M_PI * ( 3.0 / 2.0 * ( a + b ) - sqrt( a * b ) );

	return (long)min( 1024, f * ( u / 6 ) );
}

void
sgl_frame_oval(
	const sgl_rect_t*	r )
{
	sgl_frame_regular_polygon( r,
		sgl_estimate_oval( r ) );
}

void
sgl_fill_oval(
	const sgl_rect_t*	r )
{
	sgl_fill_regular_polygon( r,
		sgl_estimate_oval( r ) );
}

void
sgl_frame_arc(
	const sgl_rect_t*	r,
	float				alpha,
	float				beta )
{
	if( beta <= 0 )
		return;

	if( beta > M_2PI )
	{
		beta = M_2PI;
		alpha = 0;
	}

	sgl_flush();
	
	glBegin( GL_LINE_STRIP );
	sgl_regular_arc( r,
		sgl_estimate_oval( r, beta / M_2PI ),
		true, alpha, beta );
	glEnd();
}

void
sgl_fill_arc(
	const sgl_rect_t*	r,
	float				alpha,
	float				beta )
{
	if( beta <= 0 )
		return;

	if( beta > M_2PI )
	{
		beta = M_2PI;
		alpha = 0;
	}

	sgl_flush();
	
	glBegin( GL_POLYGON );
	sgl_regular_arc( r,
		sgl_estimate_oval( r, beta / M_2PI ),
		false, alpha, beta );
	glEnd();
}

void
sgl_frame_regular_polygon(
	const sgl_rect_t*	r,
	int					n )
{
	sgl_flush();
	
	glBegin( GL_LINE_STRIP );
	sgl_regular_polygon( r, n, true );
	glEnd();
}

void
sgl_fill_regular_polygon(
	const sgl_rect_t*	r,
	int					n )
{
	sgl_flush();
	
	glBegin( GL_POLYGON );
	sgl_regular_polygon( r, n, false );
	glEnd();
}

void
sgl_move_to(
	GLfloat				x,
	GLfloat				y )
{
	sgl_context_t*		context = obtain_sgl_context();

	if( context->line_strip )
		sgl_flush();

	context->x = x;
	context->y = y;
}

void
sgl_line_to(
	GLfloat				x,
	GLfloat				y )
{
	sgl_context_t*		context = obtain_sgl_context();

	if( not context->line_strip )
	{
		sgl_flush( SGL_MODE_LINE );
		glBegin( GL_LINE_STRIP );
		glVertex2f( context->x, context->y );
		context->line_strip = true;
	}

	glVertex2f( x, y );
}

void
sgl_put_point(
	GLfloat				x,
	GLfloat				y )
{
	sgl_flush();

	glBegin( GL_POINTS );
	glVertex2f( x, y );
	glEnd();
}

void
sgl_lines(
	const GLfloat*		v,
	long				n )
{
	sgl_flush();

	glBegin( GL_LINES );
	for( long i = 0; i < n; i++ )
	{
		glVertex2fv( v );
		v += 2;
		glVertex2fv( v );
		v += 2;
	}
	glEnd();
}

void
sgl_flush(
	int					mode )
{
	sgl_context_t*		context = obtain_sgl_context();

	if( context->line_strip )
	{
		glEnd();
		if( mode != SGL_MODE_LINE )
			context->line_strip = false;
	}

	if( context->text_mode && mode != SGL_MODE_TEXT )
	{
		//glDisable( GL_TEXTURE_2D );
		//glDisable( GL_BLEND );
		context->text_mode = false;
	}
}

sgl_color_t
sgl_get_color()
{
	sgl_color_t			color;

	color.type = SGL_COLOR_RGBA;
	glGetFloatv( GL_CURRENT_COLOR, color.rgba );

	return color;
}

void
sgl_set_color(
	sgl_color_t			color )
{
	if( color.type == SGL_COLOR_RGBA )
		glColor4fv( color.rgba );
	else
		;
		//sgl_set_indexed_color( color.index );
}

// ---------------------------------------------------------------------------

void
sgl_load_bitmap_font(
	const char*			name,
	int					size )
{
	sgl_context_t*		context = obtain_sgl_context();
	bitmap_font_t*		font;

	for( int i = 0; i < context->bitmap_font_count; i++ )
	{
		font = context->bitmap_font_cache[ i ];
	
		if( !font )
			continue;

		if( strcmp( font->name(), name ) != 0 )
			continue;

		if( font->size() != size )
			continue;

		return;
	}

	if( context->bitmap_font_count >=
		context->bitmap_font_alloc )
	{
		context->bitmap_font_alloc *= 2;
		context->bitmap_font_cache = (bitmap_font_t**)irealloc(
			context->bitmap_font_cache,
			context->bitmap_font_alloc * sizeof( bitmap_font_t* ) );
	}

	font = new bitmap_font_t( name, size );

	context->bitmap_font_cache[
		context->bitmap_font_count++ ] = font;
}

void
sgl_load_texture_font(
	const char*			name )
{
	sgl_context_t*		context = obtain_sgl_context();
	texture_font_t*		font;

	for( int i = 0; i < context->texture_font_count; i++ )
	{
		font = context->texture_font_cache[ i ];
	
		if( !font )
			continue;

		if( strcmp( font->name(), name ) != 0 )
			continue;

		return;
	}

	if( context->texture_font_count >=
		context->texture_font_alloc )
	{
		context->texture_font_alloc *= 2;
		context->texture_font_cache = (texture_font_t**)irealloc(
			context->texture_font_cache,
			context->texture_font_alloc * sizeof( texture_font_t* ) );
	}

	font = new texture_font_t( name );

	context->texture_font_cache[
		context->texture_font_count++ ] = font;
}

static bool
sgl_activate_bitmap_font()
{
	sgl_context_t*		context = obtain_sgl_context();
	bitmap_font_t*		font;

	for( int i = 0; i < context->bitmap_font_count; i++ )
	{
		font = context->bitmap_font_cache[ i ];
	
		if( !font )
			continue;

		if( strcmp( font->name(), context->font_name ) != 0 )
			continue;

		if( font->size() != (int)context->font_size )
			continue;

		context->bitmap_font = font;
		context->texture_font = 0;

		memmove(
			&context->bitmap_font_cache[ 1 ],
			&context->bitmap_font_cache[ 0 ],
			i * sizeof( font_t* ) );
		context->bitmap_font_cache[ 0 ] = font;
		return true;
	}

	return false;
}

static bool
sgl_activate_texture_font()
{
	sgl_context_t*		context = obtain_sgl_context();
	texture_font_t*		font;

	for( int i = 0; i < context->texture_font_count; i++ )
	{
		font = context->texture_font_cache[ i ];
	
		if( !font )
			continue;

		if( strcmp( font->name(), context->font_name ) != 0 )
			continue;

		context->bitmap_font = 0;
		context->texture_font = font;

		memmove(
			&context->texture_font_cache[ 1 ],
			&context->texture_font_cache[ 0 ],
			i * sizeof( font_t* ) );
		context->texture_font_cache[ 0 ] = font;
		return true;
	}

	return false;
}

static void
sgl_activate_font()
{
	sgl_context_t*		context = obtain_sgl_context();

	context->font_dirty = false;

	if( context->font_angle == 0 )
	{
		if( sgl_activate_bitmap_font() )
			return;
	}

	if( sgl_activate_texture_font() )
		return;

	icarus_fatal( "requested font has not been loaded" );
}

void
sgl_set_font(
	const char*			name )
{
	sgl_context_t*		context = obtain_sgl_context();

	if( strcmp( context->font_name, name ) == 0 )
		return;
	strncpy( context->font_name, name, 63 );
	context->font_dirty = true;
}

void
sgl_set_font_size(
	float				size )
{
	sgl_context_t*		context = obtain_sgl_context();

	context->font_size = size;
	context->font_dirty = true;
}

float
sgl_get_font_size()
{
	sgl_context_t*		context = obtain_sgl_context();
	return context->font_size;
}

float
sgl_get_font_ascent()
{
	sgl_context_t*		context = obtain_sgl_context();

	if( context->font_dirty )
		sgl_activate_font();

	texture_font_t*		tfont = context->texture_font;

	if( tfont )
	{
		return tfont->base_ascent() * (
			context->font_size / tfont->base_size() );
	}

	return context->bitmap_font->ascent();
}

float
sgl_get_font_descent()
{
	sgl_context_t*		context = obtain_sgl_context();

	if( context->font_dirty )
		sgl_activate_font();

	texture_font_t*		tfont = context->texture_font;

	if( tfont )
	{
		return tfont->base_descent() * (
			context->font_size / tfont->base_size() );
	}

	return context->bitmap_font->descent();
}

void
sgl_set_font_angle(
	float				angle )
{
	sgl_context_t*		context = obtain_sgl_context();
	context->font_angle = angle;
	context->font_dirty = true;
}

float
sgl_text_width(
	const char*			text )
{
	sgl_context_t*		context = obtain_sgl_context();

	if( context->font_dirty )
		sgl_activate_font();

	if( context->bitmap_font )
		return context->bitmap_font->text_width( text );

	if( context->texture_font )
		return context->texture_font->text_width( text ) *
			( context->font_size / context->texture_font->base_size() );

	return 0;
}

static void
sgl_draw_bitmap_text(
	float				x,
	float				y,
	const char*			text )
{
	y -= 2; // magic correction

	sgl_context_t*		context = obtain_sgl_context();

	context->bitmap_font->draw( x,
		y - context->font_size, text );
}

static void
sgl_draw_texture_text(
	float				x,
	float				y,
	const char*			text )
{
	y += 2; // magic correction

	sgl_context_t*		context = obtain_sgl_context();

	context->text_mode = true;

	bool				tex2d;
	bool				blend;
	GLint				blend_src;
	GLint				blend_dst;

	tex2d = glIsEnabled( GL_TEXTURE_2D );
	blend = glIsEnabled( GL_BLEND );
	glGetIntegerv( GL_BLEND_SRC, &blend_src );
	glGetIntegerv( GL_BLEND_DST, &blend_dst );

	if( not tex2d )
		glEnable( GL_TEXTURE_2D );
	if( not blend )
		glEnable( GL_BLEND );
	if( blend_src != GL_SRC_ALPHA ||
		blend_dst != GL_ONE_MINUS_SRC_ALPHA )
	{
		glBlendFunc( GL_SRC_ALPHA,
			GL_ONE_MINUS_SRC_ALPHA );
	}

	float				scale;	

	scale = context->font_size / context->texture_font->base_size();

	float				phi = context->font_angle * ( M_PI / 180.0 );

	glPushMatrix();
	//glLoadIdentity();
	glTranslatef( x - cos( phi ) * 3, y + sin( phi ) * 3, 0 );
	glScalef( scale, -scale, 1 );
	glRotatef( context->font_angle, 0, 0, 1 );
	context->texture_font->draw( text );
	glPopMatrix();

	if( not tex2d )
		glDisable( GL_TEXTURE_2D );
	if( not blend )
		glDisable( GL_BLEND );
	if( blend_src != GL_SRC_ALPHA ||
		blend_dst != GL_ONE_MINUS_SRC_ALPHA )
	{
		glBlendFunc( blend_src,
			blend_dst );
	}
}

void
sgl_draw_text(
	const char*			text )
{
	sgl_context_t*		context = obtain_sgl_context();

	sgl_flush( SGL_MODE_TEXT );

	if( context->font_dirty )
		sgl_activate_font();

	const GLfloat		x = context->x;
	const GLfloat		y = context->y;

	if( context->bitmap_font )
		sgl_draw_bitmap_text( x, y, text );
	else if( context->texture_font )
		sgl_draw_texture_text( x, y, text );

	context->x += sgl_text_width( text );
}

void
sgl_printf(
	const char*			format,
	... )
{
	char				text[ 256 ];
	va_list				arguments;

	if( format == NULL )
		return;

	va_start( arguments, format );
	_vsnprintf( text, 256, format, arguments );
	va_end( arguments );

	sgl_draw_text( text );
}

// ---------------------------------------------------------------------------

void
sgl_init()
{
}

void
sgl_cleanup()
{
}

END_FIZBAN_NAMESPACE
