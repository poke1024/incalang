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
//	scrap_t.h	 			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef OSWRAP_SCRAP_H
#define OSWRAP_SCRAP_H

//#pragma once

#include "oswrap.h"

BEGIN_OSWRAP_NAMESPACE

typedef	u32					clip_tag_t;

class clip_t {
public:
	virtual					~clip_t();
	
	virtual void			to_text(
								istring&			text ) = 0;
	
	virtual clip_tag_t		tag() const = 0;
};

class scrap_t {
public:
							scrap_t();
							
	virtual					~scrap_t();

	void					clear();

	clip_t*					put(
								clip_t*				clip );

	clip_t*					get(
								clip_tag_t			tag );
	
	clip_t*					get(
								const clip_tag_t*	tags,
								int					count );
	
	static scrap_t*			instance();
								
protected:
	struct node_t {
		node_t*				next;
		clip_tag_t			tag;
		clip_t*				clip;
		u64					timestamp;
	};
	
	node_t*					m_clips;
};

END_OSWRAP_NAMESPACE

#endif

