// ===========================================================================
//	os_core_mac.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "os_core.h"

#include <QDOffscreen.h>
#include <Timer.h>

#include <string>
#include <cstring>

BEGIN_ICARUS_NAMESPACE

void
os_lock_bitmap(
	void*				bmp )
{
	GWorldPtr			world = (GWorldPtr)bmp;

	if( !LockPixels( GetGWorldPixMap( world ) ) )
		throw_icarus_error( "could not lock gworld" );
}

void
os_unlock_bitmap(
	void*				bmp )
{
	GWorldPtr			world = (GWorldPtr)bmp;

	UnlockPixels( GetGWorldPixMap( world ) );
}

int
os_get_bitmap_pitch(
	void*				bmp )
{
	GWorldPtr			world = (GWorldPtr)bmp;
	
	return GetPixRowBytes( GetGWorldPixMap( world ) );
}

void*
os_get_bitmap_pixels(
	void*				bmp )
{
	GWorldPtr			world = (GWorldPtr)bmp;
	
	return GetPixBaseAddr( GetGWorldPixMap( world ) );
}

void*
os_create_bitmap(
	pixel_format_t		format,
	long				width,
	long				height,
	int					/*flags*/ )
{
	Rect				rect;
	
	rect.left = rect.top = 0;
	rect.right = width;
	rect.bottom = height;
	
	int					depth;
	
	switch( format )
	{
		case pixel_format_8_indexed_gray:
		case pixel_format_8_indexed:
			depth = 8;
			break;
			
		case pixel_format_16_555_RGB:
			depth = 16;
			break;
			
		case pixel_format_32_RGBA:
			depth = 32;
			break;
						
		default:
			throw_icarus_error( "pixel format not supported" );
	}
	
	OSErr				err;
	GWorldPtr			world = 0;

	err = NewGWorld( &world, depth, &rect, nil, nil, 0 );
	if( err != noErr || !world )
		throw_icarus_error( "could not allocate GWorld" );
		
	return world;
}

void
os_destroy_bitmap(
	void*				bmp )
{
	DisposeGWorld( (GWorldPtr)bmp );
}

// ---------------------------------------------------------------------------

u64
os_time_millis()
{
	UnsignedWide		ticks_wide;
	
	Microseconds( &ticks_wide );
	
	u64					ticks;
	
	ticks = ( ( (u64)ticks_wide.hi ) << 32 ) | ticks_wide.lo;
	
	return ticks / 1000L;
}

int
os_double_click_millis()
{
	return 250;
}

END_ICARUS_NAMESPACE
