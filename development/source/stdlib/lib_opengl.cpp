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
//	lib_opengl.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include <ddraw.h>

#include "lib_protos.h"
#include "machine_t.h"
#include "vmutils.h"

#include "fiber_t.h"
#include "vmbind.h"

#include "machine_class_t.h"
#include "machine_error.h"
#include "thread_t.h"
#include "g3world_t.h"

#if PROJECT_BUILDER
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#elif __VISC__
#include <gl/gl.h>
#include <gl/glu.h>
#else
#include <gl.h>
#include <glu.h>
#endif

#include "screen_t.h"
USING_FIZBAN_NAMESPACE

#include "pixmap_t.h"
#include "lib_string.h"

BEGIN_MACHINE_NAMESPACE

#include "vmperfdefs.h"

// ---------------------------------------------------------------------------

class_t*				s_console_class = 0;

#define CONSOLE_CLASS_FUNCTION( name, type ) \
	STDLIB_CLASS_FUNCTION( name, type ) \
	self = INSTANCE_SELF( type, dyncast( &f, instptr, s_console_class ) );

// ---------------------------------------------------------------------------

struct console_t {
	screen_t*			screen;
	u64					last_swap_time;
	u64					mean_swap_time;
};

const u64				INITIAL_SWAP_TIME = 1000000;

static void
console_init(
	console_t*			console )
{
	console->screen = new screen_t;
	console->mean_swap_time = INITIAL_SWAP_TIME;
	console->last_swap_time = rt_micros();
}

static void
console_calculate_fps(
	console_t*			console )
{
	u64					now;
	u64					diff;
	now = rt_micros();
	diff = now - console->last_swap_time;
	console->mean_swap_time =
		( console->mean_swap_time * 9 + diff ) / 10;
	console->last_swap_time = now;
}

static void
console_swap(
	console_t*			console )
{
	console->screen->swap();
	if( not vm_in_real_time_mode() )
		vm_process_os_events();
	console_calculate_fps( console );
}

STDLIB_CLASS_FUNCTION( console_make_current, console_t* )
	console_t*			me = *self;
	me->screen->make_current();
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( console_set_title, console_t* )
	console_t*			me = *self;
	string_t*			s = pop_string( f );
	istring				title;
	title.assign( s->text, s->length );
	me->screen->set_title( title.c_str() );
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( console_set_fullscreen, console_t* )
	console_t*			me = *self;
	me->screen->set_fullscreen( f.pop_bool() );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( console_set_doublebuffer, console_t* )
	console_t*			me = *self;
	me->screen->set_doublebuffer( f.pop_bool() );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( console_fps, console_t* )
	console_t*			me = *self;
	f.push_float( 1000000.0 / me->mean_swap_time );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

struct g3console_t : public console_t {
};

CONSOLE_CLASS_FUNCTION( g3console_create, g3console_t* )
	self = INSTANCE_SELF( g3console_t*,
		dyncast( &f, instptr, s_console_class ) );
	*self = new g3console_t;
	g3console_t*		me = *self;
	console_init( me );
	me->screen->set_title( "G3Console View" );
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( g3console_destroy, g3console_t* )
	g3console_t*		me = *self;
	delete me->screen;
	delete me;
	*self = 0;
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( g3console_set_resolution, g3console_t* )
	g3console_t*		me = *self;
	long				width = f.pop_int();
	long				height = f.pop_int();
	me->screen->set_display_size( width, height );
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( g3console_open, g3console_t* )
	g3console_t*		me = *self;
	monitor_t*			monitor;

	monitor = new monitor_t(
		monitor_t::enumerate() );

	me->screen->open( monitor );

	delete monitor;
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( g3console_swap, g3console_t* )
	g3console_t*		me = *self;
	console_swap( me );
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( g3console_width, g3console_t* )
	g3console_t*		me = *self;
	f.push_int( me->screen->width() );
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( g3console_height, g3console_t* )
	g3console_t*		me = *self;
	f.push_int( me->screen->height() );
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( g3console_resized, g3console_t* )
	g3console_t*		me = *self;
	f.push_bool( me->screen->fetch_resize_flag() );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

CONSOLE_CLASS_FUNCTION( g2console_open, g3console_t* )
	console_t*			me = *self;
	me->screen->set_2d_only( true );
	me->screen->set_title( "G2Console View" );
	g3console_open( f, instptr );
	glClearColor( 1, 1, 1, 0 );
	glColor3f( 0, 0, 0 );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

struct pmconsole_t : public console_t {
	point				tile_count;
	long				tex_count;
	GLuint*				tex_name;
	pixmap_t*			tiles;
	pixmap_t			pixmap;
	long				width;
	long				height;
};

const int				TILE_SHIFT = 7;
const int				TILE_SIZE = 128;
const float				TILE_EXT = 128;

CONSOLE_CLASS_FUNCTION( pmconsole_create, pmconsole_t* )
	*self = new pmconsole_t;
	pmconsole_t*		me = *self;
	console_init( me );
	me->screen->set_title( "PixmapConsole View" );
	me->width = 640;
	me->height = 480;
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( pmconsole_destroy, pmconsole_t* )
	pmconsole_t*		me = *self;
	if( f.m_lib_context.g2world == &me->pixmap )
		f.m_lib_context.g2world = 0;
	delete me->screen;
	delete me;
	*self = 0;
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( pmconsole_set_resolution, pmconsole_t* )
	pmconsole_t*		me = *self;
	me->width = f.pop_int();
	me->height = f.pop_int();
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( pmconsole_open, pmconsole_t* )
	pmconsole_t*		me = *self;

	me->screen->make_current();

	long				width = me->width;
	long				height = me->height;

	me->tile_count.h = ( width + TILE_SIZE - 1 ) / TILE_SIZE;
	me->tile_count.v = ( height + TILE_SIZE - 1 ) / TILE_SIZE;
	me->tex_count = me->tile_count.h * me->tile_count.v;
	me->tex_name = new GLuint[ me->tex_count ];
	glGenTextures( me->tex_count, me->tex_name );

	me->tiles = new pixmap_t[ me->tex_count ];
	for( int i = 0; i < me->tex_count; i++ )
		me->tiles[ i ].allocate(
			TILE_SIZE, TILE_SIZE,
			pixel_format_32_RGBA );

	me->pixmap.allocate( width, height,
		pixel_format_32_RGBA );
	
	monitor_t*			monitor;

	monitor = new monitor_t(
		monitor_t::enumerate() );

	me->screen->set_display_size(
		me->width, me->height );
	me->screen->set_2d_only( true );
	me->screen->open( monitor );

	delete monitor;

	glClearColor( 0, 0, 0, 0 );
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );

	f.m_lib_context.g2world = &me->pixmap;
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( pmconsole_set_port, pmconsole_t* )
	pmconsole_t*		me = *self;
	f.m_lib_context.g2world = &me->pixmap;
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( pmconsole_swap, pmconsole_t* )
	pmconsole_t*		me = *self;
	long				width = me->width;
	long				height = me->height;

	me->screen->make_current();

	if(	me->screen->fetch_resize_flag() )
	{
		glViewport( 0, 0,
			me->screen->width(),
			me->screen->height() );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluOrtho2D( 0, width, height, 0 );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	}

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	glTexParameteri( GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glClear( GL_COLOR_BUFFER_BIT );

	const int			tx = me->tile_count.h;
	const int			ty = me->tile_count.v;
	int					index;
	rect				dstrect;

	dstrect.left = 0;
	dstrect.top = 0;
	dstrect.right = TILE_SIZE;// - 1;
	dstrect.bottom = TILE_SIZE;// - 1;

	index = 0;
	for( int y = 0; y < ty; y++ )
	{
		for( int x = 0; x < tx; x++ )
		{
			pixmap_t*	tile = &me->tiles[ index ];
			glBindTexture( GL_TEXTURE_2D,
				me->tex_name[ index ] );
			index += 1;

			long		ix = x << TILE_SHIFT;
			long		iy = y << TILE_SHIFT;

			rect		srcrect;

			srcrect.left = ix;
			srcrect.top = iy;
			srcrect.right = ix + TILE_SIZE;// - 1;
			srcrect.bottom = iy + TILE_SIZE;// - 1;

			grafport_t::blit( *me->pixmap.m_port, *tile->m_port,
				srcrect, dstrect );

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, TILE_SIZE,
				TILE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				tile->m_port->pixels() );

			glBegin( GL_QUADS );
			glTexCoord2f( 0.0, 0.0 );
			glVertex3f( ix, iy, 0 );
			glTexCoord2f( 1.0, 0.0 );
			glVertex3f( ix + TILE_EXT, iy, 0 );
			glTexCoord2f( 1.0, 1.0 );
			glVertex3f( ix + TILE_EXT, iy + TILE_EXT, 0 );
			glTexCoord2f( 0.0, 1.0 );
			glVertex3f( ix, iy + TILE_EXT, 0 );
			glEnd();
		}
	}

	console_swap( me );
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( pmconsole_width, pmconsole_t* )
	pmconsole_t*		me = *self;
	f.push_int( me->width );
END_STDLIB_FUNCTION

CONSOLE_CLASS_FUNCTION( pmconsole_height, pmconsole_t* )
	pmconsole_t*		me = *self;
	f.push_int( me->height );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_opengl_extra(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();
	class_t*			clss;

	clss = rootclss->find_class( "Console", 7 );
	if( !clss )
		throw_icarus_error( "Console class not found" );
	s_console_class = clss;

	register_native_function(
		m, clss, "makeCurrent", console_make_current );

	register_native_function(
		m, clss, "setTitle;.String::&", console_set_title );

	register_native_function(
		m, clss, "setFullscreen;a", console_set_fullscreen );
	
	register_native_function(
		m, clss, "setDoubleBuffer;a", console_set_doublebuffer );

	register_native_function(
		m, clss, "fps", console_fps );


	clss = rootclss->find_class( "G3Console", 9 );
	if( !clss )
		throw_icarus_error( "G3Console class not found" );

	register_native_function(
		m, clss, "create", g3console_create );

	register_native_function(
		m, clss, "destroy", g3console_destroy );

	register_native_function(
		m, clss, "setResolution;i;i", g3console_set_resolution );

	register_native_function(
		m, clss, "open", g3console_open );

	register_native_function(
		m, clss, "swap", g3console_swap );

	register_native_function(
		m, clss, "width", g3console_width );

	register_native_function(
		m, clss, "height", g3console_height );

	register_native_function(
		m, clss, "resized", g3console_resized );


	clss = rootclss->find_class( "G2Console", 9 );
	if( !clss )
		throw_icarus_error( "G2Console class not found" );

	register_native_function(
		m, clss, "open", g2console_open );


	clss = rootclss->find_class( "PixmapConsole", 13 );
	if( !clss )
		throw_icarus_error( "PixmapConsole class not found" );

	register_native_function(
		m, clss, "create", pmconsole_create );

	register_native_function(
		m, clss, "destroy", pmconsole_destroy );

	register_native_function(
		m, clss, "setResolution;i;i", pmconsole_set_resolution );

	register_native_function(
		m, clss, "open", pmconsole_open );

	register_native_function(
		m, clss, "swap", pmconsole_swap );

	register_native_function(
		m, clss, "width", pmconsole_width );

	register_native_function(
		m, clss, "height", pmconsole_height );

	register_native_function(
		m, clss, "makePixmapPort", pmconsole_set_port );
}

// ---------------------------------------------------------------------------

#define OPENGL_FUNCTION( name )												\
	STDLIB_FUNCTION( name )

/*#define OPENGL_FUNCTION( name )												\
	STDLIB_FUNCTION( name )													\
	if( f.m_lib_context.g3world == 0 )										\
		f.fatal_error( ERR_NO_G3WORLD_SET );*/

// ---------------------------------------------------------------------------

#if 0

#define mglGenTextures( n, textures )										\
	f.m_lib_context.g3world->gen_textures( n, textures )

#define mglDeleteTextures( n, textures )									\
	f.m_lib_context.g3world->delete_textures( n, textures )

#define mgluNewQuadric														\
	f.m_lib_context.g3world->new_quadric
	
#define mgluDeleteQuadric( quad )											\
	f.m_lib_context.g3world->delete_quadric( quad )

#define mgluNewNurbsRenderer												\
	f.m_lib_context.g3world->new_nurbs_renderer

#define mgluDeleteNurbsRenderer( nurbs )									\
	f.m_lib_context.g3world->delete_nurbs_renderer( nurbs )

#define mgluNewTess															\
	f.m_lib_context.g3world->new_tesselator
	
#define mgluDeleteTess( tess )												\
	f.m_lib_context.g3world->delete_tesselator( tess )

// ---------------------------------------------------------------------------

#define glGenTextures( n, textures )										\
	mglGenTextures( n, textures )

#define glDeleteTextures( n, textures )										\
	mglDeleteTextures( n, textures )

#define gluNewQuadric														\
	mgluNewQuadric

#define gluDeleteQuadric( quad )											\
	mgluDeleteQuadric( quad )

#define gluNewNurbsRenderer													\
	mgluNewNurbsRenderer

#define gluDeleteNurbsRenderer( nurbs )										\
	mgluDeleteNurbsRenderer( nurbs )

#define gluNewTess															\
	mgluNewTess

#define gluDeleteTess( tess )												\
	mgluDeleteTess( tess )

#endif

// ---------------------------------------------------------------------------

#include "opengl_glue.i"

#include "openglu_glue.i"

END_MACHINE_NAMESPACE
