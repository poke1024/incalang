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
//	code_tag_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_TAG_H
#define CODE_TAG_H

//#pragma once

#include "code.h"
#include "shared_t.h"
#include "code_options.h"
#include "code_text_t.h"
#include "output_stream_t.h"

BEGIN_CODE_NAMESPACE

const char				k_tag_start = 1;
const char				k_tag_end = 2;

const int				k_tag_ptr_size = 2 * ICARUS_PTR_SIZE;
const int				k_tag_all_size = k_tag_ptr_size + 2;

enum {
	tag_block			= 1,
	tag_include			= 2
};

typedef s32				tagid_t;

// ---------------------------------------------------------------------------

class stream_t;

class tag_t : public shared_t {
public:
						tag_t(
							tagid_t				id );

	tagid_t				id() const;
	
	virtual long		token_count() const = 0;
	
	virtual void		text(
							istring&			text ) = 0;

	virtual void		serialize(
							text_t&				text,
							int					flags );

	virtual void		save(
							stream_t&			stream,
							const options_t&	options ) = 0;

protected:
	virtual				~tag_t();

private:
	tagid_t				m_id;
};

// ---------------------------------------------------------------------------

tag_t*
extract_tag(
	const char*			text );

void
take_code(
	char*				destbuf,
	const char*			textbuf,
	long				textlen );
	
void
drop_code(
	const char*			textbuf,
	long				textlen );

END_CODE_NAMESPACE

#endif

