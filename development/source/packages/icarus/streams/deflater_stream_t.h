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
//	deflater_stream_t.h		   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef DEFLATER_STREAM_H
#define DEFLATER_STREAM_H

//#pragma once

#include "output_stream_t.h"
#include "zlib.h"

BEGIN_ICARUS_NAMESPACE

class deflater_stream_t : public output_stream_t {
public:
						deflater_stream_t(
							output_stream_t&	stream,
							int					level = Z_DEFAULT_COMPRESSION );

	virtual				~deflater_stream_t();

	void				set_params(
							int					level = Z_DEFAULT_COMPRESSION,
							int					strategy = Z_DEFAULT_STRATEGY );

	virtual fpos_t		write_some_bytes(
							const void*			buffer,
							fpos_t				count );
							
	virtual void		flush();
	
protected:
	void				flush_buffers(
							int					flush );

	output_stream_t&	m_output_stream;
	z_stream			m_zip_stream;
	u08*				m_buffer;
};

END_ICARUS_NAMESPACE

#endif

