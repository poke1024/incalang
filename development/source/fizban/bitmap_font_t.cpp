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
//	bitmap_font_t.cpp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include <Windows.h>
#include "bitmap_font_t.h"
#include "screen_t.h"

BEGIN_FIZBAN_NAMESPACE

bitmap_font_t::bitmap_font_t(
	const char*			name,
	int					size )
{
	if( wglGetCurrentContext() == NULL )
		throw_icarus_error( "need an OpenGL context" );

	m_font_list_id = glGenLists( MAX_CHARS );

	m_font = CreateFont(
		-size, 0, 0, 0, 0 /*FW_BOLD*/, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH, name );
	if( !m_font )
		icarus_fatal( "could not create font" );

	m_name = istrdup( name );
	m_size = size;
		
	HFONT				hOldFont;
	HDC					hDC = working_dc();

	hOldFont = (HFONT)SelectObject( hDC, m_font );

	GetTextMetrics(
		hDC, &m_metrics );

	wglUseFontBitmaps(
		hDC, 0, MAX_CHARS - 1, m_font_list_id );

	if( !GetCharWidth32( hDC, 0, MAX_CHARS - 1, m_widths ) )
		icarus_fatal( "couldn't retrieve font character widths" );

	SelectObject( hDC, hOldFont );
}

bitmap_font_t::~bitmap_font_t()
{
	DeleteObject( m_font );
	glDeleteLists( m_font_list_id, MAX_CHARS );
	ifree( m_name );
}

void
bitmap_font_t::draw(
	int					x,
	int					y,
	const char*			text )
{
	position_text( x, y );

	glPushAttrib( GL_LIST_BIT );
	glListBase( m_font_list_id );
	glCallLists( strlen( text ),
		GL_UNSIGNED_BYTE, text );
	glPopAttrib();
}

float
bitmap_font_t::text_width(
	const char*			text ) const
{
	unsigned char		c;
	float				w = 0;
	
	while( true )
	{
		c = *text++;
		if( c == '\0' )
			break;
		w += m_widths[ c ];
	}

	return w;
}

int
bitmap_font_t::size() const
{
	return m_size;
}

int
bitmap_font_t::ascent() const
{
	return m_metrics.tmAscent;
}

int
bitmap_font_t::descent() const
{
	return m_metrics.tmDescent;
}

void
bitmap_font_t::position_text( 
	int					x,
	int					y )
{
	gl_context_t*		context = gl_context_t::current();

	if( !context )
		return;

	glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	y = context->height() - m_size - y;

	glViewport( x - 1, y - 1, 0, 0 );

	glRasterPos4f( 0, 0, 0, 1 );

	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glPopAttrib();
}

END_FIZBAN_NAMESPACE
