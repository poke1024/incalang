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
//	spline.cp
// ===========================================================================

#include "spline.h"

BEGIN_MACHINE_NAMESPACE

inline float clamp( float a, float min, float max )
	{ if( a < min ) return min; if( a > max ) return max; return a; }

// coefficients of basis matrix for the Catmull-Rom interpolation
#define CR00     -0.5
#define CR01      1.5
#define CR02     -1.5
#define CR03      0.5
#define CR10      1.0
#define CR11     -2.5
#define CR12      2.0
#define CR13     -0.5
#define CR20     -0.5
#define CR21      0.0
#define CR22      0.5
#define CR23      0.0
#define CR30      0.0
#define CR31      1.0
#define CR32      0.0
#define CR33      0.0

float
spline( float x, long nknots, const float *knot )
{
    long span;
    long nspans = nknots - 3;
    float c0, c1, c2, c3;	// coefficients of the cubic.

    if( nspans < 1 )
    	throw_icarus_error( "spline needs four or more control points" );

    // find the appropriate 4-point span of the spline
    x = clamp( x, 0.f, 1.f ) * nspans;
    span = static_cast<long>( x );
    if( span >= nknots - 3 )
        span = nknots - 3;
    x -= span;
    knot += span;

    // evaluate the span cubic at x using Horner's rule
    float k0, k1, k2, k3;
    
	k0 = knot[ 0 ];	k1 = knot[ 1 ];
	k2 = knot[ 2 ];	k3 = knot[ 3 ];
    
	c3 = CR00 * k0 + CR01 * k1 + CR02 * k2 + CR03 * k3;
	c2 = CR10 * k0 + CR11 * k1 + CR12 * k2 + CR13 * k3;
	c1 = CR20 * k0 + CR21 * k1 + CR22 * k2 + CR23 * k3;
	c0 = CR30 * k0 + CR31 * k1 + CR32 * k2 + CR33 * k3;

	return ( ( c3 * x + c2 ) * x + c1 ) * x + c0;
}

inline const float*
offset( const float *p, long amount )
{
	const u08* q = (const u08*)p;
	q += amount;
	return (const float*)q;
}

float
spline( float x, long nknots, const float *knot, long spacing )
{
    long span;
    long nspans = nknots - 3;
    float c0, c1, c2, c3;	// coefficients of the cubic

    if( nspans < 1 )
    	throw_icarus_error( "spline needs four or more control points" );

    // find the appropriate 4-point span of the spline
    x = clamp( x, 0.f, 1.f ) * nspans;
    span = static_cast<long>( x );
    if( span >= nknots - 3 )
        span = nknots - 3;
    x -= span;
    knot = offset( knot, span * spacing );

    // evaluate the span cubic at x using Horner's rule
    float k0, k1, k2, k3;
    
    k0 = *knot;
    knot = offset( knot, spacing );
    k1 = *knot;
    knot = offset( knot, spacing );
	k2 = *knot;
    knot = offset( knot, spacing );
	k3 = *knot;
        
	c3 = CR00 * k0 + CR01 * k1 + CR02 * k2 + CR03 * k3;
	c2 = CR10 * k0 + CR11 * k1 + CR12 * k2 + CR13 * k3;
	c1 = CR20 * k0 + CR21 * k1 + CR22 * k2 + CR23 * k3;
	c0 = CR30 * k0 + CR31 * k1 + CR32 * k2 + CR33 * k3;

	return ( ( c3 * x + c2 ) * x + c1 ) * x + c0;
}

END_MACHINE_NAMESPACE
