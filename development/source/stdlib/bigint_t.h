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
//	bigint_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "biguint_t.h"

BEGIN_MACHINE_NAMESPACE

class bigint_t {
public:
						bigint_t(
							heap_pool_t&		pool );

						~bigint_t();
	
	void				set_int(
							long				value );

	void				set_long(
							s64					value );
	
	void				set(
							bigint_t*			number );

	void				set(
							bigint_t*			number,
							long				bitlen );
	
	void				add(
							bigint_t*			a,
							bigint_t*			b );

	void				sub(
							bigint_t*			a,
							bigint_t*			b );
	
	void				mul(
							bigint_t*			a,
							bigint_t*			b );

	void				div(
							bigint_t*			a,
							bigint_t*			b );
	
	void				mod(
							bigint_t*			a,
							bigint_t*			b );
	
	static int			compare(
							bigint_t*			a,
							bigint_t*			b );
	
	void				shift_left(
							bigint_t*			a,
							long				bits );

	void				shift_right(
							bigint_t*			a,
							long				bits );
	
	void				abs(
							bigint_t*			a );
	
	int					sgn();
		
	void				pow(
							u64					x,							
							u64					y );

	void				pow(
							bigint_t*			x,
							bigint_t*			y );

	void				powmod(
							bigint_t*			x,
							bigint_t*			y,
							bigint_t*			z );
	
	void				sqrt(
							bigint_t*			x );

	void				gcd(
							bigint_t*			m,
							bigint_t*			n );

	void				egcd(
							bigint_t*			m,
							bigint_t*			n,
							bigint_t*			x,
							bigint_t*			y );

	void				lcm(
							bigint_t*			a,
							bigint_t*			b );

	void				modinv(
							bigint_t*			x,
							bigint_t*			n );

	void				bclear(
							long				index );

	void				bset(
							long				index );

	void				bchg(
							long				index );

	bool				btst(
							long				index );
							
	void				random(
							bigint_t*			n );
	
	void				set_string(
							const char*			text,
							long				length );

	void				to_string(
							istring&			text,
							int					base );
	
	s64					integer() const;
	
	bool				zero() const;
	
	biguint_t&			uint();
	
	void				negate();
	
protected:
	biguint_t			m_uint;
	s08					m_sign;
};

END_MACHINE_NAMESPACE
