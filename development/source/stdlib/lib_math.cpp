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
//	lib_math.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "vmutils.h"
#include "fiber_t.h"
#include "spline.h"
#include "intgcd.h"
#include "lib_point.h"
#include <math.h>

BEGIN_MACHINE_NAMESPACE

#include "combin.h"

//ICARUS_USING_STD_NAMESPACE

static const double	stab[ 51 ] = { 1e0,
		1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10,
		1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20,
		1e21, 1e22, 1e23, 1e24, 1e25, 1e26, 1e27, 1e28, 1e29, 1e30,
		1e31, 1e32, 1e33, 1e34, 1e35, 1e36, 1e37, 1e38, 1e39, 1e40,
		1e41, 1e42, 1e43, 1e44, 1e45, 1e46, 1e47, 1e48, 1e49, 1e50 };
		
// ---------------------------------------------------------------------------

template<class T>
inline T sgn( T val )
{
	T zero = 0;

	if( val < zero )
		return -1;
	else if( val > zero )
		return 1;
		
	return zero;
}

#if __VISC__
template<class T>
inline T trunc( T val )
{
	if( val < 0 )
		return ceil( val );
	return floor( val );
}
#endif

template<class T>
inline T toint( T val )
{
	if( val >= 0 )
		return trunc( val );
	
	return trunc( val ) - 1;
}

template<class T>
inline T fract( T val )
{
	return val - trunc( val );
}

template<class T>
inline T sqr( T val )
{
	return val * val;
}

template<class T>
inline T clamp( T a, T min, T max )
{
	if( a < min )
		return min;
	if( a > max )
		return max;
	return a;
}

template<class T>
inline T boxstep( T a, T b, T x )
{
	return clamp( ( x - a ) / ( b - a ), (T)0, (T)1 );
}

template<class T>
inline T smoothstep( T min, T max, T val )
{
	// smooth Hermite interpolation between 0 and 1
	if( val < min )
		return 0;
	if( val >= max )
		return 1;

	T x = ( val - min ) / ( max - min );
	return x * x * ( 3 - ( x + x ) );
}

template<class T>
inline T cosip( T x, T a, T b )
{
	T ft, f;
	
	ft = x * icarus::pi;
	f = ( 1 - cos( ft ) ) * 0.5;

	return  a * ( 1.0 - f ) + b * f;
}

template<class T>
inline T cubip( T x, T v0, T v1, T v2, T v3 )
{
	T P, Q, R, S;

	P = ( v3 - v2 ) - ( v0 - v1 );
    Q = ( v0 - v1 ) - P;
    R = v2 - v0;
    S = v1;

    return x * ( x * ( P * x + Q ) + R ) + S;
}

STDLIB_FUNCTION( math_roundn_double )
	double val = f.pop_double();
	int digits = f.pop_int();
	double shift;
	
	if( digits == 0 )
		val = iround( val );
	else if( digits >= 0 && digits < 51 )
	{
		shift = stab[ digits ];
		val = iround( val * shift ) / shift;
	}
	else if( digits > -51 && digits < 0 )
	{
		shift = stab[ -digits ];
		val = iround( val / shift ) * shift;
	}
	else
	{
		shift = pow( 10, digits );
		val = iround( val * shift ) / shift;
	}
	
	f.push_double( val );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( math_abs_double )
	f.push_double( iabs( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_abs_long )
	f.push_long( iabs( f.pop_long() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_abs_int )
	f.push_int( iabs( f.pop_int() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_sgn_double )
	f.push_double( sgn( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_sgn_long )
	f.push_long( sgn( f.pop_long() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_sgn_int )
	f.push_int( sgn( f.pop_int() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_exp )
	f.push_double( exp( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_log )
	f.push_double( log( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_log10 )
	f.push_double( log10( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_log_base )
	double value = f.pop_double();
	double base = f.pop_double();
	f.push_double( log( value ) / log( base ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_sin )
	f.push_double( sin( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_cos )
	f.push_double( cos( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_tan )
	f.push_double( tan( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_asin )
	f.push_double( asin( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_acos )
	f.push_double( acos( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_atan )
	f.push_double( atan( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_atan2 )
	double y = f.pop_double();
	double x = f.pop_double();
	f.push_double( atan2( y, x ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_floor )
	f.push_double( ifloor( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_ceil )
	f.push_double( iceil( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_sqrt )
	double x = f.pop_double();
	if( x < 0 )
		f.fatal_error( ERR_ARITHMETIC );
	f.push_double( isqrt( x ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_odd_long )
	f.push_bool( f.pop_long() & 1 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_odd_int )
	f.push_bool( f.pop_int() & 1 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_even_long )
	f.push_bool( ( f.pop_long() & 1 ) == 0 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_even_int )
	f.push_bool( ( f.pop_int() & 1 ) == 0 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_toint_double )
	f.push_double( toint( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_toint_float )
	f.push_float( toint( f.pop_float() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_trunc_double )
	f.push_double( trunc( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_trunc_float )
	f.push_float( trunc( f.pop_float() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_fract_double )
	f.push_double( fract( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_fract_float )
	f.push_float( fract( f.pop_float() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_round_double )
	f.push_double( iround( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_round_float )
	f.push_float( iround( f.pop_float() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_sqr_double )
	f.push_double( sqr( f.pop_double() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_sqr_float )
	f.push_float( sqr( f.pop_float() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_sqr_long )
	f.push_long( sqr( f.pop_long() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_sqr_int )
	f.push_int( sqr( f.pop_int() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_deg_double )
	f.push_double( f.pop_double() * ( 180 / icarus::pi ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_rad_double )
	f.push_double( f.pop_double() * ( icarus::pi / 180 ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_bclr_long )
	s64 val = f.pop_long();
	int bit = f.pop_int();
	f.push_long( val & ~( ( (u64)1 ) << bit ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_bset_long )
	s64 val = f.pop_long();
	int bit = f.pop_int();
	f.push_long( val | ( ( (u64)1 ) << bit ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_bchg_long )
	s64 val = f.pop_long();
	int bit = f.pop_int();
	f.push_long( val ^ ( ( (u64)1 ) << bit ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_btst_long )
	s64 val = f.pop_long();
	int bit = f.pop_int();
	f.push_bool( val & ( ( (u64)1 ) << bit ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_pow_float )
	float x = f.pop_float();
	float y = f.pop_float();
	f.push_float( pow( x, y ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_pow_double )
	double x = f.pop_double();
	double y = f.pop_double();
	f.push_double( pow( x, y ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_gcd_int )
	long x = f.pop_int();
	long y = f.pop_int();
	f.push_int( gcd( x, y ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_gcd_long )
	s64 x = f.pop_long();
	s64 y = f.pop_long();
	f.push_long( gcd( x, y ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_comb_int )
	long n = f.pop_int();
	long m = f.pop_int();
	f.push_long( ncomb3( n, m ) );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( math_min_int )
	long a = f.pop_int();
	long b = f.pop_int();
	f.push_int( imin( a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_min_long )
	s64 a = f.pop_long();
	s64 b = f.pop_long();
	f.push_long( imin( a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_min_float )
	float a = f.pop_float();
	float b = f.pop_float();
	f.push_float( imin( a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_min_double )
	double a = f.pop_double();
	double b = f.pop_double();
	f.push_double( imin( a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_max_int )
	long a = f.pop_int();
	long b = f.pop_int();
	f.push_int( imax( a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_max_long )
	s64 a = f.pop_long();
	s64 b = f.pop_long();
	f.push_long( imax( a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_max_float )
	float a = f.pop_float();
	float b = f.pop_float();
	f.push_float( imax( a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_max_double )
	double a = f.pop_double();
	double b = f.pop_double();
	f.push_double( imax( a, b ) );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( math_clamp_double )
	double a = f.pop_double();
	double min = f.pop_double();
	double max = f.pop_double();
	f.push_double( clamp( a, min, max ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_clamp_float )
	float a = f.pop_float();
	float min = f.pop_float();
	float max = f.pop_float();
	f.push_float( clamp( a, min, max ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_clamp_int )
	long a = f.pop_int();
	long min = f.pop_int();
	long max = f.pop_int();
	f.push_int( clamp( a, min, max ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_boxstep_double )
	double a = f.pop_double();
	double b = f.pop_double();
	double x = f.pop_double();
	f.push_double( boxstep( a, b, x ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_boxstep_float )
	float a = f.pop_float();
	float b = f.pop_float();
	float x = f.pop_float();
	f.push_float( boxstep( a, b, x ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_smoothstep_double )
	double min = f.pop_double();
	double max = f.pop_double();
	double val = f.pop_double();
	f.push_double( smoothstep( min, max, val ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_smoothstep_float )
	float min = f.pop_float();
	float max = f.pop_float();
	float val = f.pop_float();
	f.push_float( smoothstep( min, max, val ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_lerp_double )
	double t = f.pop_double();
	double x0 = f.pop_double();
	double x1 = f.pop_double();
	f.push_double( x0 + t * ( x1 - x0 ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_lerp_float )
	float t = f.pop_float();
	float x0 = f.pop_float();
	float x1 = f.pop_float();
	f.push_float( x0 + t * ( x1 - x0 ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_cosip_double )
	double x = f.pop_double();
	double a = f.pop_double();
	double b = f.pop_double();
	f.push_double( cosip( x, a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_cosip_float )
	float x = f.pop_float();
	float a = f.pop_float();
	float b = f.pop_float();
	f.push_float( cosip( x, a, b ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_cubip_double )
	double x = f.pop_double();
	double v0 = f.pop_double();
	double v1 = f.pop_double();
	double v2 = f.pop_double();
	double v3 = f.pop_double();
	f.push_double( cubip( x, v0, v1, v2, v3 ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_cubip_float )
	float x = f.pop_float();
	float v0 = f.pop_float();
	float v1 = f.pop_float();
	float v2 = f.pop_float();
	float v3 = f.pop_float();
	f.push_float( cubip( x, v0, v1, v2, v3 ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_spline_float )
	float				x = f.pop_float();
	long				nknots = f.pop_int();
	const float*		knot = (const float*)f.pop_ptr();
	f.check_access( knot, nknots * sizeof( float ) );
	f.push_float( spline( x, nknots, knot ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_spline_point )
	float				x = f.pop_float();
	long				nknots = f.pop_int();
	const void*			knot = (const void*)f.pop_ptr();

	machine_t&			m = *f.machine();
	u08					block[ POINT_CLASS_SIZE ];
	INIT_POINT( block, m );
	point_t*			r = INSTANCE_SELF( point_t, block );

	f.check_access( knot, nknots * POINT_CLASS_SIZE );
	
	point_t*			p = INSTANCE_SELF( point_t, knot );
	r->x = spline( x, nknots, &p->x, POINT_CLASS_SIZE );
	r->y = spline( x, nknots, &p->y, POINT_CLASS_SIZE );
	r->z = spline( x, nknots, &p->z, POINT_CLASS_SIZE );
	
	f.push_block( block, POINT_CLASS_SIZE );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

inline u64 rol( u64 val, int bits, long amount )
{
	if( amount < 0 )
	{
		amount = -amount;
		amount &= bits - 1;
		return ( val >> amount ) | ( val << ( bits - amount ) );
	}
	amount &= bits - 1;
	return ( val << amount ) | ( val >> ( bits - amount ) );
}

inline u64 ror( u64 val, int bits, long amount )
{
	if( amount < 0 )
	{
		amount = -amount;
		amount &= bits - 1;
		return ( val << amount ) | ( val >> ( bits - amount ) );
	}
	amount &= bits - 1;
	return ( val >> amount ) | ( val << ( bits - amount ) );
}

STDLIB_FUNCTION( math_rol_byte )
	u08 a = f.pop_byte();
	long amount = f.pop_int();
	f.push_byte( rol( a, 8, amount ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_ror_byte )
	u08 a = f.pop_byte();
	long amount = f.pop_int();
	f.push_byte( ror( a, 8, amount ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_rol_short )
	u16 a = f.pop_short();
	long amount = f.pop_int();
	f.push_short( rol( a, 16, amount ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_ror_short )
	u16 a = f.pop_short();
	long amount = f.pop_int();
	f.push_short( ror( a, 16, amount ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_rol_int )
	u32 a = f.pop_int();
	long amount = f.pop_int();
	f.push_int( rol( a, 32, amount ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_ror_int )
	u32 a = f.pop_int();
	long amount = f.pop_int();
	f.push_int( ror( a, 32, amount ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_rol_long )
	u64 a = f.pop_long();
	long amount = f.pop_int();
	f.push_long( rol( a, 64, amount ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( math_ror_long )
	u64 a = f.pop_long();
	long amount = f.pop_int();
	f.push_long( ror( a, 64, amount ) );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

/*
native double abs( double );
native long abs( long );
native int abs( int );
native double sgn( double );
native long sgn( long );
native int sgn( int );
native double exp( double );
native double log( double );
native double log10( double );
native double log( double, double );
native double sin( double );
native double cos( double );
native double tan( double );
native double asin( double );
native double acos( double );
native double atan( double );
native double atan2( double, double );
native double floor( double );
native double ceil( double );
native double sqrt( double );
native bool odd( long );
native bool odd( int );
native bool even( long );
native bool even( int );
native double toint( double );
native float toint( float );
native double trunc( double );
native float trunc( float );
native double fract( double );
native float fract( float );
native double round( double );
native float round( float );
native double round( double, int );
native double sqr( double );
native float sqr( float );
native long sqr( long );
native int sqr( int );
native double deg( double );
native double rad( double );
native long bclr( long, int );
native long bset( long, int );
native long bchg( long, int );
native bool btst( long, int );
native float pow( float, float );
native double pow( double, double );
native int gcd( int, int );
native long gcd( long, long );
native unsigned long comb( int, int );
native int  min( int, int );
native long  min( long, long );
native float  min( float, float );
native double  min( double, double );
native int  max( int, int );
native long  max( long, long );
native float  max( float, float );
native double  max( double, double );

native double clamp( double, double, double );
native float clamp( float, float, float );
native int clamp( int, int, int );
native double boxstep( double, double, double );
native float boxstep( float, float, float );
native double smoothstep( double, double, double );
native float smoothstep( float, float, float );
native double lerp( double, double, double );
native float lerp( float, float, float );
native double cosip( double, double, double );
native float cosip( float, float, float );
native double cubip( double, double, double, double, double );
native float cubip( float, float, float, float, float );
native float spline( float, int, const float* );
native Point spline( float, int, const Point* );

native byte rol( byte, int );
native byte ror( byte, int );
native short rol( short, int );
native short ror( short, int );
native int rol( int, int );
native int ror( int, int );
native long rol( long, int );
native long ror( long, int );
*/

void
init_math(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function( m, rootclss, "abs;d", math_abs_double, 0 );
	register_native_function( m, rootclss, "abs;l", math_abs_long, 0 );
	register_native_function( m, rootclss, "abs;i", math_abs_int, 0 );
	register_native_function( m, rootclss, "sgn;d", math_sgn_double );
	register_native_function( m, rootclss, "sgn;l", math_sgn_long );
	register_native_function( m, rootclss, "sgn;i", math_sgn_int );
	register_native_function( m, rootclss, "exp;d", math_exp, 0 );
	register_native_function( m, rootclss, "log;d", math_log, 0 );
	register_native_function( m, rootclss, "log10;d", math_log10, 0 );
	register_native_function( m, rootclss, "log;d;d", math_log_base, 0 );
	register_native_function( m, rootclss, "sin;d", math_sin, 0 );
	register_native_function( m, rootclss, "cos;d", math_cos, 0 );
	register_native_function( m, rootclss, "tan;d", math_tan, 0 );
	register_native_function( m, rootclss, "asin;d", math_asin, 0 );
	register_native_function( m, rootclss, "acos;d", math_acos, 0 );
	register_native_function( m, rootclss, "atan;d", math_atan, 0 );
	register_native_function( m, rootclss, "atan2;d;d", math_atan2, 0 );
	register_native_function( m, rootclss, "floor;d", math_floor, 0 );
	register_native_function( m, rootclss, "ceil;d", math_ceil, 0 );
	register_native_function( m, rootclss, "sqrt;d", math_sqrt );
	register_native_function( m, rootclss, "odd;l", math_odd_long, 0 );
	register_native_function( m, rootclss, "odd;i", math_odd_int, 0 );
	register_native_function( m, rootclss, "even;l", math_even_long, 0 );
	register_native_function( m, rootclss, "even;i", math_even_int, 0 );
	register_native_function( m, rootclss, "toint;d", math_toint_double, 0 );
	register_native_function( m, rootclss, "toint;f", math_toint_float, 0 );
	register_native_function( m, rootclss, "trunc;d", math_trunc_double, 0 );
	register_native_function( m, rootclss, "trunc;f", math_trunc_float, 0 );
	register_native_function( m, rootclss, "fract;d", math_fract_double, 0 );
	register_native_function( m, rootclss, "fract;f", math_fract_float, 0 );
	register_native_function( m, rootclss, "round;d", math_round_double, 0 );
	register_native_function( m, rootclss, "round;f", math_round_float, 0 );
	register_native_function( m, rootclss, "round;d;i", math_roundn_double, 0 );
	register_native_function( m, rootclss, "sqr;d", math_sqr_double, 0 );
	register_native_function( m, rootclss, "sqr;f", math_sqr_float, 0 );
	register_native_function( m, rootclss, "sqr;l", math_sqr_long, 0 );
	register_native_function( m, rootclss, "sqr;i", math_sqr_int, 0 );
	register_native_function( m, rootclss, "deg;d", math_deg_double, 0 );
	register_native_function( m, rootclss, "rad;d", math_rad_double, 0 );
	register_native_function( m, rootclss, "bclr;l;i", math_bclr_long, 0 );
	register_native_function( m, rootclss, "bset;l;i", math_bset_long, 0 );
	register_native_function( m, rootclss, "bchg;l;i", math_bchg_long, 0 );
	register_native_function( m, rootclss, "btst;l;i", math_btst_long, 0 );
	register_native_function( m, rootclss, "pow;f;f", math_pow_float, 0 );
	register_native_function( m, rootclss, "pow;d;d", math_pow_double, 0 );
	register_native_function( m, rootclss, "gcd;i;i", math_gcd_int, 0 );
	register_native_function( m, rootclss, "gcd;l;l", math_gcd_long, 0 );
	register_native_function( m, rootclss, "comb;i;i", math_comb_int, 0 );
	
	register_native_function( m, rootclss, "min;i;i", math_min_int, 0 );
	register_native_function( m, rootclss, "min;l;l", math_min_long, 0 );
	register_native_function( m, rootclss, "min;f;f", math_min_float, 0 );
	register_native_function( m, rootclss, "min;d;d", math_min_double, 0 );
	register_native_function( m, rootclss, "max;i;i", math_max_int, 0 );
	register_native_function( m, rootclss, "max;l;l", math_max_long, 0 );
	register_native_function( m, rootclss, "max;f;f", math_max_float, 0 );
	register_native_function( m, rootclss, "max;d;d", math_max_double, 0 );
	
	register_native_function( m, rootclss,
		"clamp;d;d;d", math_clamp_double, 0 );
	register_native_function( m, rootclss,
		"clamp;f;f;f", math_clamp_float, 0 );
	register_native_function( m, rootclss,
		"clamp;i;i;i", math_clamp_int, 0 );
	register_native_function( m, rootclss,
		"boxstep;d;d;d", math_boxstep_double, 0 );
	register_native_function( m, rootclss,
		"boxstep;f;f;f", math_boxstep_float, 0 );
	register_native_function( m, rootclss,
		"smoothstep;d;d;d", math_smoothstep_double, 0 );
	register_native_function( m, rootclss,
		"smoothstep;f;f;f", math_smoothstep_float, 0 );
	register_native_function( m, rootclss,
		"lerp;d;d;d", math_lerp_double, 0 );
	register_native_function( m, rootclss,
		"lerp;f;f;f", math_lerp_float, 0 );
	register_native_function( m, rootclss,
		"cosip;d;d;d", math_cosip_double, 0 );
	register_native_function( m, rootclss,
		"cosip;f;f;f", math_cosip_float, 0 );
	register_native_function( m, rootclss,
		"cubip;d;d;d;d;d", math_cubip_double, 0 );
	register_native_function( m, rootclss,
		"cubip;f;f;f;f;f", math_cubip_float, 0 );
	register_native_function( m, rootclss,
		"spline;f;i;.f*", math_spline_float );
	register_native_function( m, rootclss,
		"spline;f;i;.Point::*", math_spline_point );

	register_native_function( m, rootclss,
		"rol;B;i", math_rol_byte );
	register_native_function( m, rootclss,
		"ror;B;i", math_ror_byte );
	register_native_function( m, rootclss,
		"rol;s;i", math_rol_short );
	register_native_function( m, rootclss,
		"ror;s;i", math_ror_short );
	register_native_function( m, rootclss,
		"rol;i;i", math_rol_int );
	register_native_function( m, rootclss,
		"ror;i;i", math_ror_int );
	register_native_function( m, rootclss,
		"rol;l;i", math_rol_long );
	register_native_function( m, rootclss,
		"ror;l;i", math_ror_long );
}

END_MACHINE_NAMESPACE
