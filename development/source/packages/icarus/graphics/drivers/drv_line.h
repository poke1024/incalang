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
//	drv_line.h			  	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

if( !( bmp->flags & bmp_draw_clipped ) )
	goto accept;

#define CLIP_LEFT				1
#define CLIP_RIGHT				2
#define CLIP_BOTTOM				4
#define CLIP_TOP				8

#define CLIP_LEFT_RIGHT			( CLIP_LEFT | CLIP_RIGHT )
#define CLIP_TOP_BOTTOM			( CLIP_TOP | CLIP_BOTTOM )

#define FRACTION(a,b,c)\
	round((((double)a)/b)*c)

//the fast but buggy version
//#define FRACTION(a,b,c)\
//	((((((s64)a)<<32)/b)*c)>>32)

{
	unsigned long flags0, flags1;
	long x_left, y_top;
	long x_right, y_bottom;
	const rect* clip = bmp->clip;

	x_left = clip->left;
	y_top = clip->top;
	x_right = clip->right - 1;
	y_bottom = clip->bottom - 1;

	if( x1 < x_left )
	{
		if ( x2 < x_left )
			return;
		flags0 = CLIP_LEFT;
	}
	else if( x1 > x_right )
	{
		if ( x2 > x_right )
			return;
		flags0 = CLIP_RIGHT;
	}
	else
		flags0 = 0;

	if( y1 < y_top )
	{
		if( y2 < y_top )
			return;
		flags1 = ( y2 > y_bottom ? CLIP_TOP : 0 );
		flags0 |= CLIP_BOTTOM;
	}
	else if( y1 > y_bottom )
	{
		if( y2 > y_bottom )
			return;
		flags1 = ( y2 < y_top ? CLIP_BOTTOM : 0 );
		flags0 |= CLIP_TOP;
	}
	else
		flags1 = ( y2 < y_top ? CLIP_BOTTOM : ( y2 > y_bottom ? CLIP_TOP : 0 ) );

	if ( x2 < x_left )
		flags1 |= CLIP_LEFT;
	else if ( x2 > x_right )
		flags1 |= CLIP_RIGHT;

	if( flags0 | flags1 )
	{
		long dx, dy;

		dx = x2 - x1;
		dy = y2 - y1;

		if( flags0 & CLIP_LEFT_RIGHT )
		{
			if( flags0 & CLIP_LEFT )
			{
				y1 += FRACTION( x_left - x1, dx, dy );
				x1 = x_left;
			}
			else
			{
				y1 += FRACTION( x_right - x1, dx, dy );
				x1 = x_right;
			}

			if( y1 < y_top )
			{
				if( flags1 & CLIP_BOTTOM )
					return;
				else
					flags0 = CLIP_BOTTOM;
			}
			else if ( y1 > y_bottom )
			{
				if( flags1 & CLIP_TOP )
					return;
				else
					flags0 = CLIP_TOP;
			}
			else if( !flags1 )
				goto accept;
			else
				flags0 = 0;
		}
		
		if ( flags0 & CLIP_TOP_BOTTOM )
		{
	   		if( flags0 & CLIP_BOTTOM )
	   		{
	   			x1 += FRACTION( y_top - y1, dy, dx );
				y1 = y_top;
	   		}
	   		else
	   		{
	   			x1 += FRACTION( y_bottom - y1, dy, dx );
				y1 = y_bottom;
	   		}	
			
			if( ( x1 < x_left ) || ( x1 > x_right ) )
				return;
			if ( !flags1 )
				goto accept;
		}

		if( flags1 & CLIP_LEFT_RIGHT )
		{
			if( flags1 & CLIP_LEFT )
			{
				y2 += FRACTION( x_left - x2, dx, dy );
				x2 = x_left;
			}
			else
			{
				y2 += FRACTION( x_right  - x2, dx, dy );
				x2 = x_right;
			}
		
			if( y2 < y_top )
				flags1 = CLIP_BOTTOM;
			else if( y2 > y_bottom )
				flags1 = CLIP_TOP;
			else
				goto accept;
		}
		
		if( flags1 & CLIP_TOP_BOTTOM )
		{
			if( flags1 & CLIP_BOTTOM )
			{
				x2 += FRACTION( y_top - y2, dy, dx );
				y2 = y_top;
			}
			else
			{
				x2 += FRACTION( y_bottom - y2, dy, dx );
				y2 = y_bottom;
			}
			
			goto accept;
		}
	}
}

#undef CLIP_LEFT
#undef CLIP_RIGHT
#undef CLIP_BOTTOM
#undef CLIP_TOP

#undef CLIP_LEFT_RIGHT
#undef CLIP_TOP_BOTTOM

accept:

//	Using a fast jaggy line algorithm called Bresenham's run-length slice algorithm
//	described in Dr Dobbs Journal November 92. Based on example code from this article.

long delta_x, delta_y, inc_y;
long whole_step, mod_step, adj_up, adj_down, error_term;
unsigned long initial_pixel_count, final_pixel_count, run_length;

u08* pixel_ptr;

// get bytes per bitmap row.
const long pitch = bmp->pitch;

// figure out how far we're going horizontally and vertically.
// (recognize special-cases horizontal, vertical, and diagonal lines, for speed
// and to avoid nasty boundary conditions and division by 0).	
if( x2 >= x1 )
{
	pixel_ptr = ( (u08*)bmp->pixels ) +
		x1 * DRV_BYTES_PER_PIXEL + y1 * pitch;
		
	if( ( delta_x = x2 - x1 ) == 0 )
	{
		// draw pure vertical line.
		if( y2 >= y1 )
		{
			run_length = y2 - y1 + 1;
			inc_y = pitch;
		}
		else
		{
			run_length = y1 - y2 + 1;
			inc_y = -pitch;
		}

		while( run_length-- )
		{
			DRV_PUT_PIXEL( pixel_ptr, color );
			pixel_ptr += inc_y;
		}
		
		return;	
	}	

	if( ( delta_y = y2 - y1 ) != 0 )
		goto normal_line;
}
else
{
	pixel_ptr = ( (u08*)bmp->pixels ) +
		x2 * DRV_BYTES_PER_PIXEL + y2 * pitch;

	delta_x = x1 - x2;
	if( ( delta_y = y1 - y2 ) != 0 )
		goto normal_line;
}

// draw pure horizontal line.
run_length = delta_x + 1;
DRV_PUT_HLINE( pixel_ptr, run_length, color );
return;

normal_line:	
// draw a normal line.
if( delta_y < 0 )
{
	inc_y = -pitch;
	delta_y = -delta_y;
}
else
{
	inc_y = pitch;
}

// determine whether the line is X or Y major, and handle accordingly.
if( delta_x == delta_y )
{
	// draw pure diagonal line.
	delta_x++;
	do
	{
		DRV_PUT_PIXEL( pixel_ptr, color );
		pixel_ptr += inc_y + DRV_BYTES_PER_PIXEL;
	} while( --delta_x );
}
else if( delta_x > delta_y )
{
	// X major line.
	// minimum # of pixels in a run in this line.
	whole_step = delta_x / delta_y;
	mod_step = delta_x % delta_y;

	// error term adjust each time Y steps by 1; used to tell when one
	// extra pixel should be drawn as part of a run, to account for
	// fractional steps along the X axis per 1-pixel steps along Y.
	adj_up = mod_step << 1;

	// error term adjust when the error term turns over, used to factor
	// out the X step made at that time.
	adj_down = delta_y << 1;

	// initial error term; reflects an initial step of 0.5 along the Y axis
	error_term = mod_step - adj_down;

	// the initial and last runs are partial, because Y advances only 0.5
	// for these runs, rather than 1. Divide one full run, plus the
	// initial pixel, between the initial and last runs.
	initial_pixel_count = ( whole_step >> 1 ) + 1;
	final_pixel_count = initial_pixel_count;
	
	// if there're an odd number of pixels per run, we have 1 pixel that can't
	// be allocated to either the initial or last partial run, so we'll add 0.5
	// to error term so this pixel will be handled by the normal full-run loop.
	if( whole_step & 1 )
		error_term += delta_y;
			
	// if the basic run length is even and there's no fractional
	// advance, we have one pixel that could go to either the initial
	// or last partial run, which we'll arbitrarily allocate to the
	// last run.
	else if( adj_up == 0  )
		initial_pixel_count--;

	// draw the first, partial run of pixels.
	pixel_ptr = DRV_PUT_HLINE( pixel_ptr, initial_pixel_count, color );
	pixel_ptr += inc_y;
	
	// draw all full runs.
	if( --delta_y )
	{
		do
		{
			// advance the error term and add an extra pixel if the error
			// term so indicates.
			if( ( error_term += adj_up ) > 0 )
			{
				run_length = whole_step + 1;
				error_term -= adj_down;		// reset the error term
			}
			else
				run_length = whole_step;
			
			pixel_ptr = DRV_PUT_HLINE( pixel_ptr, run_length, color );
			pixel_ptr += inc_y;
		} while( --delta_y );
	}
	
	// draw the final run of pixels.
	DRV_PUT_HLINE( pixel_ptr, final_pixel_count, color );
}
else
{
	// Y major line.
	// minimum # of pixels in a run in this line.
	whole_step = delta_y / delta_x;
	mod_step = delta_y % delta_x;

	// error term adjust each time X steps by 1; used to tell when 1 extra
	// pixel should be drawn as part of a run, to account for
	// fractional steps along the Y axis per 1-pixel steps along X.
	adj_up = mod_step << 1;

	// error term adjust when the error term turns over, used to factor
	// out the Y step made at that time.
	adj_down = delta_x << 1;
	
	// initial error term; reflects initial step of 0.5 along the X axis.
	error_term = mod_step - adj_down;

	// the initial and last runs are partial, because X advances only 0.5
	// for these runs, rather than 1. Divide one full run, plus the
	// initial pixel, between the initial and last runs.
	initial_pixel_count = ( whole_step >> 1 ) + 1;
	final_pixel_count = initial_pixel_count;

	// If there are an odd number of pixels per run, we have one pixel
	// that can't be allocated to either the initial or last partial
	// run, so we'll add 0.5 to the error term so this pixel will be
	// handled by the normal full-run loop.
	if( whole_step & 1 )
		error_term += delta_x;

	// if the basic run length is even and there's no fractional advance, we
	// have 1 pixel that could go to either the initial or last partial run,
	// which we'll arbitrarily allocate to the last run */
	else if( adj_up == 0 )
		initial_pixel_count--;
		
	// draw the first, partial run of pixels.
	do {	
		DRV_PUT_PIXEL( pixel_ptr, color );
		pixel_ptr += inc_y;
	} while( --initial_pixel_count );
	
	pixel_ptr += DRV_BYTES_PER_PIXEL;
	
	// draw all full runs.
	if( --delta_x )
	{
		do
		{
			// advance the error term and add an extra pixel if the error
			// term so indicates.
			if( ( error_term += adj_up ) > 0 )
			{
				run_length = whole_step + 1;
				error_term -= adj_down;		// reset the error term
			}
			else
				run_length = whole_step;
				
			do {	
				DRV_PUT_PIXEL( pixel_ptr, color );
				pixel_ptr += inc_y;
			} while( --run_length );
				
			pixel_ptr += DRV_BYTES_PER_PIXEL;
			
		} while( --delta_x );
	}
	
	// draw the final run of pixels.
	
	do {	
		DRV_PUT_PIXEL( pixel_ptr, color );
		pixel_ptr += inc_y;
	} while( --final_pixel_count );
}
