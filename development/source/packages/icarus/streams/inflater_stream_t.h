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
//	inflater_stream_t.h		   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef INFLATER_STREAM_H
#define INFLATER_STREAM_H

//#pragma once

#include "input_stream_t.h"
#include "zlib.h"

BEGIN_ICARUS_NAMESPACE

class inflater_stream_t : public input_stream_t {
public:
						inflater_stream_t(
							input_stream_t&		stream,
							u32					avail );

	virtual				~inflater_stream_t();

	virtual fpos_t		read_some_bytes(
							void*				buffer,
							fpos_t				count );						
protected:
	input_stream_t&		m_input_stream;
	z_stream			m_zip_stream;
	s32					m_total_avail;
	u08*				m_buffer;
};

END_ICARUS_NAMESPACE

#endif

