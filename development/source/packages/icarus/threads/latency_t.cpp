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
//	latency_t.cpp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "latency_t.h"

BEGIN_RT_NAMESPACE

latency_t::latency_t()
{
	clear();
}

void
latency_t::add(
	u64					value )
{
	m_sum += value;
	m_count++;
	if( value > m_peak )
		m_peak = value;
}

void
latency_t::clear()
{
	m_count = 0;
	m_sum = 0;
	m_peak = 0;
}

END_RT_NAMESPACE
