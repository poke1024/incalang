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
//	runtime_object_t.h		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "lib_context_t.h"

BEGIN_MACHINE_NAMESPACE

class runtime_object_t {
public:
						runtime_object_t();

						runtime_object_t(
							lib_context_t&		context,
							int					index );
							
	virtual				~runtime_object_t();
		
protected:
	lib_context_t*		m_context;
	long				m_rtindex;
	runtime_object_t*	m_next;
};

END_MACHINE_NAMESPACE
