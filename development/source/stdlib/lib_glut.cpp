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
//	lib_glut.cp				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "vmutils.h"
#include "fiber_t.h"

#if PROJECT_BUILDER
#include <GLUT/glut.h>
#define ENABLE_GLUT_SUPPORT 1
#elif __VISC__
#define ENABLE_GLUT_SUPPORT 0
#else
#include <glut.h>
#define ENABLE_GLUT_SUPPORT 1
#endif

BEGIN_MACHINE_NAMESPACE

#if ENABLE_GLUT_SUPPORT

inline void*
get_glut_font( void* font )
{
	static void* table[ 7 ] = {
		GLUT_BITMAP_9_BY_15,
		GLUT_BITMAP_8_BY_13,
		GLUT_BITMAP_TIMES_ROMAN_10,
		GLUT_BITMAP_TIMES_ROMAN_24,
		GLUT_BITMAP_HELVETICA_10,
		GLUT_BITMAP_HELVETICA_12,
		GLUT_BITMAP_HELVETICA_18 };
		
	unsigned long index = (long)font;
	
	if( index >= 7 )
		return font;
	
	return table[ index ];
}

STDLIB_FUNCTION( glut_bitmap_character )
	void* font = f.pop_ptr();
	int character = f.pop_int();
	glutBitmapCharacter( get_glut_font( font ), character );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_bitmap_width )
	void* font = f.pop_ptr();
	int character = f.pop_int();
	f.push_int( glutBitmapWidth( get_glut_font( font ), character ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_stroke_character )
	void* font = f.pop_ptr();
	int character = f.pop_int();
	glutStrokeCharacter( get_glut_font( font ), character );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_stroke_width )
	void* font = f.pop_ptr();
	int character = f.pop_int();
	f.push_int( glutStrokeWidth( get_glut_font( font ), character ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_wire_sphere )
	GLdouble radius = f.pop_double();
	GLint slices = f.pop_int();
	GLint stacks = f.pop_int();
	glutWireSphere( radius, slices, stacks );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_solid_sphere )
	GLdouble radius = f.pop_double();
	GLint slices = f.pop_int();
	GLint stacks = f.pop_int();
	glutSolidSphere( radius, slices, stacks );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_wire_cone )
	GLdouble base = f.pop_double();
	GLdouble height = f.pop_double();
	GLint slices = f.pop_int();
	GLint stacks = f.pop_int();
	glutWireCone( base, height, slices, stacks );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_solid_cone )
	GLdouble base = f.pop_double();
	GLdouble height = f.pop_double();
	GLint slices = f.pop_int();
	GLint stacks = f.pop_int();
	glutSolidCone( base, height, slices, stacks );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_wire_cube )
	GLdouble size = f.pop_double();
	glutWireCube( size );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_solid_cube )
	GLdouble size = f.pop_double();
	glutSolidCube( size );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_wire_torus )
	GLdouble innerRadius = f.pop_double();
	GLdouble outerRadius = f.pop_double();
	GLint sides = f.pop_int();
	GLint rings = f.pop_int();
	glutWireTorus( innerRadius, outerRadius, sides, rings );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_solid_torus )
	GLdouble innerRadius = f.pop_double();
	GLdouble outerRadius = f.pop_double();
	GLint sides = f.pop_int();
	GLint rings = f.pop_int();
	glutSolidTorus( innerRadius, outerRadius, sides, rings );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_wire_dodecahedron )
	glutWireDodecahedron();
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_solid_dodecahedron )
	glutSolidDodecahedron();
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_wire_teapot )
	GLdouble size = f.pop_double();
	glutWireTeapot( size );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_solid_teapot )
	GLdouble size = f.pop_double();
	glutSolidTeapot( size );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_wire_octahedron )
	glutWireOctahedron();
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_solid_octahedron )
	glutSolidOctahedron();
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_wire_tetrahedron )
	glutWireTetrahedron();
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_solid_tetrahedron )
	glutSolidTetrahedron();
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_wire_icosahedron )
	glutWireIcosahedron();
END_STDLIB_FUNCTION

STDLIB_FUNCTION( glut_solid_icosahedron )
	glutSolidIcosahedron();
END_STDLIB_FUNCTION

#endif

void
init_glut(
	machine_t&			m )
{
#if ENABLE_GLUT_SUPPORT

	class_t*			rootclss = m.get_root_class();
	
	register_native_function(
		m, rootclss, "glutWireSphere;GLdouble::;GLint::;GLint::",
		glut_wire_sphere );
		
	register_native_function(
		m, rootclss, "glutSolidSphere;GLdouble::;GLint::;GLint::",
		glut_solid_sphere );

	register_native_function(
		m, rootclss, "glutWireCone;GLdouble::;GLdouble::;GLint::;GLint::",
		glut_wire_cone );

	register_native_function(
		m, rootclss, "glutSolidCone;GLdouble::;GLdouble::;GLint::;GLint::",
		glut_solid_cone );

	register_native_function(
		m, rootclss, "glutWireCube;GLdouble::",
		glut_wire_cube );

	register_native_function(
		m, rootclss, "glutSolidCube;GLdouble::",
		glut_solid_cube );

	register_native_function(
		m, rootclss, "glutWireTorus;GLdouble::;GLdouble::;GLint::;GLint::",
		glut_wire_torus );

	register_native_function(
		m, rootclss, "glutSolidTorus;GLdouble::;GLdouble::;GLint::;GLint::",
		glut_solid_torus );

	register_native_function(
		m, rootclss, "glutWireDodecahedron",
		glut_wire_dodecahedron );

	register_native_function(
		m, rootclss, "glutSolidDodecahedron",
		glut_solid_dodecahedron );

	register_native_function(
		m, rootclss, "glutWireTeapot;GLdouble::",
		glut_wire_teapot );

	register_native_function(
		m, rootclss, "glutSolidTeapot;GLdouble::",
		glut_solid_teapot );

	register_native_function(
		m, rootclss, "glutWireOctahedron",
		glut_wire_octahedron );

	register_native_function(
		m, rootclss, "glutSolidOctahedron",
		glut_solid_octahedron );

	register_native_function(
		m, rootclss, "glutWireTetrahedron",
		glut_wire_tetrahedron );

	register_native_function(
		m, rootclss, "glutSolidTetrahedron",
		glut_solid_tetrahedron );

	register_native_function(
		m, rootclss, "glutWireIcosahedron",
		glut_wire_icosahedron );

	register_native_function(
		m, rootclss, "glutSolidIcosahedron",
		glut_solid_icosahedron );

#endif
}

END_MACHINE_NAMESPACE
