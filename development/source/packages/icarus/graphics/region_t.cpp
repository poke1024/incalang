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
//	region_t.cp			 	   ©1997-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

// note that for regions, rectangles are handled a bit differently than in
// other areas of use. a rectangle (x0,y0,x1,y1) describes the set of points
// for which px >= x0 && py >= y0 and px <= x1 && py <= y1 (and not, as for
// clipping rects and the like, px < x1 and py < y1).

#include "region_t.h"
#include "fast_heap.h"
#include "grafport_t.h"
#include "input_stream_t.h"
#include "output_stream_t.h"
#include "polygon_rasterizer_t.h"
#include <cmath>

#pragma once off

BEGIN_ICARUS_NAMESPACE

// ---------------------------------------------------------------------------

#define					ROW_TABLE_HEAPS	10

static fast_heap*		s_scanline_heap = 0;

static fast_heap**		s_row_table_heap = 0;

static region_t::scanline_t**
alloc_row_table(
	unsigned long		height )
{
	/*region_t::scanline_t** ptr = (region_t::scanline_t**)
		NewPtr( sizeof( region_t::scanline_t* ) * height );
	if( !ptr )
	{
		Debugger();
		ExitToShell();
	}
	return ptr;*/

	int					bits = 32;

	for( int i = 1; i <= 32; i++ )
		if( ( height >> i ) == 0 )
		{
			bits = i;
			break;
		}
		
	if( bits <= ROW_TABLE_HEAPS )
	{
		fast_heap**		heap_table = s_row_table_heap;
	
		if( !heap_table )
		{
			heap_table = new fast_heap*[ ROW_TABLE_HEAPS ];
			s_row_table_heap = heap_table;

			for( int i = 0; i < ROW_TABLE_HEAPS; i++ )
				heap_table[ i ] = 0;
		}
	
		fast_heap*		heap = heap_table[ bits - 1 ];
		
		if( !heap )
		{
			size_t		size;
			
			size = sizeof( region_t::scanline_t* ) << bits;
			heap = new fast_heap( size, 1 );
			heap_table[ bits - 1 ] = heap;
		}
		
		return (region_t::scanline_t**)heap->alloc();
	}
		
	return new region_t::scanline_t*[ height ];
}

static void
free_row_table(
	region_t::scanline_t**		table,
	unsigned long		height )
{
	/*icarus_debug_disable( 0 );
	DisposePtr( (Ptr)table );
	icarus_debug_enable( 0 );
	return;*/

	int					bits = 32;

	for( int i = 1; i <= 32; i++ )
		if( ( height >> i ) == 0 )
		{
			bits = i;
			break;
		}
		
	if( bits <= ROW_TABLE_HEAPS )
		s_row_table_heap[ bits - 1 ]->free( table );
	else
		delete[] table;
}

// ---------------------------------------------------------------------------

#define alloc_sl( heap )							\
	( (scanline_t*)(heap)->alloc() )
	
#define free_sl( heap, scanline )					\
	(heap)->free( scanline )

// ---------------------------------------------------------------------------

#define STORE_SCANLINE(a,b)							\
	r_scanline = alloc_sl( heap );					\
	r_scanline->start = a;							\
	r_scanline->end = b;							\
	if( a < rx0 ) rx0 = a; last_rx1 = b;			\
	*r_link = r_scanline;							\
	r_link = &r_scanline->next;

#define COPY_SCANLINE(s) {							\
	long a = (s)->start, b = (s)->end;				\
	r_scanline = alloc_sl( heap );					\
	r_scanline->start = a;							\
	r_scanline->end = b;							\
	if( a < rx0 ) rx0 = a; last_rx1 = b;			\
	*r_link = r_scanline;							\
	r_link = &r_scanline->next; }

#define STORE_NEW_ROW								\
	if( last_rx1 > rx1 ) rx1 = last_rx1;			\
	*r_link = 0;									\
	r_link = ++r_row_ptr;

#define COPY_ROWS(s,rp,cnt)							\
	do {											\
		while( (s) )								\
		{											\
			COPY_SCANLINE( s );						\
			(s) = (s)->next;						\
		}											\
													\
		STORE_NEW_ROW;								\
		(s) = *((rp)++);							\
	} while( --(cnt) );

#define COPY_SCANLINE_FAST(s)						\
	r_scanline = alloc_sl( heap );					\
	r_scanline->start = (s)->start;					\
	r_scanline->end = (s)->end;						\
	*r_link = r_scanline;							\
	r_link = &r_scanline->next;

#define STORE_NEW_ROW_FAST							\
	*r_link = 0;									\
	r_link = ++r_row_ptr;
	
// ---------------------------------------------------------------------------
//		¥ region_t
// ---------------------------------------------------------------------------

region_t::region_t()
{
	if( !s_scanline_heap )
		s_scanline_heap = new fast_heap( sizeof( scanline_t ) );

	m_row_table = 0;
}

// ---------------------------------------------------------------------------
//		¥ ~region_t
// ---------------------------------------------------------------------------

region_t::~region_t()
{
	clear();
}

// ---------------------------------------------------------------------------
//		¥ serialize
// ---------------------------------------------------------------------------

void
region_t::serialize(
	output_stream_t&	stream )
{
	scanline_t**		row_table = m_row_table;
	
	if( !row_table )
	{
		stream.write_u32( 4294967295U );
		return;
	}

	const long			bottom = m_bounds.bottom;
	const long			top = m_bounds.top;
	const long			left = m_bounds.left;
	const long			right = m_bounds.right;

	stream.write_u32( top );
	stream.write_u32( bottom );
	stream.write_u32( left );
	stream.write_u32( right );
	
	const u32			max_diff = right - left;
	
	if( max_diff <= 0xff )
	{
		#define WRITE_BITS( x ) stream.write_u08( x )
		#include "serialize_region_t.i"
		#undef WRITE_BITS
	}
	else if( max_diff <= 0xffff )
	{
		#define WRITE_BITS( x ) stream.write_u16( x )
		#include "serialize_region_t.i"
		#undef WRITE_BITS
	}
	else
	{
		#define WRITE_BITS( x ) stream.write_u32( x )
		#include "serialize_region_t.i"
		#undef WRITE_BITS
	}
}

// ---------------------------------------------------------------------------
//		¥ deserialize
// ---------------------------------------------------------------------------

void
region_t::deserialize(
	input_stream_t&		stream )
{
	u32					top;

	top = stream.read_u32();
	
	if( top == 4294967295U )
	{
		clear();
		return;
	}
	
	long				bottom = stream.read_u32();
	long				height = bottom - top + 1;
	scanline_t**		row_ptr = allocate( height );
	
	long				left = stream.read_u32();
	long				right = stream.read_u32();
	
	m_bounds.top = top;
	m_bounds.bottom = bottom;
	m_bounds.left = left;
	m_bounds.right = right;

	fast_heap*			heap = s_scanline_heap;
	
	const u32			max_diff = right - left;
	
	if( max_diff <= 0xff )
	{
		#define READ_BITS stream.read_u08()
		#include "deserialize_region_t.i"
		#undef READ_BITS	
	}
	else if( max_diff <= 0xffff )
	{
		#define READ_BITS stream.read_u16()
		#include "deserialize_region_t.i"
		#undef READ_BITS	
	}
	else
	{
		#define READ_BITS stream.read_u32()
		#include "deserialize_region_t.i"
		#undef READ_BITS	
	}
}

// ---------------------------------------------------------------------------
//		¥ set_rect
// ---------------------------------------------------------------------------

void
region_t::set_rect(
	const rect&			r )
{
	unsigned long		height = r.bottom - r.top + 1;

	const long			left = r.left;
	const long			right = r.right;

	if( right < left || height <= 0 )
	{
		clear();
		return;
	}
	
  	scanline_t**		row_ptr = allocate( height );
  
  	fast_heap*			heap = s_scanline_heap;
	unsigned long		count = height;
  
 	do
 	{
 		scanline_t*		scanline;
 		
 		scanline = alloc_sl( heap );
 		scanline->next = 0;
 		scanline->start = left;
		scanline->end = right;
 
 		*row_ptr++ = scanline;
 	} while( --count );
 	
 	m_bounds = r;
}

// ---------------------------------------------------------------------------
//		¥ set_circle
// ---------------------------------------------------------------------------

void
region_t::set_circle(
	const point&		center,
	unsigned long		r )
{
	#define CIRCLE_SCANLINE( y )								\
		link = &row_ptr[ y ];									\
		scanline = alloc_sl( heap );							\
 		scanline->next = 0;										\
		scanline->start = x0;									\
		scanline->end = x1;										\
		*link = scanline;

	clear();

	if( !r )
		return;
	
	s32					x, y, d, x0, x1, cx, cy;

	cx = center.h;
	cy = center.v;
	m_bounds.set( cx - r, cy - r, cx + r, cy + r );

	scanline_t**		row_ptr = allocate( r * 2 + 1 );

  	// calculate scanlines.

 	fast_heap*			heap = s_scanline_heap;
	scanline_t*			scanline;
	scanline_t**		link;
  
	x = 0;
	y = r;
	d = r - 1;
	
	do
	{
		if( d < 0 )
		{
			y--;
			d += y + y;
		}
		
		x0 = cx - x;	x1 = cx + x;
		CIRCLE_SCANLINE( r - y )
		CIRCLE_SCANLINE( r + y )
		
		x0 = cx - y;	x1 = cx + y;
		CIRCLE_SCANLINE( r - x )
		CIRCLE_SCANLINE( r + x )
		
		d -= x + x + 1;
		x++;
	} while( x <= y );
}

// ---------------------------------------------------------------------------
//		¥ set_oval
// ---------------------------------------------------------------------------

void
region_t::set_oval(
	const rect& 		r )
{
	long				top = r.top;
	long				bottom = r.bottom;

	#define hline( x0, x1, y )												\
		if( y >= top && y <= bottom ) {										\
			link = &row_ptr[ y - top ];										\
			scanline = alloc_sl( heap );									\
 			scanline->next = 0;												\
			scanline->start = x0;											\
			scanline->end = x1;												\
			*link = scanline; }

	#define e_start( _x0, _x1, y )											\
		{ long x0 = _x0; long x1 = _x1;	hline( x0, x1, y );					\
		mx0 = x0; mx1 = x1; my0 = my1 = y; }

	#define e_yu()															\
		{ my0--; my1++; 													\
		hline( mx0, mx1, my0 ); hline( mx0, mx1, my1 ); }

	#define e_xd()															\
		{ mx0++; mx1--; }

	#define e_xdyu()														\
		{ mx0++; mx1--; my0--; my1++;										\
		hline( mx0, mx1, my0 ); hline( mx0, mx1, my1 ); }
		
	long mx0, mx1, my0, my1;
		
	long irx = ( r.right - r.left ) / 2;
	long iry = ( r.bottom - r.top ) / 2;
	long x = r.left + irx;
	long y = r.top + iry;
	long alpha, beta;
	long dy, dx;
	long alpha2, alpha4;
	long beta2, beta4;
	long alphadx, betady;
	long ddx, ddy, d;

	clear();

	if( bottom < top || r.right < r.left )
		return;
	
	m_bounds = r;

	scanline_t**		row_ptr = allocate( bottom - top + 1 );

  	// calculate scanlines.

 	fast_heap*			heap = s_scanline_heap;
	scanline_t*			scanline;
	scanline_t**		link;
  
	beta = (long) irx *(long) irx;
	alpha = (long) iry *(long) iry;

	if (alpha == 0L)
		alpha = 1L;
	if (beta == 0L)
		beta = 1L;

	dy = 0;
	dx = irx;
	alpha2 = alpha << 1;
	alpha4 = alpha2 << 1;
	beta2 = beta << 1;
	beta4 = beta2 << 1;
	alphadx = alpha * dx;
	betady = 0;
	ddx = alpha4 * (1 - dx);
	ddy = beta2 * 3;

	d = alpha2 * ((long) (dx - 1) * dx) + alpha + beta2 * (1 - alpha);
	e_start(x - dx, x + dx, y);

	do {
		if (d >= 0) {
		    d += ddx;
		    dx--;
		    alphadx -= alpha;
		    ddx += alpha4;
		    e_xdyu();
		} else
		    e_yu();
		d += ddy;
		dy++;
		betady += beta;
		ddy += beta4;
	} while (alphadx > betady);

	d = beta2 * ((long) dy * (dy + 1)) + alpha2 * ((long) dx * (dx - 2) + 1) 
		+ beta * (1 - alpha2);
	ddx = alpha2 * (3 - (dx << 1));
	ddy = beta4 * (1 + dy);

	do {
		if (d <= 0) {
		    d += ddy;
		    ddy += beta4;
		    dy++;
		    e_xdyu();
		} else
		    e_xd();
		d += ddx;
		ddx += alpha4;
		dx--;
	} while (dx > 0);

	#undef e_start
	#undef e_yu
	#undef e_xd
	#undef e_xdyu
	#undef hline
}

// ---------------------------------------------------------------------------
//		¥ set_polygon
// ---------------------------------------------------------------------------

void
region_t::set_polygon(
	const point*		points,
	long				count )
{
	polygon_rasterizer_t		rasterizer( *this );
	
	rasterizer.rasterize( count, points );
}

// ---------------------------------------------------------------------------
//		¥ set_pie
// ---------------------------------------------------------------------------

void
region_t::set_pie(
	const point&		center,
	unsigned long		r,
	float				alpha,
	float				beta )
{
	const float			pi2 = 2 * pi;
	const float			pihalf = 0.5 * pi;
	
	alpha = std::fmod( alpha, pi2 );
	beta = std::fmod( beta, pi2 );
	
	if( alpha > beta )
	{
		float			gamma;
	
		gamma = beta;
		beta = alpha;
		alpha = gamma;
	}
	
	int					qa, qb;
	int					qcnt;
	
	qa = alpha / pihalf;
	qb = beta / pihalf;
	qcnt = qb - qa;

	point				p[ 8 ];
	float				dr = ( 1.414213562 * r ) + 1;

	p[ 0 ] = center;

	p[ 1 ].h = center.h + std::cos( alpha ) * dr;
	p[ 1 ].v = center.v + std::sin( alpha ) * dr;
	
	for( int k = 0; k < qcnt; k++ )
	{
		float			gamma;
	
		gamma = ( k + 1 + qa ) * pihalf;
		p[ k + 2 ].h = center.h + std::cos( gamma ) * dr;
		p[ k + 2 ].v = center.v + std::sin( gamma ) * dr;		
	}

	p[ qcnt + 2 ].h = center.h + std::cos( beta ) * dr;
	p[ qcnt + 2 ].v = center.v + std::sin( beta ) * dr;
	
	region_t			r1, r2;
	
	r1.set_polygon( p, qcnt + 3 );
	r2.set_circle( center, r );
	
	intersect( r1, r2, *this );
}

// ---------------------------------------------------------------------------
//		¥ contains
// ---------------------------------------------------------------------------

bool
region_t::contains(
	const point&		p ) const
{
	scanline_t**		row_table;
	
	if( ( row_table = m_row_table ) == 0 )
		return false;

	const long			px = p.h;
	const long			py = p.v;
	const rect&			bounds = m_bounds;
	
	if( px < bounds.left || px > bounds.right ||
		py < bounds.top || py > bounds.bottom )
		return false;
		
	const scanline_t*	scanline;

	scanline = row_table[ py - m_bounds.top ];
	
	while( scanline )
	{
		const long		sx = scanline->start;

		if( sx > px )
			break;
			
		if( scanline->end >= px )
			return true;
		
		scanline = scanline->next;
	}
	
	return false;
}

// ---------------------------------------------------------------------------
//		¥ copy
// ---------------------------------------------------------------------------

void
region_t::copy(
	const region_t&		a,
	region_t&			r )
{	
	scanline_t**		a_row_ptr = a.m_row_table;

	if( !a_row_ptr )
	{
		r.clear();
		return;
	}

	long				ay0, ay1, ah;
	
	ay0 = a.m_bounds.top;
	ay1 = a.m_bounds.bottom;
	ah = ay1 - ay0;

	long				cy = ah + 1;
	
	scanline_t**		r_row_ptr = r.allocate( cy );

	const scanline_t*	a_scanline;
	scanline_t*			r_scanline;
	scanline_t**		r_link;
	
	a_scanline = *a_row_ptr++;
	r_link = r_row_ptr;

	fast_heap*			heap = s_scanline_heap;
		
	do {
		while( a_scanline )
		{
			COPY_SCANLINE_FAST( a_scanline );
			a_scanline = a_scanline->next;
		}
		STORE_NEW_ROW_FAST;
		a_scanline = *a_row_ptr++;
	} while( --cy );
	
	r.m_bounds = a.m_bounds;
}

// ---------------------------------------------------------------------------
//		¥ offset
// ---------------------------------------------------------------------------

void
region_t::offset(
	const region_t&		a,
	region_t&			r,
	long				dx,
	long				dy )
{	
	scanline_t**		a_row_ptr = a.m_row_table;

	if( !a_row_ptr )
	{
		r.clear();
		return;
	}

	long				ay0, ay1, ah;
	
	ay0 = a.m_bounds.top;
	ay1 = a.m_bounds.bottom;
	ah = ay1 - ay0;

	long				cy = ah + 1;
	
	scanline_t**		r_row_ptr = r.allocate( cy );

	const scanline_t*	a_scanline;
	scanline_t*			r_scanline;
	scanline_t**		r_link;
	
	a_scanline = *a_row_ptr++;
	r_link = r_row_ptr;

	fast_heap*			heap = s_scanline_heap;
		
	do {
		while( a_scanline )
		{
			r_scanline = alloc_sl( heap );
			r_scanline->start = a_scanline->start + dx;
			r_scanline->end = a_scanline->end + dx;
			*r_link = r_scanline;
			r_link = &r_scanline->next;

			a_scanline = a_scanline->next;
		}
		STORE_NEW_ROW_FAST;
		a_scanline = *a_row_ptr++;
	} while( --cy );
	
	r.m_bounds.left = a.m_bounds.left + dx;
	r.m_bounds.top = a.m_bounds.top + dy;
	r.m_bounds.right = a.m_bounds.right + dx;
	r.m_bounds.bottom = a.m_bounds.bottom + dy;
}

// ---------------------------------------------------------------------------
//		¥ intersect
// ---------------------------------------------------------------------------

void
region_t::intersect(
	const region_t&		a,
	const region_t&		b,
	region_t&			r )
{
	scanline_t**		a_row_ptr = a.m_row_table;
	scanline_t**		b_row_ptr = b.m_row_table;

	if( !a_row_ptr || !b_row_ptr )
	{
		r.clear();
		return;
	}

	long				ay0, ay1;
	long				by0, by1;
	long				ry0, ry1;
	
	ay0 = a.m_bounds.top;
	ay1 = a.m_bounds.bottom;	
	by0 = b.m_bounds.top;
	by1 = b.m_bounds.bottom;
	ry0 = ay0 > by0 ? ay0 : by0;
	ry1 = ay1 < by1 ? ay1 : by1;
	
	if( ry1 < ry0 )
	{
		// no intersection.
		r.clear();
		return;
	}
	
	{
		long			ax0, ax1;
		long			bx0, bx1;
		long			rx0, rx1;
	
		ax0 = a.m_bounds.left;
		ax1 = a.m_bounds.right;	
		bx0 = b.m_bounds.left;
		bx1 = b.m_bounds.right;
		rx0 = ax0 > bx0 ? ax0 : bx0;
		rx1 = ax1 < bx1 ? ax1 : bx1;
		
		if( rx1 < rx0 )
		{
			// no intersection.
			r.clear();
			return;
		}
	}

	long				cy = ry1 - ry0 + 1;

	fast_heap*			heap = s_scanline_heap;
	
	long				rx0 = 2147483647L;
	long				rx1 = -2147483647L - 1;
	long				last_rx1 = rx1;

	const scanline_t*	a_scanline;
	const scanline_t*	b_scanline;
	scanline_t*			r_scanline;
	scanline_t**		r_link;
	
	scanline_t**		r_row_ptr = r.allocate( cy );
	
	a_row_ptr += ry0 - ay0;
	b_row_ptr += ry0 - by0;

	a_scanline = *a_row_ptr++;
	b_scanline = *b_row_ptr++;
	
	r_link = r_row_ptr;
		
	long				ax0, ax1;
	long				bx0, bx1;
		
	// each row in the a region.
	while( true )
	{
		if( a_scanline )
		{
			if( !b_scanline )
				goto next_row;
		}
		else
			goto next_row;
			
		ax0 = a_scanline->start;
		ax1 = a_scanline->end;
	
		bx0 = b_scanline->start;
		bx1 = b_scanline->end;
			
		while( true )
		{
			if( bx1 < ax0 )
			{
				b_scanline = b_scanline->next;
				if( !b_scanline )
					break;
				bx0 = b_scanline->start;
				bx1 = b_scanline->end;
			}
			else if( bx0 > ax1 )
			{
				a_scanline = a_scanline->next;
				if( !a_scanline )
					break;
				ax0 = a_scanline->start;
				ax1 = a_scanline->end;
			}
			
			// scanlines intersect.
			
			else if( ax1 <= bx1 )
			{
				STORE_SCANLINE( ( ax0 > bx0 ? ax0 : bx0 ), ax1 );
				
				a_scanline = a_scanline->next;
				if( !a_scanline )
					break;
				ax0 = a_scanline->start;
				ax1 = a_scanline->end;
			}
			else
			{
				STORE_SCANLINE( ( ax0 > bx0 ? ax0 : bx0 ), bx1 );
				
				b_scanline = b_scanline->next;
				if( !b_scanline )
					break;
				bx0 = b_scanline->start;
				bx1 = b_scanline->end;
			}
		}
		
next_row:
		// store row.
		STORE_NEW_ROW;
		
		if( --cy == 0 )
			break;
		
		// get a's and b's next row.
		a_scanline = *a_row_ptr++;
		b_scanline = *b_row_ptr++;
	}
	
	if( rx1 < rx0 )
	{
		r.clear();
		return;
	}
	
	r.m_bounds.top = ry0;
	r.m_bounds.bottom = ry1;
	r.m_bounds.left = rx0;
	r.m_bounds.right = rx1;
	
	r.shrink_row_table();
}

// ---------------------------------------------------------------------------
//		¥ subtract
// ---------------------------------------------------------------------------

void
region_t::subtract(
	const region_t&		a,
	const region_t&		b,
	region_t&			r )
{
	scanline_t**		a_row_ptr = a.m_row_table;
	scanline_t**		b_row_ptr = b.m_row_table;

	if( !a_row_ptr )
	{
		r.clear();
		return;
	}
	if( !b_row_ptr )
	{
		copy( a, r );
		return;
	}

	long				ay0, ay1, ah;
	long				by0, by1, bh;
	
	ay0 = a.m_bounds.top;
	ay1 = a.m_bounds.bottom;
	ah = ay1 - ay0;
	
	by0 = b.m_bounds.top;
	by1 = b.m_bounds.bottom;
	bh = by1 - by0;

	if( ay0 > by1 || ay1 < by0 )
	{
		copy( a, r );
		return;
	}
	
	{
		long			ax0, ax1;
		long			bx0, bx1;
	
		ax0 = a.m_bounds.left;
		ax1 = a.m_bounds.right;	
		bx0 = b.m_bounds.left;
		bx1 = b.m_bounds.right;
				
		if( ax0 > bx1 || ax1 < bx0 )
		{
			copy( a, r );
			return;
		}
	}

	long				rx0 = 2147483647L;
	long				rx1 = -2147483647L - 1;
	long				last_rx1 = rx1;

	const scanline_t*	a_scanline;
	scanline_t*			r_scanline;
	scanline_t**		r_link;

	scanline_t**		r_row_ptr = r.allocate( ah + 1 );

	fast_heap*			heap = s_scanline_heap;

	a_scanline = *a_row_ptr++;
	r_link = r_row_ptr;
	
	long				cy, cby;
	
	if( ay0 < by0 )
	{
		long			cty = by0 - ay0;
		
		cy = ay1 - by0;
		if( bh < cy )
			cy = bh;
		cy++;
	
		cby = ah + 1 - cy - cty;
			
		// copy all a scanlines which are not affected
		// by the subtraction at the top.
		
		COPY_ROWS( a_scanline, a_row_ptr, cty );
	}
	else if( ay0 > by0 )
	{
		b_row_ptr += ay0 - by0;
		
		cy = by1 - ay0;
		if( ah < cy )
			cy = ah;
		cy++;		
		
		cby = ah + 1 - cy;
	}
	else
	{
		
		cy = ( ah < bh ? ah : bh ) + 1;		
		cby = ah + 1 - cy;
	}

	const scanline_t*	b_scanline;
	
	b_scanline = *b_row_ptr++;
		
	// each row in a.
	do
	{
		// each scanline in a.
		while( a_scanline )
		{			
			long		ax0, ax1;
			long		bx0, bx1;
			
			ax0 = a_scanline->start;
			ax1 = a_scanline->end;

			// work on one a scanline and test it against b scanlines.
			while( true )
			{
				// skip all b scanlines that lie left of a scanline.
				while( true )
				{
					if( !b_scanline )
					{
						// there are no b scanlines left for this row,
						// so store current scanline and all remaining ones.
						STORE_SCANLINE( ax0, ax1 );
						
						while( true )
						{
							a_scanline = a_scanline->next;
							if( !a_scanline )
							{
								*r_link = 0;
								b_scanline = *b_row_ptr++;
								goto next_row;
							}
							
							COPY_SCANLINE( a_scanline );
						}
					}
				
					bx0 = b_scanline->start;
					bx1 = b_scanline->end;
					
					if( bx1 >= ax0 )
						break;
						
					b_scanline = b_scanline->next;
				}

				// only scanlines right of current a scanline remain.
				if( bx0 > ax1 )
				{
					STORE_SCANLINE( ax0, ax1 );
					break;
				}
				
				// if we're here, some sort of intersection takes place
				// between the a and the b scanline.
				
				if( bx0 <= ax0 )
				{			
					if( bx1 >= ax1 )
					// case 1. source line is completely deleted.
						break;
				
					else
					// case 2. right part of source line remains.
					// (test the remaining part against next destination scanline).
					{
						ax0 = bx1 + 1;
						b_scanline = b_scanline->next;
					}
				}
				else if( bx1 >= ax1 )
				// case 3. left part of source line remains.
				{
					STORE_SCANLINE( ax0, bx0 - 1 );
					break;
				}
					
				else
				// case 4. source line is split into two parts.
				// (test the remaining right part against next destination scanline).
				{
					STORE_SCANLINE( ax0, bx0 - 1 );
					ax0 = bx1 + 1;
					b_scanline = b_scanline->next;
				}
			}
		
			a_scanline = a_scanline->next;
		}
		
		// if scanline for a is advancing one row further, we also have
		// to advance the scanline for b accordingly
		b_scanline = *b_row_ptr++;
		
next_row:
		// skip row marker.
		a_scanline = *a_row_ptr++;
		STORE_NEW_ROW;
		
	} while( --cy );

	if( cby )
	{
		// copy all a scanlines which are not affected
		// by the subtraction at the bottom.
		COPY_ROWS( a_scanline, a_row_ptr, cby );
	}

	if( rx1 < rx0 )
	{
		r.clear();
		return;
	}
	
	r.m_bounds.top = ay0;
	r.m_bounds.bottom = ay0 + ( r_row_ptr - r.m_row_table ) - 1;
	r.m_bounds.left = rx0;
	r.m_bounds.right = rx1;
	
	r.shrink_row_table();
}

// ---------------------------------------------------------------------------
//		¥ unite
// ---------------------------------------------------------------------------

void
region_t::unite(
	const region_t&		a,
	const region_t&		b,
	region_t&			r )
{
	scanline_t**		a_row_ptr = a.m_row_table;
	scanline_t**		b_row_ptr = b.m_row_table;
	
	if( !a_row_ptr && !b_row_ptr )
	{
		r.clear();
		return;
	}
	else if( !a_row_ptr )
	{
		copy( b, r );
		return;
	}
	else if( !b_row_ptr )
	{
		copy( a, r );
		return;
	}

	long				ay0, ay1;
	long				by0, by1;
	long				ry0, ry1;
	
	ay0 = a.m_bounds.top;
	ay1 = a.m_bounds.bottom;	
	by0 = b.m_bounds.top;
	by1 = b.m_bounds.bottom;
	ry0 = ay0 < by0 ? ay0 : by0;
	ry1 = ay1 > by1 ? ay1 : by1;
	
	fast_heap*			heap = s_scanline_heap;
	
	long				rx0 = 2147483647L;
	long				rx1 = -2147483647L - 1;
	long				last_rx1 = rx1;

	const scanline_t*	a_scanline;
	const scanline_t*	b_scanline;
	scanline_t*			r_scanline;
	scanline_t**		r_link;
	
	const long			rh = ry1 - ry0 + 1;
	
	scanline_t**		r_row_ptr = r.allocate( rh );

	a_scanline = *a_row_ptr++;
	b_scanline = *b_row_ptr++;
	r_link = r_row_ptr;

	if( ay1 < by0 )
	{
		long			cy;
	
		cy = ay1 - ay0 + 1;
		COPY_ROWS( a_scanline, a_row_ptr, cy );
		
		cy = by0 - ay1 - 1;
		if( cy )
		{
			do {
				STORE_NEW_ROW;
			} while( --cy );
		}
		
		cy = by1 - by0 + 1;
		COPY_ROWS( b_scanline, b_row_ptr, cy );
	
		goto done;
	}
	else if( by1 < ay0 )
	{
		long			cy;
	
		cy = by1 - by0 + 1;
		COPY_ROWS( b_scanline, b_row_ptr, cy );
		
		cy = ay0 - by1 - 1;
		if( cy )
		{
			do {
				STORE_NEW_ROW;
			} while( --cy );
		}
		
		cy = ay1 - ay0 + 1;
		COPY_ROWS( a_scanline, a_row_ptr, cy );
	
		goto done;
	}

	{
		const long		my1 = ay1 < by1 ? ay1 : by1;

		long			cy;

		if( ay0 < by0 )
		{		
			cy = my1 - by0 + 1;
				
			// copy all a scanlines which don't intersect
			// with the b scanlines at the top.
			long		cty = by0 - ay0;
			COPY_ROWS( a_scanline, a_row_ptr, cty );
		}
		else if( by0 < ay0 )
		{
			cy = my1 - ay0 + 1;
				
			// copy all b scanlines which don't intersect
			// with the a scanlines at the top.
			long		cty = ay0 - by0;
			COPY_ROWS( b_scanline, b_row_ptr, cty );
		}
		else
		{
			cy = my1 - ay0 + 1;
		}

		if( cy )
		{
			long		ax0, ax1;
			long		bx0, bx1;
		
			// each row in the intersection region.
			do {
				if( a_scanline )
				{
					if( !b_scanline )
					{
	complete_a:
						while( a_scanline )
						{
							COPY_SCANLINE( a_scanline );
							a_scanline = a_scanline->next;
						}
						
						goto next_row;
					}
				}
				else
				{
	complete_b:
					while( b_scanline )
					{
						COPY_SCANLINE( b_scanline );
						b_scanline = b_scanline->next;
					}
					
					goto next_row;
				}
					
				ax0 = a_scanline->start;
				ax1 = a_scanline->end;
			
				bx0 = b_scanline->start;
				bx1 = b_scanline->end;

				while( true )
				{
					if( bx0 > ax1 + 1 )
					{
						STORE_SCANLINE( ax0, ax1 );
						
						a_scanline = a_scanline->next;
						if( !a_scanline )
						{
							STORE_SCANLINE( bx0, bx1 );
							b_scanline = b_scanline->next;
							goto complete_b;
						}
						
						ax0 = a_scanline->start;
						ax1 = a_scanline->end;
					}
					else if( bx1 < ax0 - 1 )
					{
						STORE_SCANLINE( bx0, bx1 );
						
						b_scanline = b_scanline->next;
						if( !b_scanline )
						{
							STORE_SCANLINE( ax0, ax1 );
							a_scanline = a_scanline->next;
							goto complete_a;
						}
						
						bx0 = b_scanline->start;
						bx1 = b_scanline->end;
					}
					
					// scanlines intersect or touch each other.
					
					else if( bx1 >= ax1 )
					{
						if( ax0 < bx0 )
							bx0 = ax0;
					
						a_scanline = a_scanline->next;
						if( !a_scanline )
						{
							STORE_SCANLINE( bx0, bx1 );
							b_scanline = b_scanline->next;
							goto complete_b;
						}
						
						ax0 = a_scanline->start;
						ax1 = a_scanline->end;
					}
					else
					{
						if( bx0 < ax0 )
							ax0 = bx0;
					
						b_scanline = b_scanline->next;
						if( !b_scanline )
						{
							STORE_SCANLINE( ax0, ax1 );
							a_scanline = a_scanline->next;
							goto complete_a;
						}
						
						bx0 = b_scanline->start;
						bx1 = b_scanline->end;
					}
				}
			
	next_row:
				// store row.
				STORE_NEW_ROW;
					
				// get a's and b's next row.
				a_scanline = *a_row_ptr++;
				b_scanline = *b_row_ptr++;

			} while( --cy );
		}
		
		if( ay1 > my1 )
		{
			// copy all a scanlines which don't intersect
			// with the b scanlines at the bottom.

			long		cby = ay1 - my1;
			COPY_ROWS( a_scanline, a_row_ptr, cby );
		}
		else if( by1 > my1 )
		{
			// copy all b scanlines which don't intersect
			// with the a scanlines at the bottom.

			long		cby = by1 - my1;
			COPY_ROWS( b_scanline, b_row_ptr, cby );
		}
	}
	
done:

	r.m_bounds.top = ry0;
	r.m_bounds.bottom = ry1;
	r.m_bounds.left = rx0;
	r.m_bounds.right = rx1;
	
	r.shrink_row_table();
}

// ---------------------------------------------------------------------------
//		¥ exclusive_or
// ---------------------------------------------------------------------------

void
region_t::exclusive_or(
	const region_t&		a,
	const region_t&		b,
	region_t&			r )
{
	region_t			combined;
	region_t			common;

	unite( a, b, combined );
	intersect( a, b, common );
	subtract( combined, common, r );
}

// ---------------------------------------------------------------------------
//		¥ intersect
// ---------------------------------------------------------------------------

bool
region_t::intersect(
	const region_t&		a,
	const region_t&		b )
{
	scanline_t**		a_row_ptr = a.m_row_table;
	scanline_t**		b_row_ptr = b.m_row_table;

	if( !a_row_ptr || !b_row_ptr )
		return false;

	long				ay0, ay1;
	long				by0, by1;
	long				ry0, ry1;
	
	ay0 = a.m_bounds.top;
	ay1 = a.m_bounds.bottom;	
	by0 = b.m_bounds.top;
	by1 = b.m_bounds.bottom;
	ry0 = ay0 > by0 ? ay0 : by0;
	ry1 = ay1 < by1 ? ay1 : by1;
	
	if( ry1 < ry0 )
	{
		// no intersection.
		return false;
	}
	
	{
		long			ax0, ax1;
		long			bx0, bx1;
		long			rx0, rx1;
	
		ax0 = a.m_bounds.left;
		ax1 = a.m_bounds.right;	
		bx0 = b.m_bounds.left;
		bx1 = b.m_bounds.right;
		rx0 = ax0 > bx0 ? ax0 : bx0;
		rx1 = ax1 < bx1 ? ax1 : bx1;
		
		if( rx1 < rx0 )
		{
			// no intersection.
			return false;
		}
	}

	long				cy = ry1 - ry0 + 1;

	const scanline_t*	a_scanline;
	const scanline_t*	b_first_scanline;
	const scanline_t*	b_scanline;
	
	a_row_ptr += ry0 - ay0;
	b_row_ptr += ry0 - by0;

	a_scanline = *a_row_ptr++;
	b_first_scanline = *b_row_ptr++;
			
	long				ax0, ax1;
	long				bx0, bx1;
		
	// each row in the a region.
	while( true )
	{
		// each scanline in the a region.
		while( a_scanline )
		{
			ax0 = a_scanline->start;
			ax1 = a_scanline->end;

			// work on one a scanline and test it against all b scanlines.
			b_scanline = b_first_scanline;
			
			while( true )
			{
				// skip all b scanlines that lie left of a scanline.
				while( true )
				{
					if( !b_scanline )
						// there are no b scanlines left for this row.
						goto next_source_scanline;
				
					bx1 = b_scanline->end;
					
					if( bx1 >= ax0 )
						break;
							
					b_first_scanline = b_first_scanline->next;
					b_scanline = b_first_scanline;
				}

				bx0 = b_scanline->start;
			
				// only scanlines right of current a scanline remain.
				if( bx0 > ax1 )
					break;
				
				// if we're here, some sort of intersection takes place
				// between the a and the b scanline.
					
				return true;
			}
		
next_source_scanline:
			a_scanline = a_scanline->next;
		}
				
		if( --cy == 0 )
			break;
		
		// get a's and b's next row.
		a_scanline = *a_row_ptr++;
		b_first_scanline = *b_row_ptr++;
	}

	return false;
}

// ---------------------------------------------------------------------------
//		¥ intersect_rect
// ---------------------------------------------------------------------------

void
region_t::intersect_rect(
	const region_t&		a,
	const rect&			area,
	region_t&			r )
{
	scanline_t**		a_row_ptr = a.m_row_table;

	if( !a_row_ptr ||
		area.left > area.right ||
		area.top > area.bottom )
	{
		r.clear();
		return;
	}

	long				ay0, ay1;
	long				by0, by1;
	long				ry0, ry1;
	
	ay0 = a.m_bounds.top;
	ay1 = a.m_bounds.bottom;	
	by0 = area.top;
	by1 = area.bottom;
	ry0 = ay0 > by0 ? ay0 : by0;
	ry1 = ay1 < by1 ? ay1 : by1;
	
	if( ry1 < ry0 )
	{
		// no intersection.
		r.clear();
		return;
	}
	
	{
		long			ax0, ax1;
		long			bx0, bx1;
		long			rx0, rx1;
	
		ax0 = a.m_bounds.left;
		ax1 = a.m_bounds.right;	
		bx0 = area.left;
		bx1 = area.right;
		rx0 = ax0 > bx0 ? ax0 : bx0;
		rx1 = ax1 < bx1 ? ax1 : bx1;
		
		if( rx1 < rx0 )
		{
			// no intersection.
			r.clear();
			return;
		}
	}

	long				cy = ry1 - ry0 + 1;

	fast_heap*			heap = s_scanline_heap;
	
	long				rx0 = 2147483647L;
	long				rx1 = -2147483647L - 1;
	long				last_rx1 = rx1;

	const scanline_t*	a_scanline;
	scanline_t*			r_scanline;
	scanline_t**		r_link;
	
	scanline_t**		r_row_ptr = r.allocate( cy );
	
	a_row_ptr += ry0 - ay0;

	a_scanline = *a_row_ptr++;
	
	r_link = r_row_ptr;
		
	long				ax0, ax1;
	long				bx0, bx1;
	
	bx0 = area.left;
	bx1 = area.right;
		
	// each row in the a region.
	while( true )
	{
		if( not a_scanline )
			goto next_row;
			
		ax0 = a_scanline->start;
		ax1 = a_scanline->end;
				
		while( true )
		{
			if( bx1 < ax0 )
			{
				break;
			}
			else if( bx0 > ax1 )
			{
				a_scanline = a_scanline->next;
				if( !a_scanline )
					break;
				ax0 = a_scanline->start;
				ax1 = a_scanline->end;
			}
			
			// scanlines intersect.
			
			else if( ax1 <= bx1 )
			{
				STORE_SCANLINE( ( ax0 > bx0 ? ax0 : bx0 ), ax1 );
				
				a_scanline = a_scanline->next;
				if( !a_scanline )
					break;
				ax0 = a_scanline->start;
				ax1 = a_scanline->end;
			}
			else
			{
				STORE_SCANLINE( ( ax0 > bx0 ? ax0 : bx0 ), bx1 );
				
				break;
			}
		}
		
next_row:
		// store row.
		STORE_NEW_ROW;
		
		if( --cy == 0 )
			break;
		
		// get a's and b's next row.
		a_scanline = *a_row_ptr++;
	}
	
	if( rx1 < rx0 )
	{
		r.clear();
		return;
	}
	
	r.m_bounds.top = ry0;
	r.m_bounds.bottom = ry1;
	r.m_bounds.left = rx0;
	r.m_bounds.right = rx1;
	
	r.shrink_row_table();
}

// ---------------------------------------------------------------------------
//		¥ intersect_rect
// ---------------------------------------------------------------------------

bool
region_t::intersect_rect(
	const region_t&		a,
	const rect&			area )
{
	scanline_t**		a_row_ptr = a.m_row_table;

	if( !a_row_ptr ||
		area.left > area.right ||
		area.top > area.bottom )
		return false;

	long				ay0, ay1;
	long				by0, by1;
	long				ry0, ry1;
	
	ay0 = a.m_bounds.top;
	ay1 = a.m_bounds.bottom;	
	by0 = area.top;
	by1 = area.bottom;
	ry0 = ay0 > by0 ? ay0 : by0;
	ry1 = ay1 < by1 ? ay1 : by1;
	
	if( ry1 < ry0 )
	{
		// no intersection.
		return false;
	}
	
	{
		long			ax0, ax1;
		long			bx0, bx1;
		long			rx0, rx1;
	
		ax0 = a.m_bounds.left;
		ax1 = a.m_bounds.right;	
		bx0 = area.left;
		bx1 = area.right;
		rx0 = ax0 > bx0 ? ax0 : bx0;
		rx1 = ax1 < bx1 ? ax1 : bx1;
		
		if( rx1 < rx0 )
		{
			// no intersection.
			return false;
		}
	}

	long				cy = ry1 - ry0 + 1;

	const scanline_t*	a_scanline;
	
	a_row_ptr += ry0 - ay0;

	a_scanline = *a_row_ptr++;
			
	long				ax0, ax1;
	long				bx0, bx1;

	bx0 = area.left;
	bx1 = area.right;
		
	// each row in the a region.
	while( true )
	{
		// each scanline in the a region.
		while( a_scanline )
		{
			ax0 = a_scanline->start;
			ax1 = a_scanline->end;

			// work on one a scanline and test it against all b scanlines.			
			while( true )
			{
				// skip all b scanlines that lie left of a scanline.
				while( true )
				{
					if( bx1 >= ax0 )
						break;
							
					goto next_source_scanline;
				}

				// only scanlines right of current a scanline remain.
				if( bx0 > ax1 )
					break;
				
				// if we're here, some sort of intersection takes place
				// between the a and the b scanline.
					
				return true;
			}
		
next_source_scanline:
			a_scanline = a_scanline->next;
		}
				
		if( --cy == 0 )
			break;
		
		// get a's and b's next row.
		a_scanline = *a_row_ptr++;
	}

	return false;
}

// ---------------------------------------------------------------------------
//		¥ to_rects
// ---------------------------------------------------------------------------

long
region_t::to_rects(
	region_rects_t*		rects )
{
	scanline_t**		base_row_ptr = m_row_table;
	
	if( !base_row_ptr )
		return 0;

	const rect&			bounds = m_bounds;
	long				height = bounds.bottom - bounds.top + 1;
	long				top = bounds.top;
	
	if( !height )
		return 0;
				
	fast_heap*			heap = s_scanline_heap;
	long				rect_count = 0;
	long				rect_alloc = rects->alloc;
	rect*				rect_array = rects->array;
		
	while( true )
	{
		long			count = height;

		scanline_t**	row_ptr;
		scanline_t*		scanline;
		scanline_t**	link;

		row_ptr = base_row_ptr;
		scanline = *row_ptr++;
	
		while( !scanline )
		{
			if( --count == 0 )
				goto done;
				
			scanline = *row_ptr++;
			top++;
			height--;
		}

		base_row_ptr = row_ptr - 1;

		long			y0 = top;
		long			y1 = top;

		long			ax0, ax1;
	
		ax0 = scanline->start;
		ax1 = scanline->end;
		
		*base_row_ptr = scanline->next;
		free_sl( heap, scanline );
			
		do {
next_row:
			if( --count == 0 )
				break;

			link = row_ptr++;
			scanline = *link;
						
			while( scanline )
			{
				long	bx0 = scanline->start;
			
				if( bx0 > ax1 )
					break;
					
				long	bx1 = scanline->end;
			
				if( bx1 < ax0 )
					;
				
				else if( bx0 == ax0 )
				{
					if( bx1 == ax1 )
					{
						*link = scanline->next;
						free_sl( heap, scanline );
					
						y1++;
						goto next_row;
					}
					else if( bx1 > ax1 )
					{
						scanline->start = ax1 + 1;
						y1++;
						goto next_row;
					}
					else
						break;
				}
				else if( bx0 < ax0 )
				{
					if( bx1 == ax1 )
					{
						scanline->end = ax0 - 1;
						y1++;
						goto next_row;
					}
					else if( bx1 > ax1 )
					{
						scanline_t* sl = alloc_sl( heap );
			
						sl->start = ax1 + 1;
						sl->end = bx1;
						sl->next = scanline->next;
				
						scanline->end = ax0 - 1;
						scanline->next = sl;					
						scanline = sl;
					
						y1++;
						goto next_row;
					}
					else
						break;
				}
				else
					break;
				
				/*else if( bx0 >= ax0 && bx1 <= ax1 )
				{
					if( bx0 > ax0 || bx1 < ax1 )
						break;
				
					*link = scanline->next;
					heap->free( scanline );
					
					y1++;
					goto next_row;
				}
				else if( bx0 > ax0 || bx1 < ax1 )
					break;
				else if( bx0 < ax0 && bx1 == ax1 )
				{
					scanline->end = ax0 - 1;
					y1++;
					goto next_row;
				}
				else if( bx0 == ax0 && bx1 > ax1 )
				{
					scanline->start = ax1 + 1;
					y1++;
					goto next_row;
				}
				else
				{
					scanline_t* sl = (scanline_t*)heap->alloc();
				
					sl->start = ax1 + 1;
					sl->end = bx1;
					sl->next = scanline->next;
				
					scanline->end = ax0 - 1;
					scanline->next = sl;					
					scanline = sl;
					
					y1++;
					goto next_row;
				}*/
			
				link = &scanline->next;
				scanline = *link;
			}
		} while( false );
		
		/*rect			r;
		
		r.set( ax0, y0, ax1, y1 );
		process_rect( r, param );*/
		
		if( rect_count >= rect_alloc )
		{
			rect_alloc *= 2;
			rect_array = (rect*)irealloc(
				rect_array, rect_alloc * sizeof( rect ) );
			if( !rect_array )
				throw_icarus_error( "out of memory" );
		}
		
		rect*			r = &rect_array[ rect_count++ ];
		r->set( ax0, y0, ax1 + 1, y1 + 1 );
	}
	
done:
	clear();
	
	rects->alloc = rect_alloc;
	return rect_count;
}

// ---------------------------------------------------------------------------
//		¥ from_bitmap
// ---------------------------------------------------------------------------

void
region_t::from_bitmap(
	bitmap_t*			bitmap,
	u32					color,
	const rect&			r )
{
	const long			height = r.bottom - r.top + 1;

	if( !height )
	{
		clear();
		return;
	}

	scanline_t**		row_ptr = allocate( height );
  
  	fast_heap*			heap = s_scanline_heap;
  	
	grafport_t			port( bitmap );
	const long			left = r.left;
	const long			right = r.right;

	for( long y = r.top; y <= r.bottom; y++ )
	{
		scanline_t*		scanline;
		scanline_t**	link;
	
		link = row_ptr++;
	
		long			x = left;
	
		while( x <= right )
		{
			if( port.get_pixel( x, y ) != color )
			{
				scanline = alloc_sl( heap );
		 		scanline->next = 0;
		 		scanline->start = x;
		 		
		 		while( ++x <= right )
		 		{
					if( port.get_pixel( x, y ) == color )
						break;
		 		}
		 		
				scanline->end = x - 1;
				
				*link = scanline;
				link = &scanline->next;
			}
		
			x++;
		}
	}

	m_bounds = r;

	shrink_row_table();
}

// ---------------------------------------------------------------------------
//		¥ allocate
// ---------------------------------------------------------------------------

region_t::scanline_t**
region_t::allocate(
	long				height )
{
	clear();

	// we actually allocate one row more, because it makes some
	// operations easier to implement if we have an empty row at
	// the end we can read.

	scanline_t**		row_table;
	
	row_table = alloc_row_table( height + 1 );

	m_row_table_block = row_table;
	m_row_table_block_size = height + 1;
	
	m_row_table = row_table;
	m_row_table_size = height;
	
	long				count = height + 1;
	
	if( count )	
	{
		std::memset( row_table, 0,
			sizeof( scanline_t* ) * count );
	
		/*scanline_t**	entry = row_table;
	
		do {
			*entry++ = 0;
		} while( --count );*/
	}
	
	return row_table;
}

// ---------------------------------------------------------------------------
//		¥ import
// ---------------------------------------------------------------------------

void
region_t::import(
	long				dy,
	const long*			coords,
	long				count )
{
  	fast_heap*			heap = s_scanline_heap;
	scanline_t**		link;
	
	link = &m_row_table[ dy ];
	
	for( long i = 0; i < count; i += 2 )
	{
		scanline_t*		scanline;
	 		
	 	scanline = alloc_sl( heap );
	 	scanline->next = 0;
	 	scanline->start = coords[ i ];
		scanline->end = coords[ i + 1 ];
		
		*link = scanline;
		link = &scanline->next;
	}
}

// ---------------------------------------------------------------------------
//		¥ clear
// ---------------------------------------------------------------------------

void
region_t::clear()
{
	scanline_t**		row_table;

	if( ( row_table = m_row_table ) == 0 )
		return;

	unsigned long		height = m_row_table_size;
	unsigned long		count = height;
  	scanline_t**		row_ptr = row_table;
  	fast_heap*			heap = s_scanline_heap;
  
  	if( count )
  	{
	 	do
	 	{
	 		scanline_t*	scanline = *row_ptr++;
	 		
	 		while( scanline )
	 		{
	 			scanline_t* next = scanline->next;
	 			free_sl( heap, scanline );
	 			scanline = next;
	 		}
	 	} while( --count );
	}

	free_row_table(
		m_row_table_block, m_row_table_block_size );
	
	m_row_table = 0;
}

// ---------------------------------------------------------------------------
//		¥ to_mac_rgn
// ---------------------------------------------------------------------------

#if ICARUS_OS_MACOS
void
region_t::to_mac_rgn(
	RgnHandle			rgn ) const
{
	OpenRgn();

	scanline_t**		row_table;
	
	if( ( row_table = m_row_table ) == 0 )
	{
		SetEmptyRgn( rgn );
		return;
	}

	unsigned long		count = m_row_table_size;
	scanline_t**		row_ptr = row_table;
	long				py = m_bounds.top;

	if( count )
	{
		do
	 	{
	 		scanline_t*	scanline = *row_ptr++;
			Rect		r;

			r.top = py;
			r.bottom = py + 1;
	 		
			while( scanline )
			{			
				r.left = scanline->start;
				r.right = scanline->end + 1;
				FrameRect( &r );
				
				scanline = scanline->next;
			}
			
			py += 1;
		} while( --count );
	}
	
	CloseRgn( rgn );
}
#endif

// ---------------------------------------------------------------------------
//		¥ release_heaps
// ---------------------------------------------------------------------------

void
region_t::release_heaps()
{
	if( s_row_table_heap )
	{
		for( int i = 0; i < ROW_TABLE_HEAPS; i++ )
			if( s_row_table_heap[ i ] )
			{
				delete s_row_table_heap[ i ];
				s_row_table_heap[ i ] = 0;
			}

		delete[] s_row_table_heap;
		s_row_table_heap = 0;
	}

	if( s_scanline_heap )
	{
		delete s_scanline_heap;
		s_scanline_heap = 0;
	}
}
// ---------------------------------------------------------------------------
//		¥ shrink_row_table
// ---------------------------------------------------------------------------

void
region_t::shrink_row_table()
{
	scanline_t**		row_table;
	
	if( ( row_table = m_row_table ) == 0 )
		return;

	rect&				bounds = m_bounds;
	long				top = bounds.top;
	long				bottom = bounds.bottom;
	const long			height = bottom - top + 1;
	long				size = m_row_table_size;
	
	scanline_t**		entry;

	entry = row_table;
	
	for( long i = 0; i < height; i++ )
	{
		if( *entry )
		{
			m_row_table = entry;
			break;
		}
		entry++;
		top++;
		size--;
	}
	
	entry = &row_table[ height - 1 ];
	
	for( long i = 0; i < height; i++ )
	{
		if( *entry )
			break;

		entry--;
		bottom--;
		size--;
		if( bottom < top )
		{
			clear();
			return;
		}
	}
	
	bounds.top = top;
	bounds.bottom = bottom;
	
	m_row_table_size = size;
}

// ---------------------------------------------------------------------------

#define FLT2FIX64(flt,fix_lo,fix_hi)						\
	{ double h = std::floor( flt );							\
	fix_lo = ( flt - h ) * (double)( 4294967295U );			\
	fix_hi = h; }

#define FIX64_ADD(a_lo,a_hi,b_lo,b_hi)						\
	{	u32 new_fix_lo = a_lo + b_lo;						\
		if( new_fix_lo < a_lo ) a_hi++;						\
		a_lo = new_fix_lo;									\
		a_hi += b_hi; }

static u32				s_render_table_size = 0;
static u32*				s_render_table = 0;

static u32*
alloc_render_tables(
	long				width )
{
	long				size = ( width << 1 ) + 1;
	
	if( size > s_render_table_size )
	{
		long			new_size = 1;
		
		while( new_size < size )
			new_size <<= 1;
	
		if( s_render_table )
		{
			delete[] s_render_table;
			s_render_table = 0;
			s_render_table_size = 0;
		}
		
		s_render_table = new u32[ new_size ];
		s_render_table_size = new_size;
	}
	
	return s_render_table;
}

u32*
get_region_render_tables(
	u32					width,
	float				bx,
	float				fx )
{
	u32*				table = alloc_render_tables( width );

	s32*				bx_tab = (s32*)&table[ width ];
	s32*				bx_tab_ptr;
	u32					count = width;

	bx_tab_ptr = bx_tab;
	
	u32					bx_fix_lo, fx_fix_lo;
	s32					fx_fix_hi, bx_fix_hi;
	s32					last_bx_fix_hi;
	
	FLT2FIX64( bx, bx_fix_lo, bx_fix_hi )
	FLT2FIX64( fx, fx_fix_lo, fx_fix_hi )
	
	*( bx_tab_ptr++ ) = bx_fix_hi;
	last_bx_fix_hi = bx_fix_hi;
	
	if( count )
	{
		do
		{
			FIX64_ADD( bx_fix_lo, bx_fix_hi,
				fx_fix_lo, fx_fix_hi )
			
			if( bx_fix_hi == last_bx_fix_hi )
				return 0;
			
			*( bx_tab_ptr++ ) = bx_fix_hi;
			last_bx_fix_hi = bx_fix_hi;
		} while( --count );
	}
	
	return table;
}

END_ICARUS_NAMESPACE
