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
//	code_line_lexer_t.h		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_LINE_LEXER_H
#define CODE_LINE_LEXER_H

//#pragma once

#include "code.h"
#include "random_stream_t.h"

#if __VISC__
#include "code_line_t.h"
#include "code_tag_t.h"
#endif

BEGIN_CODE_NAMESPACE

class line_t;
class tag_t;

class line_lexer_t {
public:
						line_lexer_t();
						
						line_lexer_t(
							line_t*				line );

	void				init(
							line_t*				line );

	int					next();
	
	inline u08*			data() const;
	
	inline tag_t*		tag() const;
	
	inline const char*	string() const;
	
	inline int			flag() const;
	
	inline line_t*		line() const;

private:
	line_t*				m_line;
	s16					m_index;
	u08*				m_data;
};

inline u08*
line_lexer_t::data() const
{
	return m_data;
}

inline tag_t*
line_lexer_t::tag() const
{
	return (tag_t*)cond_swap_endian64(
		*(iptr_t*)m_data );
}
	
inline const char*
line_lexer_t::string() const
{
	return (const char*)m_data;
}	

inline int
line_lexer_t::flag() const
{
	return *m_data;
}

inline line_t*
line_lexer_t::line() const
{
	return m_line;
}

void
setup_token_stream(
	const char*			basepath,
	random_stream_t&	stream,
	line_t*				line );

END_CODE_NAMESPACE

#endif

