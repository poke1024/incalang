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
//	random_stream_t.h			   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef RANDOM_STREAM_H
#define RANDOM_STREAM_H

//#pragma once

#include "input_stream_t.h"
#include "output_stream_t.h"
#include <cstdio>

BEGIN_ICARUS_NAMESPACE

enum seek_mode {
	seek_mode_begin		= SEEK_SET,
	seek_mode_relative	= SEEK_CUR,
	seek_mode_end		= SEEK_END
};

class random_stream_t :
	public input_stream_t,
	public output_stream_t {

public:
	virtual void		seek(
							fpos_t				where,
							seek_mode			mode ) = 0;
							
	virtual fpos_t		tell() = 0;
	
	virtual fpos_t		length();

	virtual void		skip(
							fpos_t				count );
};

END_ICARUS_NAMESPACE

#endif

