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
//	latency_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_LATENCY_H
#define ICARUS_LATENCY_H

//#pragma once

#include "rt.h"

BEGIN_RT_NAMESPACE

class latency_t {
public:
							latency_t();

	void					add(
								u64					value );

	u64						mean() const;

	u64						peak() const;

	void					clear();

private:
	long					m_count;
	u64						m_sum;
	u64						m_peak;
};

inline u64
latency_t::mean() const
{
	if( m_count < 1 )
		return 0;
	return m_sum / m_count;
}

inline u64
latency_t::peak() const
{
	return m_peak;
}

END_RT_NAMESPACE

#endif

