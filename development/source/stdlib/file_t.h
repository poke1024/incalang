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
//	file_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "buffered_file_stream_t.h"
#include "runtime_object_t.h"

BEGIN_MACHINE_NAMESPACE

class file_t :
	public buffered_file_stream_t,
	public runtime_object_t {

public:

						file_t(
							lib_context_t&		context,
							const char*			path,
							const char*			mode );

	virtual				~file_t();
};

END_MACHINE_NAMESPACE
