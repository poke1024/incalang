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
//	code_stream_t.cp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_stream_t.h"

BEGIN_CODE_NAMESPACE

stream_t::stream_t(
	output_stream_t*	stream ) :
	
	m_stream( stream )
{
	m_options.space_operators = false;
	m_options.space_statements = false;
	m_options.space_brackets = false;
	m_options.space_parentheses = false;
	m_options.space_delimiters = false;
	m_options.space_clamps = false;
	m_options.space_pointer = false;
	istrcpy( m_options.comma, "," );
	istrcpy( m_options.semicolon, ";" );
}

void
stream_t::new_line()
{
	if( m_newline )
		m_text.append( "\r\n" );
		//m_text.append( "\n" );
	
	//const char* s = m_text.data();
	
	m_stream->write_bytes(
		m_text.data(),
		m_text.length() * sizeof( char ) );
		
	m_text.clear();
}

const options_t&
stream_t::options()
{
	return m_options;
}

bool
stream_t::enable_newline(
	bool				newline )
{
	bool				state = m_newline;
	
	m_newline = newline;
	return state;
}

END_CODE_NAMESPACE
