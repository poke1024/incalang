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
//	code_file_t.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_file_t.h"
#include "code_line_t.h"
#include "code_parser_t.h"
#include "code_stream_t.h"
#include "code_block_tag_t.h"
#include "code_line_lexer_t.h"

BEGIN_CODE_NAMESPACE

// open issue: handle }else{

file_t::file_t()
{
	m_dirty = false;
	m_delay_formatting = 0;

	m_line_alloc = 16;
	m_lines = (line_t**)imalloc(
		m_line_alloc * sizeof( line_t* ) );
	m_block_ident = (ident_t*)imalloc(
		m_line_alloc * sizeof( ident_t ) );
	m_stmt_ident = (ident_t*)imalloc(
		m_line_alloc * sizeof( ident_t ) );
	m_comment_balance = (ident_t*)imalloc(
		m_line_alloc * sizeof( ident_t ) );
	m_line_count = 0;

	if( m_lines == 0 ||
		m_block_ident == 0 ||
		m_stmt_ident == 0 ||
		m_comment_balance == 0 )
	{
		throw_icarus_error( "out of memory" );
	}
}

file_t::~file_t()
{
	line_t**			lines = m_lines;
	long				count = m_line_count;

	for( long i = 0; i < count; i++ )
		line_dispose( lines[ i ] );
		
	ifree( m_lines );
	ifree( m_stmt_ident );
	ifree( m_block_ident );
	ifree( m_comment_balance );
}

void
file_t::insert_line(
	long				number,
	const char*			text )
{
	if( number < 0 )
		number = 0;

	if( number > m_line_count )
		number = m_line_count;

	move_lines( number, 1 );
		
	line_t*			line;

	line = line_create(
		m_base_path.c_str(), text, istrlen( text ) );
	
	if( number > 0 )
	{
		line_t*		prev;
	
		prev = m_lines[ number - 1 ];
		m_block_ident[ number ] = m_block_ident[ number - 1 ] +
			imax( (int)prev->delimiter_balance, 0 ) +
			prev->parenthes_balance;
		m_comment_balance[ number ] =
			m_comment_balance[ number - 1 ] +
			prev->comment_balance;
	}
	else
	{
		m_block_ident[ number ] = 0;
		m_comment_balance[ number ] = 0;
	}
		
	m_lines[ number ] = line;

	if( line->delimiter_balance < 0 )
		m_block_ident[ number ] += line->delimiter_balance;
	
	update_block_ident( number + 1,
		line->delimiter_balance +
		line->parenthes_balance );

	update_comment_balance( number + 1,
		line->comment_balance );

	update_stmt_ident( number );
	
	m_dirty = true;
}

void
file_t::remove_lines(
	long				number,
	long				count,
	bool				dispose )
{
	if( number < 0 || number + count > m_line_count )
		throw_icarus_error(
			"illegal line number in file_t::replace_line" );

	line_t*				line;	
	long				delta = 0;
	long				cbal = 0;
		
	for( long i = 0; i < count; i++ )
	{
		line = m_lines[ number + i ];
		delta -= line->delimiter_balance + line->parenthes_balance;
		cbal -= line->comment_balance;
		
		if( line->delimiter_balance < 0 )
			m_block_ident[ number + i ] -= line->delimiter_balance;
			
		if( dispose )
			line_dispose( line );
	}
		
	long				lines_to_move;
	
	lines_to_move = m_line_count - number - count;

	imemmove(
		m_lines + number,
		m_lines + number + count,
		lines_to_move * sizeof( line_t* ) );

	imemmove(
		m_block_ident + number,
		m_block_ident + number + count,
		lines_to_move * sizeof( ident_t ) );

	imemmove(
		m_stmt_ident + number,
		m_stmt_ident + number + count,
		lines_to_move * sizeof( ident_t ) );

	imemmove(
		m_comment_balance + number,
		m_comment_balance + number + count,
		lines_to_move * sizeof( ident_t ) );

	m_line_count -= count;

	if( m_line_count > 0 )
	{
		update_block_ident( number, delta );
		update_comment_balance( number, cbal );

		m_stmt_ident[ number ] = 0;
		update_stmt_ident( imax( number - 1, 0L ) );
	}

	m_dirty = true;
}

void
file_t::replace_line(
	long				number,
	const char*			text )
{
	line_t*			newline;
	
	newline = line_create(
		m_base_path.c_str(), text, istrlen( text ) );
	
	replace_line( number, newline );

	m_dirty = true;
}

bool
file_t::collapsed(
	long				number )
{
	if( number < 0 || number >= m_line_count )
		throw_icarus_error(
			"illegal line number in file_t::collapse" );

	return line_collapsed( m_lines[ number ] );
}

void
file_t::collapse(
	long				number )
{
	if( number < 0 || number >= m_line_count )
		throw_icarus_error(
			"illegal line number in file_t::collapse" );

	long				count = m_line_count;
	long				top = number + 1;
	long				bottom = -1;
	
	ident_t*			identp = m_block_ident;
	long				baseident = identp[ number ];

	for( long i = number + 1; i < count; i++ )	
	{
		long			ident = identp[ i ];
	
		if( ident == baseident &&
			m_lines[ i ]->delimiter_balance < 0 )
		{
			bottom = i;
			break;
		}
	}
	
	if( bottom < 0 )
		return;

	if( line_collapse( m_lines[ number ] ) == false )
		return;
		
	long				incount;
	collapse_tag_t*		tag;
	
	incount = bottom - top + 1;

	tag = new block_tag_t(
		m_lines + top,
		incount );
	
	remove_lines( top, incount, false );

	text_t				text;
	line_t*				line;
	line_lexer_t		lexer( m_lines[ number ] );
	
	line_text( &lexer, s_code_options, text, 0, 0 );
	text.append_tag( tag );
		
	line = line_create(
		m_base_path.c_str(), text.data(), text.length() );
	replace_line( number, line );

	tag->unrefer();
}

void
file_t::uncollapse(
	long				number )
{
	if( number < 0 || number >= m_line_count )
		throw_icarus_error(
			"illegal line number in file_t::uncollapse" );

	collapse_tag_t*		tag;

	tag = line_uncollapse( m_lines[ number ] );

	if( tag == 0 )
		return;
	
	long				count;
	
	count = tag->line_count();
	
	move_lines(
		number + 1, count );
	
	tag->transfer_lines(
		m_lines + number +  1);
			
	tag->unrefer();

	m_lines[ number ]->delimiter_balance += 1;
	//m_block_ident[ number ] -= 1;

	rebuild_ident(
		number + 1,
		imin( number + count + 1, m_line_count - 1 ) );

	//m_block_ident[ number ] += 1;
}

void
file_t::set_breakpoint(
	long				number,
	bool				flag )
{
	if( flag )
		m_lines[ number ]->flags |= LF_BREAKPOINT;
	else
		m_lines[ number ]->flags &= ~LF_BREAKPOINT;
}

bool
file_t::breakpoint(
	long				number )
{
	return m_lines[ number ]->flags & LF_BREAKPOINT;
}

void
file_t::get_line(
	long				number,
	text_t&				text,
	int					flags )
{
	if( number < 0 || number >= m_line_count )
	{
		/*throw_icarus_error(
			"illegal line number in file_t::get_line" );*/
		text.clear();
		return;
	}

	int					ident;
	
	ident = m_block_ident[ number ] + m_stmt_ident[ number ];
	for( int i = 0; i < ident; i++ )
		text.append( "\t" );
	
	line_lexer_t		lexer( m_lines[ number ] );
	
	line_text( &lexer, s_code_options, text, 0, flags );
}

long
file_t::get_offset(
	long				number,
	long				index )
{
	int					ident;
	
	ident = m_block_ident[ number ] + m_stmt_ident[ number ];
	
	line_lexer_t		lexer( m_lines[ number ] );
	text_t				text;
	
	line_text( &lexer, s_code_options, text, index, 0 );

	return text.length() + ident;
}

long
file_t::get_comment_balance(
	long				number )
{
	if( number < 0 || number >= m_line_count )
		throw_icarus_error( "illegal line number" );
	return m_comment_balance[ number ];
}

void
file_t::save(
	output_stream_t*	stream,
	long				begin )
{
	//line_t**			lines = m_lines;
	long				count = m_line_count;

	stream_t			codestream( stream );
	line_lexer_t		lexer;

	/*while( count > begin &&
		m_lines[ count - 1 ]->token_count == 0 )
		count -= 1;*/

	for( long i = begin; i < count; i++ )
	{
		lexer.init( m_lines[ i ] );
		line_save( &lexer,
			codestream.options(),
			codestream );
	}
	
	m_dirty = false;
}

bool
file_t::dirty() const
{
	return m_dirty;
}

void
file_t::set_dirty(
	bool				dirty )
{
	m_dirty = dirty;
}

void
file_t::delay_formatting()
{
	m_delay_formatting++;
}

void
file_t::resume_formatting()
{
	if( --m_delay_formatting == 0 )
		update_formatting();
}

void
file_t::update_formatting()
{
	rebuild_ident( 0, m_line_count - 1 );
}

void
file_t::move_lines(
	long				number,
	long				count )
{
	if( m_line_count + count > m_line_alloc )
		expand( m_line_count + count );

	if( number < 0 )
		number = 0;
	else if( number > m_line_count )
		number = m_line_count;

	long				lines_to_move;
	
	lines_to_move = m_line_count - number;

	imemmove(
		m_lines + number + count,
		m_lines + number,
		lines_to_move * sizeof( line_t* ) );

	imemmove(
		m_block_ident + number + count,
		m_block_ident + number,
		lines_to_move * sizeof( ident_t ) );

	imemmove(
		m_comment_balance + number + count,
		m_comment_balance + number,
		lines_to_move * sizeof( ident_t ) );

	imemmove(
		m_stmt_ident + number + count,
		m_stmt_ident + number,
		lines_to_move * sizeof( ident_t ) );
		
	imemset(
		m_block_ident + number, 0,
		count * sizeof( ident_t ) );

	imemset(
		m_comment_balance + number, 0,
		count * sizeof( ident_t ) );

	imemset(
		m_stmt_ident + number, 0,
		count * sizeof( ident_t ) );

	m_line_count += count;
}

void
file_t::replace_line(
	long				number,
	line_t*				newline )
{
	if( number < 0 || number >= m_line_count )
		throw_icarus_error(
			"illegal line number in file_t::replace_line" );

	line_t*				oldline;
	
	oldline = m_lines[ number ];
	
	int					delta;
		
	delta = newline->delimiter_balance + newline->parenthes_balance;
	delta -= oldline->delimiter_balance + oldline->parenthes_balance;
		
	int					cbal;

	cbal = newline->comment_balance;
	cbal -= oldline->comment_balance;

	if( oldline->delimiter_balance < 0 )
		m_block_ident[ number ] -= oldline->delimiter_balance;

	if( newline->delimiter_balance < 0 )
		m_block_ident[ number ] += newline->delimiter_balance;
		
	line_dispose( oldline );
	m_lines[ number ] = newline;

	update_block_ident( number + 1, delta );
	update_comment_balance( number + 1, cbal );

	m_stmt_ident[ number ] = 0;
	update_stmt_ident( imax( number - 1, 0L ) );
}

void
file_t::update_block_ident(
	long				number,
	int					delta )
{
	if( m_delay_formatting )
		return;

	if( !delta )
		return;

	ident_t*			ident = m_block_ident + number;
	long				count = m_line_count - number;

	if( count <= 0 )
		return;

	do {
		*ident++ += delta;
	} while( --count );
}

void
file_t::update_stmt_ident(
	long				number )
{
	if( m_delay_formatting )
		return;

	long				anchor;
	anchor = find_stmt_anchor( number );

	number += 1;
	while( number < m_line_count )
	{
		if( m_block_ident[ number ] == 0 )
			break;
		number++;
	}

	parser_t			parser( this, anchor, number );
	parser.parse();
}

void
file_t::update_comment_balance(
	long				number,
	long				delta )
{
	if( m_delay_formatting )
		return;

	if( !delta )
		return;

	ident_t*			ident = m_comment_balance + number;
	long				count = m_line_count - number;

	if( count <= 0 )
		return;

	do {
		*ident++ += delta;
	} while( --count );
}

void
file_t::rebuild_ident(
	long				top,
	long				bottom )
{
	if( m_delay_formatting )
		return;

	ident_t*			identp = m_block_ident;
	ident_t*			cbalp = m_comment_balance;
	long				ident;
	long				cbal;
	
	if( top > 0 )
	{
		ident = identp[ top - 1 ];
		cbal = cbalp[ top - 1 ];

		line_t*			line = m_lines[ top - 1 ];

		ident += line->delimiter_balance;
		ident += line->parenthes_balance;
		cbal += line->comment_balance;
	}
	else
	{
		ident = 0;
		cbal = 0;
	}
	
	long				i = top;
	
	while( true )
	{
		line_t*			line = m_lines[ i ];
		long			dbal;
	
		dbal = line->delimiter_balance;
		if( dbal < 0 )
			ident += dbal;
	
		identp[ i ] = ident;
		cbalp[ i ] = cbal;

		if( ++i > bottom )
			break;
		
		ident += imax( dbal, 0L );
		ident += line->parenthes_balance;

		cbal += line->comment_balance;
	}
	
	long				anchor;
	anchor = find_stmt_anchor( top );
	
	parser_t			parser( this, anchor, bottom );
	parser.parse();
}

long
file_t::find_stmt_anchor(
	long				number )
{
	line_t**		lines = m_lines;

	while( number > 0 )
	{
		if( ( lines[ number ]->flags & LF_STATEMENT_ANCHOR ) &&
			m_block_ident[ number ] == 0 &&
			m_stmt_ident[ number ] == 0 )
			break;
		number--;
	}

	return number;
}

void
file_t::expand(
	long				count )
{
	while( m_line_alloc < count )
		m_line_alloc *= 2;
	
	m_lines = (line_t**)irealloc(
		m_lines, m_line_alloc * sizeof( line_t* ) );
	m_block_ident = (ident_t*)irealloc(
		m_block_ident, m_line_alloc * sizeof( ident_t ) );
	m_stmt_ident = (ident_t*)irealloc(
		m_stmt_ident, m_line_alloc * sizeof( ident_t ) );
	m_comment_balance = (ident_t*)irealloc(
		m_comment_balance, m_line_alloc * sizeof( ident_t ) );
	
	if( m_lines == 0 ||
		m_block_ident == 0 ||
		m_stmt_ident == 0 ||
		m_comment_balance == 0 )
	{
		throw_icarus_error( "out of memory" );
	}
}

void
file_t::export_breakpoints(
	output_stream_t*	stream )
{
	long				lcount = line_count();
	long				bcount = 0;

	for( long i = 0; i < lcount; i++ )
	{
		if( m_lines[ i ]->flags & LF_BREAKPOINT )
			bcount++;
	}

	stream->write_u32( bcount );

	long				offset = 0;

	for( long i = 0; i < lcount; i++ )
	{
		line_t*			line = m_lines[ i ];
		if(  line->flags & LF_BREAKPOINT )
			stream->write_u32( offset );
		offset += line->token_count;
	}
}

void
file_t::set_base_path(
	const char*			path )
{
	m_base_path = path;
}

END_CODE_NAMESPACE
