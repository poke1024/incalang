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
//	fast_flat_container_t.cp   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "fast_flat_container_t.h"

BEGIN_ICARUS_NAMESPACE

enum {
	BLOCK_SIZE			= 8192
};

fast_flat_container_t::fast_flat_container_t(
	const char*			filename ) :
	
	flat_container_t( filename )
{
	os_fseek( m_file, 0, SEEK_END );
	m_length = os_ftell( m_file );
	
	long				length;
	long				block_count;
	
	block_count = m_length / BLOCK_SIZE + 1;
	length = block_count * BLOCK_SIZE;
	
	m_buffer = new u08[ length ];
	m_bitmap = new u08[ block_count / 8 + 1 ];
	std::memset( m_bitmap, 0, block_count / 8 + 1 );
}

fast_flat_container_t::~fast_flat_container_t()
{
	delete[] m_buffer;
	delete[] m_bitmap;
}

void
fast_flat_container_t::extract(
	void*				buffer,
	fpos_t				offset,
	fpos_t				length )
{
	long				start_block = offset / BLOCK_SIZE;
	long				end_block = ( offset + length - 1 ) / BLOCK_SIZE;
	u08*				bitmap = m_bitmap;

	for( long block = start_block; block <= end_block; block++ )
	{
		if( ( bitmap[ block / 8 ] & ( 1 << ( block & 7 ) ) ) == 0 )
		{
			os_fseek( m_file, block * BLOCK_SIZE, SEEK_SET );
			os_fread( m_buffer + block * BLOCK_SIZE, BLOCK_SIZE, m_file );
			bitmap[ block / 8 ] |= ( 1 << ( block & 7 ) );
		}
	}

	std::memcpy( buffer, m_buffer + offset, length );
}

END_ICARUS_NAMESPACE
