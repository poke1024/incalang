// ===========================================================================
//	serialize_region_t.i	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

long				count = bottom - top + 1;

do {
	scanline_t*		row = *row_table++;
	
	scanline_t*		scanline;
	
	long			scount = 0;
	
	scanline = row;
	while( scanline )
	{
		scount++;
		scanline = scanline->next;
	}
	
	WRITE_BITS( scount );
	
	scanline = row;
	while( scanline )
	{
		u32			start = scanline->start - left;
		u32			end = scanline->end - left;
	
		WRITE_BITS( start );
		WRITE_BITS( end );
	
		scanline = scanline->next;
	}
} while( --count );
