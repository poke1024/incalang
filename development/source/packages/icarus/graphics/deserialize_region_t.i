// ===========================================================================
//	deserialize_region_t.i	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

long				count = height;
	
do {
	scanline_t**	link = row_ptr++;
	scanline_t*		scanline;
	
	long			scount;
	
	scount = READ_BITS;
	
	while( scount-- )
	{
		scanline = alloc_sl( heap );
		
		u32			start;
		u32			end;
		
		start = READ_BITS;
		end = READ_BITS;
		
		scanline->start = start + left;
		scanline->end = end + left;
		
		*link = scanline;
		link = &scanline->next;
	}
	
	*link = 0;
} while( --count );
