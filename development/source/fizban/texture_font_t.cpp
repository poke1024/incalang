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
//	font_t.cpp				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include <Windows.h>
#include "texture_font_t.h"
#include "machine_t.h"

BEGIN_FIZBAN_NAMESPACE

texture_font_t::texture_font_t(
	const char*			name )
{
	if( wglGetCurrentContext() == NULL )
		throw_icarus_error( "need an OpenGL context" );

	m_name = istrdup( name );

	istring				path;

	path = name;
	path.append( ".txf" );

	path = machine::machine_t::
		current()->file_path( path.c_str() );

	m_font = txfLoadFont( (char*)path.c_str() );
	if( m_font == NULL )
		icarus_fatal( "font file not found" );

	txfEstablishTexture( m_font, 0, GL_TRUE );

	// ensure best possible texture interpolation
	glTexParameteri( GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR );
}

texture_font_t::~texture_font_t()
{
	txfUnloadFont( m_font );
	ifree( m_name );
}

void
texture_font_t::draw(
	const char*			text )
{
	txfRenderString( m_font, text, (int)strlen( text ) );
}

float
texture_font_t::text_width(
	const char*			text ) const
{
	int					width;
	int					max_ascent;
	int					max_descent;

	txfGetStringMetrics( m_font, text, (int)strlen( text ),
		&width, &max_ascent, &max_descent );

	return width;
}

int
texture_font_t::base_size() const
{
	return m_font->max_ascent + m_font->max_descent;
}

int
texture_font_t::base_ascent() const
{
	return m_font->max_ascent;
}

int
texture_font_t::base_descent() const
{
	return m_font->max_descent;
}

END_FIZBAN_NAMESPACE
