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
//	bigint_t.cp				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "bigint_t.h"

BEGIN_MACHINE_NAMESPACE

bigint_t::bigint_t(
	heap_pool_t&		pool ) :
	
	m_uint( pool ),
	m_sign( 1 )
{
}

bigint_t::~bigint_t()
{
}

void
bigint_t::set_int(
	long				value )
{
	if( value > 0 )
	{
		m_uint.set_int( value );
		m_sign = 1;
	}
	else if( value < 0 )
	{
		m_uint.set_int( -value );
		m_sign = -1;
	}
	else
	{
		m_uint.set_int( 0 );
		m_sign = 0;
	}
}

void
bigint_t::set_long(
	s64				value )
{
	if( value > 0 )
	{
		m_uint.set_long( value );
		m_sign = 1;
	}
	else if( value < 0 )
	{
		m_uint.set_long( -value );
		m_sign = -1;
	}
	else
	{
		m_uint.set_int( 0 );
		m_sign = 0;
	}
}

void
bigint_t::set(
	bigint_t*			number )
{
	m_uint.set( &number->m_uint );
	m_sign = number->m_sign;
}

void
bigint_t::set(
	bigint_t*			number,
	long				bitlen )
{
	m_uint.set( &number->m_uint, bitlen );
	m_sign = number->m_sign;
}

void
bigint_t::add(
	bigint_t*			a,
	bigint_t*			b )
{
	biguint_t*			avalue = &a->m_uint;
	biguint_t*			bvalue = &b->m_uint;

	int					asign = a->m_sign;
	int					bsign = b->m_sign;
	
	if( asign == bsign )
	{
		// a + b, -a + -b, 0 + 0
	
		m_uint.add( avalue, bvalue );
		m_sign = asign;
		return;
	}

	if( asign > 0 )
	{
		// a + -b
		
		int				comp;
		comp = biguint_t::compare( avalue, bvalue );
	
		if( comp >= 0 )
		{
			m_uint.sub( avalue, bvalue );
			m_sign = comp;
			return;
		}
		
		m_uint.sub( bvalue, avalue );
		m_sign = -1;
		return;
	}
	
	// b + -a

	int					comp;
	comp = biguint_t::compare( bvalue, avalue );
	
	if( comp >= 0 )
	{
		m_uint.sub( bvalue, avalue );
		m_sign = comp;
		return;
	}
	
	m_uint.sub( avalue, bvalue );
	m_sign = -1;
}

void
bigint_t::sub(
	bigint_t*			a,
	bigint_t*			b )
{
	biguint_t*			avalue = &a->m_uint;
	biguint_t*			bvalue = &b->m_uint;

	int					asign;
	int					bsign;
	
	if( ( asign = a->m_sign ) == 0 )
	{
		m_uint.set( bvalue );
		m_sign = -b->m_sign;
		return;
	}
	
	if( ( bsign = b->m_sign ) == 0 )
	{
		m_uint.set( avalue );
		m_sign = asign;
		return;
	}
	
	if( asign != bsign )
	{
		// a - -b, -a - b
		
		m_uint.add( avalue, bvalue );
		m_sign = asign;
		return;
	}

	if( asign > 0 )
	{
		// a - b
		
		int				comp;
		comp = biguint_t::compare( avalue, bvalue );
		
		if( comp >= 0 )
		{
			m_uint.sub( avalue, bvalue );
			m_sign = comp;
			return;
		}
		
		m_uint.sub( bvalue, avalue );
		m_sign = -1;
		return;
	}
	
	// b - a

	int					comp;
	comp = biguint_t::compare( bvalue, avalue );
	
	if( comp >= 0 )
	{
		m_uint.sub( bvalue, avalue );
		m_sign = comp;
		return;
	}
	
	m_uint.sub( avalue, bvalue );
	m_sign = -1;
}

void
bigint_t::mul(
	bigint_t*			a,
	bigint_t*			b )
{
	m_uint.mul( &a->m_uint, &b->m_uint );
	m_sign = a->m_sign * b->m_sign;
}

void
bigint_t::div(
	bigint_t*			a,
	bigint_t*			b )
{
	biguint_t::div( &a->m_uint, &b->m_uint, &m_uint, 0 );
	m_sign = a->m_sign * b->m_sign;
}

void
bigint_t::mod(
	bigint_t*			a,
	bigint_t*			b )
{
	biguint_t::div( &a->m_uint, &b->m_uint, 0, &m_uint );
	m_sign = m_uint.zero() ? 0 : 1;
}

int
bigint_t::compare(
	bigint_t*			a,
	bigint_t*			b )
{
	int					asign = a->m_sign;
	int					bsign = b->m_sign;

	if( asign > bsign )
		return 1;
		
	if( asign < bsign )
		return -1;
		
	if( asign == 0 )
		return 0;

	biguint_t*			avalue = &a->m_uint;
	biguint_t*			bvalue = &b->m_uint;

	if( asign < 0 )
		return -biguint_t::compare( avalue, bvalue );

	return biguint_t::compare( avalue, bvalue );
}

void
bigint_t::shift_left(
	bigint_t*			a,
	long				bits )
{
	m_sign = a->m_sign;
	m_uint.shift_left( &a->m_uint, bits );
}

void
bigint_t::shift_right(
	bigint_t*			a,
	long				bits )
{
	m_sign = a->m_sign;
	m_uint.shift_right( &a->m_uint, bits );
}

void
bigint_t::abs(
	bigint_t*			a )
{
	m_uint.set( &a->m_uint );
	m_sign = m_uint.zero() ? 0 : 1;
}

int
bigint_t::sgn()
{
	return m_sign;
}

void
bigint_t::pow(
	u64					x,							
	u64					y )
{
	m_uint.pow( x, y );
	m_sign = m_uint.zero();
}

void
bigint_t::pow(
	bigint_t*			x,
	bigint_t*			y )
{
	if( x->m_sign < 0 ||
		y->m_sign < 0 )
	{
		throw_icarus_error( "parameter sign error" );
	}
	
	m_uint.pow( &x->m_uint, &y->m_uint );
	m_sign = m_uint.zero() ? 0 : 1;
}

void
bigint_t::powmod(
	bigint_t*			x,
	bigint_t*			y,
	bigint_t*			z )
{
	if( x->m_sign < 0 ||
		y->m_sign < 0 ||
		z->m_sign < 0 )
	{
		throw_icarus_error( "parameter sign error" );
	}
	
	m_uint.powmod( &x->m_uint, &y->m_uint, &z->m_uint );
	m_sign = m_uint.zero() ? 0 : 1;
}

void
bigint_t::sqrt(
	bigint_t*			x )
{
	if( x->m_sign < 0 )
		throw_icarus_error( "parameter sign error" );
		
	m_uint.sqrt( &x->m_uint );
	m_sign = m_uint.zero() ? 0 : 1;
}

void
bigint_t::gcd(
	bigint_t*			m,
	bigint_t*			n )
{
	if( n->zero() )
	{
		set( m );
		if( m_sign < 0 )
			m_sign = 1;
		return;
	}

	if( m->zero() )
	{
		set( n );
		if( m_sign < 0 )
			m_sign = 1;
		return;
	}
	
	long				t = 0;
	biguint_t*			mu = &m->m_uint;
	biguint_t*			nu = &n->m_uint;

	while( not mu->btst( t ) && not nu->btst( t ) )
		t++;

	long				ta = t;
	long				tb = t;
	
	while( not mu->btst( ta ) )
		ta++;

	while( not nu->btst( tb ) )
		tb++;
		
	heap_pool_t&		pool = m_uint.pool();
		
	biguint_t			tt1( pool );
	biguint_t			tt2( pool );
	biguint_t			tt3( pool );
		
	biguint_t*			a = &tt1;
	biguint_t*			b = &tt2;
	biguint_t*			tmp = &tt3;
		
	a->shift_right( mu, ta );
	b->shift_right( nu, tb );
	
	while( true )
	{
		int				comp;
		long			i;
		
		comp = biguint_t::compare( a, b );
		
		if( comp == 0 )
			break;
		else if( comp > 0 )
		{
			tmp->sub( a, b );
			iswap( a, tmp );
			
			i = 1;
			while( not a->btst( i ) )
				i++;
			tmp->shift_right( a, i );
			iswap( tmp, a );
		}
		else
		{
			tmp->sub( b, a );
			iswap( b, tmp );
			
			i = 1;
			while( not b->btst( i ) )
				i++;
			tmp->shift_right( b, i );
			iswap( tmp, b );
		}
	}
	
	m_uint.shift_left( a, t );
	m_sign = 1;
}

void
bigint_t::egcd(
	bigint_t*			m,
	bigint_t*			n,
	bigint_t*			x,
	bigint_t*			y )
{
	if( m->m_sign < 0 )
	{
		m->m_sign *= -1;
		egcd( m, n, x, y );
		m->m_sign *= -1;
		x->m_sign *= -1;
		return;
	}
	
	if( n->m_sign < 0 )
	{
		n->m_sign *= -1;
		egcd( m, n, x, y );
		n->m_sign *= -1;
		y->m_sign *= -1;
		return;
	}
	
	heap_pool_t&		pool = m_uint.pool();
	bigint_t			tt1( pool );
	bigint_t			tt2( pool );
	bigint_t			tt3( pool );
	bigint_t			tt4( pool );
	bigint_t			tt5( pool );
	bigint_t			tt6( pool );
	bigint_t			tt7( pool );
	bigint_t			tt8( pool );
	bigint_t			tt9( pool );
	
	bigint_t*			a0;
	bigint_t*			b0;
	bigint_t*			c0;
	bigint_t*			a1;
	bigint_t*			b1;
	bigint_t*			c1;
	bigint_t*			q;
	bigint_t*			t;
	bigint_t*			s;
	
	a0 = &tt1;
	b0 = &tt2;
	c0 = &tt3;
	a1 = &tt4;
	b1 = &tt5;
	c1 = &tt6;
	q = &tt7;
	t = &tt8;
	s = &tt9;

	a0->set_int( 1 );
	b0->set_int( 0 );
	c0->set( m );
	a1->set_int( 0 );
	b1->set_int( 1 );
	c1->set( n );
	
	while( not c1->zero() )
	{
		q->div( c0, c1 );
		
		iswap( t, a0 );
		a0->set( a1 );
		s->mul( q, a1 );
		a1->sub( t, s );
		
		iswap( t, b0 );
		b0->set( b1 );
		s->mul( q, b1 );
		b1->sub( t, s );
		
		iswap( t, c0 );
		c0->set( c1 );
		s->mul( q, c1 );
		c1->sub( t, s );
	}
	
	if( x )
		x->set( a0 );
	if( y )
		y->set( b0 );
	set( c0 );
}

void
bigint_t::lcm(
	bigint_t*			a,
	bigint_t*			b )
{
	heap_pool_t&		pool = m_uint.pool();
	bigint_t			tmp1( pool );
	bigint_t			tmp2( pool );

	tmp1.mul( a, b );
	tmp2.gcd( a, b );
	div( &tmp1, &tmp2 );
}

void
bigint_t::modinv(
	bigint_t*			x,
	bigint_t*			n )
{
	heap_pool_t&		pool = m_uint.pool();
	bigint_t			b( pool );

	egcd( n, x, 0, &b );

	if( m_sign > 0 && m_uint.is_int( 1 ) )
		;
	else
		throw_icarus_error( "parameters not relatively prime" );
		
	mod( &b, n );
}

void
bigint_t::bclear(
	long				index )
{
	if( m_sign == 0 )
		return;

	m_uint.bclear( index );
	if( m_uint.zero() )
		m_sign = 0;
}

void
bigint_t::bset(
	long				index )
{
	if( m_uint.bset( index ) && m_sign == 0 )
		m_sign = 1;
}

void
bigint_t::bchg(
	long				index )
{
	m_uint.bchg( index );
	if( m_uint.zero() )
		m_sign = 0;
}

bool
bigint_t::btst(
	long				index )
{
	return m_uint.btst( index );
}

void
bigint_t::random(
	bigint_t*			n )
{
	m_uint.random( &n->m_uint );

	if( not m_uint.zero() )
		m_sign = n->m_sign;
	else
		m_sign = 0;
}

void
bigint_t::set_string(
	const char*			text,
	long				length )
{
	if( length > 0 && *text == '-' )
	{
		m_sign = -1;
		text++;
		length--;
	}
	else
		m_sign = 1;
	
	if( not m_uint.set_string( text, length ) )
		m_sign = 0;
}

void
bigint_t::to_string(
	istring&			text,
	int					base )
{
	if( m_sign < 0 )
		text.append( "-" );

	m_uint.to_string( text, base );
}

s64
bigint_t::integer() const
{
#if __VISC__
	const u64			maxu64 = 9223372036854775807;
#else
	const u64			maxu64 = 9223372036854775807ULL;
#endif
	u64					value = m_uint.integer();
	
	if( m_sign >= 0 )
	{
		if( value > maxu64 )
			value = maxu64;
			
		return value;
	}

	if( value > maxu64 + 1 )
		value = maxu64 + 1;
		
	return -value;
}

bool
bigint_t::zero() const
{
	return m_sign == 0;
}

biguint_t&
bigint_t::uint()
{
	return m_uint;
}

void
bigint_t::negate()
{
	m_sign = -m_sign;
}

END_MACHINE_NAMESPACE
