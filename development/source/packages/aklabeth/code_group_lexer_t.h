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
//	code_group_lexer_t.h	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_GROUP_LEXER_H
#define CODE_GROUP_LEXER_H

//#pragma once

#include "code.h"
#include "code_line_lexer_t.h"

BEGIN_CODE_NAMESPACE

class group_t;
class line_t;
class tag_t;

class group_lexer_t {
public:
						group_lexer_t(
							group_t*			group );

	virtual				~group_lexer_t();

	int					next();
	
	long				location() const;
	
	inline u08*			data() const;
	
	inline tag_t*		tag() const;
	
	inline const char*	string() const;
	
	inline int			flag() const;
	
private:
	struct node_t {
		node_t*			next;
		group_t*		group;
		long			line;
		long			line_count;
		line_lexer_t	lexer;
	};

	void				push(
							group_t*			group );

	node_t*				pop();

	int					process_tag(
							tag_t*				tag );

	node_t*				end_of_line(
							node_t*				node );

	node_t*				m_stack;
	node_t*				m_node_pool;
	s32					m_location;
};

inline long
group_lexer_t::location() const
{
	return m_location;
}

inline u08*
group_lexer_t::data() const
{
	return m_stack->lexer.data();
}

inline tag_t*
group_lexer_t::tag() const
{
	return m_stack->lexer.tag();
}
	
inline const char*
group_lexer_t::string() const
{
	return m_stack->lexer.string();
}	

inline int
group_lexer_t::flag() const
{
	return m_stack->lexer.flag();
}

END_CODE_NAMESPACE

#endif

