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
//	lib_random.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "os_core.h"

#include <stdio.h>
#include <stdlib.h>

BEGIN_MACHINE_NAMESPACE

static void rmarin( long ij, long kl );
float ranmar();
u32 ranint();
static bool s_rand_init = false;

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( random_randomize )
	u64 val = f.pop_long();
	
	unsigned long ij = val >> 32;
	unsigned long kl = val;
	
	rmarin(
		ij % 31328,
		kl % 30081 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( random_rnd )
	f.push_float( ranmar() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( random_random )
	u32 limit = f.pop_int();
	f.push_int( ranint() % limit );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_random(
	machine_t&			m )
{
	if( not s_rand_init )
	{
		rmarin( os_time_millis() % 31328, 9373 );
		//rmarin( 1802, 9373 );
		s_rand_init = true;
	}

	class_t*			rootclss = m.get_root_class();

	register_native_function( m, rootclss, "randomize;l", random_randomize );
	register_native_function( m, rootclss, "rnd", random_rnd );
	register_native_function( m, rootclss, "random;i", random_random );
}

// ---------------------------------------------------------------------------

/*
C This random number generator originally appeared in "Toward a Universal 
C Random Number Generator" by George Marsaglia and Arif Zaman. 
C Florida State University Report: FSU-SCRI-87-50 (1987)
C 
C It was later modified by F. James and published in "A Review of Pseudo-
C random Number Generators" 
C 
C THIS IS THE BEST KNOWN RANDOM NUMBER GENERATOR AVAILABLE.
C       (However, a newly discovered technique can yield 
C         a period of 10^600. But that is still in the development stage.)
C
C It passes ALL of the tests for random number generators and has a period 
C   of 2^144, is completely portable (gives bit identical results on all 
C   machines with at least 24-bit mantissas in the floating point 
C   representation). 
C 
C The algorithm is a combination of a Fibonacci sequence (with lags of 97
C   and 33, and operation "subtraction plus one, modulo one") and an 
C   "arithmetic sequence" (using subtraction).
C======================================================================== 
This C language version was written by Jim Butler, and was based on a
FORTRAN program posted by David LaSalle of Florida State University.
*/

static float u[98], c, cd, cm;
static int i97, j97;

static void rmarin( long ij, long kl )
{
/*
C This is the initialization routine for the random number generator RANMAR()
C NOTE: The seed variables can have values between:    0 <= IJ <= 31328
C                                                      0 <= KL <= 30081
C The random number sequences created by these two seeds are of sufficient 
C length to complete an entire calculation with. For example, if sveral 
C different groups are working on different parts of the same calculation,
C each group could be assigned its own IJ seed. This would leave each group
C with 30000 choices for the second seed. That is to say, this random 
C number generator can create 900 million different subsequences -- with 
C each subsequence having a length of approximately 10^30.
C 
C Use IJ = 1802 & KL = 9373 to test the random number generator. The
C subroutine RANMAR should be used to generate 20000 random numbers.
C Then display the next six random numbers generated multiplied by 4096*4096
C If the random number generator is working properly, the random numbers
C should be:
C           6533892.0  14220222.0  7275067.0
C           6172232.0  8354498.0   10633180.0
*/
	long i, j, k, l, ii, jj, m;
	float s, t;
	
	/*if (ij<0 || ij>31328 || kl<0 || kl>30081) {
		puts("The first random number seed must have a value between 0 and 31328.");
		puts("The second seed must have a value between 0 and 30081.");
		exit(1);
	}*/
	
	i = (ij/177)%177 + 2;
	j = ij%177 + 2;
	k = (kl/169)%178 + 1;
	l = kl%169;
	
	for (ii=1; ii<=97; ii++) {
		s = 0.0;
		t = 0.5;
		for (jj=1; jj<=24; jj++) {
			m = (((i*j)%179)*k) % 179;
			i = j;
			j = k;
			k = m;
			l = (53*l + 1) % 169;
			if ((l*m)%64 >= 32) s += t;
			t *= 0.5;
		}
		u[ii] = s;
	}
	
	c = 362436.0 / 16777216.0;
	cd = 7654321.0 / 16777216.0;
	cm = 16777213.0 / 16777216.0;
	
	i97 = 97;
	j97 = 33;
	
	/*test = TRUE;*/
}

float ranmar()
{
	float uni;
	
	uni = u[i97] - u[j97];
	if (uni < 0.0) uni += 1.0;
	u[i97] = uni;
	i97--;
	if (i97==0) i97 = 97;
	j97--;
	if (j97==0) j97 = 97;
	c -= cd;
	if (c<0.0) c += cm;
	uni -= c;
	if (uni<0.0) uni += 1.0;

	return uni;
}

u32 ranint()
{
	float a = ranmar();
	float b = ranmar();
	u32 c;
	
	c = u16( 0xffff * a );
	c <<= 16;
	c|= u16( 0xffff * b );
	
	return c;
}

END_MACHINE_NAMESPACE
