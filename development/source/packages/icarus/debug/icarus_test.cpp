// ===========================================================================
//	icarus_test.cp		 	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus.h"
#include "region_t.h"
#include <cstdlib>

using namespace std;

BEGIN_ICARUS_NAMESPACE

static void rand_rect( rect& r )
{
	r.left = rand() % 1024;
	r.top = rand() % 1024;
	long width, height;
	width = 8 + ( rand() % 1024 );
	height = 8 + ( rand() % 1024 );
	r.right = r.left + width;
	r.bottom = r.top + height;
}

void test_regions(
	long			seed,
	long			iterations )
{
	region_t a, b;
	region_t c, d;
	rect r;
	
	srand( seed );
	
	FILE* fp;
	fp = fopen( "regiontest.log", "w" );
	
	for( long i = 0; i < iterations; i++ )
	{
		if( ( i % 50 ) == 0 )
		{
			a.clear();
			b.clear();
		}
		
		fprintf( fp, "#%ld\n", i );
	
		switch( rand() % 15 )
		{
			case 0:
			case 2:
			case 4:
				rand_rect( r );
				c.set_rect( r );
				fprintf( fp, "adding rect (%ld,%ld,%ld,%ld) to a\n",
					r.left, r.top, r.right, r.bottom );
				region_t::unite( a, c, d );
				region_t::copy( d, a );
				fprintf( fp, "done\n" );
				break;

			case 1:
			case 3:
			case 5:
				rand_rect( r );
				fprintf( fp, "adding rect (%ld,%ld,%ld,%ld) to b\n",
					r.left, r.top, r.right, r.bottom );
				c.set_rect( r );
				region_t::unite( b, c, d );
				region_t::copy( d, b );
				fprintf( fp, "done\n" );
				break;
				
			case 6:
			case 8:
				rand_rect( r );
				fprintf( fp, "subtracting rect (%ld,%ld,%ld,%ld) from a\n",
					r.left, r.top, r.right, r.bottom );
				c.set_rect( r );
				region_t::subtract( a, c, d );
				region_t::copy( d, a );
				fprintf( fp, "done\n" );
				break;
				
			case 7:
			case 9:
				rand_rect( r );
				fprintf( fp, "subtracting rect (%ld,%ld,%ld,%ld) from b\n",
					r.left, r.top, r.right, r.bottom );
				c.set_rect( r );
				region_t::subtract( b, c, d );
				region_t::copy( d, b );
				fprintf( fp, "done\n" );
				break;
				
			case 10:
			case 14:
				fprintf( fp, "uniting a and b\n" );
				region_t::unite( a, b, c );
				region_t::copy( c, a );
				fprintf( fp, "done\n" );
				break;

			case 11:
				fprintf( fp, "intersecting a and b\n" );
				region_t::intersect( a, b, c );
				region_t::copy( c, a );
				fprintf( fp, "done\n" );
				break;

			case 12:
				fprintf( fp, "subtracting a from b\n" );
				region_t::subtract( a, b, c );
				region_t::copy( c, a );
				fprintf( fp, "done\n" );
				break;

			case 13:
				fprintf( fp, "subtracting b from a\n" );
				region_t::subtract( b, a, c );
				region_t::copy( c, a );
				fprintf( fp, "done\n" );
				break;
		}
		
		fflush( fp );
	}
	
	fclose( fp );
}

END_ICARUS_NAMESPACE