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
//	code_view_t.cp	 		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include <ctype.h>
#include <math.h>

#include "code_view_t.h"
#include "code_select_t.h"
#include "code_tag_t.h"
#include "code_undo_t.h"
#include "text_clip_t.h"
#include "code_clip_t.h"
#include "code_submit_t.h"
#include "caret_blink_t.h"
#include "array_t.h"

#if ICARUS_OS_WIN32
char*
pathForResource(
	const char*			name,
	const char*			extension );
#else
extern "C" {
char*
pathForResource(
	const char*			name,
	const char*			extension );
};
#endif

BEGIN_AKLABETH_NAMESPACE

using code::tag_t;
using code::text_t;

const int				KEYWORD_COUNT = 58;

static char				s_keywords[ KEYWORD_COUNT ][ 20 ] = {
	"unsigned",
	"signed",
	"bool",
	"byte",
	"char",
	"double",
	"float",
	"int",
	"long",
	"wide",
	"short",
	"sizeof",
	"void",
	"true",
	"false",
	"typedef",
	"static",
	"const",
	"native",
	"abstract",
	"public",
	"private",
	"protected",
	"friend",
	"case",
	"default",
	"if",
	"else",
	"switch",
	"while",
	"do",
	"for",
	"goto",
	"continue",
	"break",
	"return",
	"class",
	"struct",
	"extends",
	"new",
	"delete",
	"null",
	"this",
	"not",
	"cast",
	"module",
	"operator",
	"enum",
	"try",
	"catch",
	"throw",
	"namespace",
	"template",
	"using",
	"of",
	"include",
	"import_dll",
	"compiler_setting" };

void
code_view_t::init(
	const typeface_t*	typeface )
{
	m_typeface.set( typeface );
	typeface_changed();

	m_autoscroll_deltas.h = m_tab_width;
	m_autoscroll_deltas.v = m_line_height;
	m_left_margin = 16; //4;
	
	istring				text;
	char*				path;
	
	path = pathForResource( "system", "txt" );
	text = "include \"";
	text.append( path );
	text.append( "\"" );
	ifree( path );
	
	m_codefile.insert_line( 0, text.c_str() );
#if 1
	m_codefile.insert_line( 1, "" );
#else
	m_codefile.insert_line( 1, "void main()" );
	m_codefile.insert_line( 2, "{" );
	m_codefile.insert_line( 3, "}" );
#endif
	m_codefile.set_dirty( false );
	
	m_zero_line = 1;
	m_hilited_line = 0;
	
	m_focus_line = -1;
	line_text( m_zero_line, m_focus_text );

	m_focus_line = m_zero_line;
	m_focus_dirty = false;

	m_code_undo = new code_undo_t( this );

	// more initialization

	m_selfrom.line = m_zero_line;
	m_selfrom.offset = 0;
	m_selto.line = m_zero_line;
	m_selto.offset = 0;
	
	m_caret_blink = new caret_blink_t( this );
	m_caret_blink->start();
	m_caret_visible = true;
	m_caret_column = 0;

	m_selfrom_position.h = 0;
	m_selfrom_position.v = 0;
	m_selto_position = m_selfrom_position;
	m_selside = 0;
	
	recalc_selection_position();

	recalc_image_size();
	
	m_keywords = hcreate( 6 );
	for( int i = 0; i < KEYWORD_COUNT; i++ )
	{
		const char*		text = s_keywords[ i ];
		
		hadd( m_keywords,
			(const u08*)text,
			istrlen( text ) * sizeof( char ), 0 );
	}
}

code_view_t::code_view_t()
{
}

code_view_t::~code_view_t()
{
	if( m_caret_blink )
		delete m_caret_blink;
		
	if( m_code_undo )
		delete m_code_undo;
		
	hdispose( m_keywords );
}

void
code_view_t::insert_text(
	codepos_t			begin,
	const char*			textbuf,
	long				textlen )
{
	m_code_undo->record_insert(
		begin, textbuf, textlen );

	bool				newlines = false;

	if( begin.line == m_focus_line )
	{
		istring			linetext;
	
		linetext = m_focus_text;

		if( begin.offset > linetext.length() )
#if ICARUS_SAFE
			throw_icarus_error( "illegal insert" );
#else
			begin.offset = linetext.length();
#endif
		
		linetext.insert(
			begin.offset, textbuf, textlen );
			
		long			from = 0;
		long			linelen = linetext.length();
		long			line = begin.line + 1;
			
		for( long i = begin.offset; i < linelen; i++ )
		{
			if( linetext[ i ] == '\n' )
			{
				if( from == 0 )
					m_focus_text = linetext.substr( 0, i );
				else
				{
					istring s;
					s = linetext.substr( from, i - from );
					m_codefile.insert_line( line, s.c_str() );
					line += 1;
					newlines = true;
				}
				from = i + 1;
			}
		}
		
		if( from )
		{
			istring s;
			s = linetext.substr( from, linelen );
			m_codefile.insert_line( line, s.c_str() );
			newlines = true;
		}
		else
			m_focus_text = linetext;
			
		m_focus_dirty = true;
	}
	else
	{
#if ICARUS_SAFE
		throw_icarus_error( "illegal insert text mode" );
#endif
	}
	
	if( newlines )
		recalc_image_size();
		
	request_update();
}

void
code_view_t::remove_text(
	const codepos_t&	begin,
	const codepos_t&	end )
{
	if( compare_codepos( begin, end ) == 0 )
		return;

	if( m_code_undo->recording() )
	{
		istring			text;
	
		extract_text( begin, end, text );
	
		m_code_undo->record_remove(
			begin, text.data(), text.length() );
	}

	bool				selflag = false;
	codepos_t			selfrom;
	codepos_t			selto;

	if( compare_codepos( m_selfrom, m_selto ) != 0 )
	{
		if( compare_codepos( begin, m_selto ) < 0 &&
			compare_codepos( end, m_selto ) >= 0 )
		{		
			long		netcount = text_length( begin, end );
			
			selfrom = m_selfrom;
			selto = advance_pos( m_selto, -netcount );
			selflag = true;
		}
	}
	else if( compare_codepos( m_selfrom, begin ) > 0 )
	{	
		selfrom = begin;
		selto = begin;
		selflag = true;
	}
	
	if( begin.line <= m_focus_line  &&
		end.line >= m_focus_line )
	{
		m_focus_dirty = true;
	}
	
	bool				remlines = false;
	
	istring				text;
	istring				tail;
	
	line_text( begin.line, text );
	text = text.substr( 0, begin.offset );

	line_text( end.line, tail );
	if( end.offset < tail.length() )
	{
		long			index = end.offset;

		while( index < tail.length() )
		{
			if( tail[ index ] == '\t' )
				index++;
			else
				break;
		}

		text.append( tail.substr( index ) );
	}
#if ICARUS_SAFE
	else if( end.offset > tail.length() )
		throw_icarus_error( "invalid remove" );
#endif

	m_focus_line = begin.line;
	m_focus_text = text;
	m_focus_dirty = true;

	if( begin.line < end.line )
	{
		// make sure that line is submitted in
		// case of tail appends so that tags
		// embedded in tail.substr( end.offset )
		// are not temporarily reference counted
		// to zero and deleted.
		code_submit_t	submit( this );
	}

	if( begin.line < end.line )
	{
		m_codefile.remove_lines(
			begin.line + 1,
			end.line - begin.line );
		remlines = true;
	}
	
	if( remlines )
		recalc_image_size();

	if( selflag )
		set_selection( selfrom, selto );

	request_update();
}

void
code_view_t::set_selection(
	codepos_t			selfrom,
	codepos_t			selto )
{		
	if( compare_codepos( selfrom, selto ) > 0 )
	{
		codepos_t h = selto;
		selto = selfrom;
		selfrom = h;
	}

	if( compare_codepos( m_selfrom, selfrom ) == 0 &&
		compare_codepos( m_selto, selto ) == 0 )
		return;

	update_selection();

	if( compare_codepos( m_selfrom, m_selto ) == 0 )
		m_caret_blink->stop();
	
	m_selfrom = selfrom;
	m_selto = selto;

	if( compare_codepos( selfrom, selto ) == 0 )
	{
		set_focus( m_selfrom.line );

		long			length;
	
		length = line_length( m_selfrom.line );
		if( selfrom.offset > length )
		{
			m_selfrom.offset = length;
			m_selto.offset = m_selfrom.offset;
		}
	}

	recalc_selection_position();

	if( compare_codepos( selfrom, selto ) == 0 )
	{
		m_caret_blink->start();
		m_caret_visible = true;
		m_selside = 0;
	}
	
	update_selection();
}

void
code_view_t::get_selection(
	codepos_t&			selfrom,
	codepos_t&			selto )
{
	selfrom = m_selfrom;
	selto = m_selto;
}

#define CHECK_MATCH											\
	{ if( ( flags & 2 ) == 0 )								\
		return offset;										\
	if( offset > 0 && iisalnum( text[ offset - 1 ] ) );		\
	else if( offset + patlen < textlen &&					\
		iisalnum( text[ offset + patlen ] ) );				\
	else return offset; }

static long
find_in_text_forward(
	const char*			text,
	long				textlen,
	const char*			pattern,
	long				patlen,
	long				offset,
	int					flags )
{
	if( flags & 1 )
	{
		while( offset + patlen <= textlen )
		{
			if( strncasecmp( text + offset, pattern, patlen ) == 0 )
				CHECK_MATCH
			offset++;
		}
	}
	else
	{
		while( offset + patlen <= textlen )
		{
			if( strncmp( text + offset, pattern, patlen ) == 0 )
				CHECK_MATCH
			offset++;
		}
	}

	return -1;
}

static long
find_in_text_backward(
	const char*			text,
	long				textlen,
	const char*			pattern,
	long				patlen,
	long				offset,
	int					flags )
{
	if( offset + patlen > textlen )
		offset = textlen - patlen;

	if( flags & 1 )
	{
		while( offset >= 0 )
		{
			if( strncasecmp( text + offset, pattern, patlen ) == 0 )
				CHECK_MATCH
			offset--;
		}
	}
	else
	{
		while( offset >= 0 )
		{
			if( strncmp( text + offset, pattern, patlen ) == 0 )
				CHECK_MATCH
			offset--;
		}
	}

	return -1;
}

bool
code_view_t::find(
	const char*			pattern,
	int					flags,
	int					direction )
{
	long				patlen = istrlen( pattern );

	istring				s;
	long				offset;
	long				line;
	long				start_line;
	long				line_count;

	line = m_selfrom.line;
	start_line = line;
	offset = m_selfrom.offset;
	line_count = m_codefile.line_count();

	if( m_selto.line == m_selfrom.line &&
		m_selto.offset > m_selfrom.offset )
	{
		offset += direction;
	}

	do {
		line_text( line, s );
	
		if( direction > 0 )
			offset = find_in_text_forward(
				s.c_str(), s.length(),
				pattern, patlen,
				offset, flags );
		else
			offset = find_in_text_backward(
				s.c_str(), s.length(),
				pattern, patlen,
				offset, flags );

		if( offset >= 0 )
		{
			codepos_t		from;
			codepos_t		to;
		
			from.line = line;
			from.offset = offset;
			to.line = line;
			to.offset = offset + patlen;
			set_selection( from, to );
			return true;
		}
		
		if( direction > 0 )
		{
			if( ++line == line_count )
			{
				if( flags & 4 )
					break;
				line = m_zero_line;
			}
			offset = 0;
		}
		else
		{
			if( --line == m_zero_line - 1 )
			{
				if( flags & 4 )
					break;
				line = line_count - 1;
			}
			offset = 65536L;
		}
		
	} while( line != start_line );
	
	return false;
}

long
code_view_t::replace_all(
	const char*			pattern,
	const char*			reptext,
	int					flags )
{
	codepos_t			selfrom;
	codepos_t			selto;
	
	get_selection( selfrom, selto );

	codepos_t			begin;
	codepos_t			end;
	long				reptextlen = istrlen( reptext );
	long				count = 0;
	bool				selonly = ( flags & 128 );

	if( selonly )
	{
		begin = m_selfrom;
		end = m_selto;
	}
	else
	{
		begin.line = m_zero_line;
		begin.offset = 0;
	}
	
	set_selection( begin, begin );
	flags |= 4;
	
	while( find( pattern, flags, 1 ) )
	{
		if( selonly && ( m_selfrom.line > end.line || (
			m_selfrom.line == end.line &&
			m_selfrom.offset > end.offset ) ) )
		{
			break;
		}
	
		codepos_t			selfrom = m_selfrom;
		codepos_t			selto = m_selto;
	
		{
			code_submit_t	submit( this );
			
			remove_text( selfrom, selto );
			insert_text( selfrom, reptext, reptextlen );

			begin.line = selfrom.line;
			begin.offset = selfrom.offset + reptextlen;

			submit.patch( begin, begin );
		}
		
		count++;
	}
	
	set_selection( selfrom, selto );
	
	return count;
}

void
code_view_t::update_formatting()
{
	m_codefile.update_formatting();
	request_update();
}

void
code_view_t::set_base_path(
	const char*			path )
{
	m_codefile.set_base_path( path );
}

int
code_view_t::tab_width() const
{
	return m_tab_width;
}

int
code_view_t::line_height() const
{
	return m_line_height;
}

void
code_view_t::scroll_to_selection()
{
	point				delta = { 0, 0 };

	const point&		where = image_location();
	const rect&			r = bounds();

	if( not( m_selfrom_position.h >= where.h &&
		m_selfrom_position.h <= where.h + r.width() ) )
	{
		delta.h = m_selfrom_position.h -
			where.h - r.width() / 2;
	}

	if( not( m_selfrom_position.v >= where.v &&
		m_selfrom_position.v <= where.v + r.height() ) )
	{
		delta.v = m_selfrom_position.v -
			where.v - r.height() / 2;
	}

	pinned_scroll_by( delta.h, delta.v );
}

void
code_view_t::select_token(
	long				index )
{
	long				line_count;
	long				line_index = 0;
	code::line_t*		line;
	codepos_t			selfrom;

	while( true )
	{
		line_count = m_codefile.line_count();

		long			token_count;
		
		for( ;line_index < line_count; line_index++ )
		{		
			line = m_codefile.line( line_index );		
			token_count = line->token_count;
		
			if( index > token_count )
				index -= token_count;
			else
				break;
		}
		
		if( line_index >= line_count )
		{
			line_index = line_count - 1;
			index = m_codefile.line( line_index )->token_count;
			break;
		}

		if( not code::line_nth_token_in_tag( line, index ) )
			break;

		if( line_index < m_zero_line )
			return;

		selfrom.line = line_index;
		selfrom.offset = 0;
		set_selection( selfrom, selfrom );
		
		collapse();
		flush_focus();
		
		//index += token_count - 1;
	}

	if( line_index < m_zero_line )
		return;

	selfrom.line = line_index;
	selfrom.offset = m_codefile.get_offset(
		line_index, index );

	set_selection( selfrom, selfrom );
}

void
code_view_t::hilite_line(
	long				line )
{
	if( m_hilited_line == line )
		return;

	m_hilited_line = line;
	request_update();
}

void
code_view_t::collapse()
{
	if( m_selfrom.line != m_selto.line )
		return;
		
	if( m_selfrom.line != m_focus_line )
		return;

	long				line = m_focus_line;
	
	m_code_undo->record_collapse( line );

	// disable focus
	{
		code_submit_t	submit( this );
		//submit_focus();
	}

	// (un)collapse
	bool				collapse;
			
	if( m_codefile.collapsed( line ) )
	{
		m_codefile.uncollapse( line );
		collapse = false;
	}
	else
	{	
		m_codefile.collapse( line );
		collapse = true;
	}
	
	// enable focus
	load_focus( line );

	// update selection
	codepos_t			pos = m_selfrom;

	if( collapse )
		jump_tags( pos );
	else if( pos.offset > m_focus_text.length() )
		pos.offset = m_focus_text.length();

	set_selection( pos, pos );
	
	recalc_image_size();
	
	request_update();
}


void
code_view_t::extract_text(
	const codepos_t&	begin,
	const codepos_t&	end,
	istring&			text )
{
	long				endline;
	
	endline = imin( (long)end.line,
		m_codefile.line_count() - 1 );
	
	if( endline < begin.line )
		return;
	
	istring				s;

	if( begin.line == endline )
	{
		line_text( begin.line, s );
		text = s.substr( begin.offset,
			end.offset - begin.offset );
		return;
	}

	line_text( begin.line, s );
	text = s.substr( begin.offset );

	for( long i = begin.line + 1; i < endline; i++ )
	{
		text.append( 1, '\n' );
		line_text( i, s );
		text.append( s );
	}
	
	text.append( 1, '\n' );
	line_text( endline, s );
	s = s.substr( 0, end.offset );
	text.append( s );
}

long
code_view_t::text_length(
	const codepos_t&	begin,
	const codepos_t&	end )
{
	long				endline;
	
	endline = imin( (long)end.line,
		m_codefile.line_count() - 1 );

	if( endline < begin.line )
		return 0;

	long				netcount;

	if( begin.line == endline )
		netcount = end.offset - begin.offset;
	else
	{
		netcount = line_length( begin.line ) - begin.offset;
		
		for( long i = begin.line + 1; i < endline; i++ )
			netcount += line_length( i ) + 1;
			
		netcount += end.offset + 1;
	}

	return netcount;
}

void
code_view_t::save(
	output_stream_t*	stream )
{	
	code_submit_t		submit( this );

	submit_focus();
	m_codefile.save( stream, m_zero_line );
	load_focus( m_focus_line );
	m_focus_dirty = false;
}

static bool
check_for_lf(
	char				c,
	random_stream_t*	stream,
	long&				i,
	long				length )
{
	if( c == '\n' )
		return true;

	if( c == '\r' )
	{
		if( i < length - 1 )
		{
			c = stream->read_char();
			if( c == '\n' )
			{
				i += 1;
				return true;
			}
			stream->seek( -1, seek_mode_relative );
		}
		return true;
	}

	return false;
}

void
code_view_t::load(
	random_stream_t*	stream )
{
	m_codefile.delay_formatting();

	long				count = m_codefile.line_count();

	if( count > m_zero_line )
	{
		m_codefile.remove_lines(
			m_zero_line,
			count - m_zero_line,
			true );
	}

	long				line = m_zero_line;
	istring				s;
	long				length = stream->length();
	array_t				collapsed( sizeof( long ) );
	
	for( long i = 0; i < length; i++ )
	{
		char			c;
	
		c = stream->read_char();
		
		if( check_for_lf( c, stream, i, length ) )
		{
			long		len = s.length();
		
			if( len == 6 &&
				imemcmp( s.data(), "/*c */", 6 ) == 0 )
			{
				long	index = line - 1;
				
				collapsed.append( &index );
			}
			else
			{
				m_codefile.insert_line( line, s.c_str() );
				line++;
			}
	
			s.clear();
		}
		else
			s.append( 1, c );
	}

	if( s.length() > 0 )
		m_codefile.insert_line( line, s.c_str() );
	
	for( long i = collapsed.count() - 1; i >= 0; i-- )
	{
		long			line = *(long*)collapsed[ i ];
		codepos_t		where;
	
		where.line = line;
		where.offset = 0;
	
		set_selection( where, where );
		collapse();
	}
	
	m_codefile.resume_formatting();

	load_focus( m_focus_line );
	m_code_undo->clear();
	m_codefile.set_dirty( false );
	m_focus_dirty = false;

	recalc_image_size();

	request_update();
}

bool
code_view_t::dirty() const
{
	return m_focus_dirty || m_codefile.dirty();
}

code::file_t*
code_view_t::file()
{
	return &m_codefile;
}

void
code_view_t::draw_self(
	grafport_t*			port )
{
#if PROJECT_BUILDER
	m_typeface.init();
#endif

	rect				cliprect = port->raw_clip_rect();

	port->fill_rect( 0, cliprect.top,
		m_left_margin - 1, cliprect.bottom + 1,
		make_color( port->format(), 240, 240, 240 ) );

	draw_selection( port );
	
	draw_hilited_line( port );
	
	long				bottom;
	long				top;
	
	visible_area( port, bottom, top );
	
	long				y = m_typeface.ascent();
	
	y += ( bottom - m_zero_line ) * m_line_height;

	for( long i = bottom; i < top; i++ )
	{
		istring			text;
	
		line_text( i, text );

		custom_draw_text( port, 0,
			y, text.data(),
			text.length(), &m_typeface,
			m_codefile.get_comment_balance( i ) );
			
		if( m_codefile.breakpoint( i ) )
		{
			int			size = 11;
			long		x0, y0;
			x0 = m_left_margin / 2 - size / 2;
			y0 = m_line_height / 2  - size / 2 + y - m_typeface.ascent();
			port->fill_oval( x0, y0, x0 + size, y0 + size,
				make_color( port->format(), 255, 0, 0 ) );
		}

		y += m_line_height;
	}

	hilite_text( port );

	draw_caret( port );

}

static bool
isvalid(
	char				c )
{
	if( c < 0 || c > 255 )
		return false;

	return isprint( c ) ||
		c == '\n' ||
		c == key_tabulator;
}

bool
code_view_t::key_down_self(
	char				c,
	int					modifiers )
{
	// under Windows this is actually
	// shift-option! see IncaView.cpp
	// line 443

	if( modifiers & key_modifier_option )
	{
		if( c == key_tabulator )
		{
			collapse();
			return true;
		}
		/*else if( c == -37 ) // option - e
		{
			codepos_t	where = m_selfrom;
			
			load_focus( m_focus_line, 2 );
			where.offset = 9;
			set_selection( where, where );
			request_update();
			return true;
		}*/
	}
	
	if( modifiers & key_modifier_shift )
	{
		if( modify_selection( c ) )
			return true;
	}
		
	codepos_t			caret;
	
	switch( c )
	{
		case key_arrow_right:
			if( compare_codepos( m_selfrom, m_selto ) != 0 )
				caret = m_selto;
			else
				caret = advance_pos( m_selto, 1 );
			
			set_selection( caret, caret );
			caret_column_changed( caret );
			scroll_down_if_necessary();
			break;
			
		case key_arrow_left:
			if( compare_codepos( m_selfrom, m_selto ) != 0 )
				caret = m_selfrom;
			else
				caret = advance_pos( m_selfrom, -1 );

			set_selection( caret, caret );
			caret_column_changed( caret );
			scroll_up_if_necessary();
			break;
	
		case key_arrow_up:
			//flush_focus();

			if( compare_codepos( m_selfrom, m_selto ) != 0 )
			{
				caret = m_selfrom;
				caret_column_changed( caret );
			}
			else
			{
				caret = move_up_down(
					m_selfrom, m_caret_column, -1 );
			}
				
			set_selection( caret, caret );
			scroll_up_if_necessary();
			break;
			
		case key_arrow_down:
			//flush_focus();

			if( compare_codepos( m_selfrom, m_selto ) != 0 )
			{
				caret = m_selto;
				caret_column_changed( caret );
			}
			else
			{
				caret = move_up_down(
					m_selfrom, m_caret_column, 1 );
			}
				
			set_selection( caret, caret );
			scroll_down_if_necessary();				
			break;
			
		case key_backspace:
			if( compare_codepos( m_selfrom, m_selto ) == 0 )
			{
				remove_text(
					advance_pos( m_selfrom, -1 ),
					m_selfrom );
				caret_column_changed( m_selfrom );
			}
			else
				remove_text( m_selfrom, m_selto );
				
			break;
			
		case key_delete:
			if( compare_codepos( m_selfrom, m_selto ) == 0 )
			{
				remove_text(
					m_selfrom,
					advance_pos( m_selfrom, 1 ) );
			}
			else
				remove_text( m_selfrom, m_selto );
				
			break;
					
		case key_enter:
		{
			long				tab_count = 0;
		
			long				line = m_selfrom.line;
			istring				text;
			long				length;
			
			line_text( line, text );
			length = text.length();
			
			for( long i = 0; i < length; i++ )
				if( text[ i ] == '\t' )
					tab_count += 1;
				else
					break;
			
			key_down_self( '\n', 0 );
			
			for( long i = 0; i < tab_count; i++ )
				key_down_self( '\t', 0 );

			scroll_down_if_necessary();
			break;
		}
		
		default:
			if( isvalid( c ) )
			{
				if( compare_codepos( m_selfrom, m_selto ) != 0 )
					remove_text( m_selfrom, m_selto );
	
				insert_text( m_selfrom, &c, 1 );
				key_down_self( key_arrow_right, 0 );
			}
			break;
	}
		
	return false;
}

bool
code_view_t::modify_selection(
	char				c )
{
	codepos_t			selfrom;
	codepos_t			selto;
	
	switch( c )
	{
		case key_arrow_right:
			if( m_selside == 0 )
				m_selside = 1;
			
			if( m_selside == 1 )
			{
				selfrom = m_selfrom;
				selto = advance_pos( m_selto, 1 );
				caret_column_changed( selto );
			}
			else
			{
				selfrom = advance_pos( m_selfrom, 1 );
				selto = m_selto;
				caret_column_changed( selfrom );
			}
				
			set_selection( selfrom, selto );
			scroll_down_if_necessary();
			return true;
			
		case key_arrow_left:
			if( m_selside == 0 )
				m_selside = -1;
		
			if( m_selside == 1 )
			{
				selfrom = m_selfrom;
				selto = advance_pos( m_selto, -1 );
				caret_column_changed( selto );
			}
			else
			{
				selfrom = advance_pos( m_selfrom, -1 );
				selto = m_selto;
				caret_column_changed( selfrom );
			}
			
			set_selection( selfrom, selto );
			scroll_up_if_necessary();
			return true;
	
		case key_arrow_up:
			if( m_selside == 0 )
				m_selside = -1;
		
			if( m_selside == 1 )
			{
				selfrom = m_selfrom;
				selto = move_up_down(
					m_selto, m_caret_column, -1 );
			}
			else
			{
				selfrom = move_up_down(
					m_selfrom, m_caret_column, -1 );
				selto = m_selto;
			}
			
			if( compare_codepos( selfrom, selto ) > 0 )
				m_selside = -m_selside;
			
			set_selection( selfrom, selto );
			scroll_up_if_necessary();
			return true;
			
		case key_arrow_down:
			if( m_selside == 0 )
				m_selside = 1;
		
			if( m_selside == 1 )
			{
				selfrom = m_selfrom;
				selto = move_up_down(
					m_selto, m_caret_column, 1 );
			}
			else
			{
				selfrom = move_up_down(
					m_selfrom, m_caret_column, 1 );
				selto = m_selto;
			}

			if( compare_codepos( selfrom, selto ) > 0 )
				m_selside = -m_selside;
			
			set_selection( selfrom, selto );
			scroll_down_if_necessary();				
			return true;
	}

	return false;
}

bool
code_view_t::process_command_self(
	command_t			command,
	void*				/*io_param*/ )
{
	switch( command )
	{	
		case cmd_Cut:
			copy();
			clear();
			return true;

		case cmd_Clear:
			clear();
			scroll_to_selection();
			return true;

		case cmd_Copy:
			copy();
			return true;
			
		case cmd_Paste:
			paste();
			return true;
		
		case cmd_SelectAll:
			select_all();
			return true;
			
		case cmd_Undo:
			m_code_undo->undo();
			scroll_to_selection();
			return true;
		
		case cmd_Redo:
			m_code_undo->redo();
			scroll_to_selection();
			return true;
	}
	
	return false;
}

bool
code_view_t::command_status_self(
	command_t			command,
	command_status_t&	status )
{
	switch( command )
	{
		case cmd_Cut:
		case cmd_Clear:
		case cmd_Copy:
			status.enabled =
				( compare_codepos( m_selfrom, m_selto ) < 0 );
			return true;
	
		case cmd_Paste:
			status.enabled = can_paste();
			return true;
	
		case cmd_SelectAll:
			if( m_codefile.line_count() == m_zero_line + 1 &&
				line_length( m_zero_line ) == 0 )
			{
				status.enabled = false;
			}
			else
				status.enabled = true;
			return true;
	
		case cmd_Undo:
			m_code_undo->undo_status( status );
			return true;

		case cmd_Redo:
			m_code_undo->redo_status( status );
			return true;
	}
	
	return false;
}

void
code_view_t::activate_self()
{
	m_caret_blink->start();
}

void
code_view_t::typeface_changed()
{
#if PROJECT_BUILDER
	m_typeface.init();
#endif

	m_line_height = m_typeface.ascent() +
		m_typeface.descent() +
		m_typeface.leading();
	
	m_glyph_height = m_typeface.ascent() +
		m_typeface.descent();

	m_tab_width = m_typeface.text_width( "    ", 4 );
}

codepos_t
code_view_t::point_to_offset(
	long				x,
	long				y )
{
	long				line = y / m_line_height;
	long				line_count = m_codefile.line_count();
	codepos_t			pos;

	line += m_zero_line;

	if( line < m_zero_line )
	{
		pos.line = m_zero_line;
		pos.offset = 0;
		return pos;
	}
	else if( line >= line_count )
	{
		pos.line = line_count - 1;
		pos.offset = line_length( pos.line );
		return pos;
	}

	istring				text;
	
	line_text( line, text );
	
	f64					offset = x - m_left_margin;

	pos.line = line;

	pos.offset = custom_pixel_to_char(
		text.data(), text.length(), &offset );
	
	return pos;
}

bool
code_view_t::mouse_down_self(
	long				x,
	long				y,
	mouse_button_t		button )
{
	if( x < m_left_margin )
	{
		long			index = y / m_line_height + 1;

		if( index < 0 || index >= m_codefile.line_count() )
			return true;

		m_codefile.set_breakpoint(
			index, not m_codefile.breakpoint( index ) );
		request_update();
		return true;
	}

	code_select_t*		dragger;
	
	dragger = new code_select_t(
		this, x, y, button );

	dragger->start();

	m_selside = 0;

	return true;
}

codepos_t
code_view_t::move_up_down(
	codepos_t			pos,
	f64					column,
	int					dy )
{
	long				line = pos.line;
	long				line_count;
	
	line_count = m_codefile.line_count();
		
	if( line + dy < m_zero_line )
	{
		pos.line = m_zero_line;
		pos.offset = 0;
		caret_column_changed( pos );
		return pos;
	}
	else if( line + dy >= line_count )
	{
		pos.line = m_codefile.line_count() - 1;
		pos.offset = line_length( pos.line );
		caret_column_changed( pos );
		return pos;
	}

	istring				text;

	line += dy;
	line_text( line, text );

	codepos_t			caret;
	
	caret.line = line;
		
	caret.offset = custom_pixel_to_char(
		text.data(), text.length(), &column );
		
	return caret;
}

f64
code_view_t::caret_column(
	const codepos_t&	pos )
{
	istring				text;

	line_text( pos.line, text );

	return custom_text_width(
		text.data(), pos.offset );
}

void
code_view_t::caret_column_changed(
	const codepos_t&	pos )
{
	m_caret_column = caret_column( pos );
}

inline bool
isident(
	char				c )
{
	if( c < 0 )
		return false;

	if( iisalnum( c ) )
		return true;
	if( c == '_' )
		return true;
		
	return false;
}

void
code_view_t::extend_selection(
	codepos_t&			selfrom,
	codepos_t&			selto,
	int					mode )
{
	if( mode == extend_mode_word )
	{
		istring			text;
		long			length;
		
		if( selfrom.line != selto.line )
			return;
			
		line_text( selfrom.line, text );
		length = text.length();

		if( selfrom.offset >= 0 &&
			selfrom.offset < length &&
			not isident( text[ selfrom.offset ] ) )
		{
			if( selfrom.offset > 0 &&
				isident( text[ selfrom.offset - 1 ] ) )
			{
				selfrom.offset -= 1;
			}	
		}
	
		if( selfrom.offset >= 0 &&
			selfrom.offset < length &&
			isident( text[ selfrom.offset ] ) )
		{
			while( selfrom.offset > 0 )
			{
				if( not isident( text[ selfrom.offset - 1 ] ) )
					break;
				selfrom.offset--;
			}
		}
		
		if( selto.offset > 0 && selto.offset < text.length() &&
			isident( text[ selto.offset ] ) )
		{
			while( selto.offset < length )
			{
				if( not isident( text[ selto.offset ] ) )
					break;
				selto.offset++;
			}
		}
	}
	else if( mode == extend_mode_line )
	{	
		selfrom.offset = 0;

		if( selto.line + 1 < m_codefile.line_count() )
		{
			selto.line += 1;
			selto.offset = 0;
		}
		else
		{	
			istring		text;

			line_text( selto.line, text );
			selto.offset = text.length();
		}
	}
}

void
code_view_t::clear()
{
	if( compare_codepos( m_selfrom, m_selto ) != 0 )
	{
		m_code_undo->begin();
		remove_text( m_selfrom, m_selto );
	}
}

void
code_view_t::copy()
{
	if( compare_codepos( m_selfrom, m_selto ) == 0 )
		return;

	istring				text;
	code_clip_t*		clip;

	extract_text( m_selfrom, m_selto, text );
			
	clip = new code_clip_t( text.data(), text.length() );
	scrap_t::instance()->put( clip );
}

void
code_view_t::paste()
{
	const clip_tag_t	clip_tags[ 2 ] = {
							text_clip_tag,
							code_clip_tag };

	scrap_t*			scrap;
	clip_t*				clip;
	
	scrap = scrap_t::instance();
	clip = scrap->get( clip_tags, 2 );

	if( !clip )
		return;

	switch( clip->tag() )
	{
		case text_clip_tag:
			paste_text( (text_clip_t*)( clip ) );
			break;
			
		case code_clip_tag:
			paste_code( (code_clip_t*)( clip ) );
			break;
	}
}

void
code_view_t::paste_text(
	text_clip_t*		clip )
{
	if( not clip )
		return;
		
	m_code_undo->begin();

	long				length = clip->text_length();
	codepos_t			where = m_selfrom;
		
	clear();
	insert_text( where,
		clip->text_ptr(), length );

	codepos_t			pos = advance_pos( where, length );
		
	set_selection( pos, pos );
}

void
code_view_t::paste_code(
	code_clip_t*		clip )
{
	if( not clip )
		return;
		
	m_code_undo->begin();

	long				length = clip->text_length();
	codepos_t			where = m_selfrom;
		
	clear();
	insert_text( where,
		clip->text_ptr(), length );
		
	codepos_t			pos = advance_pos( where, length );
	set_selection( pos, pos );
}

void
code_view_t::select_all()
{
	codepos_t			from;
	codepos_t			to;

	from.line = m_zero_line;
	from.offset = 0;
	to.line = m_codefile.line_count() - 1;
	to.offset = line_length( to.line );

	set_selection( from, to );
}

bool
code_view_t::can_paste()
{
	const clip_tag_t	clip_tags[ 2 ] = {
							text_clip_tag,
							code_clip_tag };

	scrap_t*			scrap;
	
	scrap = scrap_t::instance();

	return scrap->get( clip_tags, 2 ) != 0;
}

void
code_view_t::draw_selection(
	grafport_t*			port )
{
	if( compare_codepos( m_selfrom, m_selto ) == 0 )
		return;
		
	u32					selcolor;
	
	selcolor = port->selection_color();
		
	const long			view_width = image_size().width;
	
	if( m_selfrom.line == m_selto.line )
	{
		port->fill_rect(
			m_selfrom_position.h,
			m_selfrom_position.v,
			m_selto_position.h,
			m_selto_position.v + m_glyph_height,
			selcolor );
	}
	else
	{
		port->fill_rect(
			m_selfrom_position.h,
			m_selfrom_position.v,
			view_width,
			m_selfrom_position.v + m_glyph_height,
			selcolor );

		if( m_selfrom.line < m_selto.line - 1 )
		{
			port->fill_rect(
				m_left_margin,
				m_selfrom_position.v + m_glyph_height + 1,
				view_width,
				m_selto_position.v - 1,
				selcolor );		
		}

		if( m_selto_position.h > 0 && m_left_margin < m_selto_position.h )
		{
			port->fill_rect(
				m_left_margin,
				m_selto_position.v,
				m_selto_position.h,
				m_selto_position.v + m_glyph_height,
				selcolor );
		}
	}
}

void
code_view_t::hilite_text(
	grafport_t*			port )
{
#if ICARUS_OS_WIN32
	if( compare_codepos( m_selfrom, m_selto ) == 0 )
		return;
		
	port->set_hilite_text( true );
		
	const long			view_width = image_size().width;
	const long			ascent = m_typeface.ascent();
	istring				text;

	if( m_selfrom.line == m_selto.line )
	{	
		line_text( m_selfrom.line, text );

		custom_draw_text( port, m_selfrom_position.h - m_left_margin,
			m_selfrom_position.v + ascent,
			text.data() + m_selfrom.offset,
			m_selto.offset - m_selfrom.offset, &m_typeface, 0 );
	}
	else
	{
		long y = m_selfrom_position.v + ascent;

		line_text( m_selfrom.line, text );

		custom_draw_text( port, m_selfrom_position.h - m_left_margin,
			y,
			text.data() + m_selfrom.offset,
			text.length() - m_selfrom.offset, &m_typeface, 0 );
		y += m_line_height;

		for( long i = m_selfrom.line + 1; i < m_selto.line; i++ )
		{
			line_text( i, text );
			custom_draw_text( port, 0,
				y, text.data(), text.length(), &m_typeface, 0 );
			y += m_line_height;
		}

		if( m_selto_position.h > 0 )
		{
			line_text( m_selto.line, text );
			custom_draw_text( port, 0,
				y, text.data(), m_selto.offset, &m_typeface, 0 );
		}
	}

	port->set_hilite_text( false );
#endif
}

void
code_view_t::draw_caret(
	grafport_t*			port )
{
	if( compare_codepos( m_selfrom, m_selto ) != 0 )
		return;

	if( not m_caret_visible )
		return;
		
	if( not is_activated() )
		return;
	
	const long			x = m_selfrom_position.h;
	const long			y = m_selfrom_position.v;

	port->vline( x, y, y + m_glyph_height, 0 );
}

void
code_view_t::draw_hilited_line(
	grafport_t*			port )
{
	if( m_hilited_line < m_zero_line )
		return;
		
	long				y;
	
	y = ( m_hilited_line - m_zero_line ) * m_line_height;
	
	long				width = image_size().width;
	
	port->fill_rect(
		0, y, width, y + m_line_height,
		make_color( port->format(), 160, 200, 160 ) );
}

void
code_view_t::toggle_caret()
{
	if( not on_duty() ||
		not is_activated() )
	{
		m_caret_blink->stop();
		m_caret_visible = false;
	}
	else
		m_caret_visible = !m_caret_visible;
	
	update_caret();
}

void
code_view_t::update_selection()
{
	if( compare_codepos( m_selfrom, m_selto ) == 0 )
		update_caret();
	else
		request_update();
}

void
code_view_t::update_caret()
{							
	long				x = m_selfrom_position.h;
	long				y = m_selfrom_position.v;
	
	x -= image_location().h;
	y -= image_location().v;
	
	rect				r;
	
	r.left = x;
	r.right = x;
	r.top = y;
	r.bottom = y + m_glyph_height;
		
	request_update( &r );
}

void
code_view_t::recalc_selection_position()
{
	pixel_position( m_selfrom, m_selfrom_position );
	
	if( compare_codepos( m_selfrom, m_selto ) == 0 )
		m_selto_position = m_selfrom_position;
	else
		pixel_position( m_selto, m_selto_position );
		
}

void
code_view_t::pixel_position(
	const codepos_t&	pos,
	point&				where )
{
	long				line = pos.line;
	long				offset = pos.offset;

	istring				text;
	long				x;

	if( line < m_codefile.line_count() )
	{
		line_text( line, text );

		if( offset > text.length() )
			offset = text.length();
				
		x = custom_text_width(
			text.data(), offset ) + 0.5 + m_left_margin;
	}
	else
		x = m_left_margin;

	long				y = ( line - m_zero_line ) * m_line_height;
	
	where.h = x;
	where.v = y;
}

void
code_view_t::scroll_up_if_necessary()
{
	long				delta;
	
	if( m_selside < 0 )
		delta = m_selfrom_position.v - image_location().v;
	else
		delta = m_selto_position.v - image_location().v;

	if( delta >= 0 )
		return;

	pinned_scroll_by( 0, delta );
}

void
code_view_t::scroll_down_if_necessary()
{
	long				delta;
	
	if( m_selside > 0 )
		delta =	( m_selto_position.v + m_line_height ) -
			( image_location().v + bounds().height() );
	else
		delta =	( m_selfrom_position.v + m_line_height ) -
			( image_location().v + bounds().height() );

	if( delta <= 0 )
		return;

	pinned_scroll_by( 0, delta );
}

void
code_view_t::recalc_image_size()
{
	long				line_count;
	
	line_count = m_codefile.line_count() - m_zero_line;
	
	dimension			size;
		
	size.width = 1024;
	size.height = line_count * m_line_height;

	set_image_size( size );
}

f64
code_view_t::custom_text_width(
	const char*			text_buf,
	long				text_len )
{
	long				offset = 0;
	f64					width = 0;

	for( long i = 0; i < text_len; i++ )
	{
		char			c = text_buf[ i ];
		
		if( c == '\t' )
		{
			width += m_typeface.text_width(
				&text_buf[ offset ], i - offset );

			offset = i + 1;
			
			width += m_tab_width -
				ifmod( width, m_tab_width );
		}
		else if( c == code::k_tag_start )
		{
			width += m_typeface.text_width(
				&text_buf[ offset ], i - offset );

			tag_t*		tag;
			istring		s;
			
			tag = code::extract_tag( text_buf + i + 1 );
			tag->text( s );
			width += m_typeface.text_width(
				s.data(), s.length() );

			i += code::k_tag_all_size - 1;
			offset = i + 1;
		}
	}

	if( text_len > offset )
		width += m_typeface.text_width(
			&text_buf[ offset ], text_len - offset );

	return width;
}

void
code_view_t::custom_draw_text(
	grafport_t*			port,
	f64					x,
	f64					y,
	const char*			text_buf,
	long				text_len,
	const typeface_t*	typeface,
	int					cbal )
{
	int					flags = 0;
	long				offset = 0;

	for( long i = 0; i < text_len; i++ )
	{
		if( text_buf[ i ] == '\t' )
		{
			x = draw_text_block(
				port, x + m_left_margin, y, &text_buf[ offset ],
				i - offset, typeface, flags, cbal ) - m_left_margin;

			offset = i + 1;
			
			x += m_tab_width -
				ifmod( x, m_tab_width );
		}
	}

	draw_text_block(
		port, x + m_left_margin, y, &text_buf[ offset ],
		text_len - offset, typeface, flags, cbal );
}

f64
code_view_t::draw_text_block(
	grafport_t*			port,
	f64					x,
	f64					y,
	const char*			text,
	long				length,
	const typeface_t*	typeface,
	int&				flags,
	int&				cbal )
{
	long				begin = 0;
	long				end = 0;
	int					mode = 1;
	
	long				i = 0;
	u32					color = 0;
	
	if( flags & 1 )
		goto comment_state;
	
	for( ; i <= length; i++ )
	{
		char			c;

		if( i < length )
		{
			c = text[ i ];
			
			if( c == code::k_tag_start )
				;
			else if( c == '\"' && ( flags & 1 ) == 0 && cbal == 0 )
			{
				if( flags & 2 )
				{
					color = make_color(
						port->format(), 128, 0, 128 );
					x = port->draw_text(
						x, y, text + begin, i - begin,
						typeface, color );
					begin = i;
				}
				else
					i++;
				flags ^= 2;
			}
			else if( flags & 2 )
				continue;
			else if( c == '\'' && ( flags & 1 ) == 0 && cbal == 0 )
			{
				if( flags & 4 )
				{
					color = make_color(
						port->format(), 128, 0, 128 );
					x = port->draw_text(
						x, y, text + begin, i - begin,
						typeface, color );
					begin = i;
				}
				else
					i++;
				flags ^= 4;
			}
			else if( flags & 4 )
				continue;
			else if( c != '/' && c != '*' )
			{
				if( ( isident( c ) != 0 ) == mode )
					continue;
			}
		}
		else
			c = '\0';
		
		color = 0;

		if( begin + 1 < length &&
			text[ begin ] == '/' &&
			text[ begin + 1 ] == '*' )
		{
			cbal += 1;
			i++;
		}

		end = i;

		if( flags & 1 )
		{
			color = make_color(
				port->format(), 255, 0, 0 );
		}
		if( cbal > 0 )
		{
			color = make_color(
				port->format(), 255, 0, 0 );
		}
		else if( begin + 1 < length &&
			text[ begin ] == '/' &&
			text[ begin + 1 ] == '/' )
		{
			flags |= 1;

comment_state:
			color = make_color(
				port->format(), 255, 0, 0 );
		}
		else if( mode && ( flags & ( 1 + 2 + 4 ) ) == 0 )
		{
			if( hfind( m_keywords,
				(const u08*)( text + begin ),
				( end - begin ) * sizeof( char ) ) )
			{
				color = make_color(
					port->format(), 0, 0, 255 );
			}
			else if( end > begin && isupper( text[ begin ] ) )
			{
				color = make_color(
					port->format(), 0, 128, 0 );
			}
		}
		
		if( begin + 1 < length &&
			text[ begin ] == '*' &&
			text[ begin + 1 ] == '/' )
		{
			cbal -= 1;
			i += 1;
			end = i;
		}

		x = port->draw_text(
			x, y, text + begin, end - begin,
			typeface, color );

		begin = i;
		mode = 1 - mode;
	
		if( c == code::k_tag_start )
		{
			tag_t*		tag;
			istring		s;
			
			tag = code::extract_tag( text + begin + 1 );
			tag->text( s );
		
			x = draw_text_block(
				port, x, y, s.data(),
				s.length(), typeface, flags, cbal );
				
			i += code::k_tag_all_size - 1;
			begin = i + 1;
		}
	}
	
	return x;
}

long
code_view_t::custom_pixel_to_char(
	const char*			text_buf,
	long				text_len,
	f64*				offset_ptr )
{
	long				offset = 0;
	
	f64					x = 0;
	f64					m = *offset_ptr;
	long				index;

	long				i = 0;

	while( true )
	{
		char			c;

		while( i < text_len )
		{
			c = text_buf[ i ];

			if( c == '\t' || c == code::k_tag_start )
				break;

			i++;
		}

		*offset_ptr = m - x;

		index = m_typeface.pixel_to_char(
			&text_buf[ offset ],
			i - offset, offset_ptr ) + offset;

		x += *offset_ptr;

		if( i == text_len || index < i - offset )
		{
			*offset_ptr = x;
			break;
		}

		offset = i;

		if( c == '\t' )
		{
			*offset_ptr = m - x;
				
			index = m_typeface.pixel_to_char(
				&text_buf[ offset ],
				i - offset, offset_ptr ) + offset;
			
			if( index < i )
			{
				*offset_ptr += x;
				return index;
			}
			
			offset = i + 1;
			
			x += *offset_ptr;
		
			f64			dx = m_tab_width -
							ifmod( x, m_tab_width );
							
			if( x + dx > m )
			{
				*offset_ptr = x;

				if( x + 0.5 * dx > m )
					return i;
					
				*offset_ptr += dx;
				return i + 1;
			}

			x += dx;
		}
		else if( c == code::k_tag_start )
		{
			tag_t*		tag;
			istring		s;
			
			tag = code::extract_tag( text_buf + i + 1 );
			tag->text( s );
			i += code::k_tag_all_size - 1;
			
			f64			dx;
						
			dx = m_typeface.text_width(
				s.data(), s.length() );
			
			if( x + dx > m )
			{
				*offset_ptr = x;
			
				if( x + 0.5 * dx > m )
					return offset;
			
				*offset_ptr += dx;
				return i + 1;
			}
						
			x += dx;
			offset = i + 1;
		}

		i = offset;
	}
	
	return index;
}

void
code_view_t::line_text(
	long				line,
	istring&			text,
	int					flags )
{
	if( line == m_focus_line )
		text = m_focus_text;
	else
	{
		text_t			codetext;
		
		m_codefile.get_line( line, codetext, flags );
		text.assign( codetext.data(), codetext.length() );
	}
}

long
code_view_t::line_length(
	long				line )
{	
	istring				text;
	
	line_text( line, text );
	return text.length();
}

codepos_t
code_view_t::advance_pos(
	codepos_t			pos,
	long				amount )
{
	istring				text;
	long				line = pos.line;
	long				offset;
	
	offset = pos.offset + amount;

	if( offset > 0 )
	{
		long			line_count;
		
		line_count = m_codefile.line_count();

		/*if( line >= line_count )
			line = line_count - 1;*/
		
		while( true )
		{
			long			length;
			
			length = line_length( line );
			if( offset <= length )
				break;
			
			if( ++line == line_count )
			{
				line = line_count - 1;
				offset = length;
				break;
			}
			
			offset -= length + 1;
		}
		
		pos.offset = offset;
		pos.line = line;
	}
	else if( offset < 0 )
	{
		while( true )
		{
			line--;
			if( line < m_zero_line )
			{
				line = m_zero_line;
				offset = 0;
				break;
			}
		
			long			length;
			
			length = line_length( line );
			offset += length + 1;
			
			if( offset >= 0 )
				break;
		}
		
		pos.offset = offset;
		pos.line = line;
	}
	else
		pos.offset = 0;

	jump_tags( pos );
	
	return pos;
}

void
code_view_t::visible_area(
	grafport_t*			port,
	long&				bottom,
	long&				top )
{
	rect				cliprect = port->raw_clip_rect();
	point				scroll = image_location();
	long				pixeltop, pixelbot;

	pixeltop = cliprect.top;// + scroll.v;
	pixelbot = cliprect.bottom;// + scroll.v;

	bottom = imax(
		pixeltop / m_line_height, m_zero_line );
		
	top = imin(
		pixelbot / m_line_height + 2L,
		(long)m_codefile.line_count() );

	// caret updating only works properly if we
	// add at least 2L here
}

void
code_view_t::jump_tags(
	codepos_t&			pos )
{
	istring				text;

	line_text( pos.line, text );

	long				offset = pos.offset;

	// reposition cursor if we accidentally ran
	// into a tag field due to left/right cursor
	// movements
	
	for( int i = 1; i <= code::k_tag_ptr_size + 1; i++ )
	{
		if( offset >= i &&	
			text[ offset - i ] == code::k_tag_start )
		{
			if( i < code::k_tag_ptr_size / 2 )
				offset += code::k_tag_all_size - i;
			else
				offset -= i;

			pos.offset = offset;
			break;
		}
	}
}

// ---------------------------------------------------------------------------

// focus managment

void
code_view_t::set_focus(
	long				number )
{
	if( m_focus_line == number )
		return;

	submit_focus();	
	load_focus( number );
		
	// FIXME
	request_update();
}

void
code_view_t::restore_focus(
	long				number,
	const char*			textbuf,
	long				textlen )
{
	if( number != m_focus_line )
	{
		codepos_t		pos = { -1, 0 };

		set_selection( pos, pos );
		set_focus( number );
	}
	
	if( textbuf )
		m_focus_text.assign( textbuf, textlen );
}

void
code_view_t::submit_focus()
{
	if( not m_focus_dirty )
		return;
		
	if( m_focus_line < 0 )
		throw_icarus_error( "illegal focus submit" );
	
	if( m_selfrom.line == m_focus_line ||
		m_selto.line == m_focus_line )
	{
		throw_icarus_error( "illegal focus submit" );
	}
	
	m_code_undo->record_focus(
		m_focus_line,
		m_focus_text.data(),
		m_focus_text.length() );

	m_codefile.replace_line(
		m_focus_line, m_focus_text.c_str() );
		
	m_focus_line = -1;
	m_focus_text.clear();
	m_focus_dirty = false;
}

void
code_view_t::load_focus(
	long				line,
	int					flags )
{
	if( line < 0 )
	{
		m_focus_line = line;
		m_focus_text.clear();
		m_focus_dirty = false;
		return;
	}

	m_focus_line = -1;
	line_text( line, m_focus_text, flags );
	m_focus_line = line;
	
	m_code_undo->record_focus(
		m_focus_line, 0, 0 );

	m_focus_dirty = false;
}

void
code_view_t::flush_focus()
{
	submit_focus();
	load_focus( m_focus_line );
}
	
// ---------------------------------------------------------------------------

long
compare_codepos(
	const codepos_t&	posa,
	const codepos_t&	posb )
{
	long				delta;
	
	delta = posa.line - posb.line;
	if( delta )
		return delta;
	
	delta = posa.offset - posb.offset;
	return delta;
}

END_AKLABETH_NAMESPACE
