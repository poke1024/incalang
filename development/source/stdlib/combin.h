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


/*
** Compute C(n,m) = the number of combinations of n items,
** taken m at a time.
**
** Written by Thad Smith III, Boulder County, CO.
** Released to the Public Domain  10/14/91.
**
** The first version will not overflow if C(n,m) * (n-m+1) < ULONG_MAX.
** The second version will not overflow if C(n,m) < ULONG_MAX, but
** is slightly more complex.
** Function domain: n >= 0,  0 <= m <= n.
**
** Both versions work by reducing the product as it is computed.  It
** relies on the property that the product on n consecutive integers
** must be evenly divisible by n.
**
** The first version can be changed to make cnm and the return value
** double to extend the range of the function.
*/

/*unsigned long ncomb1 (int n, int m)
{
      unsigned long cnm = 1UL;
      int i;

      if (m*2 >n) m = n-m;
      for (i=1 ; i <= m; n--, i++)
            cnm = cnm * n / i;
      return cnm;
}

unsigned long ncomb2 (int n, int m)
{
      unsigned long cnm = 1UL;
      int i, f;

      if (m*2 >n) m = n-m;
      for (i=1 ; i <= m; n--, i++)
      {
            if ((f=n) % i == 0)
                  f   /= i;
            else  cnm /= i;
            cnm *= f;
      }
      return cnm;
}*/

// added by Bernhard Liebl 03/25/02

u64 ncomb3 (long n, long m);

u64 ncomb3 (long n, long m)
{
	 u64 cnm = 1UL;
     long i, f;

     if (m*2 >n) m = n-m;
     for (i=1 ; i <= m; n--, i++)
     {
           if ((f=n) % i == 0)
                 f   /= i;
           else  cnm /= i;
           cnm *= f;
     }
     return cnm;
}
