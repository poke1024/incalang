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
//	code_undo_t.cp	 		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_undo_t.h"
#include "code_view_t.h"
#include "code_tag_t.h"
#include <string.h>

BEGIN_AKLABETH_NAMESPACE

static long				s_capacity = 16;

// ---------------------------------------------------------------------------

inline bool
code_undo_t::node_t::regular() const
{
	switch( m_action )
	{
		case action_insert:
		case action_remove:
		case action_collapse:
			return true;
	}
	
	return false;
}

// ---------------------------------------------------------------------------

code_undo_t::node_t::node_t() :

	m_textbuf( 0 ),
	m_textlen( 0 ),
	m_closed( false )
{
}

code_undo_t::node_t::~node_t()
{
	if( m_textbuf )
	{
		code::drop_code( m_textbuf, m_textlen );
		ifree( m_textbuf );
	}
}

void
code_undo_t::node_t::undo(
	code_view_t*	view )
{
	switch( m_action )
	{
		case action_insert:
			view->remove_text(
				m_location,
				view->advance_pos( m_location, m_textlen ) );
			view->set_selection(
				m_location, m_location );
			break;
			
		case action_remove:
			view->set_selection(
				m_location, m_location );
			view->insert_text(
				m_location,
				m_textbuf,
				m_textlen );
			view->set_selection(
				m_location,
				view->advance_pos( m_location, m_textlen ) );
			break;
	
		case action_focus:
			if( m_textbuf )
				view->restore_focus(
					m_focus.oldline,
					m_textbuf,
					m_textlen );
			else
				view->set_focus( m_focus.oldline );
			break;
			
		case action_collapse:
			view->set_selection( m_location, m_location );
			view->collapse();
			break;
	}
}

void
code_undo_t::node_t::redo(
	code_view_t*	view )
{
	switch( m_action )
	{
		case action_insert:
			view->set_selection(
				m_location, m_location );
			view->insert_text(
				m_location,
				m_textbuf,
				m_textlen );
			view->set_selection(
				m_location,
				view->advance_pos( m_location, m_textlen ) );
			break;
			
		case action_remove:
			view->remove_text(
				m_location,
				view->advance_pos( m_location, m_textlen ) );
			view->set_selection(
				m_location, m_location );
			break;
			
		case action_focus:
			view->restore_focus( m_focus.oldline, 0, 0 );
			break;

		case action_collapse:
			view->set_selection( m_location, m_location );
			view->collapse();
			break;
	}
}

void
code_undo_t::node_t::append(
	const char*			textbuf,
	long				textlen )
{
	if( textlen < 1 )
		return;

	expand( textlen );
			
	code::take_code(
		m_textbuf + m_textlen,
		textbuf, textlen * sizeof( char ) );
		
	m_textlen += textlen;
}

void
code_undo_t::node_t::push(
	const char*			textbuf,
	long				textlen )
{
	if( textlen < 1 )
		return;

	expand( textlen );

	memmove(
		m_textbuf + textlen,
		m_textbuf,
		m_textlen * sizeof( char ) );
			
	code::take_code(
		m_textbuf,
		textbuf, textlen * sizeof( char ) );
		
	m_textlen += textlen;
}

void
code_undo_t::node_t::expand(
	long				textlen )
{
	if( not m_textbuf )
	{
		m_textbuf = (char*)imalloc(
			textlen * sizeof( char ) );
	}
	else
	{
		m_textbuf = (char*)irealloc(
			m_textbuf,
			( m_textlen + textlen ) * sizeof( char ) );
	}
			
	if( not m_textbuf )
		throw std::bad_alloc();
}
	
// ---------------------------------------------------------------------------

code_undo_t::stack_t::stack_t() :

	m_head( 0 ),
	m_tail( 0 ),
	m_count( 0 )
{
}

code_undo_t::stack_t::~stack_t()
{
	clear();
}

void
code_undo_t::stack_t::push(
	node_t*				node )
{
	node->prev = 0;
	node->next = m_head;

	if( not m_head )
		m_tail = node;

	if( m_head )
		m_head->prev = node;
	m_head = node;
	
	if( node->regular() )
		m_count += 1;
	
	if( m_count >= s_capacity )
		shrink();
}

code_undo_t::node_t*
code_undo_t::stack_t::pop()
{
	node_t*				node = m_head;

	if( not node )
		return 0;

	m_head = node->next;
	if( m_head )
		m_head->prev = 0;
	else
		m_tail = 0;

	if( node->regular() )
		m_count -= 1;
		
	return node;		
}

code_undo_t::node_t*
code_undo_t::stack_t::head()
{
	return m_head;
}

void
code_undo_t::stack_t::clear()
{
	node_t*				node = m_head;
	node_t*				next;
	
	while( node )
	{
		next = node->next;
		delete node;
		node = next;
	}
	
	m_head = 0;
	m_tail = 0;
	m_count = 0;
}

void
code_undo_t::stack_t::shrink()
{
	while( m_count >= s_capacity )
	{
		node_t*			node = m_tail;
		
		if( not node )
			break;

		if( node->regular() )
			m_count -= 1;
			
		m_tail = node->prev;
		if( m_tail )
			m_tail->next = 0;
		else
			m_head = 0;
		delete node;
	}
}

// ---------------------------------------------------------------------------

code_undo_t::code_undo_t(
	code_view_t*		view ) :

	m_view( view ),
	m_record( true )
{
}
	
code_undo_t::~code_undo_t()
{
}

void
code_undo_t::undo()
{			
	m_record = false;
	
	try
	{
		node_t*			node;

		do {
			node = m_undos.pop();
			if( not node )
				break;
		
			node->undo( m_view );
			m_redos.push( node );
			
		} while( not node->regular() );
		
	}
	catch(...)
	{
	}
	
	m_record = true;
}

void
code_undo_t::redo()
{
	m_record = false;

	try
	{
		node_t*			node;

		do {
			node = m_redos.pop();
			if( not node )
				break;
		
			node->redo( m_view );
			m_undos.push( node );
			
		} while( not node->regular() );
		
	}
	catch(...)
	{
	}

	m_record = true;
}

void
code_undo_t::undo_status(
	command_status_t&	status )
{
	if( m_undos.head() == 0 )
	{
		status.enabled = false;
		status.text = "Can't Undo";		
	}
	else
	{
		status.enabled = true;
		status.text = "Undo Typing";		
	}
}

void
code_undo_t::redo_status(
	command_status_t&	status )
{
	if( m_redos.head() == 0 )
	{
		status.enabled = false;
		status.text = "Can't Redo";		
	}
	else
	{
		status.enabled = true;
		status.text = "Redo Typing";
	}
}

void
code_undo_t::begin()
{
	node_t*				node = m_undos.head();

	if( node )
		node->m_closed = true;
}

void
code_undo_t::clear()
{
	m_undos.clear();
	m_redos.clear();
}

void
code_undo_t::record_insert(
	codepos_t			location,
	const char*			textbuf,
	long				textlen )
{
	if( not m_record )
		return;

	node_t*				node = m_undos.head();
	
	if( node &&
		node->m_action == action_insert &&
		not node->m_closed )
	{
		if( location.line == node->m_location.line &&
			location.offset == node->m_location.offset + node->m_textlen )
		{
			node->append( textbuf, textlen );
			return;
		}
	}
	
	node = new node_t;
	node->m_action = action_insert;
	node->m_location = location;

	try {
		node->append( textbuf, textlen );	
		m_undos.push( node );
		m_redos.clear();
	}
	catch(...)
	{
		delete node;
		throw;
	}
}

void
code_undo_t::record_remove(
	codepos_t			location,
	const char*			textbuf,
	long				textlen )
{
	if( not m_record )
		return;

	node_t*				node = m_undos.head();
	
	if( node &&
		node->m_action == action_remove &&
		not node->m_closed )
	{
		if( location.line == node->m_location.line &&
			location.offset + textlen == node->m_location.offset )
		{
			node->m_location = m_view->advance_pos(
				node->m_location, -textlen );
			node->push( textbuf, textlen );
			return;
		}
	}
	
	node = new node_t;
	node->m_action = action_remove;
	node->m_location = location;

	try {
		node->append( textbuf, textlen );
		m_undos.push( node );
		m_redos.clear();
	}
	catch(...)
	{
		delete node;
		throw;
	}
}

void
code_undo_t::record_focus(
	long				oldline,
	const char*			textbuf,
	long				textlen )
{
	if( oldline < 0 )
		throw_icarus_error( "illegal record focus" );

	if( not m_record )
		return;

	node_t*				node = m_undos.head();
	
	if( node &&
		node->m_action == action_focus &&
		not node->m_textlen &&
		not textlen )
	{
		node->m_focus.oldline = oldline;
		return;
	}
		
	node = new node_t;
	node->m_action = action_focus;
	node->m_focus.oldline = oldline;

	try {
		if( textlen )
			node->append( textbuf, textlen );
		m_undos.push( node );
		m_redos.clear();
	}
	catch(...)
	{
		delete node;
		throw;
	}
}

void
code_undo_t::record_collapse(
	long				line )
{
	if( not m_record )
		return;

	node_t*				node;
	
	node = new node_t;
	node->m_action = action_collapse;
	node->m_location.line = line;
	node->m_location.offset = 0;

	try {
		m_undos.push( node );
		m_redos.clear();
	}
	catch(...)
	{
		delete node;
		throw;
	}
}

bool
code_undo_t::recording() const
{
	return m_record;
}

void
code_undo_t::set_capacity(
	long				capacity )
{
	s_capacity = capacity;
}

END_AKLABETH_NAMESPACE
