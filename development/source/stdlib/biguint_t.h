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
//	biguint_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "heap_pool_t.h"

BEGIN_MACHINE_NAMESPACE

struct field_t;

class biguint_t {
public:
	typedef				u32 digit_t;

						biguint_t(
							heap_pool_t&		pool );

						~biguint_t();
	
	void				set_int(
							u32					value );
	
	void				set_long(
							u64					value );
	
	bool				is_int(
							u32					value ) const;
	
	size_t				set(
							biguint_t*			number );

	size_t				set(
							biguint_t*			number,
							long				bitlen );
	
	void				add(
							biguint_t*			a,
							biguint_t*			b );

	void				sub(
							biguint_t*			a,
							biguint_t*			b );
	
	void				mul(
							biguint_t*			a,
							biguint_t*			b );

	static void			div(
							biguint_t*			a,
							biguint_t*			b,
							biguint_t*			q,
							biguint_t*			r );
	
	static int			compare(
							biguint_t*			a,
							biguint_t*			b );
	
	void				shift_left(
							biguint_t*			a,
							long				bits,
							digit_t				accum = 0 );

	void				shift_right(
							biguint_t*			a,
							long				bits );
	
	void				pow(
							u64					m,
							u64					n );

	void				pow(
							biguint_t*			m,
							biguint_t*			n );

	void				powmod(
							biguint_t*			m,
							biguint_t*			n,
							biguint_t*			modulo );
	
	void				sqrt(
							biguint_t*			x );
	
	bool				bclear(
							long				index );

	bool				bset(
							long				index );

	bool				bchg(
							long				index );
							
	bool				btst(
							long				index );
	
	void				random(
							biguint_t*			n );
	
	bool				set_string(
							const char*			text,
							long				length );
	
	void				to_string(
							istring&			text,
							int					base );
	
	u64					integer() const;
	
	heap_pool_t&		pool() const;

	size_t				bitlen() const;
	
	bool				zero() const;
		
protected:
	void				allocate(
							size_t				size );

	void				extend(
							size_t				size );

	size_t				digitlen() const;
	
	digit_t				lsd() const;
	
	void				to_binary(
							istring&			text );
	
	void				to_hex(
							istring&			text );

	void				agsub(
							biguint_t*			a,
							biguint_t*			b,
							size_t				sizea,
							size_t				sizeb );
	
	void				bgsub(
							biguint_t*			a,
							biguint_t*			b,
							size_t				sizea,
							size_t				sizeb );
	
	static void			extract_dividend(
							biguint_t*			a,
							field_t*			field,
							heap_pool_t&		pool );

	static void			extract_divisor(
							biguint_t*			b,
							field_t*			field,
							heap_pool_t&		pool );
	
	static void			import_from_field(
							biguint_t*			bigint,
							u08*				array,
							long				size );
	
	digit_t*			m_digits;
	size_t				m_size;

	heap_pool_t&		m_pool;
};

inline 	heap_pool_t&
biguint_t::pool() const
{
	return m_pool;
}

END_MACHINE_NAMESPACE
