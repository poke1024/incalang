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
//	flat_file_t.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "flat_file_t.h"
#include "icarus_decrypt.h"

BEGIN_ICARUS_NAMESPACE

flat_file_t::flat_file_t(
	flat_container_t*	container,
	toc_entry_t*		entry )
{
	m_container = container;
	m_entry = entry;
	m_position = 0;
}

void
flat_file_t::seek(
	fpos_t				offset,
	int					mode )
{
	fpos_t				pos;
	fpos_t				len = m_entry->length;

	switch( mode )
	{
		case SEEK_SET:
			pos = offset;
			break;
		
		case SEEK_CUR:
			pos = m_position + offset;
			break;
		
		case SEEK_END:
			pos = len - offset;
			break;
			
		default:
			throw_icarus_error( "illegal seek mode" );
	}
	
	if( pos < 0 )
		pos = 0;
	else if( pos > len )
		pos = len;
		
	m_position = pos;
}

fpos_t
flat_file_t::tell() const
{
	return m_position;
}

fpos_t
flat_file_t::read(
	void*				buffer,
	fpos_t				count )
{
	fpos_t				pos = m_position;
	fpos_t				len = m_entry->length;

	if( pos + count > len )
	{
		count = len - pos;
		if( count <= 0 )
			return 0;
	}

	fpos_t				effpos = pos + m_entry->offset;

	m_container->extract(
		buffer, effpos, count );

	if( m_entry->flags & 1 )
		decrypt( buffer, count, effpos );

	pos += count;
	m_position = pos;
		
	return count;
}

END_ICARUS_NAMESPACE
