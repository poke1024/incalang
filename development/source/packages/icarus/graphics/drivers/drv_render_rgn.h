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
//	drv_render_rgn.h		   ©1997-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

if( rgn->is_empty() )
	return;
	
// gather information on sizes of involved components.
// ==============================================

rect					i_dest_rect;
long					d_width, d_height;
long					s_width, s_height;
double					fx, fy;
double					bx_start, by_start;

double					dr_left_floor = floor( dest_rect.left );
double					dr_top_floor = floor( dest_rect.top );

// get height and width of source and destination rectangles.
i_dest_rect.left = dr_left_floor;
i_dest_rect.top = dr_top_floor;
i_dest_rect.right = ceil( dest_rect.right );
i_dest_rect.bottom = ceil( dest_rect.bottom );

const rect&				bounds = rgn->bounds();

s_width = bounds.right - bounds.left + 1;
s_height = bounds.bottom - bounds.top + 1;
d_width = i_dest_rect.right - i_dest_rect.left;
d_height = i_dest_rect.bottom - i_dest_rect.top;

if( d_width <= 0 || d_height <= 0 || d_width > s_width )
	return;

// get rectangle's ratios.
fx = (double)s_width / (double)( dest_rect.right - dest_rect.left );
fy = (double)s_height / (double)( dest_rect.bottom - dest_rect.top );

// get start pixel fraction (used as an error measure for
// the float-rect int-rect conversion).
bx_start = -( dest_rect.left - dr_left_floor ) * fx;
by_start = -( dest_rect.top - dr_top_floor ) * fy;

// obtain bitmap data.
// ==============================================

u08*					pixel_ptr;
const long				pitch = bmp->pitch;

pixel_ptr = ( (u08*)bmp->pixels )
	+ i_dest_rect.left * DRV_BYTES_PER_PIXEL
	+ i_dest_rect.top * (long)pitch;

// perform clipping.
// ==============================================

const rect*				clip = bmp->clip;

if( i_dest_rect.top < clip->top )
{
	long				delta;
	delta = clip->top - i_dest_rect.top;
	pixel_ptr += delta * pitch;		
	by_start += delta * fy;
	if( ( d_height -= delta ) <= 0 )
		return;
}

if( i_dest_rect.left < clip->left )
{
	long				delta;
	delta = clip->left - i_dest_rect.left;
	pixel_ptr += delta * DRV_BYTES_PER_PIXEL;
	bx_start += delta * fx;
	if( ( d_width -= delta ) <= 0 )
		return;
}

if( i_dest_rect.bottom > clip->bottom )
{
	long				delta;
	delta = i_dest_rect.bottom - clip->bottom;
	if( ( d_height -= delta ) <= 0 )
		return;
}
	
if( i_dest_rect.right > clip->right )
{
	long				delta;
	delta = i_dest_rect.right - clip->right;
	if( ( d_width -= delta ) <= 0 )
		return;
}
		
// set up tables and variables.
// ==============================================

region_t::scanline_t*	scanline;
region_t::scanline_t**	row_table;

s32*					bx_tab;
u32*					count_tab;
u32*					count_tab_ptr;
double					by0, by1;

count_tab = get_region_render_tables(
	d_width, bx_start + rgn->bounds().left, fx );
	
if( !count_tab )
	return;
	
bx_tab = (s32*)&count_tab[ d_width ];

by_start += 0.5;
by0 = by1 = by_start;

row_table = rgn->m_row_table;

// perform the drawing.
// ==============================================

u32						count;
u32						count_x;
s32						x, y, by, by_top, by_bottom;
s32						map_by_start, map_by_end, map_bx_end;	
s32						px_start, px_end, first_x, last_x;

y = d_height;
do
{
	by1 += fy;

	map_by_start = (long)by0;
	map_by_end = (long)by1;
	
	if( map_by_start == map_by_end )
		// double rows in case of vertical enlarging (you
		// may use it, but note that it's not very efficient)
		map_by_end = map_by_start + 1;

	count_x = d_width;
	count_tab_ptr = count_tab;

	do {
		*( count_tab_ptr++ ) = 0;
	} while( --count_x );

	first_x = 2147483647L;
	last_x = -first_x;
	
	by_top = std::max( map_by_start, 0L );
	by_bottom = std::min( map_by_end, (long)s_height );
		
	for( by = by_top; by < by_bottom; by++ )
	{	
		scanline = row_table[ by ];
		
		if( !scanline )
		{
next_row:
			continue;
		}
	
		px_end = scanline->end;
		px_start = scanline->start;
		scanline = scanline->next;
				
		s32 map_bx_start = bx_tab[ 0 ]; 
		while( px_end < map_bx_start )
		{
			if( !scanline )
				goto next_row;
			px_start = scanline->start;
			px_end = scanline->end;
			scanline = scanline->next;
		}
		if( px_start < map_bx_start )
			px_start = map_bx_start;

		x = 0;
		do
		{					
			// find first pixel to intersect scanline.
			while( ( map_bx_end = bx_tab[ x + 1 ] ) <= px_start )
			{
				if( ++x >= d_width )
					goto next_row;
			}
										
			if( x < first_x )
				first_x = x;
			if( x > last_x )
				last_x = x;
								
			// work on pixel-scanline intersection. we're now
			// collection scanlines which are < map_bx_end. note
			// that map_bx_end itself is not in the pixel anymore.
			
			count_tab_ptr = &count_tab[ x ];
			count = *count_tab_ptr;
			while( true )
			{
				if( map_bx_end > px_end )
				{
					// pixel covers complete scanline.
					// get next scanline and continue.
					count += px_end - px_start + 1;
					
					if( !scanline )
					{
						*count_tab_ptr = count;
						goto next_row;
					}
					
					px_start = scanline->start;
					px_end = scanline->end;
					scanline = scanline->next;
				
					if( map_bx_end <= px_start )
						break;
				}
				else
				{
					// scanline goes further than current pixel,
					// so it's covered partially. remove the part
					// from scanline that's covered and go on to
					// the next pixel.
					count += map_bx_end - px_start;
					px_start = map_bx_end;
					break;
				}
			}
			*count_tab_ptr = count;
			
		} while( ++x < d_width );
	}
	
	if( first_x <= last_x )
	{				
		u08*			old_pixel_ptr = pixel_ptr;
		const s32*		bx_tab_ptr;
		const u32*		count_tab_ptr;
		s32				by_diff;
	
		by_diff = by_bottom - by_top;
		pixel_ptr += first_x * DRV_BYTES_PER_PIXEL;
		count_tab_ptr = &count_tab[ first_x ];
		bx_tab_ptr = &bx_tab[ first_x ];
		x = last_x - first_x + 1;
		
		do
		{
			if( ( count = *( count_tab_ptr++ ) ) != 0 )
			{
				// could optimize bx_tab access via a bx_diff_tab here!
			
				u32		diff = ( bx_tab_ptr[ 1 ] - bx_tab_ptr[ 0 ] ) * by_diff;

				if( count == diff )
				{
					DRV_PUT_PIXEL( pixel_ptr, draw_color );
				}
				else
				{
					unsigned int atten = ( count << 8 ) / diff;
					unsigned int inv_atten = 0xff - atten;

					DRV_MIX_PIXEL_COLORS( pixel_ptr, draw_color, atten, inv_atten );
				}
			}
		
			pixel_ptr += DRV_BYTES_PER_PIXEL;		
			bx_tab_ptr++;
		} while( --x );
		
		pixel_ptr = old_pixel_ptr + pitch;
	}
	else
		pixel_ptr += pitch;

	by0 = by1;
} while( --y );
