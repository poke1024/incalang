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
//	fiber_thread_t.h	 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "thread_t.h"

#include "machine_t.h"
#include "fiber_t.h"

BEGIN_MACHINE_NAMESPACE

class fiber_thread_t : public ithread_t {
public:
						fiber_thread_t(
							machine_t*			machine,
							instance_t*			instance );

protected:
	virtual				~fiber_thread_t();

	virtual void		run();
	
	machine_t*			m_machine;
	fiber_t*			m_fiber;
	instance_t*			m_instance;
};

END_MACHINE_NAMESPACE
