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
//	code_module_tag_t.h   	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_MODULE_TAG_H
#define CODE_MODULE_TAG_H

//#pragma once

#include "code_collapse_tag_t.h"
#include <sys/types.h>
#include <sys/stat.h>

BEGIN_CODE_NAMESPACE

class include_tag_t : public collapse_tag_t {
public:
						include_tag_t(
							const char*			base,
							const char*			path );

	virtual long		token_count() const;

	virtual void		text(
							istring&			text );

	virtual void		serialize(
							text_t&				text,
							int					flags );

	virtual void		save(
							stream_t&			stream,
							const options_t&	options );

	virtual bool		can_expand();

	virtual void		update();

protected:
	void				load_file(
							const char*			path );
	
	istring				m_base;
	istring				m_path;
	istring				m_full_path;
	bool				m_load_error;
	long				m_token_count;
	struct _stat		m_file_stat;
};

END_CODE_NAMESPACE

#endif

