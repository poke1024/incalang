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
//	preproc_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_PREPROC_H
#define ARIADNE_PREPROC_H

//#pragma once

#include "compiler.h"
#include "reader_t.h"
#include "input_stream_t.h"

BEGIN_COMPILER_NAMESPACE

class preproc_t : public input_stream_t {
public:
						preproc_t(
							reader_t*			reader );

	virtual				~preproc_t();

	virtual fpos_t		read_some_bytes(
							void*				buffer,
							fpos_t				count );

	void				set_source_path(
							const char*			path );

	void				set_system_path(
							const char*			path );

private:
	u32					read_line(
							fileref_t*			fpos,
							u32*				netto );

	void				handle_directive(
							const char*			mark,
							const fileref_t*	fpos );

	reader_t*			m_reader;
	char*				m_line;
	char*				m_mark;
	u32					m_left;

	istring				m_source_path;
	istring				m_system_path;
};

END_COMPILER_NAMESPACE

#endif

