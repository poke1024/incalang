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
//	code_block_tag_t.cp	       ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_block_tag_t.h"
#include "code_line_t.h"
#include "code_stream_t.h"
#include "code_line_lexer_t.h"

BEGIN_CODE_NAMESPACE

block_tag_t::block_tag_t(
	line_t**			lines,
	long				count ) :

	collapse_tag_t( tag_block )
{
	m_lines = new line_t*[ count ];
	m_line_count = count;
	
	long				token_count = 0;
	
	for( long i = 0; i < count; i++ )
	{
		line_t*			line = lines[ i ];

		token_count += line->token_count;
		m_lines[ i ] = line;
	}
	
	m_token_count = token_count;
}

long
block_tag_t::token_count() const
{
	return m_token_count;
}

void
block_tag_t::save(
	stream_t&			stream,
	const options_t&	options )
{
	if( m_lines == 0 )
		throw_icarus_error( "no lines to save" );

	stream.append( "{" );
	stream.new_line();
	stream.append( "/*c */" );
	stream.new_line();
		
	long				count = m_line_count;
	line_t**			lines = m_lines;
	line_lexer_t		lexer;
	
	for( long i = 0; i < count - 1; i++ )
	{
		lexer.init( lines[ i ] );
		line_save( &lexer, options, stream );
	}
	
	if( count > 0 )
	{
		stream.enable_newline( false );
		lexer.init( lines[ count - 1 ] );
		line_save( &lexer, options, stream );
		stream.enable_newline( true );
	}
}
END_CODE_NAMESPACE
