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
//	icarus_string.cp	 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

istring::istring()
{
	init();
}

istring::istring(
	const char*			s )
{
	init();
	assign( s );
}

istring::istring(
	const char*			s,
	isize_t				length )
{
	init();
	assign( s, length );
}

istring::istring(
	const istring&		s )
{
	init();
	assign( s );
}

istring::~istring()
{
	if( m_data )
		ifree( m_data );
}

void
istring::operator=(
	const char*			s )
{
	assign( s );
}

void
istring::operator=(
	const istring&		s )
{
	assign( s.m_data, s.m_length );
}

void
istring::assign(
	const char*			s )
{
	assign( s, istrlen( s ) );
}

void
istring::assign(
	const istring&		s )
{
	assign( s.m_data, s.m_length );
}

void
istring::assign(
	const char*			s,
	isize_t				length )
{
	reserve( length );
	m_length = length;
	imemcpy( m_data, s, length * sizeof( char ) );
	m_data[ m_length ] = '\0';
}

void
istring::append(
	const char*			s )
{
	append( s, istrlen( s ) );
}

void
istring::append(
	const istring&		s )
{
	append( s.m_data, s.m_length );
}

void
istring::append(
	const char*			s,
	isize_t				length )
{
	reserve( m_length + length );
	imemcpy( m_data + m_length, s, length * sizeof( char ) );
	m_length += length;
	m_data[ m_length ] = '\0';
}

void
istring::append(
	isize_t				count,
	char				c )
{
	reserve( m_length + count );
	
	char*				s = m_data + m_length;
	for( long i = 0; i < (long)count; i++ )
		*s++ = c;
	*s = '\0';
		
	m_length += count;
}

void
istring::insert(
	long				pos,
	const char*			s )
{
	insert( pos, s, istrlen( s ) );
}

void
istring::insert(
	long				pos,
	const char*			s,
	isize_t				len )
{
	if( pos < 0 )
		pos = 0;
	if( pos > (long)m_length )
		pos = m_length;

	reserve( m_length + len );

	char*				data = m_data;

	imemmove( data + pos + len, data + pos,
		( m_length - pos + 1 ) * sizeof( char ) );
	
	imemcpy( data + pos, s, len * sizeof( char ) );

	m_length += len;
}

void
istring::erase(
	long				from )
{
	m_length = imin( imax( from, 0L ), (long)m_length );
	m_data[ m_length ] = '\0';
}

istring
istring::substr(
	long				from ) const
{
	if( from < 0 )
		from = 0;
	if( from >= (long)m_length )
		return istring();
		
	return istring( &m_data[ from ], m_length - from );
}

istring
istring::substr(
	long				from,
	long				n ) const
{
	if( from < 0 )
		from = 0;
	if( from >= (long)m_length )
		return istring();
		
	if( from + n > (long)m_length )
		n = m_length - from;
	if( n < 1 )
		return istring();
		
	return istring( &m_data[ from ], n );
}

long
istring::rfind(
	const char*			pattern ) const
{
	isize_t				patlen = istrlen( pattern );
	const char*			data = m_data;

	for( long i = m_length - patlen - 1; i >= 0; i-- )
	{
		if( imemcmp( &data[ i ],
			pattern, patlen * sizeof( char ) ) == 0 )
		{
			return i;
		}
	}
	
	return -1;
}

long
istring::rfind(
	const char*			pattern,
	long				from ) const
{
	isize_t				patlen = istrlen( pattern );

	if( from >= (long)( m_length - patlen ) )
		from = m_length - patlen - 1;

	if( from < 0 )
		return -1;

	const char*			data = m_data;

	for( long i = from; i >= 0; i-- )
	{
		if( imemcmp( &data[ i ],
			pattern, patlen * sizeof( char ) ) == 0 )
		{
			return i;
		}
	}
	
	return -1;
}

void
istring::reserve(
	isize_t				length )
{
	length += 1;

	if( length <= m_alloc )
		return;
		
	isize_t				alloc = m_alloc;
		
	while( alloc < length )
		alloc <<= 1;
		
	char*				data;
		
	data = (char*)irealloc( m_data, alloc * sizeof( char ) );
	if( data == 0 )
		throw std::bad_alloc();

	m_data = data;
	m_alloc = alloc;
}

void
istring::clear()
{
	m_length = 0;
	m_data[ 0 ] = '\0';
}

void
istring::init()
{
	m_data = (char*)imalloc( sizeof( char ) );
	if( m_data == 0 )
		throw std::bad_alloc();
	m_data[ 0 ] = '\0';
	
	m_length = 0;
	m_alloc = 1;
}

END_ICARUS_NAMESPACE
