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
//	variable_t.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "aklabeth.h"
#include "random_stream_t.h"

BEGIN_AKLABETH_NAMESPACE

class variable_array_t;

class variable_t {
public:
						variable_t();

						~variable_t();

	void				init_as_variable(
							random_stream_t*	stream,
							u08*				base_addr,
							int					ident );

	void				init_as_globals(
							random_stream_t*	stream,
							void*				clss );

	void				init_as_separator(
							const char*			name,
							int					ident );

	void				setup(
							random_stream_t*	stream );

	const char*			name();

	const char*			value();

	bool				is_expandable();

	variable_array_t*	childs(
							random_stream_t*	stream );

	int					ident() const;

	bool				expanded() const;

	void				set_expanded(
							bool				expanded );

	bool				is_separator() const;

	long				collapse();

	static void			init(
							const char*			system_file );

private:
	void				setup_childs(
							random_stream_t*	stream );

	void				setup_globals(
							random_stream_t*	stream,
							void*				clss );

	istring				m_name;
	istring				m_type;
	istring				m_value;

	s32					m_offset;
	u08*				m_base_addr;

	void*				m_clss;
	u32					m_deref;
	u08					m_flags;

	u16					m_ident;
	bool				m_expanded;

	variable_array_t*	m_childs;
};

END_AKLABETH_NAMESPACE
