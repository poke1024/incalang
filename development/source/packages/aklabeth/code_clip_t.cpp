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
//	code_clip_t.cp	 		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_clip_t.h"
#include "code_tag_t.h"

BEGIN_AKLABETH_NAMESPACE

code_clip_t::code_clip_t(
	const char*			text,
	long				length )
{
	m_textbuf = new char[ length ];
	m_textlen = length;

	code::take_code( m_textbuf, text, length );
}

code_clip_t::~code_clip_t()
{
	if( m_textbuf )
	{
		code::drop_code( m_textbuf, m_textlen );
		delete[] m_textbuf;
		m_textbuf = 0;
		m_textlen = 0;
	}
}

const char*
code_clip_t::text_ptr() const
{
	return m_textbuf;
}

long
code_clip_t::text_length() const
{
	return m_textlen;
}

void
code_clip_t::to_text(
	istring&			text )
{
	text.assign( m_textbuf, m_textlen );
}

clip_tag_t
code_clip_t::tag() const
{
	return code_clip_tag;
}

END_AKLABETH_NAMESPACE
