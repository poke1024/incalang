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
//	code_submit_t.h		 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "code_view_t.h"

BEGIN_AKLABETH_NAMESPACE

class code_submit_t {
public:
							code_submit_t(
								code_view_t*		view );

							~code_submit_t();

	void					release();
	
	void					patch(
								const codepos_t&	from,
								const codepos_t&	to );

protected:
	code_view_t*			m_view;
	codepos_t				m_selfrom;
	codepos_t				m_selto;
};

END_AKLABETH_NAMESPACE
