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
//	flat_file_t.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "icarus.h"
#include "flat_container_t.h"

BEGIN_ICARUS_NAMESPACE

class flat_file_t {
public:
						flat_file_t(
							flat_container_t*	container,
							toc_entry_t*		entry );

	void				seek(
							fpos_t				offset,
							int					mode );
							
	fpos_t				tell() const;

	fpos_t				read(
							void*				buffer,
							fpos_t				count );

private:
	flat_container_t*	m_container;
	toc_entry_t*		m_entry;
	fpos_t				m_position;
};

END_ICARUS_NAMESPACE
