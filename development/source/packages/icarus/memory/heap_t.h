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
//	heap_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_HEAP_H
#define ICARUS_HEAP_H

//#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class heap_t {
public:
	typedef s64				priority_t;

	struct node_t {
		priority_t			priority;
		void*				data;
	};

							heap_t(
								long				alloc = 32 );

	virtual					~heap_t();

	void					enqueue(
								priority_t			priority,
								void*				data );

	void*					dequeue();
	
	void*					top();
	
	void					remove(
								void*				data );
																
	long					count() const;

#if ICARUS_SAFE || ICARUS_DEBUG
	bool					contains(
								void*				data );
#endif

protected:	
	node_t*					m_heap;
	long					m_alloc;
	long					m_bottom;
	
	void					down();
	
	void					up();
	
	void					expand();
};

END_ICARUS_NAMESPACE

#endif

