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
//	icarus_error.cp			   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus.h"
#include <stdio.h>

#if __VISC__
#include <stdarg.h>
#include <stdexcept>
#endif

#if __GNUC__
#include <stdexcept>
#endif

#include "thread_t.h"

BEGIN_ICARUS_NAMESPACE

void
issue_icarus_error(
	const char*			format,
	... )
{
	static char			buffer[ 256 ];

	va_list args;
	va_start( args, format );
	
	ivsnprintf( buffer,
		sizeof( buffer ) / sizeof( char ) - 1,
		format, args );

#if ICARUS_THREADS
	rt::ithread_t*		thread;

	thread = rt::ithread_t::current();
	if( thread )
		thread->error( buffer );
	else
		icarus_error( buffer );
#else
	icarus_error( buffer );
#endif
}

void
icarus_error(
	const char*			err )
{
	ICARUS_DEBUG_WRITELN( err );
	throw std::runtime_error( err );
}

void
icarus_trace(
	const char*			err )
{
	fprintf( stderr, err );
}

void
icarus_fatal(
	const char*			err )
{
	MessageBox( NULL, err, "Fatal Error", MB_OK );
	exit( EXIT_FAILURE );
}

END_ICARUS_NAMESPACE
