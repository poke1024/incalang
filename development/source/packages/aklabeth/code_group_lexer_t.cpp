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
//	code_group_lexer_t.cp	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_group_lexer_t.h"
#include "code_collapse_tag_t.h"
#include "aklabeth_lexer.h"

BEGIN_CODE_NAMESPACE

group_lexer_t::group_lexer_t(
	group_t*			group ) :
	
	m_stack( 0 ),
	m_node_pool( 0 ),
	m_location( 0 )
{
	push( group );

	if( m_stack == 0 )
		throw_icarus_error( "lexing on empty file" );
}

group_lexer_t::~group_lexer_t()
{
	node_t*				node = m_node_pool;
	node_t*				next;
	
	while( node )
	{
		next = node->next;
		delete node;
		node = next;
	}
}

int
group_lexer_t::next()
{
	m_location += 1;

	node_t*				node = m_stack;

	if( node == 0 )
		return token_EndOfLine;

	while( true )
	{
		int				token = node->lexer.next();
		
		switch( token )
		{
			case token_EndOfLine:
				node = end_of_line( node );

				if( node == 0 )
					return token_EndOfLine;
				break;
			
			case token_Tag:
				token = process_tag( node->lexer.tag() );
				if( token )
					return token;
				
				node = m_stack;
				break;
				
			default:
				return token;
		}
	}
}

int
group_lexer_t::process_tag(
	tag_t*				tag )
{
	switch( tag->id() )
	{
		case tag_block:
			push( (collapse_tag_t*)tag );
			return token_DelimOpen;

		case tag_include:
			push( (collapse_tag_t*)tag );
			break;
	}
	
	return 0;
}

group_lexer_t::node_t*
group_lexer_t::end_of_line(
	node_t*				node )
{
	long				line;

	line = node->line + 1;
	
	if( line == node->line_count )
		return pop();

	node->lexer.init(
		node->group->line( line ) );
	node->line = line;
	
	return node;
}

void
group_lexer_t::push(
	group_t*			group )
{
	group->update();

	long				line_count;

	line_count = group->line_count();
	if( line_count < 1 )
		return;

	node_t*				node;
	
	node = m_node_pool;
	if( node )
		m_node_pool = node->next;
	else
		node = new node_t;
	
	node->group = group;
	node->line = 0;
	node->line_count = line_count;
	node->lexer.init( group->line( 0 ) );

	node->next = m_stack;
	m_stack = node;
}

group_lexer_t::node_t*
group_lexer_t::pop()
{
	node_t*				node = m_stack;
	node_t*				next;
	
	next = node->next;
	m_stack = next;
	
	node->next = m_node_pool;
	m_node_pool = node;
	
	return next;
}

END_CODE_NAMESPACE
