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
//	code_text_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_TEXT_H
#define CODE_TEXT_H

//#pragma once

#include "code.h"

BEGIN_CODE_NAMESPACE

class tag_t;

class text_t {
public:
						text_t();

	void				append(
							const char*			text );

	void				append_literal(
							const char*			text );

	void				append_declaration(
							const char*			text );

	void				append_space();

	void				trim_tail();
		
	void				append_tag(
							tag_t*				tag );
	
	void				append_ptr(
							void*				ptr );
	
	const char*			data() const;
	
	long				length() const;

	void				clear();
	
protected:
	istring				m_text;
};

END_CODE_NAMESPACE

#endif

