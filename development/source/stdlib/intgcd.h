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


/* binary GCD alg. Based on following facts.
   If N and M are even, then gcd(N,M) = 2 * gcd (N/2, M/2)
   If N even and M odd, then gcd(N,M) = gcd(N/2, M)
   If N,M odd, then |N-M|<max(N,M) thus guaranteeing that
             we can use gcd(N,M) = gcd(N-M,M) to reduce.
   fact: a&1 is 1 iff a is odd. Mult. and Div. by 2 more efficiently
   implemented as shift-left 1 and shift-right 1 resp.
*/   

template<class T>
inline T gcd( T a, T b)
{
  int t;

  if (a < 0) a = -a;
  if (!b) return a;
  if (b < 0) b = -b;
  if (!a) return b;
  t = 0;
  while (! ((a|b) & 1)) {a >>= 1; b >>= 1; ++t;}
  while (! (a&1)) a >>= 1;                     
  while (! (b&1)) b >>= 1;
  while (a != b)
     {
     if (a > b)
         {
         a -= b; 
         do {a >>= 1;} while (! (a&1));
         }
     else {
         b -= a;
         do {b >>= 1;} while (! (b&1));
         }
     }
  return(a<<t);
}
