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
//	variable_t.cpp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "variable_t.h"
#include "ariadne_messages.h"
#include "variable_array_t.h"
#include "array_t.h"
#include "htab_t.h"

BEGIN_AKLABETH_NAMESPACE

static htab_t*			s_system_globals = 0;

#define snprintf		_snprintf

static bool
string_value(
	istring&			value,
	void*				addr,
	random_stream_t*	stream )
{
	u08*				p = (u08*)addr;
	void*				text;
	u32					length;

	p += sizeof( void* ); // skip header

	stream->write_u32( msg_ReadPointer );
	stream->write_ptr( p );
	if( stream->read_u08() == 0 )
		return false;
	text = stream->read_ptr();

	stream->write_u32( msg_ReadQuad );
	stream->write_ptr( p + sizeof( void* ) );
	if( stream->read_u08() == 0 )
		return false;
	length = stream->read_u32();
	length *= sizeof( char );

	stream->write_u32( msg_ReadBlock );
	stream->write_ptr( text );
	stream->write_u32( length );
	if( stream->read_u08() == 0 )
		return false;

	value = "\"";
	if( length > 0 )
	{
		value.append( length, ' ' );
		stream->read_bytes( &value[ 1 ], length );
	}
	value.append( "\"" );

	return true;
}

static bool
atomic_value(
	char*				buf,
	int					bufsize,
	const char*			type,
	long				typelen,
	void*				addr,
	random_stream_t*	stream )
{
	if( typelen == 1 )
	{
		switch( *type )
		{
			case 'c':
			{
				s08		val;
				stream->write_u32( msg_ReadByte );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				val = stream->read_u32();
				snprintf( buf, bufsize,
					"'%c' (%d)", (char)val, (int)val );
				break;
			}

			case 'b':
				stream->write_u32( msg_ReadByte );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%ld", (long)((s08)stream->read_u32()) );
				break;

			case 'B':
				stream->write_u32( msg_ReadByte );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%ld", (long)((u08)stream->read_u32()) );
				break;
				
			case 's':
				stream->write_u32( msg_ReadWord );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%ld", (long)((s16)stream->read_u32()) );
				break;

			case 'S':
				stream->write_u32( msg_ReadWord );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%ld", (long)((u16)stream->read_u32()) );
				break;

			case 'i':
				stream->write_u32( msg_ReadQuad );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%ld", (long)((s32)stream->read_u32()) );
				break;

			case 'I':
				stream->write_u32( msg_ReadQuad );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%lu", (long)((u32)stream->read_u32()) );
				break;

			case 'l':
				stream->write_u32( msg_ReadOcta );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%I64d", (s64)stream->read_u64() );
				break;

			case 'L':
				stream->write_u32( msg_ReadOcta );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%I64u", (u64)stream->read_u64() );
				break;

			case 'a':
				stream->write_u32( msg_ReadByte );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				if( stream->read_u32() )
					strcpy( buf, "true" );
				else
					strcpy( buf, "false" );
				break;

			case 'f':
				stream->write_u32( msg_ReadFloat );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%g", (float)stream->read_f32() );
				break;

			case 'd':
				stream->write_u32( msg_ReadDouble );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"%lg", (double)stream->read_f64() );
				break;
		}
	}
	
	return true;
}

static int
raw_typelen(
	const char*			type,
	int					typelen )
{
	while( typelen > 0 )
		if( type[ typelen - 1 ] == '&' ||
			type[ typelen - 1 ] == '*' )
			typelen--;
		else
			break;

	return typelen;
}

static bool
is_string_type(
	const char*			type,
	int					typelen )
{
	if( typelen == 8 &&
		memcmp( type, "String::", 8 * sizeof( char ) ) == 0 )
		return true;

	return false;
}

static bool
variable_value(
	istring&			value,
	const char*			type,
	long				typelen,
	void*				addr,
	random_stream_t*	stream )
{
	if( addr == 0 )
	{
		value = "<null>";
		return true;
	}

	if( is_string_type( type, typelen ) )
	{
		return string_value( value, addr, stream );
	}

	if( typelen >= 1 )
	{
		switch( *type )
		{
			case '.':
			case ':':
				return variable_value( value,
					type + 1, typelen - 1, addr, stream );
		}
	}

	static char			buf[ 128 + 1 ];
	long				bufsize = 128;

	if( typelen == 1 )
	{
		if( not atomic_value( buf, bufsize,
			type, typelen, addr, stream ) )
			return false;

		value = buf;
		return true;
	}
	else if( typelen > 1 )
	{
		switch( type[ typelen - 1 ] )
		{
			case '*':
			default:
				stream->write_u32( msg_ReadPointer );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				snprintf( buf, bufsize,
					"0x%.8lx", (long)stream->read_ptr() );
				value = buf;
				break;
				
			case '&':
				stream->write_u32( msg_ReadPointer );
				stream->write_ptr( addr );
				if( stream->read_u08() == 0 )
					return false;
				return variable_value( value,
					type, typelen - 1, stream->read_ptr(), stream );		
		}
	}

	return true;
}

// ===========================================================================

variable_t::variable_t()
{
	m_childs = 0;

	m_expanded = false;
}

variable_t::~variable_t()
{
	if( m_childs )
		delete m_childs;
}

void
variable_t::init_as_variable(
	random_stream_t*	stream,
	u08*				base_addr,
	int					ident )
{
	stream->read_string( m_name );
	stream->read_string( m_type );
	m_offset = stream->read_u32();
	m_base_addr = base_addr;
	m_ident = ident;
}

void
variable_t::init_as_globals(
	random_stream_t*	stream,
	void*				clss )
{
	m_name = "globals";
	m_offset = 0;
	m_base_addr = 0;
	m_ident = 0;
	m_clss = 0;
	m_flags = 0;
	setup_globals( stream, clss );
}

void
variable_t::init_as_separator(
	const char*			name,
	int					ident )
{
	m_name = name;
	m_offset = 0;
	m_base_addr = 0;
	m_ident = ident;
	m_clss = 0;
	m_flags = 0;
}

void
variable_t::setup(
	random_stream_t*	stream )
{
	const char*			type = m_type.c_str();
	long				typelen = m_type.length();
	u08*				baseaddr = (u08*)m_base_addr;
	
	if( typelen == 0 )
	{
		m_value = "";
		return;
	}
	
	stream->write_u32( msg_FindTypeClass );
	stream->write_string( type );

	m_clss = stream->read_ptr();
	m_deref = stream->read_u32();
	m_flags = stream->read_u08();

	int					rtypelen;

	rtypelen = raw_typelen( type, typelen );

	if( is_string_type( type, rtypelen ) )
		m_clss = 0;

	if( m_clss )
	{
		char			buf[ 64 ];

		snprintf( buf, 63,
			"0x%.8lx", (long)( baseaddr + m_offset ) );
		m_value = buf;
	}
	else
	{
		m_value = "<illegal address>";
		variable_value( m_value, type, typelen,
			baseaddr + m_offset, stream );
	}
}

const char*
variable_t::name()
{
	return m_name.c_str();
}

const char*
variable_t::value()
{
	return m_value.c_str();
}

bool
variable_t::is_expandable()
{
	if( m_childs )
		return true;

	if( m_clss && ( m_flags & 1 ) )
		return true;

	return false;
}

variable_array_t*
variable_t::childs(
	random_stream_t*	stream )
{
	if( m_childs == 0 )
		setup_childs( stream );
	return m_childs;
}

void
variable_t::setup_childs(
	random_stream_t*	stream )
{
	bool				annotate;
	u08*				addr = m_base_addr + m_offset;

	if( m_childs || m_clss == 0 )
		return;

	m_childs = new variable_array_t;

	for( long i = 0; i < m_deref; i++ )
	{
		stream->write_u32( msg_ReadPointer );
		stream->write_ptr( addr );
		if( stream->read_u08() == 0 )
			return;
		addr = (u08*)stream->read_ptr();
	}

	stream->write_u32( msg_ExpandClass );
	stream->write_ptr( m_clss );
	
	annotate = stream->read_u08();
	while( stream->read_u08() )
	{
		if( annotate )
		{
			istring		s;
			istring		t;

			s = "<class ";
			stream->read_string( t );
			s.append( t );
			s.append( ">" );

			variable_t*	item;
			item = new variable_t;
			item->init_as_separator(
				s.c_str(), m_ident + 1 );
			m_childs->append( item );
		}
			
		long			count;

		count = stream->read_u32();
		
		for( long i = 0; i < count; i++ )
		{
			variable_t*	item;
			item = new variable_t;
			item->init_as_variable(
				stream, addr, m_ident + 1 );
			m_childs->append( item );
		}
	}
	
	for( long i = m_childs->count() - 1; i >= 0; i-- )
		m_childs->at( i )->setup( stream );
}

void
variable_t::setup_globals(
	random_stream_t*	stream,
	void*				clss )
{
	if( m_childs )
		return;

	bool				first = true;

	m_childs = new variable_array_t;

	stream->write_u32( msg_ExpandGlobals );
	stream->write_ptr( clss );

	while( stream->read_u08() )
	{
		istring			s;
		istring			t;
		bool			is_global_scope;

		stream->read_string( t );
		if( strcmp( t.c_str(), "root" ) == 0 )
		{
			is_global_scope = true;
			if( not first )
				s = "<global scope>";
		}	
		else
		{
			is_global_scope = false;
			s = "<class ";
			s.append( t );
			s.append( ">" );
		}

		if( s.length() > 0 )
		{
			variable_t*	item;
			item = new variable_t;
			item->init_as_separator(
				s.c_str(), m_ident + 1 );
			m_childs->append( item );
		}
			
		long			count;
		u08*			addr;

		addr = (u08*)stream->read_ptr();
		count = stream->read_u32();

		for( long i = 0; i < count; i++ )
		{
			variable_t*	var;
			const char*	name;

			var = new variable_t;
			var->init_as_variable(
				stream, addr, m_ident + 1 );
			name = var->name();
			if( is_global_scope &&
				hfind( s_system_globals,
					(u08*)name,
					strlen( name ) * sizeof( char ) ) )
				delete var;
			else
				m_childs->append( var );
		}

		first = false;
	}

	for( long i = m_childs->count() - 1; i >= 0; i-- )
		m_childs->at( i )->setup( stream );

	if( m_childs->count() == 0 )
	{
		delete m_childs;
		m_childs = 0;
	}
}

int
variable_t::ident() const
{
	return m_ident;
}

bool
variable_t::expanded() const
{
	return m_expanded;
}

void
variable_t::set_expanded(
	bool				expanded )
{
	m_expanded = expanded;
}

bool
variable_t::is_separator() const
{
	return m_base_addr == 0 && m_childs == 0;
}

void
variable_t::init(
	const char*			system_file )
{
	s_system_globals = hcreate( 5 );

	const char*			path;
	FILE*				fp;
	char				buf[ 64 + 1 ];
	
	fp = fopen( system_file, "r" );
	if( !fp )
		return;
	while( fgets( buf, 64, fp ) )
	{
		if( memcmp( buf, "// #", 4 ) == 0 )
		{
			int			i = strlen( buf ) - 1;

			while( i >= 0 && (
				buf[ i ] == '\r' ||
				buf[ i ] == '\n' ) )
			{
				buf[ i ] = '\0';
				i--;
			}

			char*		s = strdup( buf + 4 );

			hadd( s_system_globals,
				(u08*)s, strlen( s ) * sizeof( char ), 0 );
		}
	}

	fclose( fp );
}

long
variable_t::collapse()
{
	if( m_childs == 0 )
		return 0;

	if( not m_expanded )
		return 0;

	m_expanded = false;

	long				n = 0;
	long				count = m_childs->count();
	variable_t*			var;

	for( long i = 0; i < count; i++ )
	{
		var = m_childs->at( i );
		n += var->collapse();
	}

	return n + count;
}

END_AKLABETH_NAMESPACE
