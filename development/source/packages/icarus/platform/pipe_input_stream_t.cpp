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
//	pipe_input_stream_t.cpp	   �2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include <Windows.h>
#include "pipe_input_stream_t.h"

BEGIN_ICARUS_NAMESPACE

pipe_input_stream_t::pipe_input_stream_t(
	HANDLE				pipe ) :

	mPipe( pipe )
{
}

fpos_t
pipe_input_stream_t::read_some_bytes(
	void*				buffer,
	fpos_t				count )
{
	DWORD				dwRead;

	if( ReadFile( mPipe, buffer, count, &dwRead, NULL ) == FALSE )
		return 0;

	return dwRead;
}

END_ICARUS_NAMESPACE
