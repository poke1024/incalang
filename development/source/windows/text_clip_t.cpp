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
//	text_clip_t.cp	 		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "text_clip_t.h"

BEGIN_OSWRAP_NAMESPACE

text_clip_t::text_clip_t(
	const char*			text,
	long				length )
{
	m_text.assign( text, length );
}

const char*
text_clip_t::text_ptr() const
{
	return m_text.data();
}

long
text_clip_t::text_length() const
{
	return m_text.length();
}

void
text_clip_t::to_text(
	istring&			text )
{
	text = m_text;
}

clip_tag_t
text_clip_t::tag() const
{
	return text_clip_tag;
}

END_OSWRAP_NAMESPACE
