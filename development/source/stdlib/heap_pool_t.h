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
//	heap_pool_t.h			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"
#include "safe_memory_t.h"

BEGIN_MACHINE_NAMESPACE

class heap_pool_t {
public:
						heap_pool_t(
							const size_t*		distribution,
							int					dimensions,
							memory_t*			memory );
						
						~heap_pool_t();

	void*				alloc(
							long				length );

	void				free(
							void*				block,
							long				length );
							
protected:
	u08**				m_buckets;
	size_t*				m_count;
	size_t*				m_distribution;
	int					m_dimensions;
	memory_t*			m_memory;
};

END_MACHINE_NAMESPACE
