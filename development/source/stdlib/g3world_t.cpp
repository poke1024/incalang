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
//	g3world_t.cp		 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "g3world_t.h"

#include "thread_t.h"
#include "os_core.h"

BEGIN_MACHINE_NAMESPACE

g3world_t::g3world_t() :
	m_busy( false )
{
	m_dimension.width = 0;
	m_dimension.height = 0;
	m_resized = true;
	m_resize_request = 0;
	m_paused = false;
}

g3world_t::~g3world_t()
{
}

void
g3world_t::pause()
{
}

void
g3world_t::stop()
{
	m_paused = true;
}

void
g3world_t::resume()
{
	m_paused = false;
}

void
g3world_t::reset()
{
	resume();

	cleanup_textures();
	cleanup_quadrics();
	cleanup_nurbs_renderers();
	cleanup_tesselators();
	
	freeze();
}

void
g3world_t::set_busy(
	bool					busy )
{
	while( m_busy == false && busy && m_resize_request )
		ithread_t::yield();

	m_busy = busy;
}

bool
g3world_t::fetch_resize_flag()
{
	bool				flag = m_resized;
	
	m_resized = false;
	return flag;
}

void
g3world_t::uilock()
{
	m_resize_request++;

	long				count = 0;

	while( m_busy && not m_paused && count < 4 )
	{
		ithread_t::yield();
		count++;
	}
}

void
g3world_t::uiunlock()
{
	m_resize_request--;
}

// ---------------------------------------------------------------------------

void
g3world_t::gen_textures(
	GLsizei				n,
	GLuint*				textures )
{
	glGenTextures( n, textures );
	
	for( long i = 0; i < n; i++ )
		m_textures.insert( textures[ i ] );
}
							
void
g3world_t::delete_textures(
	GLsizei				n,
	const GLuint*		textures )
{
	for( long i = 0; i < n; i++ )
		m_textures.erase( textures[ i ] );

	glDeleteTextures( n, textures );
}

GLUquadric*
g3world_t::new_quadric()
{
	GLUquadric*			quadric = gluNewQuadric();
	
	m_quadrics.insert( quadric );
	
	return quadric;
}	

void
g3world_t::delete_quadric(
	GLUquadric*			quadric )
{
	gluDeleteQuadric( quadric );
	m_quadrics.erase( quadric );
}

GLUnurbs*
g3world_t::new_nurbs_renderer()
{
	GLUnurbs*			nurbs = gluNewNurbsRenderer();
	
	m_nurbs.insert( nurbs );
	
	return nurbs;
}	

void
g3world_t::delete_nurbs_renderer(
	GLUnurbs*			nurbs )
{
	gluDeleteNurbsRenderer( nurbs );
	m_nurbs.erase( nurbs );
}

GLUtesselator*
g3world_t::new_tesselator()
{
	GLUtesselator*		tesselator = gluNewTess();
	
	m_tesselators.insert( tesselator );
	
	return tesselator;
}	

void
g3world_t::delete_tesselator(
	GLUtesselator*		tesselator )
{
	gluDeleteTess( tesselator );
	m_tesselators.erase( tesselator );
}

// ---------------------------------------------------------------------------

void
g3world_t::cleanup_textures()
{
	std::set<GLuint>::iterator	it;
	
	it = m_textures.begin();
	while( it != m_textures.end() )
	{
		GLuint				tex = *it;
		
		glDeleteTextures( 1, &tex );
		it++;
	}
	
	m_textures.clear();
}

void
g3world_t::cleanup_quadrics()
{
	std::set<GLUquadric*>::iterator	it;
	
	it = m_quadrics.begin();
	while( it != m_quadrics.end() )
	{
		gluDeleteQuadric( *it );
		it++;
	}
	
	m_quadrics.clear();
}

void
g3world_t::cleanup_nurbs_renderers()
{
	std::set<GLUnurbs*>::iterator	it;
	
	it = m_nurbs.begin();
	while( it != m_nurbs.end() )
	{
		gluDeleteNurbsRenderer( *it );
		it++;
	}
	
	m_nurbs.clear();
}

void
g3world_t::cleanup_tesselators()
{
	std::set<GLUtesselator*>::iterator	it;
	
	it = m_tesselators.begin();
	while( it != m_tesselators.end() )
	{
		gluDeleteTess( *it );
		it++;
	}
	
	m_tesselators.clear();
}

END_MACHINE_NAMESPACE
