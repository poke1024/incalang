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
//	gl_context_t.cpp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "gl_context_t.h"
#include <gl/glu.h>

BEGIN_FIZBAN_NAMESPACE

static
gl_context_t* s_current_context = 0;

gl_context_t::gl_context_t()
{
	clear();
}

gl_context_t::~gl_context_t()
{
	release();
}

void
gl_context_t::make_current()
{
	if( m_hRC == wglGetCurrentContext() )
		return;

	if( m_hDC && m_hRC && !wglMakeCurrent( m_hDC, m_hRC ) )
	   throw_icarus_error( "cannot activate gl rendering context" );

	s_current_context = this;
}

gl_context_t*
gl_context_t::current()
{
	return s_current_context;
}

void
gl_context_t::init(
	HDC					hDC,
	HGLRC				hRC  )
{
	m_hDC = hDC;
	m_hRC = hRC;

	make_current();
	sgl_context_init( &m_sgl_context );
}

void
gl_context_t::release()
{
	if( m_hRC )
	{
		if( m_hRC == wglGetCurrentContext() )
			wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_hRC );
		m_hRC = 0;
	}

	sgl_context_cleanup( &m_sgl_context );

	if( s_current_context == this )
		s_current_context = 0;

	clear();
}

void
gl_context_t::setup_viewport(
	long				width,
	long				height,
	bool				is_2d )
{
	if( height == 0 )
		height = 1;

	m_width = width;
	m_height = height;

	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if( is_2d )
	{
		glOrtho( 0, width, height, 0, -1, 1 );
	}
	else
	{
		gluPerspective( 45.0f,
			(GLfloat)width / (GLfloat)height, 0.1f, 100.0f );
	}

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void
gl_context_t::clear()
{
	m_hDC = NULL;
	m_hRC = NULL;
	sgl_context_clear( &m_sgl_context );
	m_width = 0;
	m_height = 0;
}

END_FIZBAN_NAMESPACE
