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
//	lib_protos.h			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine_t.h"

BEGIN_MACHINE_NAMESPACE

void
init_lib(
	machine_t&			m );

void
init_debug(
	machine_t&			m );

void
init_math(
	machine_t&			m );

void
init_noise(
	machine_t&			m );
	
void
init_input_stream(
	machine_t&			m );

void
init_input_stream_operators(
	machine_t&			m );

void
init_output_stream(
	machine_t&			m );

void
init_output_stream_operators(
	machine_t&			m );

void
init_native_streams(
	machine_t&			m );

void
init_string(
	machine_t&			m );

void
init_random(
	machine_t&			m );

void
init_system(
	machine_t&			m );

void
init_pixmap(
	machine_t&			m );

void
init_spixmap(
	machine_t&			m );

void
init_opengl(
	machine_t&			m );

void
init_openglu(
	machine_t&			m );
	
void
init_glut(
	machine_t&			m );

void
init_opengl_extra(
	machine_t&			m );

void
init_threads(
	machine_t&			m );

void
init_bigint(
	machine_t&			m );

void
init_complex(
	machine_t&			m );

void
init_point(
	machine_t&			m );

void
init_storage(
	machine_t&			m );

void
init_lib_sgl(
	machine_t&			m );

void
init_sound(
	machine_t&			m );

void
init_color(
	machine_t&			m );

void
init_zlib(
	machine_t&			m );

// ===========================================================================

class fiber_t;
union instance_t;

void
ostream_write_chunk(
	fiber_t&			f,
	instance_t*			instance,
	const char*			buf,
	u32					bufsize );

END_MACHINE_NAMESPACE