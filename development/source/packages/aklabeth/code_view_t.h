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
//	code_view_t.h	 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_VIEW_H
#define CODE_VIEW_H

//#pragma once

#include "aklabeth.h"
#include "qd_typeface_t.h"
#include "qd_grafport_t.h"
#include "code_text_t.h"
#include "code_file_t.h"
#include "random_stream_t.h"
#include "caret_blink_t.h"
#include "text_clip_t.h"
#include "timer_t.h"
#include "htab_t.h"
#include "view_t.h"

BEGIN_AKLABETH_NAMESPACE					

class code_clip_t;
class code_undo_t;

struct codepos_t {
	u32						line;
	u32						offset;
};

class code_view_t : public view_t, public caret_view_t {
public:

	void					init(
								const typeface_t*	typeface );
						
							code_view_t();
						
	virtual					~code_view_t();

	void					insert_text(
								codepos_t			begin,
								const char*			text,
								long				length );
	
	void					remove_text(
								const codepos_t&	begin,
								const codepos_t&	end );

	void					collapse();

	void					set_selection(
								codepos_t			selfrom,
								codepos_t			selto );
	
	void					get_selection(
								codepos_t&			selfrom,
								codepos_t&			selto );

	bool					find(
								const char*			pattern,
								int					flags,
								int					direction );

	long					replace_all(
								const char*			pattern,
								const char*			reptext,
								int					flags );

	void					select_token(
								long				index );

	void					scroll_to_selection();

	void					hilite_line(
								long				line );

	void					extract_text(
								const codepos_t&	begin,
								const codepos_t&	end,
								istring&			text );

	long					text_length(
								const codepos_t&	begin,
								const codepos_t&	end );

	codepos_t				advance_pos(
								codepos_t			pos,
								long				amount );
							
	void					save(
								output_stream_t*	stream );
							
	void					load(
								random_stream_t*	stream );
							
	bool					dirty() const;
							
	code::file_t*			file();

	void					set_focus(
								long				number );

	void					restore_focus(
								long				number,
								const char*			textbuf,
								long				textlen );

	void					recalc_image_size();
	
	void					update_formatting();
	
	void					set_base_path(
								const char*			path );

	int						tab_width() const;

	int						line_height() const;

protected:
	friend class			code_select_t;
	friend class			code_submit_t;


							// === pane methods ===
	
	virtual void			draw_self(
								grafport_t*			port );

	virtual bool			key_down_self(
								char				c,
								int					modifiers );

	virtual bool			mouse_down_self(
								long				x,
								long				y,
								mouse_button_t		button );

	virtual bool			process_command_self(
								command_t			command,
								void*				io_param );

	virtual bool			command_status_self(
								command_t			command,
								command_status_t&	status );

	virtual void			activate_self();

							// === drawing ===
							
	void					draw_selection(
								grafport_t*			port );

	void					draw_caret(
								grafport_t*			port );

	void					draw_hilited_line(
								grafport_t*			port );

	void					hilite_text(
								grafport_t*			port );
								
	void					toggle_caret();
	
	void					update_selection();
	
	void					update_caret();
	
	void					recalc_selection_position();
	
							// === text related ===

	void					typeface_changed();

	codepos_t				point_to_offset(
								long				x,
								long				y );

	void					pixel_position(
								const codepos_t&	pos,
								point&				where );

	codepos_t				move_up_down(
								codepos_t			pos,
								f64					column,
								int					dy );
	
	f64						caret_column(
								const codepos_t&	pos );
	
	void					caret_column_changed(
								const codepos_t&	pos );
									
	inline const point&		autoscroll_deltas() const;
	
	inline int				left_margin() const;
	
	void					extend_selection(
								codepos_t&			selfrom,
								codepos_t&			selto,
								int					mode );

	bool					modify_selection(
								char				c );

							// === clipboard ===
							
	void					clear();
							
	void					copy();
	
	void					paste();
	
	void					paste_text(
								text_clip_t*		clip );

	void					paste_code(
								code_clip_t*		clip );
	
	void					select_all();
	
	bool					can_paste();

							// === focus ===
							
	void					submit_focus();

	void					load_focus(
								long				line,
								int					flags = 1 );
								
	void					flush_focus();


private:
	void					scroll_up_if_necessary();

	void					scroll_down_if_necessary();

	f64						custom_text_width(
								const char*			text_buf,
								long				text_len );

	void					custom_draw_text(
								grafport_t*			port,
								f64					x,
								f64					y,
								const char*			text_buf,
								long				text_len,
								const typeface_t*	typeface,
								int					cbal );

	long					custom_pixel_to_char(
								const char*			text_buf,
								long				text_len,
								f64*				offset_ptr );

	f64						draw_text_block(
								grafport_t*			port,
								f64					x,
								f64					y,
								const char*			text,
								long				length,
								const typeface_t*	typeface,
								int&				flags,
								int&				cbal );

	void					line_text(
								long				line,
								istring&			text,
								int					flags = 0 );

	long					line_length(
								long				line );

	void					visible_area(
								grafport_t*			port,
								long&				bottom,
								long&				top );

	void					jump_tags(
								codepos_t&			pos );

	typeface_t				m_typeface;
	int						m_line_height;
	int						m_glyph_height;
	f64						m_tab_width;
	point					m_autoscroll_deltas;

	timer_t*				m_caret_blink;
	u64						m_caret_visible;
	f64						m_caret_column;
	
	codepos_t				m_selfrom;
	codepos_t				m_selto;
	point					m_selfrom_position;
	point					m_selto_position;
	int						m_selside;

	istring					m_focus_text;
	s32						m_focus_line;
	bool					m_focus_dirty;
	long					m_zero_line;
	long					m_hilited_line;
	int						m_left_margin;

	htab_t*					m_keywords;

	code::file_t			m_codefile;
	code_undo_t*			m_code_undo;
};

inline const point&
code_view_t::autoscroll_deltas() const
{
	return m_autoscroll_deltas;
}

inline int
code_view_t::left_margin() const
{
	return m_left_margin;
}

long
compare_codepos(
	const codepos_t&		posa,
	const codepos_t&		posb );


END_AKLABETH_NAMESPACE

#endif

