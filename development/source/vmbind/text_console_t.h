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
//	text_console_t.h		   �2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"
#include <Windows.h>

BEGIN_MACHINE_NAMESPACE

class text_console_t {
public:
						text_console_t();

	virtual				~text_console_t();

	s32					inkey();

	void				input(
							istring&			text );
							
	void				print(
							const char*			text,
							long				length );
							
	void				flush();

	int					crscol();

	int					crslin();

	void				htab(
							int					h );

	void				vtab(
							int					v );

	void				set_forecolor(
							int					r,
							int					g,
							int					b );

	void				set_backcolor(
							int					r,
							int					g,
							int					b );

protected:
	void				output(
							const char*			text,
							long				length );
	
	bool				process_commands(
							const char*			text,
							long				length );

	void				update_sbi();

	long				m_buffer_size;
	char*				m_buffer;
	long				m_buffer_index;

	HANDLE				m_out_handle;
	HANDLE				m_in_handle;

	CONSOLE_SCREEN_BUFFER_INFO
						m_sbi;
	bool				m_sbi_dirty;
};

END_MACHINE_NAMESPACE
