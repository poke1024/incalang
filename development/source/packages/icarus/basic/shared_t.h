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
//	shared_t.h				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_SHARED_H
#define ICARUS_SHARED_H

//#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class shared_t {
public:
						shared_t();
						
	void				refer();
	
	void				unrefer();

#if ICARUS_DEBUG
	s32					ref_count() const;
#endif
	
protected:
	virtual				~shared_t();
	
private:	
	s32					m_ref_count;
};

// ---------------------------------------------------------------------------

inline
shared_t::shared_t() :
	m_ref_count( 1 )
{
}

inline
shared_t::~shared_t()
{
}

END_ICARUS_NAMESPACE

#endif

