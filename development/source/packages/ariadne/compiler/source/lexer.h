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
//	lexer.h					   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_LEXER_H
#define ARIADNE_LEXER_H

//#pragma once

#include "compiler.h"
#include "input_stream_t.h"

BEGIN_COMPILER_NAMESPACE

void
init_lexer(
	input_stream_t*		yystream );

void
cleanup_lexer();

void
get_filepos(
	fileref_t*			filepos );
	
enum {
	LEX_CHAR_FILEREF	= '\001',
	
	LEX_FILEID_CHARS	= 8,
	LEX_FILEPOS_CHARS	= 8,
	LEX_FILEREF_SIZE	= 1 + LEX_FILEID_CHARS + LEX_FILEPOS_CHARS
};

END_COMPILER_NAMESPACE

#endif

