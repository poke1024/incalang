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
//	xheap_t.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef XHEAP_H
#define XHEAP_H

//#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

typedef void param_func_t( void* );

struct block_t;
struct destructor_t;

class xframe_t {
public:
	void				open();
	
	void				close();

	void*				alloc(
							isize_t				s );

	void				free(
							void*				p,
							isize_t				s );
							
	void				destructor(
							param_func_t*		func,
							void*				param );

	static void			free_unused_memory();


private:
	struct frame_t {
		isize_t			size;
		isize_t			index;
		block_t*		block;
		block_t*		custom;
		destructor_t*	destructors;
	};

	frame_t				m_frame;
};

class xheap_t : public xframe_t {
public:
						xheap_t();
						
						~xheap_t();
};

END_ICARUS_NAMESPACE

#endif

