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
//	code_line_lexer_t.cp	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_line_lexer_t.h"
#include "code_line_t.h"
#include "code_tag_t.h"
#include "code_include_tag_t.h"
#include "aklabeth_lexer.h"

BEGIN_CODE_NAMESPACE

line_lexer_t::line_lexer_t()
{
}
						
line_lexer_t::line_lexer_t(
	line_t*				line )
{
	init( line );
}

void
line_lexer_t::init(
	line_t*				line )
{
	m_line = line;
	m_index = 0;
}

int
line_lexer_t::next()
{
	line_t*				line = m_line;
	int					index = m_index;
	int					token;
	u08*				buffer;

	buffer = &line->tokens[ index++ ];
	token = *buffer++;
	m_data = buffer;
		
	switch( token )
	{
		case token_EndOfLine:
			index -= 1;
			break;
	
		case token_Identifier:
			index += 1;
		
		case token_HexConstant:
		case token_BinConstant:
		case token_OctConstant:
		case token_IntConstant:
		case token_ChrConstant:
		case token_FltConstant:
		case token_StringLiteral:
		case token_CommentCpp:
			while( line->tokens[ index++ ] != '\0' )
				;
			break;

		case token_Star:
		case token_Plus:
		case token_Minus:
		case token_And:
		case token_ClampOpen:
		case token_ClampClose:
		case token_BrackOpen:
			index += 1;
			break;

		case token_Tag:
			index += ICARUS_PTR_SIZE;
			break;
	}
	
	m_index = index;
	
	return token;
}

// ---------------------------------------------------------------------------

void
setup_token_stream(
	const char*			basepath,
	random_stream_t&	stream,
	line_t*				line )
{
	int					dbal = 0;
	int					pbal = 0;
	int					cbal = 0;
	bool				anchor = false;
	long				token_count = 0;

	while( true )
	{
		int				token;
	
		token = yylex();

process:
		if( token == 0 )
			break;
						
		stream.write_u08( token );
		token_count++;
			
		switch( token )
		{
			case token_Identifier:
				stream.write_u08( 0 );

			case token_HexConstant:
			case token_BinConstant:
			case token_OctConstant:
			case token_IntConstant:
			case token_ChrConstant:
			case token_FltConstant:
			case token_StringLiteral:
			case token_CommentCpp:
				stream.write_bytes( yylval.text,
					( istrlen( yylval.text )
						+ 1 ) * sizeof( char ) );
				break;
				
			case token_Star:
			case token_Plus:
			case token_Minus:
			case token_And:
			case token_ClampOpen:
			case token_ClampClose:
				stream.write_u08( 0 );
				break;
				
			case token_Tag:
			{
				tag_t*	tag = yylval.tag;
				
				tag->refer();
				stream.write_ptr( tag );
				token_count += tag->token_count() - 1;
				break;
			}
			
			case token_Include:
			{
				token = yylex();
				if( token != token_StringLiteral )
					goto process;
			
				istring path = yylval.text;
												
				token = yylex();
				if( token != 0 )
					goto process;

				tag_t*	tag;

				tag = new include_tag_t( basepath, path.c_str() );
				stream.seek( -1, seek_mode_relative );
				stream.write_u08( token_Tag );
				stream.write_ptr( tag );
				token_count += tag->token_count() - 1;
				
				goto process;
			}
			
			case token_BrackOpen:
				stream.write_u08( 0 );
				// fall through
				
			case token_ParenOpen:
				pbal += 1;
				break;		

			case token_ParenClose:
			case token_BrackClose:
				pbal -= 1;
				break;
				
			case token_DelimOpen:
				dbal += 1;
				break;
				
			case token_DelimClose:
				dbal -= 1;
				break;

			case token_If:
			case token_For:
			case token_While:
				if( token_count == 1 )
					anchor = true;
				break;

			case token_CommentBegin:
				cbal += 1;
				break;

			case token_CommentEnd:
				cbal -= 1;
				break;
		}
	}
	
	stream.write_u08( token_EndOfLine );

	line->delimiter_balance = dbal;
	line->parenthes_balance = pbal;
	line->comment_balance = cbal;
	line->flags = 0;
	line->token_count = token_count;

	if( anchor )
		line->flags |= LF_STATEMENT_ANCHOR;
}

END_CODE_NAMESPACE
