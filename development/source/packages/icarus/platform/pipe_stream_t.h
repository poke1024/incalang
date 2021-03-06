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
//	pipe_stream_t.h			   �2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef PIPE_STREAM_H
#define PIPE_STREAM_H

//#pragma once

#include "input_stream_t.h"
#include "output_stream_t.h"
#include "random_stream_t.h"

BEGIN_ICARUS_NAMESPACE

class pipe_stream_t : public random_stream_t {
public:
						pipe_stream_t(
							input_stream_t*		in,
							output_stream_t*	out );

	virtual fpos_t		read_some_bytes(
							void*				buffer,
							fpos_t				count );

	virtual fpos_t		write_some_bytes(
							const void*			buffer,
							fpos_t				count );

	virtual void		flush();

	virtual void		seek(
							fpos_t				where,
							seek_mode			mode );
							
	virtual fpos_t		tell();

private:
	input_stream_t*		m_in;
	output_stream_t*	m_out;
	bool				m_write_flag;
};

END_ICARUS_NAMESPACE

#endif

