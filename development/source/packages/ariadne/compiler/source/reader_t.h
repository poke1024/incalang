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
//	reader_t.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_READER_H
#define ARIADNE_READER_H

//#pragma once

#include "compiler.h"
#include "lexer.h"

BEGIN_COMPILER_NAMESPACE

class reader_t {
private:
	struct binding_t {
		binding_t*		next;
		char*			buffer;
		char*			mark;
		fileid_t		fileid;
		bool			submit;
	};

	struct file_t {
		file_t*			next;
		fileid_t		id;
		istring			path;
	};

public:
						reader_t();
						
	virtual				~reader_t();

	u32					read_line(
							void*				buffer,
							u32					count );

	void				push(
							char*				buffer );
							
	//void				push_file(
	//						const char*			path );

	bool				lookup_fileid(
							fileid_t			fileid,
							istring&			path ) const;

private:
	bool				pop();
		
	fileid_t			register_fileid(
							const char*			path );
							
	static char*		submit_fileid(
							binding_t*			binding,
							char*				buffer,
							u32&				count );
	
	binding_t*			m_binding;
	file_t*				m_files;
};

END_COMPILER_NAMESPACE

#endif

