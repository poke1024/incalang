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
//	key_buffer_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"

BEGIN_MACHINE_NAMESPACE

class key_buffer_t {
public:
						key_buffer_t();

	virtual				~key_buffer_t();

	void				submit_key(
							s32					c );

	s32					fetch_key();

private:
	struct node_t {
		node_t*			next;
		u64				timestamp;
		s32				c;
	};
	
	void				remove_expired(
							u64					timestamp );
	
	void				pop();
	
	node_t*				m_nodes;
	node_t**			m_link;
	long				m_count;
};

END_MACHINE_NAMESPACE
