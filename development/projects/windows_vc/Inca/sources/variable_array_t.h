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
//	variable_array_t.h		   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "aklabeth.h"
#include "random_stream_t.h"
#include "array_t.h"

BEGIN_AKLABETH_NAMESPACE

class variable_t;

class variable_array_t {
public:
						variable_array_t();

						~variable_array_t();

	void				load_stack_frame(
							int					frame_index,
							random_stream_t*	stream );

	long				count() const;

	variable_t*			at(
							long				index );

	void				append(
							variable_t*			var );

private:
	void				cleanup();

	void*				m_base;
	array_t				m_vars;
};

END_AKLABETH_NAMESPACE
