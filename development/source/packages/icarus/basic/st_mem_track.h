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
//	st_mem_track.h								    ©1999-2002 Bernhard Liebl.
// ===========================================================================

#ifndef ICARUS_ST_MEM_TRACK
#define ICARUS_ST_MEM_TRACK

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class st_mem_track {
public:
						st_mem_track(
							bool				track );

						~st_mem_track();
						
private:
	bool				m_track;
};

END_ICARUS_NAMESPACE

#endif

