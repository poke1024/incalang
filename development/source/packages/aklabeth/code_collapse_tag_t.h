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
//	code_collapse_tag_t.h     ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_COLLAPSE_TAG_H
#define CODE_COLLAPSE_TAG_H

//#pragma once

#include "code_tag_t.h"
#include "code_group_t.h"

BEGIN_CODE_NAMESPACE

class collapse_tag_t : public tag_t, public group_t {
public:
						collapse_tag_t(
							tagid_t				tag );

	virtual void		text(
							istring&			text );

	virtual bool		can_expand();

	void				transfer_lines(
							line_t**			lines );							

protected:
	void				cleanup();

	virtual				~collapse_tag_t();
};

END_CODE_NAMESPACE

#endif

