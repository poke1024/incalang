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
//	cheap_flat_container_t.cp  ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "cheap_flat_container_t.h"

BEGIN_ICARUS_NAMESPACE

cheap_flat_container_t::cheap_flat_container_t(
	const char*			filename ) :
	
	flat_container_t( filename )
{
	m_offset = 0;
}

void
cheap_flat_container_t::extract(
	void*				buffer,
	fpos_t				offset,
	fpos_t				length )
{
	if( offset != m_offset )
		os_fseek( m_file, offset, SEEK_SET );
		
	os_fread( buffer, length, m_file );
		
	m_offset = offset + length;
}

END_ICARUS_NAMESPACE
