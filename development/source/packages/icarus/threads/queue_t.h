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
//	queue_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef QUEUE_H
#define QUEUE_H

//#pragma once

#include "rt.h"
#include "signal_t.h"

BEGIN_RT_NAMESPACE

class queue_t {
public:
						queue_t(
							size_t				data_size,
							unsigned long		capacity );

						~queue_t();

	void				put(
							void*				data );

	bool				put_cond(
							void*				data );

	bool				put_until(
							void*				data,
							u64					micros );

	void				get(
							void*				data );

	bool				get_cond(
							void*				data );

	bool				get_until(
							void*				data,
							u64					micros );

private:
	void				put_prim(
						   void*				data );

	struct node_t {
		node_t*			next;
		u08				data[ 1 ];
	};

	size_t				m_data_size;
	size_t				m_node_size;

	u32					m_count;
	u32					m_capacity;

	signal_t			m_get_signal;
	signal_t			m_put_signal;
	
	node_t*				m_nodes;
	node_t**			m_link;
};

END_RT_NAMESPACE

#endif

