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
//	code_text_t.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_text_t.h"
#include "code_tag_t.h"
#include "code_options.h"

BEGIN_CODE_NAMESPACE

text_t::text_t()
{
}

void
text_t::append(
	const char*			text )
{
	m_text.append( text );
}

void
text_t::append_literal(
	const char*			text )
{
	const char*			p = text;
	
	char				buf[ 64 ];
	int					index = 0;
	
	while( true )
	{
		char			c;
	
		c = *p++;
		if( c == '\0' )
			break;
			
		switch( c )
		{
			case '\n':
				buf[ index++ ] = '\\';
				buf[ index++ ] = 'n';
				break;

			case '\t':
				buf[ index++ ] = '\\';
				buf[ index++ ] = 't';
				break;

			case '\r':
				buf[ index++ ] = '\\';
				buf[ index++ ] = 'r';
				break;

			case '\"':
				buf[ index++ ] = '\\';
				buf[ index++ ] = '"';
				break;

			case '\'':
				buf[ index++ ] = '\\';
				buf[ index++ ] = '\'';
				break;

			case '\\':
				buf[ index++ ] = '\\';
				buf[ index++ ] = '\\';
				break;
				
			default:
				buf[ index++ ] = c;
				break;
		}
		
		if( index > 60 )
		{
			m_text.append( buf, index );
			index = 0;
		}
	}

	if( index > 0 )
		m_text.append( buf, index );
}

void
text_t::append_declaration(
	const char*			text )
{
	int					tabs;
	
	tabs = s_code_options.declaration_tabs;
	
	if( tabs == 0 )
		append_space();
	else
	{
		long			length = m_text.length();
		long			count = 0;
		
		for( long i = 0; i < length; i++ )
			if( m_text[ i ] == '\t' )
				count += 4;
			else
				count += 1;
	
		long			ident = tabs * 4;
	
		while( count < ident )
		{
			append( "\t" );
			count += 4 - ( count % 4 );
		}
	}
	
	append_space();
	append( text );
}

void
text_t::append_space()
{
	int					length;
	
	length = m_text.length();
	if( !length )
		return;

	char				c;
	
	c = m_text[ length - 1 ];

	if( c == ' ' || c == '\t' )
		return;
		
	m_text.append( " " );
}

void
text_t::trim_tail()
{
	int					length;
	int					index;
	
	length = m_text.length();
	index = 0;

	while( true )
	{
		if( index == length )
			break;

		char			c;
		
		c = m_text[ length - index - 1 ];

		if( c != ' ' && c != '\t' )
			break;
		
		index += 1;
	}

	if( index > 0 )
		m_text.erase( length - index );
}

void
text_t::append_tag(
	tag_t*			tag )
{
	m_text.append( 1, k_tag_start );
	append_ptr( tag );
	m_text.append( 1, k_tag_end );
}

void
text_t::append_ptr(
	void*				ptr )
{
	static const char	hextab[ 17 ] =
		{ "0123456789abcdef" };

	iptr_t				val = (iptr_t)ptr;
	
	for( int i = k_tag_ptr_size - 1; i >= 0; i-- )
	{
		int				digit;
		
		digit = ( val >> ( 4 * i ) );
		digit &= 0xf;
		
		m_text.append( 1, hextab[ digit ] );
	}
}
							
const char*
text_t::data() const
{
	return m_text.data();
}
	
long
text_t::length() const
{
	return m_text.length();
}

void
text_t::clear()
{
	m_text.clear();
}

END_CODE_NAMESPACE
