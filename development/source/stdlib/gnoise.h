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

extern const unsigned short _gradient_perm1[ 1024 ];
extern const unsigned short _gradient_perm2[ 1024 ];
extern const unsigned short _gradient_perm3[ 1024 ];

float
_gradient_noise( float x, const unsigned short *perm );

float
_gradient_noise( float x, float y, const unsigned short *perm );

float
_gradient_noise( float x, float y, float z, const unsigned short *perm );
