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
//	lib_protos.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"

BEGIN_MACHINE_NAMESPACE

void
init_lib(
	machine_t&			m )
{
	init_debug( m );
	init_math( m );
	init_noise( m );
	init_input_stream( m );
	init_input_stream_operators( m );
	init_output_stream( m );
	init_output_stream_operators( m );
	init_native_streams( m );
	init_zlib( m );
	init_string( m );
	init_random( m );
	init_system( m );
	init_pixmap( m );
	init_spixmap( m );
	init_opengl( m );
	init_openglu( m );
	init_glut( m );
	init_opengl_extra( m );
	init_threads( m );
	init_bigint( m );
	init_complex( m );
	init_point( m );
	init_storage( m );
	init_lib_sgl( m );
	init_sound( m );
	init_color( m );
}

END_MACHINE_NAMESPACE
