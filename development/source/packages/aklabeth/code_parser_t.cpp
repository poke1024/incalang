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
//	code_parser_t.cp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_parser_t.h"
#include "code_file_t.h"
#include "code_tag_t.h"
#include "aklabeth_lexer.h"
#include "parser.h"

BEGIN_CODE_NAMESPACE

enum {
	skip_Conditional,
	skip_Statement,
	skip_Case,
	skip_Enumerator,
	skip_Catch
};

static bool
left_token_operand(
	int					id )
{
	switch( id )
	{
		case token_Identifier:
		case token_HexConstant:
		case token_BinConstant:
		case token_OctConstant:
		case token_IntConstant:
		case token_ChrConstant:
		case token_FltConstant:
		case token_StringLiteral:

		case token_True:
		case token_False:
		
		case token_IncOp:
		case token_DecOp:
		
		case token_Null:
		case token_This:
		
		case token_BrackClose:
		case token_ParenClose:
			return true;
	}
	
	return false;
}

// ---------------------------------------------------------------------------

parser_t::parser_t(
	file_t*				file,
	long				line,
	long				bottom ) :
	
	m_file( file )
{
	long				count = file->line_count() - line;

	if( count <= 0 )
		throw_icarus_error( "illegal line count" );

	m_lexer.init( m_file->line( line ) );

	m_line_index = line;
	m_line_bottom = bottom;
	m_lines_left = count;
	m_line_count = count;
	
	m_line_ident = 0;
	m_anchor_found = false;
	m_line_feed = false;

	m_stmt_tabs = 0;
	m_paren_balance = 0;
	
	next_token();
}

void
parser_t::parse()
{
	while( true )
	{
		switch( m_token )
		{
			case token_EndOfLine:
				return;
		
			default:
				if( parse_simple_stmt() == false )
					return;
				
				if( m_anchor_found &&
					m_line_index >= m_line_bottom )
				{
					store_ident();

					// next statement might also
					// be affected in identation

					s32	index = m_line_index;

					while( m_lines_left && m_line_index == index )
						parse_simple_stmt();

					return;
				}
				
				break;
		}
	}
}

bool
parser_t::parse_simple_stmt()
{
	switch( m_token )
	{
		case token_If:
			m_anchor_found = true;
			return parse_if();
				
		case token_For:
		case token_While:
			m_anchor_found = true;
			return parse_loop();
		
		case token_Do:
			m_anchor_found = true;
			return parse_do_while();

		case token_Switch:
			m_anchor_found = true;
			return parse_switch();

		case token_Enum:
			return parse_enum();

		case token_Try:
			next_token();
			return true;

		case token_Catch:
			return parse_catch();

		case token_DelimOpen:
			if( parse_compound_stmt( false ) == false )
				return false;
			break;
			
		case token_Public:
		case token_Private:
		case token_Protected:
		{
			int			ident;
			
			ident = s_code_options.access_modifier_tabs;
			m_line_ident += ident;
			store_ident();
			m_line_ident -= ident;
			next_token();
			break;
		}
		
		case token_CommentBegin:
			if( skip_until( token_CommentEnd ) == false )
				return false;
			next_token();
			break;
		
		case token_CommentCpp:
			next_token();
			break;
		
		case token_Case:
		case token_Default:
			if( skip( skip_Case ) == false )
				return false;
			break;
				
		default:
			if( skip( skip_Statement ) == false )
				return false;
			break;
	}
	
	return true;
}

bool
parser_t::parse_if()
{	
	next_token();
	
	if( skip( skip_Conditional ) == false )
		return false;
	
	if( parse_compound_stmt( false ) == false )
		return false;
		
	if( m_token == token_Else )
	{
		next_token();

		if( parse_compound_stmt( m_token == token_If ) == false )
			return false;
	}
	
	return true;
}

bool
parser_t::parse_loop()
{
	next_token();
	
	if( skip( skip_Conditional ) == false )
		return false;
	
	if( parse_compound_stmt( false ) == false )
		return false;
	
	return true;
}

bool
parser_t::parse_do_while()
{
	next_token();
	
	if( parse_compound_stmt( false ) == false )
		return false;
		
	if( m_token != token_While )
		return false;
		
	next_token();
		
	if( skip( skip_Conditional ) == false )
		return false;
	
	return true;
}

bool
parser_t::parse_switch()
{
	next_token();
	
	if( skip( skip_Conditional ) == false )
		return false;

	if( m_token != token_DelimOpen )
	{
		if( parse_compound_stmt( true ) == false )
			return false;
	}
	
	next_token();

	int					ident;

	ident = s_code_options.switch_tabs;
		
	m_line_ident += ident;
	
	while( m_token != token_DelimClose )
	{
		if( parse_simple_stmt() == false )
			return false;
	}
				
	m_line_ident -= ident;
	store_ident();
	next_token();
		
	return true;
}

bool
parser_t::parse_enum()
{
	if( skip_until( token_DelimOpen ) == false )
		return false;

	next_token();

	while( m_token != token_DelimClose )
	{
		if( m_token == token_EndOfLine )
			return false;
			
		if( skip( skip_Enumerator ) == false )
			return false;
	}
	
	next_token();
		
	return true;
}

bool
parser_t::parse_catch()
{
	next_token();
	
	skip( skip_Catch );
		
	return true;
}

bool
parser_t::parse_compound_stmt(
	bool				inside_block )
{
	if( m_token == token_DelimOpen )
	{	
		next_token();
		
		while( true )
		{
			switch( m_token )
			{
				case token_EndOfLine:
					return false;
				
				case token_DelimClose:
					next_token();
					return true;
					
				default:
					if( parse_compound_stmt( true ) == false )
						return false;
					break;
			}
		}
	}
	else
	{
		if( inside_block == false )
		{
			m_line_ident += 1;
			if( m_line_feed )
				store_ident();
		}
	
		if( parse_simple_stmt() == false )
			return false;

		if( inside_block == false )
		{
			m_line_ident -= 1;
			if( m_line_feed )
				store_ident();
		}
	}
	
	return true;
}

bool
parser_t::skip(
	int						mode )
{
	// [declaration] is a flag, that describes if we expect
	// a declaration of variables to take place. if it is set
	// then we assume to be inside an expression like int *b;
	// the simple rule we use is, as long as we read identifiers,
	// scope ops, stars, type names and commas, we assume to
	// be inside a declaration. A special handling for "("
	// is used.
	
	// [variable] describes if we expect to see a variable
	// name as next token (or at least, if this would be legal
	// to happen). we basically increment, after we
	// have read a sequence of tokens that resembles a valid
	// type declaration, and zero it, if we have not.
	
	// [typecast] counts if we are inside a type cast
	// expression like cast<type>( value ). if we read
	// "cast<", we increment it, after we read the next
	// ">", we decrement it.

	int						pbal;
	bool					declaration = true;

	switch( mode )
	{
		case skip_Statement:
			m_stmt_tabs = 1;
			declaration = true;
			pbal = 0;
			break;
			
		case skip_Conditional:
			if( m_token != token_ParenOpen )
				return false;
		
			next_token();
			declaration = false;
			pbal = 1;
			break;
	
		case skip_Enumerator:
			declaration = false;
			pbal = 0;
			break;
	
		case skip_Case:
			next_token();
			declaration = false;
			pbal = 0;
			m_line_ident -= 1;
			break;
	
		case skip_Catch:
			if( m_token != token_ParenOpen )
				return false;
			next_token();
			declaration = true;
			pbal = 1;
			break;
	}

	int						typecast = 0;
	int						tmpltype = 0;
	int						variable = 0;
	u08*					identdata = 0;

	int						last = 0;

	while( true )
	{
		int					token = m_token;
	
		while( true )
		{
			if( token == token_Cast )
			{
				next_token();
				if( m_token != token_ClampOpen )
					return false;
				annotate_token( 1 );
				typecast += 1;
				next_token();
				token = m_token;
			}
			else if( token == token_Template )
			{
				next_token();
				while( true )
				{
					if( m_token != token_Identifier )
						return false;
					next_token();
					if( m_token != token_Comma )
						break;
				}
				token = m_token;
			}
			else
				break;
		}
	
		switch( token )
		{
			case token_Operator:
				skip_operator();
				variable += 1;
				break;

			case token_Identifier:
				identdata = m_lexer.data();
				annotate_token( variable ? 2 : 0 );
				variable += 1;
				break;
			
			case token_ScopeOp:
			case token_Typedef:
				variable = 0;
				break;

			case token_Star:
			case token_And:
				break;
				
			case token_Static:
			case token_Const:
			case token_Native:
			case token_Abstract:

			case token_Unsigned:
			case token_Signed:
			case token_Bool:
			case token_Byte:
			case token_Char:
			case token_Double:
			case token_Float:
			case token_Int:
			case token_Long:
			case token_Wide:
			case token_Short:
			case token_Void:
				variable += 1;
				break;
				
			case token_Comma:
				if( mode == skip_Statement && pbal == 0 )
					declaration = true;
	
				variable = 0;
				break;
				
			case token_ParenOpen:
				// we probably read a function call or a function
				// declaration. if it was the latter, variable
				// should be > 1 (whereas for a call, it's <= 1).
				// on this basis we decide, whether we expect an
				// expression like ( a * b ) to be an arithmetic
				// value or a pointer type declaration (i.e. a *b ).
				
				// if we're inside an if-clause or the like, the
				// function call argument will also hold.
				
				if( variable > 1 )
				{
					// inside a function declaration header
					declaration = true;
					
					if( identdata )
					{
						*identdata = 1;
						identdata = 0;
					}
				}
				else
					declaration = false;
					
				variable = 0;
				break;

			default:
				declaration = false;
				variable = 0;
				break;
		}
	
		switch( token )
		{
			case token_EndOfLine:
				return false;
				
			case token_ParenOpen:
				pbal += 1;
				m_paren_balance += 1;
				break;

			case token_ClampOpen:
				if( last == token_Identifier &&
					compiler::is_type_name(
						(char*)( identdata + 1 ) ) )
				{
					annotate_token( 1 );
					tmpltype += 1;
				}
				else
					annotate_token( 0 );
				break;
			
			case token_BrackOpen:
				m_paren_balance += 1;
				annotate_token( variable ? 1 : 0 );
				break;

			case token_ParenClose:
				pbal -= 1;
				
				if( pbal == 0 )
				{
					switch( mode )
					{
						case skip_Conditional:
						case skip_Catch:
							goto accept;
					}
				}
				
				m_paren_balance = -1;
				break;

			case token_ClampClose:
				if( tmpltype )
				{
					annotate_token( 2 );
					tmpltype--;
					variable += 1;
				}
				else if( typecast )
				{
					annotate_token( 1 );
					typecast--;
				}
				else
					annotate_token( 0 );
				break;
			
			case token_BrackClose:
				m_paren_balance = -1;
				break;

			case token_And:
			{
				int			flag;
				bool		operand;
				
				operand = left_token_operand( last );
			
				if( last == token_And && declaration )
					flag = 3;
				else if( declaration && variable &&
                    s_code_options.bind_pointer_left == false )
					flag = 2;			
				else if( declaration || operand == false )
					flag = 1;
				else
					flag = 0;
			
				annotate_token( flag );
				
				if( s_code_options.bind_pointer_left == false )
					variable = 0;
					
				break;
			}

			case token_Star:
			{
				int			flag;
				bool		operand;
				
				operand = left_token_operand( last );
			
				if( last == token_Star && declaration )
					flag = 3;
				else if( declaration && variable && 
                    s_code_options.bind_pointer_left == false )
					flag = 2;			
				else if( declaration || operand == false )
					flag = 1;
				else
					flag = 0;
			
				annotate_token( flag );
				
				if( s_code_options.bind_pointer_left == false )
					variable = 0;
					
				break;
			}

			case token_Plus:
			case token_Minus:
				annotate_token(
					left_token_operand( last ) ? 1 : 0 );
				break;

			case token_DelimClose:
				if( mode == skip_Enumerator )
				{
					m_paren_balance = 0;
					return true;
				}

			case token_DelimOpen:

			case token_Semicolon:
				if( mode == skip_Statement )
					goto accept;
				break;
				
			case token_Comma:
				if( mode == skip_Enumerator )
					goto accept;
				break;
				
			case token_Colon:
				if( mode == skip_Case )
					goto accept;
				break;
				
			case token_Tag:
				if( mode == skip_Statement )
				{
					switch( m_lexer.tag()->id() )
					{
						case tag_block:
						case tag_include:
							goto accept;
					}
				}
				break;
		}
		
		next_token();
		last = token;
	}
	
accept:
	switch( mode )
	{
		case skip_Statement:
			m_stmt_tabs = 0;
			break;
			
		case skip_Case:
			store_ident();
			m_line_ident += 1;
			break;
	}
	
	m_paren_balance = 0;
	next_token();
					
	return true;
}

bool
parser_t::skip_until(
	int					pattern )
{
	int					token;
	do {
		token = next_token();

		if( token == token_EndOfLine )
			return false;
	} while( token != pattern );

	return true;
}

void
parser_t::skip_operator()
{
	next_token();
	switch( m_token )
	{
		case token_Star:
			annotate_token( 4 );
			break;

		case token_BrackOpen:
			next_token();
			break;
	}
}

int
parser_t::next_token()
{
	if( !m_lines_left )
		return token_EndOfLine;
	
	m_line_feed = false;
	
	int					token;
	
	while( true )
	{
		token = m_lexer.next();
	
		if( token != token_EndOfLine )
			break;
	
		if( --m_lines_left == 0 )
			break;

		m_lexer.init(
			m_file->line( ++m_line_index ) );
			
		store_ident();
		m_line_feed = true;
	}

	m_token = token;

	return token;
}

void
parser_t::store_ident(
	int					line )
{
	long				index = m_line_index + line;

	if( index < 0 || index >= m_line_count )
	{
#if ICARUS_SAFE
		throw_icarus_error( "invalid line" );
#else
		return;
#endif
	}

	long				ident = m_line_ident;
	long				stmt_tabs = m_stmt_tabs;

	if( stmt_tabs > 0 )
	{
		ident += stmt_tabs;
		if( m_paren_balance != 0 )
			ident -= 1;	
	}

	m_file->m_stmt_ident[
		 m_line_index + line ] = ident;
}

void
parser_t::annotate_token(
	int					note )
{
	*m_lexer.data() = note;
}

END_CODE_NAMESPACE
