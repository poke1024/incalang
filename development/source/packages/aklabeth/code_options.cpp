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
//	code_options.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_options.h"
#include "icarus_file.h"

//#include "code_options_xml.i"

extern "C" {
code_options_t			s_code_options;
}

BEGIN_CODE_NAMESPACE

//using namespace			code_options_xml;

void
read_options(
	const char*			path )
{
	code_options_t*		options;

	options = &s_code_options;

	options->space_operators = true;
	options->space_statements = false;
	options->space_brackets = true;
	options->space_parentheses = true;
	options->space_delimiters = true;
	options->space_clamps = false;
	options->space_pointer = true;
	options->bind_pointer_left = false;
	options->declaration_tabs = 0;
	options->access_modifier_tabs = -1;
	options->switch_tabs = 1;
	
	strcpy( options->comma, ", " );
	strcpy( options->semicolon, "; " );

	/*if( ifexists( path ) )
	{
		element_t*		root = new root_t( &s_code_options );
		parser_t		parser( path, root );

		parser.parse_document();
	}
	else
	{
		body_t			body( &s_code_options );
	}*/
}

END_CODE_NAMESPACE
