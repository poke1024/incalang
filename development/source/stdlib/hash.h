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

#pragma once

// simple integer hash functions.

// we hash n dimensions to 1 dimension by xoring with the products
// ( x ^ 0b10101010... ) * prime1 and ( x ^ 0b01010101... ) * prime2.

inline u32 hash2d( long x, long y )
{
	return x ^ ( 12637L * ( y ^ 0xaaaaaaaa ) ) ^ ( 29573L * ( y ^ 0x55555555 ) );
}

inline u32 hash3d( long x, long y, long z )
{
	return hash2d( x, y ) ^ ( 890125421L * ( z ^ 0xaaaaaaaa ) ) ^ ( 123467579L * ( z ^ 0x55555555 ) );
}

// high quality floating point hash functions.

// we are basically using the technique described by Steve Worley in the
// Ray Tracing News Volume 9, Number 1.
// see http://jedi.ks.uiuc.edu/~johns/raytracer/rtn/rtnv9n1.html

// IMPORTANT: we assume that f32 and u32 is 32-bit here!

static const unsigned char sbox[ 512 ] = {
	// this is simply a random permutation of the numbers 0..255.
	145, 99, 132, 49, 158, 195, 2, 36, 23, 21, 110, 174, 165, 200, 66, 18, 48, 216, 166, 230, 78,
	50, 4, 79, 234, 251, 133, 121, 77, 7, 193, 61, 98, 221, 243, 247, 51, 82, 81, 46, 180, 151, 122,
	16, 0, 44, 33, 76, 67, 106, 197, 142, 71, 215, 31, 128, 86, 43, 113, 108, 143, 58, 29, 170, 35,
	102, 149, 103, 227, 104, 37, 60, 88, 199, 186, 137, 205, 65, 38, 152, 3, 209, 237, 95, 17, 155,
	206, 176, 175, 45, 120, 107, 207, 34, 135, 214, 85, 244, 225, 153, 141, 229, 172, 147, 171, 160,
	11, 211, 162, 123, 236, 89, 204, 192, 9, 246, 156, 12, 219, 245, 115, 136, 191, 75, 6, 119, 189,
	47, 30, 150, 226, 96, 10, 101, 42, 15, 217, 184, 183, 114, 241, 13, 203, 198, 164, 250, 73, 80,
	68, 54, 159, 20, 185, 146, 228, 210, 53, 240, 56, 140, 93, 25, 173, 59, 27, 14, 83, 161, 169, 105,
	224, 19, 63, 129, 252, 28, 194, 131, 100, 177, 201, 144, 5, 87, 126, 231, 52, 248, 254, 212, 179,
	139, 208, 91, 55, 24, 190, 69, 92, 125, 157, 90, 238, 222, 187, 253, 111, 235, 57, 154, 117, 70,
	213, 22, 26, 39, 163, 130, 8, 178, 134, 32, 220, 196, 138, 97, 148, 202, 64, 112, 239, 116, 242, 167,
	1, 94, 233, 72, 181, 41, 182, 232, 118, 188, 124, 40, 255, 74, 249, 218, 127, 168, 62, 223, 109, 84,
	
	// ...once again
	145, 99, 132, 49, 158, 195, 2, 36, 23, 21, 110, 174, 165, 200, 66, 18, 48, 216, 166, 230, 78,
	50, 4, 79, 234, 251, 133, 121, 77, 7, 193, 61, 98, 221, 243, 247, 51, 82, 81, 46, 180, 151, 122,
	16, 0, 44, 33, 76, 67, 106, 197, 142, 71, 215, 31, 128, 86, 43, 113, 108, 143, 58, 29, 170, 35,
	102, 149, 103, 227, 104, 37, 60, 88, 199, 186, 137, 205, 65, 38, 152, 3, 209, 237, 95, 17, 155,
	206, 176, 175, 45, 120, 107, 207, 34, 135, 214, 85, 244, 225, 153, 141, 229, 172, 147, 171, 160,
	11, 211, 162, 123, 236, 89, 204, 192, 9, 246, 156, 12, 219, 245, 115, 136, 191, 75, 6, 119, 189,
	47, 30, 150, 226, 96, 10, 101, 42, 15, 217, 184, 183, 114, 241, 13, 203, 198, 164, 250, 73, 80,
	68, 54, 159, 20, 185, 146, 228, 210, 53, 240, 56, 140, 93, 25, 173, 59, 27, 14, 83, 161, 169, 105,
	224, 19, 63, 129, 252, 28, 194, 131, 100, 177, 201, 144, 5, 87, 126, 231, 52, 248, 254, 212, 179,
	139, 208, 91, 55, 24, 190, 69, 92, 125, 157, 90, 238, 222, 187, 253, 111, 235, 57, 154, 117, 70,
	213, 22, 26, 39, 163, 130, 8, 178, 134, 32, 220, 196, 138, 97, 148, 202, 64, 112, 239, 116, 242, 167,
	1, 94, 233, 72, 181, 41, 182, 232, 118, 188, 124, 40, 255, 74, 249, 218, 127, 168, 62, 223, 109, 84
};

inline u32 hash1d( f32 x )
{
	u32 ix = *(u32*)&x;
	
	return	30113L	* sbox[ ( ix & 0xff ) + sbox[ ix >> 24 ] ] +
			34871L	* sbox[ ( ( ix >> 8 ) & 0xff ) ^ 123 ] +
			27901L	* sbox[ ( ( ix >> 16 ) & 0xff ) ^ 47 ] +
			27487L	* sbox[ ( ix >> 24 ) ^ 176 ];
}

inline u32 hash2d( f32 x, f32 y )
{
	u32 iy = *(u32*)&y;
	
	return	hash1d( x ) +
			71741L	* sbox[ ( iy & 0xff ) + sbox[ iy >> 24 ] ] +
			68633L	* sbox[ ( ( iy >> 8 ) & 0xff ) ^ 25 ] +
			72931L	* sbox[ ( ( iy >> 16 ) & 0xff ) ^ 172 ] +
			76991L	* sbox[ ( iy >> 24 ) ^ 252 ];
}

inline u32 hash3d( f32 x, f32 y, f32 z )
{
	u32 iz = *(u32*)&z;
	
	return	hash2d( x, y ) +
			17491L	* sbox[ ( iz & 0xff ) + sbox[ iz >> 24 ] ] +
			20477L	* sbox[ ( ( iz >> 8 ) & 0xff ) ^ 152 ] +
			16787L	* sbox[ ( ( iz >> 16 ) & 0xff ) ^ 138 ] +
			15451L	* sbox[ ( iz >> 24 ) ^ 12 ];
}
