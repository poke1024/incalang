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
//	code_file_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_FILE_H
#define CODE_FILE_H

//#pragma once

#include "code_text_t.h"
#include "code_line_t.h"
#include "code_group_t.h"
#include "output_stream_t.h"

BEGIN_CODE_NAMESPACE

class file_t : public group_t {
public:
						file_t();
	
	virtual				~file_t();

	void				insert_line(
							long				number,
							const char*			text );
	
	void				remove_lines(
							long				number,
							long				count,
							bool				dispose = true );
	
	void				replace_line(
							long				number,
							const char*			text );
							
	void				collapse(
							long				number );
	
	void				uncollapse(
							long				number );
	
	bool				collapsed(
							long				number );

	void				set_breakpoint(
							long				number,
							bool				flag );

	bool				breakpoint(
							long				number );

	void				get_line(
							long				number,
							text_t&				text,
							int					flags );
							
	long				get_offset(
							long				number,
							long				index );

	long				get_comment_balance(
							long				number );
							
	void				save(
							output_stream_t*	stream,
							long				begin );
			
	bool				dirty() const;

	void				set_dirty(
							bool				dirty );
							
	void				delay_formatting();
	
	void				resume_formatting();
							
	void				update_formatting();

	void				export_breakpoints(
							output_stream_t*	stream );

	void				set_base_path(
							const char*			path );
									
protected:
	friend class		parser_t;

	void				move_lines(
							long				number,
							long				count );

	void				replace_line(
							long				number,
							line_t*				newline );

	void				update_block_ident(
							long				number,
							int					delta );
	
	void				update_stmt_ident(
							long				number );
	
	void				update_comment_balance(
							long				number,
							long				delta );

	void				rebuild_ident(
							long				top,
							long				bottom );
	
	long				find_stmt_anchor(
							long				number );
	
	void				expand(
							long				count );
	
	typedef s08			ident_t;
		
	ident_t*			m_block_ident;
	ident_t*			m_stmt_ident;
	ident_t*			m_comment_balance;
	s32					m_line_alloc;
	bool				m_dirty;
	istring				m_base_path;
	s16					m_delay_formatting;
};

END_CODE_NAMESPACE

#endif

