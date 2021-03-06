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
//	cleanup_thread_t.cp		   �2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_CLEANUP_THREAD_H
#define ICARUS_CLEANUP_THREAD_H

#include "cleanup_thread_t.h"

BEGIN_RT_NAMESPACE

cleanup_thread_t::cleanup_thread_t() :

	ithread_t( "cleanup thread", -1, 8192, 0 )
{
}

void
cleanup_thread_t::add(
	ithread_t*			thread )
{
	m_free_queue.enqueue( 0, thread );
	wake();
}

void
cleanup_thread_t::run()
{
	while( true )
	{
		ithread_t*		thread;
		
		while( true )
		{
			thread = (ithread_t*)m_free_queue.dequeue();
			if( thread == 0 )
				break;
			delete thread;
		}
	
		block();
	}
}

END_RT_NAMESPACE

#endif

