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
//	code_line_t.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_line_t.h"
#include "memory_buffer_t.h"
#include "code_stream_t.h"
#include "code_line_lexer_t.h"
#include "code_collapse_tag_t.h"
#include "xalloc.h"
#include "aklabeth_lexer.h"
#include "parser.h"

BEGIN_CODE_NAMESPACE

YYSTYPE					yylval;

int yywrap()
{
	return 1;
}

// ---------------------------------------------------------------------------

line_t*
line_create(
	const char*			basepath,
	const char*			text,
	long				length )
{
	int					flags = 0;

	if( length >= 5 && memcmp( text, "/*b*/", 5 ) == 0 )
	{
		flags |= LF_BREAKPOINT;
		text += 5;
		length -= 5;
	}

	memory_buffer_t		textbuf;	
	
	textbuf.write_bytes( text,
		length * sizeof( char ) );
	textbuf.write_char( '\0' );
	textbuf.seek( 0, seek_mode_begin );
	
	memory_buffer_t		tokenbuf;

	push_xalloc_frame();
	init_lexer( &textbuf );
	
	line_t				temp;
	
	setup_token_stream( basepath, tokenbuf, &temp );
	
	cleanup_lexer();
	pop_xalloc_frame();
	
	line_t*				line;
	long				size;
	
	tokenbuf.seek( 0, seek_mode_begin );
	size = tokenbuf.length();
	
	line = (line_t*)new u08[ sizeof( line_t ) + size ];
	imemcpy( line->tokens, tokenbuf.access(), size );

	line->delimiter_balance = temp.delimiter_balance;
	line->parenthes_balance = temp.parenthes_balance;
	line->comment_balance = temp.comment_balance;
	line->flags = temp.flags | flags;
	line->token_count = temp.token_count;
		
	return line;
}

void
line_dispose(
	line_t*				line )
{
	line_lexer_t		lexer( line );
	int					token;
	
	while( ( token = lexer.next() ) != token_EndOfLine )
	{
		if( token == token_Tag )
			lexer.tag()->unrefer();
	}
	
	delete[] (u08*)line;
}

// ---------------------------------------------------------------------------

bool
line_collapse(
	line_t*				line )
{
	line_lexer_t		lexer( line );
	int					token;
	u08*				delim = 0;
	
	while( ( token = lexer.next() ) != token_EndOfLine )
	{
		if( token == token_DelimOpen )
			delim = lexer.data() - 1;
		else
			delim = 0;
	}

	if( delim )
	{
		*delim = token_EndOfLine;
		return true;
	}
	
	return false;
}

collapse_tag_t*
line_uncollapse(
	line_t*				line )
{
	line_lexer_t		lexer( line );
	int					token;
	
	while( ( token = lexer.next() ) != token_EndOfLine )
	{
		if( token != token_Tag )
			continue;
			
		tag_t*			tag = lexer.tag();
		tagid_t			id = tag->id();
		
		if( id != tag_block && id != tag_include )
			continue;
			
		if( ( (collapse_tag_t*)tag )->can_expand() == false )
			return 0;
			
		u08*			ptr = lexer.data() - 1;

		if( lexer.next() != token_EndOfLine )
			break;

		if( id == tag_block )
		{
			ptr[ 0 ] = token_DelimOpen;
			ptr[ 1 ] = token_EndOfLine;
			line->token_count -= tag->token_count();
			line->token_count += 1;
		}
		else if( id == tag_include )
		{
			ptr[ 0 ] = token_EndOfLine;
			line->token_count -= tag->token_count();
		}
		
		return (collapse_tag_t*)tag;
	}

	return 0;
}

bool
line_collapsed(
	line_t*				line )
{
	line_lexer_t		lexer( line );
	int					token;
	
	while( ( token = lexer.next() ) != token_EndOfLine )
	{
		if( token != token_Tag )
			continue;
			
		tag_t*			tag = lexer.tag();
		tagid_t			id = tag->id();
		
		if( id != tag_block && id != tag_include )
			continue;

		return true;
	}

	return false;
}

bool
line_nth_token_in_tag(
	line_t*				line,
	long				index )
{
	line_lexer_t		lexer( line );
	int					token;
	
	while( ( token = lexer.next() ) != token_EndOfLine )
	{
		if( index == 1 )
			return false;
			
		index--;
		
		if( token != token_Tag )
			continue;
	
		tag_t*			tag = lexer.tag();
		long			count = tag->token_count();
		
		if( index < count )
			return true;
			
		index -= count;
	}

	return false;
}

// ---------------------------------------------------------------------------

#include "token_table.i"

enum {
	nofunc_state		= 1,
	typecast_state		= 2
};

#define TYPE_SEEN {							\
	if( last == token_ParenOpen &&			\
		( state & nofunc_state ) )			\
	{										\
		state |= typecast_state;			\
		text.trim_tail();					\
	} }

void
line_text(
	line_lexer_t*		lexer,
	const options_t&	options,
	text_t&				text,
	long				count,
	int					optflags )
{
	int					id;
	bool				flag;
	int					last = 0;
	int					state = 0;
	int					opcount = 0;

	while( ( id = lexer->next() ) != token_EndOfLine )
	{
		if( --count == 0 )
			break;
	
		switch( id )
		{
			#include "format_token.i"
				
			case token_Tag:
				flag = options.space_delimiters;
				if( flag )
					text.append_space();
				lexer->tag()->serialize( text, optflags );
				if( flag )
					text.append_space();
				break;
		}
		
		last = id;
		opcount--;
	}

	text.trim_tail();
}

void
line_save(
	line_lexer_t*		lexer,
	const options_t&	options,
	stream_t&			text )
{
	if( lexer->line()->flags & LF_BREAKPOINT )
		text.append( "/*b*/" );

	int					id;
	bool				flag;
	int					last = 0;
	int					state = 0;
	int					opcount = 0;
	
	while( ( id = lexer->next() ) != token_EndOfLine )
	{
		switch( id )
		{
			#include "format_token.i"

			case token_Tag:
				flag = options.space_delimiters;
				if( flag )
					text.append_space();
					
				lexer->tag()->save( text, options );

				if( flag )
					text.append_space();
				break;
		}
		
		last = id;
		opcount--;
	}

	text.trim_tail();
	text.new_line();
}

END_CODE_NAMESPACE
