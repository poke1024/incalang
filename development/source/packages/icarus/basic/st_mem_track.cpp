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
//	st_mem_track.cp								    ©1999-2002 Bernhard Liebl.
// ===========================================================================

#include "st_mem_track.h"

BEGIN_ICARUS_NAMESPACE

st_mem_track::st_mem_track(
	bool				track )
{
	m_track = s_mem_track;
	s_mem_track = track;
}

st_mem_track::~st_mem_track()
{
	s_mem_track = m_track;
}

END_ICARUS_NAMESPACE
