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
//	code_line_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_LINE_H
#define CODE_LINE_H

//#pragma once

#include "code_text_t.h"
#include "code_options.h"

BEGIN_CODE_NAMESPACE

enum {
	LF_STATEMENT_ANCHOR	= 1,
	LF_BREAKPOINT		= 2				
};

struct line_t {
	s08					delimiter_balance;
	s08					parenthes_balance;
	s08					comment_balance;
	u08					flags;
	u16					token_count;
	u08					tokens[ 1 ];
};

class collapse_tag_t;
class stream_t;
class line_lexer_t;

line_t*
line_create(
	const char*			basepath,
	const char*			text,
	long				length );

void
line_dispose(
	line_t*				line );

bool
line_collapse(
	line_t*				line );

collapse_tag_t*
line_uncollapse(
	line_t*				line );

bool
line_collapsed(
	line_t*				line );

bool
line_nth_token_in_tag(
	line_t*				line,
	long				index );

void
line_text(
	line_lexer_t*		lexer,
	const options_t&	options,
	text_t&				text,
	long				count,
	int					flags );
	
void
line_save(
	line_lexer_t*		lexer,
	const options_t&	options,
	stream_t&			text );

END_CODE_NAMESPACE

#endif

