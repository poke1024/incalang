// ===========================================================================
//	icarus_stdfunc.h		   ©2000-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_STDFUNC_H
#define ICARUS_STDFUNC_H

//#pragma once

BEGIN_ICARUS_NAMESPACE

template<class T> T imin( T x, T y )
{
	if( x < y )
		return x;
	return y;
}

template<class T> T imax( T x, T y )
{
	if( x > y )
		return x;
	return y;
}

template<class T> void iswap( T& x, T& y )
{
	T z = x;
	x = y;
	y = z;
}

template<class T>T iabs( T x )
{
	if( x < 0 )
		return -x;
	return x;
}

#define ifmod fmod
#define iisalnum isalnum
#define iisalpha isalpha
#define iisdigit isdigit
#define iisspace isspace
#define imemcpy memcpy
#define imemcmp memcmp
#define imemmove memmove
#define imemset memset
#define istrcmp strcmp
#define istrcpy strcpy
#define istrlen strlen
#define itolower tolower
#define itoupper toupper
#define istrncmp strncmp
#define istrtod strtod
#define isqrt sqrt
#define iinvsqrt(x) (1.0/sqrt(x))
#define iround round
#define ifloor floor
#define iceil ceil

#if __VISC__
float roundf( float );
double round( double );
#define isnprintf _snprintf
#define ivsnprintf _vsnprintf
#define strncasecmp _strnicmp
#define istrtoll _strtoi64
#elif PROJECT_BUILDER
#define isnprintf snprintf
#define istrtoll strtoq
#else
#define isnprintf snprintf
#define ivsnprintf vsnprintf
#define istrtoll strtoll
#endif

END_ICARUS_NAMESPACE

#endif

