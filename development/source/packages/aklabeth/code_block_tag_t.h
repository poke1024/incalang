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
//	code_block_tag_t.h    	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_BLOCK_TAG_H
#define CODE_BLOCK_TAG_H

//#pragma once

#include "code_collapse_tag_t.h"

BEGIN_CODE_NAMESPACE

class block_tag_t : public collapse_tag_t {
public:
						block_tag_t(
							line_t**			lines,
							long				count );

	virtual long		token_count() const;

	virtual void		save(
							stream_t&			stream,
							const options_t&	options );

protected:
	long				m_token_count;
};

END_CODE_NAMESPACE

#endif

