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
//	output_stream_t.h		   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef OUTPUT_STREAM_H
#define OUTPUT_STREAM_H

//#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class output_stream_t {
public:
	virtual				~output_stream_t();

	virtual void		reserve(
							fpos_t				size );

	virtual fpos_t		write_some_bytes(
							const void*			buffer,
							fpos_t				count ) = 0;

	void				write_bytes(
							const void*			buffer,
							fpos_t				count );

	virtual void		flush() = 0;

	void				write_char(
							char				character );

	void				write_u08(
							u08					value );

	void				write_u16(
							u16					value );

	void				write_u32(
							u32					value );

	void				write_u64(
							u64					value );

	void				write_ptr(
							void*				ptr );

	void				write_f32(
							f32					value );

	void				write_f64(
							f64					value );
	
	void				write_string(
							const char*			string );
};

inline void
output_stream_t::write_char(
	char				character )
{
	write_u08( character );
}

inline void
output_stream_t::write_u08(
	u08					value )
{
	write_bytes( &value, sizeof( u08 ) );
}

inline void
output_stream_t::write_u16(
	u16					value )
{
	value = cond_swap_endian16( value );
	write_bytes( &value, sizeof( u16 ) );
}

inline void
output_stream_t::write_u32(
	u32					value )
{
	value = cond_swap_endian32( value );
	write_bytes( &value, sizeof( u32 ) );
}

inline void
output_stream_t::write_u64(
	u64					value )
{
	write_u32( (u32)( value >> 32 ) );
	write_u32( (u32)value );
}

inline void
output_stream_t::write_ptr(
	void*				ptr )
{
#if ICARUS_PTR_SIZE == 8
	write_u64( (u64)ptr );
#elif ICARUS_PTR_SIZE == 4
	write_u32( (u64)ptr );
#else
	#error unknown pointer size
#endif
}

inline void
output_stream_t::write_f32(
	f32					value )
{
	write_u32( *(u32*)&value );
}

inline void
output_stream_t::write_f64(
	f64					value )
{
	const u32*			x = (u32*)&value;
#if ICARUS_IS_BIG_ENDIAN
	write_u32( x[ 0 ] );
	write_u32( x[ 1 ] );
#else
	write_u32( x[ 1 ] );
	write_u32( x[ 0 ] );
#endif
}

END_ICARUS_NAMESPACE

#endif

