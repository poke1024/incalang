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
//	input_stream_t.h			   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

//#pragma once

#include "icarus.h"
#include <string>

BEGIN_ICARUS_NAMESPACE

class input_stream_t {
public:
						input_stream_t();

	virtual				~input_stream_t();

	virtual fpos_t		read_some_bytes(
							void*				buffer,
							fpos_t				count ) = 0;

	void				read_bytes(
							void*				buffer,
							fpos_t				count );

	virtual void		skip(
							fpos_t				count );

	char				read_char();

	u08					read_u08();

	u16					read_u16();

	u32					read_u32();

	u64					read_u64();
	
	void*				read_ptr();

	f32					read_f32();

	f64					read_f64();
	
	void				read_string(
							istring&			string );
};

inline char
input_stream_t::read_char()
{
	return read_u08();
}

inline u08
input_stream_t::read_u08()
{
	u08 x;
	read_bytes( &x, sizeof( u08 ) );
	return x;
}

inline u16
input_stream_t::read_u16()
{
	u16 x;
	read_bytes( &x, sizeof( u16 ) );
	return cond_swap_endian16( x );
}

inline u32
input_stream_t::read_u32()
{
	u32 x;
	read_bytes( &x, sizeof( u32 ) );
	return cond_swap_endian32( x );
}

inline u64
input_stream_t::read_u64()
{
	u32 hi, lo;
	hi = read_u32();
	lo = read_u32();
	return ( ( (u64)hi ) << 32 ) | lo;
}

inline void*
input_stream_t::read_ptr()
{
#if ICARUS_PTR_SIZE == 8
	return (void*)read_u64();
#elif ICARUS_PTR_SIZE == 4
	return (void*)read_u32();
#else
	#error unknown pointer size
#endif
}

inline f32
input_stream_t::read_f32()
{
	u32 x = read_u32();
	return *( (float*)&x );
}

inline f64
input_stream_t::read_f64()
{
	u32 x[ 2 ];
#if ICARUS_IS_BIG_ENDIAN
	x[ 0 ] = read_u32();
	x[ 1 ] = read_u32();
#else
	x[ 1 ] = read_u32();
	x[ 0 ] = read_u32();
#endif
	return *( (double*)x );
}

END_ICARUS_NAMESPACE

#endif

