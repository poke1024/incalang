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
//	grep_t.cpp
// ===========================================================================

// based on public domain UNIX grep utility source code

// This implementation is essentially the same as what you'd come
// across in an MS-DOS, CP/M, AmigaDOS or UNIX implementation, and
// was derived from the original 1980 DECUS source code. Since that
// source code is in the public domain, so is this implementation of it.

#include "grep_t.h"
#include <ctype.h>

BEGIN_MACHINE_NAMESPACE

enum {
	CHAR = 1,		// Any character
	BOL,			// Start of line
	EOL,			// End of line
	ANY,			// Wildcard character
	CLASS,			// Class of characters
	NOTCLASS,		// Not Class
	ZERO_OR_MORE,
	ONE_OR_MORE,
	OR,
	ALPHA,
	DIGIT,
	HEXDIGIT,
	QUOTE,
	PUNCT,			// Punctuation character ,.; etc.
	NALPHA,			// Alphanumeric
	WHITESPACE,		// Whitespace
	RANGE,			// Range of characters
	ENDPAT
};

#define TOLOWER( x )			( ( m_flags & 1 ) ? (x) : tolower((x)) )

grep_t::grep_t()
{
	m_pbuf.reserve( 32 );
	m_flags = 1;
}

void
grep_t::set_pattern(
	const char*					source,
	long						length,
	int							flags )
{
	m_flags = flags;

	const char*					s;		// source string pointer
	//const char*				lp;		// last pattern pointer
	size_t						lp;
	int							c;		// current character
	int							o;		// temp
	//char*						spp;	// save beginning of pattern
	const char*					t;

    s = source;
	t = s + length;

    //pp = pbuf;
    //lp = pp;
	m_pbuf.clear();
	lp = 0;

	while( s < t )
	{
		c = *s++;

		// ZERO_OR_MORE, ONE_OR_MORE and OR are special
        if( c == '*' || c == '+' || c == '-' )
		{
			size_t					len = m_pbuf.length();

			if( len == 0 ||
				( o = m_pbuf[ len - 1 ] ) == BOL ||
				o == EOL || o == ZERO_OR_MORE ||
				o == ONE_OR_MORE || o == OR )
			{
                badpat( "illegal operator", source, s );
			}

            store( ENDPAT );
            store( ENDPAT );
            
			size_t					pp = m_pbuf.length();
			
			while( --pp > lp )				// Move pattern down
				m_pbuf[ pp ] = m_pbuf[ pp -1 ];	// one byte

			m_pbuf[ pp ] = ( c == '*' ) ?
				ZERO_OR_MORE : ( c == '-' ) ? OR : ONE_OR_MORE;

            continue;
		}
		
		// All the rest.
		
		//lp = pp;						// Remember start
		lp = m_pbuf.length();

		switch( c )
		{
			case '^':
				store( BOL );
				break;

			case '$':
				store( EOL );
				break;

			case '.':
				store( ANY );
				break;

			case '[':
				s = cclass( source, s );
				break;

			case ':':
				if( *s )
				{
					c = *s++;
					switch( tolower( c ) )
					{
						case 'a':
							store(ALPHA);
							break;
						case 'd':
							store(DIGIT);
							break;
						case 'n':
							store(NALPHA);
							break;
						case 'b':
							store(WHITESPACE);
							break;
						case 'x':
							store(HEXDIGIT);
							break;
						case 'p':
							store(PUNCT);
							break;
						case 'q':
							store(QUOTE);
							break;
						default:
							badpat( "unknown predefined class", source, s );
					}
					break;
				}
				else
					badpat( "':' needs class", source, s );

			case '\\':
				if( *s )
					c = *s++;

			default:
				store( CHAR );
				store( TOLOWER( c ) );
		}
    }

    store( ENDPAT );
    store( 0 );							// Terminate string
}

const char*
grep_t::cclass(
	const char*					source,
	const char*					src )
{
	const char*					s;		// Source pointer
    //const char*				cp;		// Pattern start
	size_t						cp;
    int							c;		// Current character
    int							o;		// Temp

    s = src;
    o = CLASS;

    if( *s == '^' )
	{
        ++s;
        o = NOTCLASS;
    }

    store( o );
	//cp = pp;
	cp = m_pbuf.length();
    store( 0 );							// Byte count

    while( ( ( c = *s++ ) != 0 ) && c != ']' )
	{
		size_t					pp = m_pbuf.length();

        if( c == '\\' )					// Store quoted char
		{
            if( ( c = *s++ ) == '\0' )	// Gotta get something
                badpat( "bad terminator", source, s );
            else
                store( TOLOWER( c ) );
        }
        else if( c == '-' && ( pp - cp ) > 1 &&
			*s != ']' && *s != '\0' )
		{
            c = m_pbuf[ pp - 1 ];			// Range start
            m_pbuf[ pp - 1 ] = RANGE;		// Range signal
            store( c );					// Re-store start
            c = *s++;					// Get end char and
            store( TOLOWER( c ) );		// Store it
        }
        else
		{
            store( TOLOWER( c ) );		// Store normal char
        }
    }

    if( c != ']' )
        badpat( "need ']'", source, s );

    if( ( c = ( m_pbuf.length() - cp ) ) >= 256 )
        badpat( "class too large", source, s );

    if( c == 0 )
        badpat( "empty class", source, s );

    m_pbuf[ cp ] = c;

    return s;
}

void
grep_t::store(
	int							op )
{
	m_pbuf.append( 1, op );
}

void
grep_t::badpat(
	const char*					err,
	const char*					s,
	const char*					t )
{
	istring						text;
	char						buf[ 32 ];
	text = "illegal grep pattern";
	sprintf( buf, " (%ld): ", (long)( t - s ) );
	text.append( buf );
	text.append( err );
	throw_icarus_error( text.c_str() );
}

const char*
grep_t::match(
	const char*					lbuf )
{
	const char*					l;

	for( l = lbuf; *l; l++ )
	{
		if( pmatch( lbuf, l, m_pbuf.data() ) )
			return l;
	}

	return 0;
}

const char*
grep_t::pmatch(
	const char*					lbuf,
	const char*					line,
	const char*					pattern )
{
	const char*					l;		// Current line pointer
	const char*					p;		// Current pattern pointer
	char						c;		// Current character
	const char*					e;		// End for ZERO_OR_MORE and ONE_OR_MORE match
	int							op;		// Pattern operation
	int							n;		// Class counter
	const char*					are;	// Start of ZERO_OR_MORE match

    l = line;
    p = pattern;

    while( ( op = *p++ ) != ENDPAT )
	{
        switch( op )
		{
            case CHAR:
                if( TOLOWER( *l ) != *p++ )
                    return 0;
                l++;
                break;

            case BOL:
                if( l != lbuf )
                    return 0;
                break;

            case EOL:
                if( *l != '\0' )
                    return 0;
                break;

            case ANY:
                if( *l++ == '\0' )
                    return 0;
                break;

            case DIGIT:
                if( ( c = *l++ ) < '0' || ( c > '9' ) )
                    return 0;
                break;

            case HEXDIGIT:
                c = *l++;
                if( !isxdigit( c ) )
                    return 0;
                break;

            case PUNCT:
                c = *l++;
                if( !ispunct( c ) )
                    return 0;
                break;

            case QUOTE:
                c = *l++;
                if( !( c == '\"' || c == '\'' ) )
                    return 0;
                break;

            case ALPHA:
                c = *l++;
                if( !isalpha( c ) )
                    return 0;
                break;

            case NALPHA:
                c=*l++;
                if( !isalnum( c ) )
                    return 0;
                break;

            case WHITESPACE:
                c = *l++;
                if( c == 0 || !isspace( c ) )
                    return 0;
                break;

            case CLASS:
            case NOTCLASS:
                c = TOLOWER( *l );
                l++;
                n = *p++ & 0xff;

                do
				{
                    if( *p == RANGE )
					{
                        p += 3;
                        n -= 2;
                        if( c >= p[ -2 ] && c <= p[ -1 ] )
                            break;
                    }
                    else if( c == *p++ )
                        break;

                } while( --n > 1 );

                if( ( op == CLASS ) == ( n <= 1 ) )
                    return 0;

                if( op == CLASS )
                    p += n - 2;
                break;

            case OR:
                e = pmatch( lbuf, l, p );           // Look for a match

                while( *p++ != ENDPAT )
                    ;                               // Skip over pattern
                if( e )                             // Got a match?
                    l = e;                          // Yes, update string
                break;                              // Always succeeds

            case ONE_OR_MORE:                       // One or more ...
                if( ( l = pmatch( lbuf, l, p ) ) == 0 )
                    return 0;                       // Gotta have a match

            case ZERO_OR_MORE:                      // Zero or more ...
                are = l;                            // Remember line start

                while( *l && ( ( e = pmatch( lbuf, l, p ) ) != 0 ) )
                    l = e;                          // Get longest match

                while( *p++ != ENDPAT )
                    ;                               // Skip over pattern

                while( l >= are )                   // Try to match rest
				{
                    if( ( e = pmatch( lbuf, l, p ) ) != 0 )
                        return e;
                    --l;                            // Nope, try earlier
                }
                return 0;                           // Nothing else worked

            default:
				throw_icarus_error( "bad grep operator" );
        }
    }

    return l;
}

END_MACHINE_NAMESPACE
