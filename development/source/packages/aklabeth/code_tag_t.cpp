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
//	code_tag_t.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_tag_t.h"

BEGIN_CODE_NAMESPACE

tag_t::tag_t(
	tagid_t				id ) :
	
	m_id( id )
{
}

tag_t::~tag_t()
{	
}

tagid_t
tag_t::id() const
{
	return m_id;
}

void
tag_t::serialize(
	text_t&				text,
	int					flags )
{
	text.append_tag( this );
}

// ---------------------------------------------------------------------------

tag_t*
extract_tag(
	const char*			text )
{
	iptr_t				val = 0;
	
	for( int i = 0; i < k_tag_ptr_size; i++ )
	{
		char			c = text[ i ];
		int				digit;
		
		if( c >= 'a' && c <= 'f' )
			digit = 10 + c - 'a';
		else if( c >= '0' && c <= '9' )
			digit = c - '0';
		else
			return 0;
			
		val <<= 4;
		val |= digit;
	}
	
	return (tag_t*)val;
}

void
take_code(
	char*				destbuf,
	const char*			textbuf,
	long				textlen )
{
	if( !textlen )
		return;
		
	do {
		char			c = *textbuf++;
		
		*destbuf++ = c;

		if( c == k_tag_start )
		{
			tag_t*	tag;
		
			tag = extract_tag( textbuf );
			tag->refer();

			memcpy( destbuf, textbuf,
				k_tag_all_size - 1 );

			textbuf += k_tag_all_size - 1;
			destbuf += k_tag_all_size - 1;
			textlen -= k_tag_all_size - 1;
		}
	} while( --textlen );
}
	
void
drop_code(
	const char*			textbuf,
	long				textlen )
{
	if( !textlen )
		return;
		
	do {
		char			c = *textbuf++;
		
		if( c == k_tag_start )
		{
			tag_t*	tag;
		
			tag = extract_tag( textbuf );
			tag->unrefer();

			textbuf += k_tag_all_size - 1;
			textlen -= k_tag_all_size - 1;
		}
	} while( --textlen );
}

END_CODE_NAMESPACE
