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
//	icarus_decrypt.cp		   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus_decrypt.h"

BEGIN_ICARUS_NAMESPACE

static u08				s_pad[ 32 ] =
	
	{	0x91, 0x6f, 'b', 'l', 'i', 0x01, 0x73, 0xf2,
		0xa4, 0x99, 0xb5, 0x3c, 0x5f, 0x14, 0x32, 0xe0,
		0x61, 0xda, 0xe5, 0x64, 0x10, 0x17, 0xb5, 0xb9,
		0x1a, 0x18, 0x9a, 0x7c, 0x4b, 0xd3, 0xb1, 0x12 }; 

void
decrypt(
	void*				buffer,
	long				length,
	long				offset )
{
	u08*				ptr = (u08*)buffer;
	const u08*			pad = s_pad;

	for( long i = 0; i < length; i++ )
		ptr[ i ] ^= pad[ ( i + offset ) & 31 ];
}

END_ICARUS_NAMESPACE
