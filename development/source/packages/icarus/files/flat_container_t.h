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
//	flat_container_t.h		   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "flat_container_t.h"
#include "os_file.h"
#include <string>

BEGIN_ICARUS_NAMESPACE

enum {
	TOC_HASH_SIZE		= 64
};

struct toc_entry_t {
	toc_entry_t*		next;
	std::string			name;
	fpos_t				offset;
	fpos_t				length;
	u08					flags;
};

class flat_container_t {
public:
						flat_container_t(
							const char*			filename );
							
	virtual				~flat_container_t();
	
	toc_entry_t*		lookup(
							const char*			filename );
	
	virtual void		extract(
							void*				buffer,
							fpos_t				offset,
							fpos_t				length ) = 0;
	
protected:
	void				read_toc();
	
	os_file_t			m_file;
	toc_entry_t*		m_toc[ TOC_HASH_SIZE ];
};

END_ICARUS_NAMESPACE
