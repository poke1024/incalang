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
//	buffered_file_stream_t.h   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef BUFFERED_FILE_STREAM_H
#define BUFFERED_FILE_STREAM_H

//#pragma once

#include "file_stream_t.h"

BEGIN_ICARUS_NAMESPACE

class buffered_file_stream_t : public file_stream_t {
public:
						buffered_file_stream_t(
							const char*			filename,
							const char*			permission );
							
	virtual				~buffered_file_stream_t();

	virtual void		seek(
							fpos_t				where,
							seek_mode			mode );
							
	virtual fpos_t		tell();
	
	virtual fpos_t		read_some_bytes(
							void*				buffer,
							fpos_t				count );

	virtual fpos_t		write_some_bytes(
							const void*			buffer,
							fpos_t				count );
	
	virtual void		flush();
							
private:
	void				inval_cache();
	
	void				fill_cache();
	
	fpos_t				m_offset;
	u08*				m_buffer;
	u08*				m_mark;
	u32					m_cached;
};

END_ICARUS_NAMESPACE

#endif

