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
//	cleanup_thread_t.h		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "thread_t.h"
#include "heap_t.h"

BEGIN_RT_NAMESPACE

class thread_t;

class cleanup_thread_t : public ithread_t {
public:
						cleanup_thread_t();

	virtual void		run();
	
	void				add(
							ithread_t*			thread );

private:
	heap_t				m_free_queue;
};

END_RT_NAMESPACE
