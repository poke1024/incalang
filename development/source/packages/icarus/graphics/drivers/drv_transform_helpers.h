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
//	drv_transform_helpers.h	   ©1999-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

inline s32 CUBIC_X( s32 v0, s32 v1, s32 v2, s32 v3, s32 x )
{
	s32 c3, c2, c1, c0, r;

	x = ( x + 0x80 ) >> 8;

	c3 = -( v0 << 7 ) + ( v1 << 8 ) + ( v1 << 7 ) -
		( v2 << 8 ) - ( v2 << 7 ) + ( v3 << 7 );
	c2 = ( v0 << 8 ) - ( v1 << 9 ) - ( v1 << 7 ) +
		( v2 << 9 ) - ( v3 << 7 );
	c1 = -( v0 << 7 ) + ( v2 << 7 );
	c0 = v1 << 8;
	
	if( c3 > -0x80 )
		r = ( ( c3 * x + 0x80 ) >> 8 ) + c2;
	else
		r = ( ( c3 * x - 0x80 ) >> 8 ) + c2;

	if( r > -0x80 )
		r = ( ( r * x + 0x80 ) >> 8 ) + c1;
	else
		r = ( ( r * x - 0x80 ) >> 8 ) + c1;
		
	if( r > -0x80 )
		r = ( ( r * x + 0x80 ) >> 8 ) + c0;
	else
		r = ( ( r * x - 0x80 ) >> 8 ) + c0;

	if( r < 0 )
		r = 0;
	else if( r > 0xff00 )
		r = 0xff00;
		
	return r;
}

inline s32 CUBIC_Y( s32 v0, s32 v1, s32 v2, s32 v3, s32 x )
{
	s32 c3, c2, c1, c0, r;

	x = ( x + 0x80 ) >> 8;

	c3 = -( v0 >> 1 ) + v1 + ( v1 >> 1 ) -
		v2 - ( v2 >> 1 ) + ( v3 >> 1 );
	c2 = v0 - ( v1 << 1 ) - ( v1 >> 1 ) +
		( v2 << 1 ) - ( v3 >> 1 );
	c1 = -( v0 >> 1 ) + ( v2 >> 1 );
	c0 = v1;
	
	if( c3 > -0x80 )
		r = ( ( c3 * x + 0x80 ) >> 8 ) + c2;
	else
		r = ( ( c3 * x - 0x80 ) >> 8 ) + c2;

	if( r > -0x80 )
		r = ( ( r * x + 0x80 ) >> 8 ) + c1;
	else
		r = ( ( r * x - 0x80 ) >> 8 ) + c1;
		
	if( r > -0x80 )
		r = ( ( r * x + 0x80 ) >> 8 ) + c0;
	else
		r = ( ( r * x - 0x80 ) >> 8 ) + c0;

	r = ( r + 0x80 ) >> 8;
	
	if( r < 0 )
		r = 0;
	else if( r > 0xff )
		r = 0xff;

	return r;
}
