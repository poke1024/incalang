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
//	text_clip_t.h	 		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef OSWRAP_TEXT_CLIP_H
#define OSWRAP_TEXT_CLIP_H

//#pragma once

#include "scrap_t.h"

BEGIN_OSWRAP_NAMESPACE

enum {
	text_clip_tag			= 'text'
};

class text_clip_t : public clip_t {
public:
							text_clip_t(
								const char*			text,
								long				length );

	const char*				text_ptr() const;
	
	long					text_length() const;
	
	void					to_text(
								istring&			text );

	virtual clip_tag_t		tag() const;

private:
	istring					m_text;
};

END_OSWRAP_NAMESPACE

#endif

