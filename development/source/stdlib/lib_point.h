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
//	lib_point.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "point_t.h"

BEGIN_MACHINE_NAMESPACE

#define POINT_CLASS_SIZE ( sizeof( point_t ) + CLASS_HEADER_SIZE )

inline point_t*
pop_point(
	fiber_t&			f )
{
	instance_t*			param = f.pop_instance();
	
	return INSTANCE_SELF( point_t, param );
}

#define INIT_POINT( block, m )												\
	u08*				_template;											\
	if( ( _template = m.m_lib_static.point_template ) == 0 )				\
		_template = create_point_template( m );								\
	*( (void**)block ) = *(void**)_template;

u08*
create_point_template(
	machine_t&			m );

END_MACHINE_NAMESPACE
