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
//	bitmap_t.cp				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "bitmap_t.h"
#include "input_stream_t.h"
#include "gfx_vtable.h"
#include "grafport_t.h"
#include "os_core.h"

BEGIN_ICARUS_NAMESPACE

bitmap_t::bitmap_t(
	pixel_format_t		format,
	long				width,
	long				height,
	int					flags ) :
	
	m_lock_count( 0 ),
	m_flags( flags )
{
	m_vtable = &gfxdrv_vtable[ format ];
	m_data = create_bitmap( format, width, height, flags );
}

bitmap_t::bitmap_t(
	bitmap_data_t*		data ) :
	
	m_lock_count( 0 )
{
	m_data = data;
	m_vtable = &gfxdrv_vtable[ m_data->format ];
}

bitmap_t::~bitmap_t()
{
	delete_bitmap( m_data );
}

void
bitmap_t::set_palette_entry(
	int					index,
	const u08*			color )
{
	u08*				palette = m_data->palette;
	
	if( !palette )
		return;
		
	index *= 3;
		
	palette[ index + 0 ] = color[ 0 ];
	palette[ index + 1 ] = color[ 1 ];
	palette[ index + 2 ] = color[ 2 ];
}

bitmap_t*
bitmap_t::to_format(
	pixel_format_t		format )
{
	if( this->format() == format )
		return this;
		
	bitmap_t*			bmp;
	int					width = this->width();
	int					height = this->height();
		
	bmp = new bitmap_t(
		format, width, height, flags() );

	try {
		rect			r;
	
		r.set( 0, 0, width, height );

		grafport_t		src_port( this );
		grafport_t		dst_port( bmp );

		grafport_t::blit( src_port, dst_port, r, r );
		
	} catch(...) {
		delete bmp;
		throw;
	}
	
	delete this;
	return bmp;
}

void
bitmap_t::lock()
{
	if( ++m_lock_count != 1 )
		return;
		
	void*				native = m_data->native;
	
	if( !native )
		return;
		
	os_lock_bitmap( native );

	m_data->pixels = os_get_bitmap_pixels( native );
}

void
bitmap_t::unlock()
{
	if( --m_lock_count != 0 )
		return;

	void*				native = m_data->native;

	if( !native )
		return;
		
	os_unlock_bitmap( native );
	
	m_data->pixels = 0;
}

END_ICARUS_NAMESPACE
