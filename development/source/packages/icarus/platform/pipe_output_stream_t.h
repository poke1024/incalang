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
//	pipe_output_stream_t.h	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef PIPE_OUTPUT_STREAM_H
#define PIPE_OUTPUT_STREAM_H

//#pragma once

#include "output_stream_t.h"

BEGIN_ICARUS_NAMESPACE

class pipe_output_stream_t : public output_stream_t {
public:
						pipe_output_stream_t(
							HANDLE				pipe );

	virtual fpos_t		write_some_bytes(
							const void*			buffer,
							fpos_t				count );

	virtual void		flush();

private:
	HANDLE				mPipe;
};

END_ICARUS_NAMESPACE

#endif

