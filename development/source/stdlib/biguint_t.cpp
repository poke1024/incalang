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
//	biguint_t.cp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "biguint_t.h"
#include <ctype.h>

BEGIN_MACHINE_NAMESPACE

extern u32 ranint();

// ---------------------------------------------------------------------------

#define mul32( a, b, lo, hi ) {												\
	unsigned long a0, b0, a1, b1, m0, m1, m2, m3;							\
	unsigned long mask = ( 1L << 16 ) - 1L;									\
																			\
	a0 = (a) & mask;														\
	b0 = (b) & mask;														\
	a1 = (a) >> 16;															\
	b1 = (b) >> 16;															\
																			\
	m0 = a0 * b0;															\
	m1 = a0 * b1;															\
	m2 = a1 * b0;															\
	m3 = a1 * b1;															\
																			\
	if( ( m1 += ( m0 >> 16 ) + m2 ) <  m2 )									\
		m3 += 1L << 16;														\
																			\
	(hi) = m3 + ( m1 >> 16 );												\
	(lo) = ( ( m1 & mask ) << 16 ) + ( m0 & mask ); }

// ---------------------------------------------------------------------------

biguint_t::biguint_t(
	heap_pool_t&		pool ) :
	
	m_pool( pool )
{
	m_size = 0;
	m_digits = 0;
	set_int( 0 );
}

biguint_t::~biguint_t()
{
	allocate( 0 );
}

void
biguint_t::set_int(
	u32					value )
{
	allocate( 1 );
	m_digits[ 0 ] = value;
}

void
biguint_t::set_long(
	u64					value )
{
	allocate( 2 );
	m_digits[ 0 ] = (u32)( value >> 32 );
	m_digits[ 1 ] = (u32)value;
}

bool
biguint_t::is_int(
	u32					value ) const
{
	if( m_digits[ 0 ] == value && bitlen() <= 32 )
		return true;
		
	return false;
}

size_t
biguint_t::set(
	biguint_t*			number )
{
	if( this == number )
		return 0;

	size_t				diglen = number->digitlen();
	size_t				size = diglen;
	
	if( size < 1 )
		size = 1;
	
	allocate( size );
	
	imemcpy( m_digits,
		number->m_digits + number->m_size - size,
		size * sizeof( digit_t ) );

	return diglen;
}

size_t
biguint_t::set(
	biguint_t*			number,
	long				bitlen )
{
	size_t				minalloc = bitlen >> 5;

	if( bitlen & 31 )
		minalloc++;

	size_t				diglen = number->digitlen();
	size_t				size = diglen;
	
	if( size < 1 )
		size = 1;
		
	size_t				alloc = size;
	
	if( alloc < minalloc )
		alloc = minalloc;
	
	allocate( alloc );
	
	imemcpy( m_digits + alloc - size,
		number->m_digits + number->m_size - size,
		size * sizeof( digit_t ) );
		
	imemset( m_digits, 0,
		( alloc - size ) * sizeof( digit_t ) );

	return diglen;
}
void
biguint_t::add(
	biguint_t*			a,
	biguint_t*			b )
{
	size_t				sizea = a->m_size;
	size_t				sizeb = b->m_size;
	size_t				sizec;

	if( sizea > sizeb )
	{
		iswap( a, b );
		iswap( sizea, sizeb );
	}
	
	sizec = sizeb + 1;
	allocate( sizec );

	digit_t*			ad = a->m_digits;
	digit_t*			bd = b->m_digits;
	digit_t*			cd = m_digits;

	long				i = sizea - 1;
	long				j = sizeb - 1;
	long				k = m_size - 1;
	
	digit_t				digit;
	digit_t				sum;
	digit_t				carry = 0;

	for( ; i >= 0; i--, j--, k-- )
	{		
		digit = ad[ i ];
		sum = bd[ j ] + digit + carry;
		cd[ k ] = sum;
		
		if( sum < digit )
			carry = 1;
		else if( sum > digit )
			carry = 0;
	}

	for( ; j >= 0; j--, k-- )
	{
		digit = bd[ j ];
		sum = digit + carry;
		cd[ k ] = sum;
		carry = ( sum < digit ) ? 1 : 0;
	}

	cd[ k ] = carry;
}

void
biguint_t::sub(
	biguint_t*			a,
	biguint_t*			b )
{
	size_t				sizea = a->m_size;
	size_t				sizeb = b->m_size;

	if( sizeb > sizea )
		bgsub( a, b, sizea, sizeb );
	else
		agsub( a, b, sizea, sizeb );
}

void
biguint_t::agsub(
	biguint_t*			a,
	biguint_t*			b,
	size_t				sizea,
	size_t				sizeb )
{
	size_t				sizec = sizea;

	allocate( sizec );

	digit_t*			ad = a->m_digits;
	digit_t*			bd = b->m_digits;
	digit_t*			cd = m_digits;

	long				i = sizea - 1;
	long				j = sizeb - 1;
	long				k = m_size - 1;

	digit_t				digit;
	digit_t				sum;
	digit_t				carry = 0;
	
	for( ; j >= 0; i--, j--, k-- )
	{
		digit = ad[ i ];
		sum = digit - bd[ j ] - carry;
		cd[ k ] = sum;
		
		if( sum > digit )
			carry = 1;
		else if( sum < digit )
			carry = 0;
	}

	for( ; i >= 0; i--, k-- )
	{
		digit = ad[ i ];
		sum = digit - carry;
		cd[ k ] = sum;
		carry = ( sum > digit ) ? 1 : 0;
	}

	if( carry )
		throw_icarus_error( "bigint underflow" );
}

void
biguint_t::bgsub(
	biguint_t*			a,
	biguint_t*			b,
	size_t				sizea,
	size_t				sizeb )
{
	size_t				sizec = sizea;

	allocate( sizec );

	digit_t*			ad = a->m_digits;
	digit_t*			bd = b->m_digits;
	digit_t*			cd = m_digits;

	long				i = sizea - 1;
	long				j = sizeb - 1;
	long				k = m_size - 1;

	digit_t				digit;
	digit_t				sum;
	digit_t				carry = 0;
	
	for( ; i >= 0; i--, j--, k-- )
	{
		digit = ad[ i ];
		sum = digit - bd[ j ] - carry;
		cd[ k ] = sum;
		
		if( sum > digit )
			carry = 1;
		else if( sum < digit )
			carry = 0;
	}

	if( carry )
		throw_icarus_error( "bigint underflow" );

	for( ; j >= 0; j-- )
		if( bd[ j ] )
			throw_icarus_error( "bigint underflow" );
}

#define propagate_carry( q )					\
	{ digit_t* p = (q);							\
	while( ++*p == 0 ) p--; }

void
biguint_t::mul(
	biguint_t*			a,
	biguint_t*			b )
{
	size_t				sizea = a->m_size;
	size_t				sizeb = b->m_size;
	size_t				sizec;

	sizec = sizea + sizeb + 1;
	allocate( sizec );

	digit_t*			ad = a->m_digits;
	digit_t*			bd = b->m_digits;
	digit_t*			cd = m_digits;

	imemset( cd, 0, sizec * sizeof( digit_t ) );

	long				i = sizea - 1;
	long				jtop = sizeb - 1;
	long				k = m_size - 1;

	digit_t				digit;
	
	for( ; i >= 0; i--, k-- )
	{		
		digit = ad[ i ];
		
		long			l = k - 1;
		
		for( long j = jtop; j >= 0; j--, l-- )
		{
			digit_t		factor = bd[ j ];
			digit_t		lo;
			digit_t		hi;
			digit_t		sum;
			digit_t		carry = 0;
		
			mul32( digit, factor, lo, hi );
						
			sum = cd[ l + 1 ] + lo + carry;
			cd[ l + 1 ] = sum;

			if( sum < lo )
				carry = 1;
			else if( sum > lo )
				carry = 0;

			sum = cd[ l ] + hi + carry;
			cd[ l ] = sum;

			if( sum < hi )
				carry = 1;
			else if( sum > hi )
				carry = 0;
		
			if( carry )
				propagate_carry( cd + l - 1 );
		}
	}
}

struct field_t {
	u08*				digits;
	size_t				size;
	size_t				alloc;
};

void
biguint_t::extract_dividend(
	biguint_t*			a,
	field_t*			field,
	heap_pool_t&		pool )	
{
	long				alen;
	bool				zero;
	long				atemplen;

	alen = a->m_size;
	atemplen = alen * 4;

	if( alen > 0 && ( a->m_digits[ 0 ] >> 16 ) == 0 )
		zero = false;
	else
	{
		zero = true;
		atemplen += 2;
	}
	
	u08*				atempn;
	
	atempn = (u08*)pool.alloc( atemplen );

	digit_t*			p = a->m_digits;
	u08*				q = atempn;
	digit_t				digit;
	long				count = alen;
	
	if( zero )
	{
		*q++ = 0;
		*q++ = 0;
	}
		
	do {
		digit = *p++;
		*q++ = digit >> 24;
		*q++ = digit >> 16;
		*q++ = digit >> 8;
		*q++ = digit;
	} while( --count );

	field->digits = atempn;
	field->size = q - atempn;
	field->alloc = atemplen;
}

void
biguint_t::extract_divisor(
	biguint_t*			b,
	field_t*			field,
	heap_pool_t&		pool )
{
	long				blen;

	blen = b->m_size;

	digit_t*			bn = b->m_digits;
	long				bpos = 0;

	while( true )
	{
		if( bpos >= blen )
			throw_icarus_error( "divide by zero" );
			
		if( bn[ bpos ] )
			break;
			
		bpos++;
	}

	// create a copy of divisor, making
	// sure that first digit != 0

	digit_t				digit;

	long				count;
	long				btemplen;
	u08*				btempn;
	
	count = blen - bpos;
	btemplen = count * 4;
	btempn = (u08*)pool.alloc( btemplen );
	
	digit_t*			p = bn + bpos;
	u08*				q = btempn;
	
	digit = *p++;
	
	for( int i = 24; i >= 0; i -= 8 )
	{
		if( digit >> i )
			*q++ = digit >> i;
	}
	
	while( --count )
	{
		digit = *p++;
		*q++ = digit >> 24;
		*q++ = digit >> 16;
		*q++ = digit >> 8;
		*q++ = digit;
	}
	
	field->digits = btempn;
	field->size = q - btempn;
	field->alloc = btemplen;
}

void
biguint_t::import_from_field(
	biguint_t*			bigint,
	u08*				array,
	long				size )
{
	long				dsize = ( size + 3 ) / 4;

	bigint->allocate( dsize );
	
	u08*				ap = array + size;
	digit_t*			dp = bigint->m_digits + dsize;
	digit_t				val;

	while( size >= 4 )
	{
		ap -= 4;
		val = ap[ 0 ];
		val <<= 8;
		val |= ap[ 1 ];
		val <<= 8;
		val |= ap[ 2 ];
		val <<= 8;
		val |= ap[ 3 ];
		*--dp = val;
		size -= 4;
	}
	
	if( size )
	{
		ap -= 4;
	
		val = ap[ 3 ];
		if( --size )
		{
			val |= ( (digit_t)ap[ 2 ] ) << 8;
			if( --size )
			{
				val |= ( (digit_t)ap[ 1 ] ) << 16;
			}
		}
	
		*--dp = val;
	}
}

void
biguint_t::div(
	biguint_t*			a,
	biguint_t*			b,
	biguint_t*			_q,
	biguint_t*			_r )
{
	#define MODULO		256

	heap_pool_t&		pool = a->m_pool;

	field_t				afield;
	field_t				bfield;

	extract_dividend( a, &afield, pool );
	extract_divisor( b, &bfield, pool );
	
	u08*				u;
	u08*				v;
	long				ulen;
	long				vlen;
	
	u = afield.digits;
	v = bfield.digits;
	ulen = afield.size;
	vlen = bfield.size;
	
	u08*				q;
	long				qlen;
	
	qlen = ulen - vlen;
	q = (u08*)pool.alloc( qlen );

	unsigned int		carry;
	unsigned int		quo;
	long				d;
	
	long				m1;
	long				m2;
	long				m3;
	
	long				i, j;
	long				bpos;

	d = MODULO / ( v[ 0 ] + 1 );

	for( carry = 0, j = ulen - 1; j >= 0; j-- )   
	{
		m1 = d * u[ j ] + carry;
		u[ j ] = m1;
		carry = m1 / MODULO;
	}
	
	if( carry )
		throw_icarus_error( "illegal carry during division" );
	
	for( carry = 0, j = vlen - 1; j >= 0; j-- )   
	{
		m1 = d * v[ j ] + carry;
		v[ j ] = m1;
		carry = m1 / MODULO;
	}

	if( carry )
		throw_icarus_error( "illegal carry during division" );

	for( j = 0; j < ulen - vlen; j++ )
	{
		if( v[ 0 ] == u[ j ] )
			quo = MODULO - 1L;
		else
		{
			m1 = u[ j ] * MODULO;
			m1 = ( m1 + u[ j + 1 ] ) / v[ 0 ];
			quo = m1;
		}
		
		m3 = ( u[ j ] * MODULO ) + u[ j + 1 ];
		
		do {
			if( vlen > 1 )
				m1 = quo * v[ 1 ];
			else
				m1 = 0;
				
			m2 = quo * v[ 0 ];
			m2 = ( ( m3 - m2 ) * MODULO ) + u[ j + 2 ];
		
			if( m1 > m2 )
				quo--;
				
		} while( m1 > m2 );
		
		bpos = vlen - 1;
		i = j + vlen;
		m2 = 0;
		
		while( i >= j )
		{
			if( bpos >= 0 )
				m1 = quo * v[ bpos ];
			else
				m1 = 0;
			
			m3 = u[ i ] - m1 + m2;	
			if( m3 < 0 )
			{
				m2 = m3 / MODULO;
				m3 = m3 % MODULO;
				if( m3 < 0 )
				{
					m3 += MODULO;
					m2--;
				}
			}
			else
				m2 = 0;
				
			u[ i ] = m3;
			bpos--;
			i--;
		}
	
		if( m2 == 0 )
			q[ j ] = quo;
		else
		{
			q[ j ] = quo - 1;
			bpos = vlen - 1;
			i = j + vlen;
			carry = 0;
			
			long tmp;
			
			while( i >= j )
			{
				if( bpos >= 0 )
					carry += v[ bpos ];
					
				tmp = carry + u[ i ];
				carry = ( tmp >= MODULO );	
				u[ i ] = tmp % MODULO;
				bpos--;
				i--;
			}
			
			// ?
			while( carry && i >= 0 )
			{
				tmp = carry + u[ i ];
				carry = ( tmp >= MODULO );	
				u[ i ] = tmp % MODULO;
				i--;
			}
			// ?
			
			if( carry )
				throw_icarus_error( "illegal carry during division" );
		}
	}

	if( _r )
	{
		j = ulen - vlen;
		carry = 0;
		
		while( j < ulen )
		{
			m3 = carry * MODULO + u[ j ];
			u[ j ] = m3 / d;
			carry = m3 % d;
			j++;
		}
		
		import_from_field( _r, u + ulen - vlen, vlen  );
	}
	
	if( _q)
		import_from_field( _q, q, ulen - vlen );
		
	pool.free( afield.digits, afield.alloc );
	pool.free( bfield.digits, bfield.alloc );
	pool.free( q, qlen );
}

int
biguint_t::compare(
	biguint_t*			a,
	biguint_t*			b )
{
	size_t				sizea = a->m_size;
	size_t				sizeb = b->m_size;
	int					one;

	if( sizeb > sizea )
	{
		iswap( a, b );
		iswap( sizea, sizeb );
		one = -1;
	}
	else
		one = 1;

	digit_t*			ad = a->m_digits;
	digit_t*			bd = b->m_digits;
	
	long				i = 0;
	long				k = sizea - sizeb;
	
	for( ; i < k; i++ )
	{
		if( ad[ i ] )
			return one;
	}
	
	for( long j = 0; j  < sizeb; j++, i++ )
	{
		digit_t			va = ad[ i ];
		digit_t			vb = bd[ j ];
	
		if( va > vb )
			return one;
		if( va < vb )
			return -one;
	}
	
	return 0;
}

void
biguint_t::shift_left(
	biguint_t*			a,
	long				bits,
	digit_t				accum )
{
	if( bits < 1 )
	{
		set( a );
		return;
	}

	long				digitshift;
	int					bitshift;
	digit_t				bitmask;
	
	digitshift = bits >> 5;
	bits &= 31;
	bitshift = 32 - bits;
	bitmask = ( 1L << bits ) - 1L;
	
	size_t				sizea = a->m_size;
	size_t				sizec;

	sizec = sizea + digitshift + 1;
	allocate( sizec );
	
	digit_t*			ad = a->m_digits;
	digit_t*			cd = m_digits;

	long				i = sizea - 1;
	long				k = m_size - 1;

	for( long l = digitshift; l > 0; l--, k-- )
		cd[ k ] = 0;
		
	for( ; i >= 0; i--, k-- )
	{
		digit_t			digit;
	
		digit = ad[ i ];
		cd[ k ] = ( digit << bits ) | accum;
		accum = ( digit >> bitshift ) & bitmask;
	}

	cd[ k ] = accum;
}

void
biguint_t::shift_right(
	biguint_t*			a,
	long				bits )
{
	long				digitshift;

	digitshift = bits >> 5;

	if( digitshift >= a->m_size )
	{
		set_int( 0 );
		return;
	}

	int					bitshift;
	digit_t				bitmask;
	
	bits &= 31;
	bitshift = 32 - bits;
	bitmask = ( 1L << bits ) - 1L;
	
	size_t				sizea = a->m_size;
	size_t				sizec;

	sizec = sizea - digitshift;
	allocate( sizec );
	
	digit_t*			ad = a->m_digits;
	digit_t*			cd = m_digits;

	long				i = 0;
	long				ktop = m_size - 1;
	
	digit_t				accum = 0;
	
	for( long k = 0; k <= ktop; k++, i++ )
	{
		digit_t			digit;
	
		digit = ad[ i ];
		cd[ k ] = ( digit >> bits ) | accum;
		accum = ( digit & bitmask ) << bitshift;
	}
}

void
biguint_t::allocate(
	size_t				size )
{
	digit_t*			digits = m_digits;

	if( digits )
	{
		m_pool.free( digits,
			m_size * sizeof( digit_t ) );
	
		m_digits = 0;
	}
	
	if( size )
	{
		m_digits = (digit_t*)m_pool.alloc(
			size * sizeof( digit_t ) );
	}
	
	m_size = size;
}

size_t
biguint_t::bitlen() const
{
	const digit_t*		cd = m_digits;
	const digit_t*		p = cd;
	size_t				size = m_size;
	size_t				left = size;

	do {
		digit_t			digit;
	
		if( ( digit = *p++ ) != 0 )
		{
			size_t		bitlen;

			bitlen = ( size - ( p - cd ) ) << 5;

			int			shift = 31;

			while( ( digit >> shift ) == 0 )
				shift--;
		
			return bitlen + shift + 1;
		}

	} while( --left );

	return 0;
}

size_t
biguint_t::digitlen() const
{
	const digit_t*		cd = m_digits;
	const digit_t*		p = cd;
	size_t				size = m_size;
	size_t				left = size;

	do {
		digit_t			digit;
		
		if( ( digit = *p++ ) != 0 )
			return size - ( p - cd ) + 1;

	} while( --left );
	
	return 0;
}

biguint_t::digit_t
biguint_t::lsd() const
{
	return m_digits[ m_size - 1 ];
}

bool
biguint_t::zero() const
{
	return digitlen() < 1;
}

// ---------------------------------------------------------------------------

void
biguint_t::pow(
	u64					m,
	u64					n )
{
	biguint_t			ta( m_pool );
	biguint_t			tb( m_pool );
	biguint_t			tt( m_pool );

	biguint_t*			a = &ta;
	biguint_t*			b = &tb;
	biguint_t*			tmp = &tt;

	b->set_int( 1 );
	a->set_long( m );
	
	while( n )
	{
		if( n & 1 )
		{
			tmp->mul( a, b );
			iswap( b, tmp );
		}
	
		n >>= 1;
		
		if( n )
		{
			tmp->mul( a, a );
			iswap( a, tmp );
		}
	}

	set( b );
}

void
biguint_t::pow(
	biguint_t*			m,
	biguint_t*			n )
{	
	long				bits = n->bitlen();
	
	if( bits < 1 )
	{
		set_int( 1 );
		return;
	}

	biguint_t			ta( m_pool );
	biguint_t			tb( m_pool );
	biguint_t			tt( m_pool );

	biguint_t*			a = &ta;
	biguint_t*			b = &tb;
	biguint_t*			tmp = &tt;
	
	b->set_int( 1 );
	a->set( m );
	
	long				index = 0;
	
	digit_t*			nd = n->m_digits + n->m_size - 1;
	
	while( true )
	{
		if( nd[ -( index >> 5 ) ] & ( 1L << ( index & 31 ) ) )
		{
			tmp->mul( a, b );
			iswap( b, tmp );
		}
		
		if( ++index >= bits )
			break;
		
		tmp->mul( a, a );
		iswap( a, tmp );
	}

	set( b );
}

void
biguint_t::powmod(
	biguint_t*			m,
	biguint_t*			n,
	biguint_t*			modulo )
{	
	long				bits = n->bitlen();
	
	if( bits < 1 )
	{
		set_int( 1 );
		return;
	}

	biguint_t			ta( m_pool );
	biguint_t			tb( m_pool );
	biguint_t			tt( m_pool );

	biguint_t*			a = &ta;
	biguint_t*			b = &tb;
	biguint_t*			tmp = &tt;
	
	b->set_int( 1 );
	a->set( m );
	
	long				index = 0;
	
	digit_t*			nd = n->m_digits + n->m_size - 1;
	
	while( true )
	{
		if( nd[ -( index >> 5 ) ] & ( 1L << ( index & 31 ) ) )
		{
			tmp->mul( a, b );
			iswap( b, tmp );
			
			div( b, modulo, 0, tmp );
			iswap( b, tmp );
		}
		
		if( ++index >= bits )
			break;
		
		tmp->mul( a, a );
		iswap( a, tmp );
		
		div( a, modulo, 0, tmp );
		iswap( a, tmp );
	}

	set( b );
}

void
biguint_t::sqrt(
	biguint_t*			x )
{
	if( x->zero() )
	{
		set_int( 0 );
		return;
	}
	
	biguint_t			tt1( m_pool );
	biguint_t			tt2( m_pool );
	
	biguint_t*			y = this;
	biguint_t*			q = &tt1;
	biguint_t*			tmp = &tt2;
	
	y->shift_right( x, 1 );
	
	while( true )
	{
		div( x, y, q, 0 );
		if( compare( q, y ) >= 0 )
			break;
			
		tmp->add( y, q );
		y->shift_right( tmp, 1 );
	}
}

// ---------------------------------------------------------------------------

#define BITFUNC_COMMON								\
	long				size = m_size;				\
	long				bits = size << 5;			\
													\
	if( index < 0 || index >= bits )				\
		return false;								\
													\
	long				offset;						\
													\
	offset = size - 1 - ( index >> 5 );

bool
biguint_t::bclear(
	long				index )
{
	BITFUNC_COMMON
	m_digits[ offset ] &= ~( 1L << ( index & 31 ) );
	return true;
}

bool
biguint_t::bset(
	long				index )
{
	BITFUNC_COMMON
	m_digits[ offset ] |= ( 1L << ( index & 31 ) );
	return true;
}

bool
biguint_t::bchg(
	long				index )
{
	BITFUNC_COMMON
	m_digits[ offset ] ^= ( 1L << ( index & 31 ) );
	return true;
}

bool
biguint_t::btst(
	long				index )
{
	BITFUNC_COMMON
	return m_digits[ offset ] & ( 1L << ( index & 31 ) );
}

void
biguint_t::random(
	biguint_t*			n )
{
	biguint_t			tmp( m_pool );
	long				size = n->m_size;

	tmp.allocate( size );

	digit_t*			cd = tmp.m_digits;
	
	for( long i = 0; i < size; i++ )
		cd[ i ] = ranint();
	
	div( &tmp, n, 0, this );
}

bool
biguint_t::set_string(
	const char*			text,
	long				length )
{
	biguint_t			a( m_pool );
	biguint_t			b( m_pool );

	biguint_t			decimal( m_pool );
	biguint_t			base( m_pool );
	bool				nonzero = false;

	a.set_int( 0 );
	base.set_int( 10 );

	for( long i = 0; i < length; i++ )
	{
		char			c = text[ i ];
		
		if( not iisdigit( c ) )
			break;
			
		int				value = c - '0';

		if( value )
			nonzero = true;
		else if( not nonzero )
			continue;
						
		b.mul( &a, &base );
		decimal.set_int( value );
		a.add( &b, &decimal );
	}

	set( &a );

	return nonzero;
}

void
biguint_t::to_string(
	istring&			text,
	int					intbase )
{
	if( intbase == 2 )
	{
		to_binary( text );
		return;
	}
	else if( intbase == 16 )
	{
		to_hex( text );
		return;
	}

	size_t				blen = bitlen();

	if( blen < 1 )
	{
		text.assign( "0" );
		return;
	}

	istring				digits;
	
	biguint_t			base( m_pool );
	biguint_t			number( m_pool );

	base.set_int( intbase );
	number.set( this );

	biguint_t			quotient( m_pool );
	biguint_t			remainder( m_pool );
	
	do {
		/*if( number.m_size < 2 &&
			number.lsd() % 100 && number.lsd() != 1 )
			Debugger();*/

		div( &number, &base, &quotient, &remainder );
		
		char			c;
		
		c = remainder.lsd() + '0';
		digits.append( 1, c );
		
		/*if( quotient.m_size < 2 &&
			quotient.lsd() % 100 && quotient.lsd() > 10 )
			Debugger();*/

	} while( number.set( &quotient ) );
	
	
	for( long i = digits.length() - 1; i >= 0; i-- )
		text.append( 1, digits[ i ] );
}

u64
biguint_t::integer() const
{
	const digit_t*		cd = m_digits;
	size_t				size = m_size;
	u64					value;
	size_t				bits;
	
	if( size <= 1 )
		bits = 32;
	else if( size <= 2 )
		bits = 64;
	else
		bits = bitlen();

	if( bits <= 32 )
		value = cd[ size - 1 ];
	else if( bits <= 64 )
	{
		value = cd[ size - 2 ];
		value <<= 32;
		value |= cd[ size - 1 ];
	}
	else
#if __VISC__
		value = 0x7fffffffffffffff;
#else
		value = 0x7fffffffffffffffULL;
#endif

	return value;
}

void
biguint_t::to_binary(
	istring&			text )
{
	digit_t*			p = m_digits;
	size_t				count = m_size;
	bool				nonnull = false;
	
	if( count < 1 )
	{
		text = "0";
		return;
	}

	do {
		digit_t			digit = *p++;
		
		for( int i = 31; i >= 0; i-- )
		{
			if( ( digit >> i ) & 1 )
			{
				text.append( 1, '1' );
				nonnull = true;
			}
			else if( nonnull )
				text.append( 1, '0' );
		}
		
	} while( --count );
	
	if( not nonnull )
		text = "0";
}

void
biguint_t::to_hex(
	istring&			text )
{
	const static char	hextable[ 17 ] = "0123456789ABCDEF";

	digit_t*			p = m_digits;
	size_t				count = m_size;
	bool				nonnull = false;
	
	if( count < 1 )
	{
		text = "0";
		return;
	}
	
	do {
		digit_t			digit = *p++;
		
		for( int shift = 32 - 4; shift >= 0; shift -= 4 )
		{
			int			value;
		
			value = ( digit >> shift ) & 15;
		
			if( value )
			{
				text.append( 1, hextable[ value ] );
				nonnull = true;
			}
			else if( nonnull )
				text.append( 1, '0' );
		}
		
	} while( --count );
	
	if( not nonnull )
		text = "0";
}

END_MACHINE_NAMESPACE
