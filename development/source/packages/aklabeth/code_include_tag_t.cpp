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
//	code_include_tag_t.cp      ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_include_tag_t.h"
#include "code_line_t.h"
#include "code_stream_t.h"

#include "file_stream_t.h"
#include "os_file.h"
#include "array_t.h"

#include "compiler_t.h"
using namespace compiler;

BEGIN_CODE_NAMESPACE

include_tag_t::include_tag_t(
	const char*			base,
	const char*			path ) :

	collapse_tag_t( tag_include ),
	
	m_base( base ),
	m_path( path ),
	m_load_error( false ),
	m_token_count( 0 )
{
	if( path[ 0 ] == '/' )
		m_full_path = path;
	else
	{
		m_full_path = base;
		m_full_path.append( path );
	}

	if( _stat( m_full_path.c_str(), &m_file_stat ) )
		m_load_error = true;
	else if( ifexists( m_full_path.c_str() ) )
	{
		try
		{
			load_file( m_full_path.c_str() );
		}
		catch(...)
		{
			m_load_error = true;
		}
	}
	else
		m_load_error = true;
}

long
include_tag_t::token_count() const
{
	return m_token_count;
}

void
include_tag_t::text(
	istring&			text )
{
#if 0
	char s[ 32 ];
	std::sprintf( s, "%ld", (long)ref_count() );
	text = "{ .";
	text.append( s );
	text.append( ". }" );
#else
	text = "include \"";
	text.append( m_path );
	text.append( "\" " );
	
	if( m_lines )
		text.append( "< ... >" );
	else if( m_load_error )
		text.append( "<cannot load>" );
	else
		text.append( "<file not found>" );
#endif
}

void
include_tag_t::serialize(
	text_t&				text,
	int					flags )
{
	//if( flags & 2 )
	{
		text.append( "include \"" );
		text.append( m_path.c_str() );
		text.append( "\" " );
	}
	/*else
	{
		text.append_tag( this );			
	}*/
}

void
include_tag_t::save(
	stream_t&			stream,
	const options_t&	/*options*/ )
{
	stream.append( "include \"" );
	stream.append( m_path.c_str() );
	stream.append( "\"" );
}

bool
include_tag_t::can_expand()
{
	// include expansion is not implemented so far. we would
	// have to use special tags for marking the beginning
	// and the end of the include. these tags would have to
	// be unremovable (except via collapsing the include again).
	// several problems occur: opening the same include more than
	// once in the same file; synchronization during editing such
	// a set of modules
	
	return false;
}

void
include_tag_t::update()
{
	if( m_load_error )
	{
		compile_error( 0,
			ERR_INCLUDE_FILE_NOT_FOUND,
			m_path.c_str() );
		return;
	}

	struct _stat		buffer;

	if( _stat( m_full_path.c_str(), &buffer ) )
		return;

	if( buffer.st_mtime != m_file_stat.st_mtime )
	{
		cleanup();
		load_file( m_full_path.c_str() );
		m_file_stat = buffer;
	}
}

void
include_tag_t::load_file(
	const char*			path )
{
	file_stream_t		stream( path, "rb" );
	long				length = stream.length();

	istring				s;
	line_t*				line;
	array_t				array( sizeof( line_t* ) );
	long				token_count = 0;
		
	istring				base = m_full_path;
	base.erase( base.rfind( "/" ) + 1 );
		
	for( long i = 0; i < length; i++ )
	{
		char			c;
	
		c = stream.read_char();
		if( c == '\r' )
		{
			line = line_create( base.c_str(), s.data(), s.length() );
			token_count += line->token_count;
			array.append( &line );
			s.clear();
		}
		else
			s.append( 1, c );
	}

	if( s.length() )
	{
		line = line_create( base.c_str(), s.data(), s.length() );
		token_count += line->token_count;
		array.append( &line );
	}
	
	/*line = line_create( "}", 1 );
	token_count += 1;
	array.append( &line );*/
	
	long				line_count = array.count();
	
	m_line_count = line_count;
	m_lines = new line_t*[ line_count ];
	
	for( long i = 0; i < line_count; i++ )
		m_lines[ i ] = *(line_t**)array[ i ];
		
	m_token_count = token_count;
}

END_CODE_NAMESPACE
