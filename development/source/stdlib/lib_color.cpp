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
//	lib_color.cpp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "lib_point.h"
#include "lib_string.h"
#include "lib_color.h"

BEGIN_MACHINE_NAMESPACE

// ===========================================================================

struct coldesc_t {
	u32					color;
	const char*			name;
};

static htab_t*			s_color_table = 0;

static const coldesc_t	s_colors[] = {
	0xFFFFFF,			"white",
	0x000000,			"black",
	0xFFFF00,			"yellow",
	0x00FFFF,			"cyan",
	0x0000FF,			"blue",
	0x00FF00,			"green",
	0xFF0000,			"red",
	0x00FFFF,			"aqua",
	0xFF00FF,			"magenta",

	0x808080,			"grey",
	0x808080,			"gray",
	0xFFA500,			"orange",
	0xEE82EE,			"violet",
	0xF0FFFF,			"azure",
	0xDC143C,			"crimson",
	0xADEAEA,			"turquoise",
	0xE6E6FA,			"lavender",
	0xDB9370,			"tan",

	0xF5F5DC,			"beige",
	0x7CFC00,			"lawngreen",
	0xE6E8FA,			"silver",
	0xDB70DB,			"orchid",
	0xD9D9F3,			"quartz",
	0xFA8072,			"salmon",
	0xCD7F32,			"gold",
	0x9F9F5F,			"khaki",
	0x8C7853,			"bronze",
	0xFAF0E6,			"linen",
	0xEAADEA,			"plum",
	0x70DB93,			"aquamarine",
	0xF5DEB3,			"wheat",
	0xB87333,			"copper",
	0xBC8F8F,			"pink",

	0xFFE4B5,			"moccasin",
	0xFFFAFA,			"snow",
	0xFFFFF0,			"ivory",
	0xA0522D,			"sienna",
	0xCD853F,			"peru",
	0xFF6347,			"tomato",
	0xA020F0,			"purple",
	0xA52A2A,			"brown",
	0xB03060,			"maroon",
	0xFFE4C4,			"bisque",

	0xF0FFF0,			"honeydew",
	0xF5FFFA,			"mintcream",
	0x191970,			"midnightblue",
	0x000080,			"navyblue",
	0x4682B4,			"steelblue",
	0x7FFF00,			"chartreuse",
	0xCD5C5C,			"indianred",
	0xDEB887,			"burlywood",
	0xFF7F50,			"coral"
};

// ---------------------------------------------------------------------------

bool
retrieve_color_by_name(
	const char*			name,
	u08*				out )
{
	htab_t*				table = s_color_table;

	if( not hfind( table, (u08*)name,
		strlen( name ) * sizeof( char ) ) )
		return false;

	u32					color = ( (coldesc_t*)hstuff( table ) )->color;
	out[ 2 ] = ( color & 0xff );
	out[ 1 ] = ( ( color >> 8 ) & 0xff );
	out[ 0 ] = ( ( color >> 16 ) & 0xff );
	return true;
}

void
retrieve_color_by_index(
	long				index,
	u08*				out )
{
	u32					color;
	index %= sizeof( s_colors ) / sizeof( coldesc_t );
	color = s_colors[ index ].color;
	out[ 2 ] = ( color & 0xff );
	out[ 1 ] = ( ( color >> 8 ) & 0xff );
	out[ 0 ] = ( ( color >> 16 ) & 0xff );
}

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( color_make_by_name )
	machine_t&			m = *f.machine();
	string_t*			s = pop_string( f );
	const char*			name = string_get_c_str( s );

	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );
	
	bool				found = false;

	if( name )
	{
		htab_t*			table = s_color_table;

		found = hfind( table, (u08*)name,
			strlen( name ) * sizeof( char ) );

		if( found )
		{
			u32			color = ( (coldesc_t*)hstuff( table ) )->color;
			p->z = ( color & 0xff ) / 255.0;
			p->y = ( ( color >> 8 ) & 0xff ) / 255.0;
			p->x = ( ( color >> 16 ) & 0xff ) / 255.0;
		}
	}

	if( not found )
		p->x = p->y = p->z = 0;
	
	f.push_block( block, POINT_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( color_make_by_index )
	machine_t&			m = *f.machine();
	u32					index = f.pop_int();

	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			p = INSTANCE_SELF( point_t, block );

	u32					color;
	index %= sizeof( s_colors ) / sizeof( coldesc_t );
	color = s_colors[ index ].color;
	p->z = ( color & 0xff ) / 255.0;
	p->y = ( ( color >> 8 ) & 0xff ) / 255.0;
	p->x = ( ( color >> 16 ) & 0xff ) / 255.0;

	f.push_block( block, POINT_CLASS_SIZE );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_color(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "color;.String::&",
		color_make_by_name );

	register_native_function(
		m, rootclss, "color;i",
		color_make_by_index );

	const coldesc_t*	entry = s_colors;
	const int			count = sizeof( s_colors ) /
							sizeof( coldesc_t );

	if( s_color_table == 0 )
	{
		s_color_table = hcreate( 6 );

		for( int i = 0; i < count; i++, entry++ )
		{
			hadd( s_color_table,
				(u08*)entry->name,
				strlen( entry->name ) * sizeof( char ),
				(void*)entry );
		}
	}
}

END_MACHINE_NAMESPACE
