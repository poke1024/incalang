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
//	shared_t.cp				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

//#pragma once

#include "shared_t.h"

BEGIN_ICARUS_NAMESPACE

void
shared_t::refer()
{
	m_ref_count += 1;

#if ICARUS_DEBUG
	if( m_ref_count < 0 )
		throw_icarus_error( "refcount exceeded integer" );
#endif
}

void
shared_t::unrefer()
{
	if( --m_ref_count == 0 )
		delete this;
}

#if ICARUS_DEBUG
s32
shared_t::ref_count() const
{
	return m_ref_count;
}
#endif

END_ICARUS_NAMESPACE

