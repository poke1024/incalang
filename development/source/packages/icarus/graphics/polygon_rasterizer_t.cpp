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
//	polygon_rasterizer_t.cp	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "polygon_rasterizer_t.h"
#include "fast_heap.h"
#include "region_t.h"

BEGIN_ICARUS_NAMESPACE

struct edge_t {
    edge_t*			next;
    long			y_top;
    long			y_bot;
    long			x_now_whole;
    long			x_now_num;
    long			x_now_den;
    long			x_now_dir;
    long			x_now_num_step;

	void			step();
};

inline void
edge_t::step()
{
	long			num;
	long			den;
	long			dir;
	long			whole;

	num = x_now_num + x_now_num_step;
	den = x_now_den;
	dir = x_now_dir;
	whole = x_now_whole;
	
	while( num >= den )
	{
		whole += dir;
		num -= den;
	}

	x_now_num = num;
	x_now_whole = whole;
}

static fast_heap*		s_edge_heap = 0;

static edge_t**			s_edge_table = 0;
static long				s_edge_table_size = 0;

static long*			s_coords = 0;
static long				s_max_coords = 0;

polygon_rasterizer_t::polygon_rasterizer_t(
	region_t&			region ) :
	
	m_region( region )
{
	if( !s_edge_heap )
		s_edge_heap = new fast_heap( sizeof( edge_t ) );
	
	if( !s_max_coords )
	{
		s_max_coords = 64;
		s_coords = (long*)imalloc(
			sizeof( long ) * ( s_max_coords + 1 ) );
		if( !s_coords )
			throw_icarus_error( "out of memory" );
	}
}

void
polygon_rasterizer_t::rasterize(
	long				point_count,
	const point*		points )
{
	m_region.clear();

	if( point_count < 3 )
		return;
		
	long				miny;
	long				maxy;
	long				minx;
	long				maxx;
	
	minx = maxx = points[ 0 ].h;
	miny = maxy = points[ 0 ].v;	
		
	for( long i = 1; i < point_count; i++ )
	{
		long			x = points[ i ].h;

		if( x < minx )
			minx = x;
		else if( x > maxx )
			maxx = x;

		long			y = points[ i ].v;
		
		if( y < miny )
			miny = y;
		else if( y > maxy )
			maxy = y;
	}
	
	m_miny = miny;

	m_region.m_bounds.top = miny;
	m_region.m_bounds.bottom = maxy;
	m_region.m_bounds.left = minx;
	m_region.m_bounds.right = maxx;
		
	long				height = maxy - miny + 1;

	m_region.allocate( height );
	
	prepare_edge_table( height + 1 );
	
	fill_edges( point_count, points );

	edge_t**			edge_table = s_edge_table;
	
	long				cury;
	
	for( cury = 0; edge_table[ cury ] == 0; cury++ )
		if( cury == height - 1 )
			return;

	edge_t*				active = 0;

	while( ( active = update_active( active, cury ) ) != 0 )
	{
		draw_runs( active, cury );
		cury++;
	}
}

void
polygon_rasterizer_t::prepare_edge_table(
	long				n )
{
	if( n > s_edge_table_size )
	{
		if( s_edge_table )
			delete[] s_edge_table;
		s_edge_table = 0;
		
		if( !s_edge_table_size )
			s_edge_table_size = 8;
		
		while( n > s_edge_table_size )
			s_edge_table_size *= 2;
		
		s_edge_table = new edge_t*[ s_edge_table_size ];
	}
	
	std::memset( s_edge_table, 0, sizeof( edge_t* ) * n );
}

inline long
pmod(
	long				x,
	long				m )
{
	if( x >= m )
		return x - m;

	return x;
}

inline long
abs(
	long				x )
{
	if( x < 0 )
		return -x;
	return x;
}

inline long
sgn(
	long				x )
{
	if( x < 0 )
		return -1;
	return 1;
}

void
polygon_rasterizer_t::fill_edges(
	long				n,
	const point*		points )
{
	edge_t**			edge_table = s_edge_table;
	fast_heap*			heap = s_edge_heap;
	long				miny = m_miny;
	
	for( long i = 0; i < n; i++ )
	{
		const point*	p1;
		const point*	p2;
		const point*	p3;
		long			p1v;
		long			p2v;
			
		p1 = points + i;
		p2 = points + pmod( i + 1, n );
		
		p1v = p1->v;
		p2v = p2->v;
	
		if( p1v == p2v )
			continue;
			
		for( long j = pmod( i + 2, n ); ; j = pmod( j + 1, n ) )
		{
			p3 = points + j;
			if( p2v != p3->v )
				break;
		}
		
		long			p1h;
		long			p2h;
		
		p1h = p1->h;
		p2h = p2->h;
		
		edge_t*			e;

		e = (edge_t*)heap->alloc();
		e->x_now_num_step = abs( p1h - p2h );

		if( p2v > p1v )
		{
			e->y_top = p1v;
			e->y_bot = p2v;
			e->x_now_whole = p1h;
			e->x_now_dir = sgn( p2h - p1h );
			e->x_now_den = e->y_bot - e->y_top;
			e->x_now_num = e->x_now_den >> 1;
			if( p3->v > p2v )
				e->y_bot--;
		}
		else
		{
			e->y_top = p2v;
			e->y_bot = p1v;
			e->x_now_whole = p2h;
			e->x_now_dir = sgn( p1h - p2h );
			e->x_now_den = e->y_bot - e->y_top;
			e->x_now_num = e->x_now_den >> 1;
			if( p3->v < p2v )
			{
				e->y_top++;
				e->step();
			}
		}
		
		edge_t**		link = &edge_table[ e->y_top - miny ];
		
		e->next = *link;
		*link = e;
	}
}

edge_t*
polygon_rasterizer_t::update_active(
	edge_t*				active,
	long				cury )
{
	edge_t**			edge_table = s_edge_table;
	fast_heap*			heap = s_edge_heap;

	edge_t*				e;
	edge_t**			ep;
	long				y = cury + m_miny;
	
	for( ep = &active, e = *ep; e != 0; e = *ep )
	{
		if( e->y_bot < y )
		{
			*ep = e->next;
			heap->free( e );
		}
		else
			ep = &e->next;
	}
	
	*ep = edge_table[ cury ];
	return active;
}

static void
expand_xcoords()
{
	s_max_coords *= 2;
	
	s_coords = (long*)irealloc(
		s_coords,
		sizeof( long ) * ( s_max_coords + 1 ) );
		
	if( !s_coords )
		throw_icarus_error( "out of memory" );
}

void
polygon_rasterizer_t::draw_runs(
	edge_t*				active,
	long				cury )
{
	edge_t*				e;
	long				num_coords = 0;
	
	long*				xcoords = s_coords;
	long				max_coords = s_max_coords;

	for( e = active; e != 0; e = e->next )
	{	
		if( num_coords > max_coords )
		{
			expand_xcoords();
			xcoords = s_coords;
			max_coords = s_max_coords;
		}

		long			i;
		long			whole = e->x_now_whole;
	
		for( i = num_coords; i > 0 &&
			xcoords[ i - 1 ] > whole; i-- )
		{
			xcoords[ i ] = xcoords[ i - 1 ];
		}
		
		xcoords[ i ] = whole;
		num_coords++;
				
		e->step();
	}
	
	if( num_coords & 1 )
	{
		xcoords[ num_coords ] = xcoords[ num_coords - 1 ];
        num_coords++;
	}
	
	m_region.import( cury,
		xcoords, num_coords );
}

END_ICARUS_NAMESPACE
