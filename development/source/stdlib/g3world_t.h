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
//	g3world_t.h			 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"
#include <set>

#if __VISC__
#include <Windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#elif PROJECT_BUILDER
#import <OpenGL/OpenGL.h>
#import <OpenGL/GLU.h>
#else
#include <gl.h>
#include <glu.h>
#endif

BEGIN_MACHINE_NAMESPACE

class g3world_t {
public:
						g3world_t();
						
	virtual				~g3world_t();
	
	void				reset();

	virtual void		setup_from_fiber() = 0;
	
	virtual void		pause();

	virtual void		stop();

	virtual void		resume();
	
	virtual void		swap() = 0;
	
	virtual void		freeze() = 0;

	virtual void		unfreeze() = 0;
	
	void				set_busy(
							bool				busy );

	bool				busy() const;

	virtual const dimension&	size();
				
	virtual bool		fetch_resize_flag();

	void				uilock();

	void				uiunlock();

// ---------------------------------------------------------------------------

	void				gen_textures(
							GLsizei				n,
							GLuint*				textures );
							
	void				delete_textures(
							GLsizei				n,
							const GLuint*		textures );

	GLUquadric*			new_quadric();
	
	void				delete_quadric(
							GLUquadric*			quadric );

	GLUnurbs*			new_nurbs_renderer();
	
	void				delete_nurbs_renderer(
							GLUnurbs*			nurbs );

	GLUtesselator*		new_tesselator();
	
	void				delete_tesselator(
							GLUtesselator*		tesselator );

	void				cleanup_textures();

	void				cleanup_quadrics();

	void				cleanup_nurbs_renderers();

	void				cleanup_tesselators();
								
protected:
	bool				m_busy;
	bool				m_paused;
	dimension			m_dimension;
	bool				m_resized;
	int					m_resize_request;
	
	std::set<GLuint>	m_textures;
	std::set<GLUquadric*>	m_quadrics;
	std::set<GLUnurbs*>	m_nurbs;
	std::set<GLUtesselator*>	m_tesselators;
};

inline const dimension&
g3world_t::size()
{
	return m_dimension;
}

inline bool
g3world_t::busy() const
{
	return m_busy;
}

END_MACHINE_NAMESPACE
