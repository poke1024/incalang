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
//	code_undo_t.h	 		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "code_view_t.h"

BEGIN_AKLABETH_NAMESPACE

class code_undo_t {
public:
	enum action_t {
		action_insert,
		action_remove,
		action_focus,
		action_collapse
	};

							code_undo_t(
								code_view_t*	view );
							
	virtual					~code_undo_t();

	void					undo();

	void					redo();

	void					undo_status(
								command_status_t&	status );

	void					redo_status(
								command_status_t&	status );
	
	void					begin();
	
	void					clear();

	void					record_insert(
								codepos_t			location,
								const char*			textbuf,
								long				textlen );

	void					record_remove(
								codepos_t			location,
								const char*			textbuf,
								long				textlen );
	
	void					record_focus(
								long				oldline,
								const char*			textbuf,
								long				textlen );
	
	void					record_collapse(
								long				line );
								
	bool					recording() const;
								
	static void				set_capacity(
								long				capacity );
	
protected:
	class node_t {
	public:
							node_t();
							
							~node_t();
		
		void				undo(
								code_view_t*	view );
		
		void				redo(
								code_view_t*	view );
							
		void				append(
								const char*			textbuf,
								long				textlen );
		
		void				push(
								const char*			textbuf,
								long				textlen );

		void				expand(
								long				textlen );

		bool				regular() const;
		
		action_t			m_action;
		union {
			codepos_t		m_location;
			struct {
				long		oldline;
				long		newline;
			} m_focus;
		};
			
		bool				m_closed;
		
		node_t*				next;
		node_t*				prev;
		
		char*				m_textbuf;
		long				m_textlen;
	};
	
	class stack_t {
	public:
							stack_t();
							
							~stack_t();
	
		void				push(
								node_t*				node );
	
		node_t*				pop();
		
		node_t*				head();
		
		void				clear();
	
	private:
		void				shrink();
	
		node_t*				m_head;
		node_t*				m_tail;
		long				m_count;
	};
	
	stack_t					m_undos;
	stack_t					m_redos;
	
	code_view_t*			m_view;
	bool					m_record;
};

END_AKLABETH_NAMESPACE
