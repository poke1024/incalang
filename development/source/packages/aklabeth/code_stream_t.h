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
//	code_stream_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_STREAM_H
#define CODE_STREAM_H

//#pragma once

#include "aklabeth.h"
#include "code_text_t.h"
#include "code_line_t.h"
#include "output_stream_t.h"

BEGIN_CODE_NAMESPACE

class stream_t {
public:
						stream_t(
							output_stream_t*	stream );

	void				append(
							const char*			text );

	void				append_literal(
							const char*			text );

	void				append_declaration(
							const char*			text );

	void				append_space();

	void				trim_tail();
	
	void				new_line();
	
	const options_t&	options();
	
	bool				enable_newline(
							bool				newline );
	
protected:
	text_t				m_text;
	output_stream_t*	m_stream;
	options_t			m_options;
	bool				m_newline;
};

inline void
stream_t::append(
	const char*			text )
{
	m_text.append( text );
}

inline void
stream_t::append_literal(
	const char*			text )
{
	m_text.append_literal( text );
}

inline void
stream_t::append_declaration(
	const char*			text )
{
	m_text.append_declaration( text );
}

inline void
stream_t::append_space()
{
	m_text.append_space();
}

inline void
stream_t::trim_tail()
{
	m_text.trim_tail();
}

END_AKLABETH_NAMESPACE

#endif

