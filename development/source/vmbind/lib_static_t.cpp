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
//	lib_static_t.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_static_t.h"
#include "grep_t.h"

BEGIN_MACHINE_NAMESPACE

static const size_t		s_string_pool_dist[ 18 ] = {

	-1,					// 1
	-1,					// 2
	-1,					// 4
	-1,					// 8
	-1,					// 16
	-1,					// 32
	-1,					// 64
	1024,				// 128
	512,				// 256
	256,				// 512
	128,				// 1024
	64,					// 2048
	32,					// 4096
	16,					// 8192
	8,					// 16384
	4,					// 32768
	2,					// 65536
	1					// 128K

};

static const size_t		s_bigint_pool_dist[ 18 ] = {

	-1,					// 1
	-1,					// 2
	-1,					// 4
	-1,					// 8
	-1,					// 16
	-1,					// 32
	-1,					// 64
	1024,				// 128
	512,				// 256
	256,				// 512
	128,				// 1024
	64,					// 2048
	32,					// 4096
	16,					// 8192
	8,					// 16384
	4,					// 32768
	2,					// 65536
	1					// 128K

};
	
lib_static_t::lib_static_t(
	memory_t*			memory ) :

	string_pool( s_string_pool_dist, 18, memory ),
	bigint_pool( s_bigint_pool_dist, 18, memory )
{
	write_chunk_func_id = 0;
	write_bytes_func_id = 0;
	write_some_bytes_func_id = 0;
	read_chunk_func_id = 0;
	read_bytes_func_id = 0;
	read_some_bytes_func_id = 0;
	string_template = 0;
	bigint_template = 0;
	complex_template = 0;
	point_template = 0;
	pixmap_template = 0;

	for( int i = 0; i < GREP_POOL_SIZE; i++ )
		grep_pool[ i ] = 0;
	grep_index = 0;
}

lib_static_t::~lib_static_t()
{
	for( int i = 0; i < GREP_POOL_SIZE; i++ )
		if( grep_pool[ i ] )
			delete grep_pool[ i ];
}

END_MACHINE_NAMESPACE
