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
//	string_t.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "string_t.h"
#include "grep_t.h"

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

static char*
alloc_text(
	machine_t&			machine,
	long				length )
{
	if( length <= 0 )
		return 0;

	length += 1;

	return (char*)machine.m_lib_static.string_pool.alloc(
		length * sizeof( char ) );
}

static void
free_text(
	machine_t&			machine,
	char*				text,
	long				length )
{
	if( length <= 0 )
		return;

	length += 1;

	machine.m_lib_static.string_pool.free( text, length );
}

static grep_t*
find_grep(
	machine_t&			machine,
	const char*			pattern,
	long				length,
	int					flags )
{
	grep_t**			pool = machine.m_lib_static.grep_pool;
	int					j = machine.m_lib_static.grep_index;
	int					i;
	grep_t*				grep;
	int					slot = -1;

	for( i = 0; i < GREP_POOL_SIZE; i++ )
	{
		grep = pool[ ( j + i ) & GREP_POOL_MASK ];

		if( grep == 0 )
			slot = i;
		else if( grep->has_pattern( pattern, length, flags ) )
			break;
	}

	if( i < GREP_POOL_SIZE )
	{
		iswap( pool[ i ], pool[ j ] );
	}
	else if( slot >= 0 )
	{
		pool[ slot ] = grep = new grep_t;
		grep->set_pattern( pattern, length, flags );
	}
	else
	{
		grep = pool[ j ];
		grep->set_pattern( pattern, length, flags );
	}

	machine.m_lib_static.grep_index =
		( j + 1 ) & GREP_POOL_MASK;

	return grep;
}

// ---------------------------------------------------------------------------

void
string_create(
	machine_t&			m,
	string_t*			self,
	const char*			text,
	u32					length )
{
	self->length = length;

	if( length < 1 )
		self->text = 0;
	else
	{
		char*			mtext;
	
		mtext = alloc_text( m, length );
		imemcpy( mtext, text, length * sizeof( char ) );

		self->text = mtext;
	}
}

void
string_dispose(
	machine_t&			m,
	string_t*			self )
{
	char*				text = self->text;
	
	if( text )
		free_text( m, text, self->length );
}

void
string_assign(
	machine_t&			m,
	string_t*			self,
	string_t*			what )
{
	string_dispose( m, self );
	
	string_create( m, self, what->text, what->length );
}

void
string_add(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	string_t*			b )
{
	long				lena = a->length;
	long				lenb = b->length;
	long				lenr = lena + lenb;
	
	if( lenr < 1 )
		r->text = 0;
	else
	{
		char*			text;
	
		text = alloc_text( m, lenr );
		imemcpy( text, a->text,
			lena * sizeof( char ) );
		imemcpy( text + lena, b->text,
			lenb * sizeof( char ) );
			
		r->text = text;
	}

	r->length = lenr;
}

void
string_string(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				count )
{
	long				lena = a->length;
	long				length;
	
	if( lena < 1 || count < 1 )
	{
		r->text = 0;
		length = 0;
	}
	else
	{
		length = lena * count;
	
		char*			text;
	
		text = alloc_text( m, length );
		r->text = text;
		
		char*			p = text;
		const char*		q = a->text;
		
		if( lena == 1 )
		{
			char		c = *q;
		
			do {
				*p++ = c;
			} while( --count );
		}
		else
		{
			do {
				imemcpy( p, q, lena * sizeof( char ) );
				p += lena;
			} while( --count );
		}
	}
	
	r->length = length;
}

void
string_upper(
	machine_t&			m,
	string_t*			r,
	string_t*			a )
{
	long				length = a->length;
	
	if( length < 1 )
		r->text = 0;
	else
	{
		char*			text;
	
		text = alloc_text( m, length );

		char*			p = text;
		char*			q = a->text;
		
		for( long i = 0; i < length; i++ )
			*p++ = itoupper( *q++ );
					
		r->text = text;
	}
		
	r->length = length;
}

void
string_lower(
	machine_t&			m,
	string_t*			r,
	string_t*			a )
{
	long				length = a->length;
	
	if( length < 1 )
		r->text = 0;
	else
	{
		char*			text;
	
		text = alloc_text( m, length );

		char*			p = text;
		char*			q = a->text;
		
		for( long i = 0; i < length; i++ )
			*p++ = itolower( *q++ );
					
		r->text = text;
	}
		
	r->length = length;
}

void
string_left(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				count )
{
	long				length = a->length;

	if( count > length )
		count = length;
	
	if( count < 1 )
	{
		r->text = 0;
		count = 0;
	}
	else
	{		
		char*			text;
	
		text = alloc_text( m, count );

		imemcpy(
			text, a->text,
			count * sizeof( char ) );
					
		r->text = text;
	}
		
	r->length = count;
}

void
string_right(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				count )
{
	long				length = a->length;

	if( count > length )
		count = length;
	
	if( count < 1 )
	{
		r->text = 0;
		count = 0;
	}
	else
	{		
		char*			text;
	
		text = alloc_text( m, count );

		imemcpy(
			text, a->text + length - count,
			count * sizeof( char ) );
					
		r->text = text;
	}
		
	r->length = count;
}

void
string_mid(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				index )
{
	long				length = a->length;
	long				count;

	if( index < 0 )
		index = 0;

	count = length - index;
	
	if( count < 1 )
	{
		r->text = 0;
		count = 0;
	}
	else
	{		
		char*			text;
	
		text = alloc_text( m, count );

		imemcpy(
			text, a->text + index,
			count * sizeof( char ) );
					
		r->text = text;
	}
		
	r->length = count;
}

void
string_mid(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				index,
	long				count )
{
	long				length = a->length;

	if( index < 0 )
	{
		count += index;
		index = 0;
	}

	if( index + count > length )
		count = length - index;
	
	if( count < 1 )
	{
		r->text = 0;
		count = 0;
	}
	else
	{		
		char*			text;
	
		text = alloc_text( m, count );

		imemcpy(
			text, a->text + index,
			count * sizeof( char ) );
					
		r->text = text;
	}
		
	r->length = count;
}

long
string_instr(
	machine_t&			/*m*/,
	string_t*			self,
	string_t*			what,
	long				index )
{
	long				patlen = what->length;

	if( patlen < 1 )
		return -1;

	long				buflen = self->length;
	
	buflen -= patlen - 1;

	if( index < 0 )
		index = 0;

	if( index >= buflen )
		return -1;
			
	const char*			s0 = self->text + index;
	const char*			s1 = s0;
	long				count = buflen - index;

	const char*			p1 = what->text;
	char				c1;
	char				firstc = *p1++;
	
	if( ( patlen -= 1 ) == 0 )
	{
		do {
			c1 = *s1++;
			if( c1 == firstc )
				return s1 - s0 - 1;
		} while( --count );
	}
	else
	{
		while( true )
		{
			c1 = *s1++;
		
			if( c1 == firstc )
			{
				const char*	s2 = s1;
				const char*	p2 = p1;
				char		c2;
				long		len = patlen;

				do {
					c1 = *s2++;
					c2 = *p2++;
				
					if( c1 != c2 )
						goto mismatch;
						
				} while( --len );
				
				return s1 - s0 - 1;
			}
			
mismatch:
			if( --count == 0 )
				break;
		}
	}
	
	return -1;
}

long
string_grep(
	machine_t&			m,
	string_t*			self,
	string_t*			pattern,
	long				index,
	int					flags )
{
	if( pattern->length < 1 )
		return -1;

	long				length = self->length;

	if( length < 1 )
		return -1;

	if( index < 0 )
		index = 0;

	if( index >= length )
		return -1;

	grep_t*				grep;
	const char*			s;
	const char*			t;

	grep = find_grep( m,
		string_get_c_str( pattern ),
		pattern->length,
		flags );

	s = string_get_c_str( self );
	t = grep->match( s + index );

	if( t == 0 )
		return -1;

	return t - s;
}

int
string_asc(
	machine_t&			/*m*/,
	string_t*			self )
{
	if( self->length < 1 )
		return 0;
		
	return self->text[ 0 ];
}

double
string_val(
	machine_t&			/*m*/,
	string_t*			self )
{
	long				length = self->length;

	if( length < 1 )
		return 0;
		
	char*				text = self->text;
		
	text[ length ] = '\0';
		
	return istrtod( text, 0 );
}

s64
string_ival(
	machine_t&			/*m*/,
	string_t*			self )
{
	long				length = self->length;

	if( length < 1 )
		return 0;
		
	char*				text = self->text;
		
	text[ length ] = '\0';

	return istrtoll( text, 0, 10 );
}

int
string_valn(
	machine_t&			/*m*/,
	string_t*			self )
{
	long				length = self->length;

	if( length < 1 )
		return 0;
		
	char*				text = self->text;
	char*				p = 0;
		
	text[ length ] = '\0';
		
	istrtod( text, &p );

	return p - text;
}

bool
string_equal(
	machine_t&			/*m*/,
	string_t*			a,
	string_t*			b )
{
	long				length = a->length;

	if( length != b->length )
		return false;
		
	return istrncmp(
		a->text, b->text,
		length ) == 0;
}

int
string_compare(
	machine_t&			/*m*/,
	string_t*			a,
	string_t*			b )
{
	long				lena = a->length;
	long				lenb = b->length;
	int					cmp;

	if( lena > lenb )
	{
		if( ( cmp = istrncmp( a->text, b->text, lenb ) ) != 0 )
			return cmp;

		return 1;
	}
	else if( lena < lenb )
	{
		if( ( cmp = istrncmp( a->text, b->text, lena ) ) != 0 )
			return cmp;

		return -1;
	}
	
	return istrncmp( a->text, b->text, lena );
}

const char*
string_get_c_str(
	string_t*			self )
{
	long				length = self->length;

	if( length < 1 )
		return 0;
		
	char*				text = self->text;
	text[ length ] = '\0';

	return text;
}

END_MACHINE_NAMESPACE
