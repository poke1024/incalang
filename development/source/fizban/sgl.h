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
//	sgl.h					   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

// simple graphics layer

#pragma once

#include "fizban.h"
#include <gl/gl.h>

BEGIN_FIZBAN_NAMESPACE

class bitmap_font_t;
class texture_font_t;

struct sgl_context_t {
	GLfloat				x;
	GLfloat				y;
	GLfloat				z;

	bool				line_strip;
	bool				text_mode;

	float				font_size;
	float				font_angle;
	char				font_name[ 64 ];
	bool				font_dirty;

	bitmap_font_t**		bitmap_font_cache;
	int					bitmap_font_count;
	int					bitmap_font_alloc;

	texture_font_t**	texture_font_cache;
	int					texture_font_count;
	int					texture_font_alloc;

	bitmap_font_t*		bitmap_font;
	texture_font_t*		texture_font;
};

void
sgl_context_clear(
	sgl_context_t*		context );

void
sgl_context_init(
	sgl_context_t*		context );

void
sgl_context_cleanup(
	sgl_context_t*		context );

struct sgl_point_t {
	GLfloat				x;
	GLfloat				y;
};

struct sgl_rect_t {
	GLfloat				left;
	GLfloat				top;
	GLfloat				right;
	GLfloat				bottom;
};

enum {
	SGL_MODE_NONE,
	SGL_MODE_LINE,
	SGL_MODE_TEXT
};

enum sgl_color_type_t {
	SGL_COLOR_RGBA,
	SGL_COLOR_INDEXED
};

struct sgl_color_t {
	sgl_color_type_t	type;
	union {
		int				index;
		GLfloat			rgba[ 4 ];
	};
};

bool
operator==(
	const sgl_color_t&	col1,
	const sgl_color_t&	col2 );

void
sgl_set_rect(
	sgl_rect_t*			r,
	GLfloat				left,
	GLfloat				top,
	GLfloat				right,
	GLfloat				bottom );

void
sgl_frame_rect(
	const sgl_rect_t*	r );

void
sgl_fill_rect(
	const sgl_rect_t*	r );

void
sgl_frame_oval(
	const sgl_rect_t*	r );

void
sgl_fill_oval(
	const sgl_rect_t*	r );

void
sgl_frame_arc(
	const sgl_rect_t*	r,
	float				alpha,
	float				beta );

void
sgl_fill_arc(
	const sgl_rect_t*	r,
	float				alpha,
	float				beta );

void
sgl_frame_regular_polygon(
	const sgl_rect_t*	r,
	int					n );

void
sgl_fill_regular_polygon(
	const sgl_rect_t*	r,
	int					n );

void
sgl_move_to(
	GLfloat				x,
	GLfloat				y );

void
sgl_line_to(
	GLfloat				x,
	GLfloat				y );

void
sgl_put_point(
	GLfloat				x,
	GLfloat				y );

void
sgl_flush(
	int					mode = SGL_MODE_NONE );

sgl_color_t
sgl_get_color();

void
sgl_set_color(
	sgl_color_t			color );

void
sgl_load_bitmap_font(
	const char*			name,
	int					size );

void
sgl_load_texture_font(
	const char*			name );

void
sgl_set_font(
	const char*			name );

void
sgl_set_font_size(
	float				size );

float
sgl_get_font_size();

float
sgl_get_font_ascent();

float
sgl_get_font_descent();

void
sgl_set_font_angle(
	float				angle );

float
sgl_text_width(
	const char*			text );

void
sgl_draw_text(
	const char*			text );

void
sgl_printf(
	const char*			format,
	... );

void
sgl_init();

void
sgl_cleanup();

END_FIZBAN_NAMESPACE
