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
//	code_parser_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_PARSER_H
#define CODE_PARSER_H

//#pragma once

#include "code_line_lexer_t.h"

BEGIN_CODE_NAMESPACE

class file_t;
class tag_t;

class parser_t {
public:
						parser_t(
							file_t*				file,
							long				line,
							long				bottom );

	void				parse();

protected:
	bool				parse_if();
	
	bool				parse_loop();
	
	bool				parse_do_while();
	
	bool				parse_switch();
	
	bool				parse_enum();
	
	bool				parse_catch();
	
	bool				parse_simple_stmt();
	
	bool				parse_compound_stmt(
							bool				inside_block );
	
	bool				skip(
							int					mode );
							
	bool				skip_until(
							int					pattern );

	void				skip_operator();

	int					next_token();
	
	void				store_ident(
							int					line = 0 );
							
	void				annotate_token(
							int					note );
	
	file_t*				m_file;
	u08					m_token;
	line_lexer_t		m_lexer;
	
	s32					m_lines_left;
	s32					m_line_count;
	s32					m_line_index;
	s32					m_line_bottom;
	
	s32					m_line_ident;
	bool				m_anchor_found;
	bool				m_line_feed;
	
	int					m_stmt_tabs;
	long				m_paren_balance;
};

END_CODE_NAMESPACE

#endif

