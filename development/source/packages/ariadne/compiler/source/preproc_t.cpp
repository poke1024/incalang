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
//	preproc_t.cp			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "preproc_t.h"

BEGIN_COMPILER_NAMESPACE

const u32				max_line_length = 8192;

preproc_t::preproc_t(
	reader_t*			reader ) :
	
	m_reader( reader )
{
	m_line = new char[ max_line_length ];
	m_mark = m_line;
	m_left = 0;
}

preproc_t::~preproc_t()
{
	delete[] m_line;
}

fpos_t
preproc_t::read_some_bytes(
	void*				buffer,
	fpos_t				count )
{
	char*				outbuf = (char*)buffer;
	fileref_t			fposrec = { fileid_invalid };
	u32					netto = 0;
	
	do {	
		u32				left;

		if( ( left = m_left ) != 0 )
		{
			char*		mark = m_mark;
		
			if( count <= left )
			{
				imemcpy( outbuf, mark, count );
				outbuf += count;
				m_left = left - count;
				m_mark = mark + count;
				goto done;
			}
			else
			{
				imemcpy( outbuf, mark, left );
				outbuf += left;
				count -= left;
			}
		}
	} while( read_line( &fposrec, &netto ) );
	
done:
	return outbuf - (char*)buffer;
}

void
preproc_t::set_source_path(
	const char*			path )
{
	m_source_path = path;
}

void
preproc_t::set_system_path(
	const char*			path )
{
	m_system_path = path;
}

static u32
parse_hex(
	const char*			buffer,
	int					charcnt )
{
	u32					value = 0;

	for( int i = 0; i < charcnt; i++ )
	{
		char			c =  *buffer++;
		
		value <<= 4;

		if( c <= '9' )
			value += c - '0';
		else
			value += c - 'A' + 10;
	}
	
	return value;
}

u32
preproc_t::read_line(
	fileref_t*			fpos,
	u32*				nettop )
{
	char*				zline = m_line;
	u32					netto = *nettop;

	char*				line = zline;
	u32					count;
	
	while( true )
	{
		u32				max_count;
		
		max_count = max_line_length - ( line - zline );
	
		count = m_reader->read_line(
			line, max_count );

		if( count == max_count )
			compile_error( 0, ERR_LINE_TOO_LONG );
			
		if( !count )
			break;
		
		while( *line == LEX_CHAR_FILEREF )
		{
			fpos->id = parse_hex(
				line + 1, LEX_FILEID_CHARS );
			fpos->pos = parse_hex(
				line + 1 + LEX_FILEID_CHARS,
				LEX_FILEPOS_CHARS );
			netto = 0;

			line += LEX_FILEREF_SIZE;
			count -= LEX_FILEREF_SIZE;
		}
		
		netto += count;
	
		if( *line == '#' )
		{
			if( fpos->id == fileid_invalid )
				get_filepos( fpos );

			fileref_t	fposrec = *fpos;
			
			fposrec.pos += netto - 1;
			handle_directive( line + 1, &fposrec );
		}
		else
			break;
	}
	
	u32					left = count + ( line - zline );
	
	*nettop = netto;
	m_mark = zline;
	m_left = left;
	
	return left;
}

void
preproc_t::handle_directive(
	const char*			mark,
	const fileref_t*	fpos )
{
	if( !imemcmp( mark, "include", 7 ) )
	{
		mark += 7;
		
		if( !isspace( *mark++ ) )
			goto error;
		while( isspace( *mark ) )
			mark++;
		
		char			delimit;
		
		delimit = *mark++;
	
		istring			path;

		if( delimit == '<' )
			path = m_system_path;
		else
		{
			if( m_reader->lookup_fileid( fpos->id, path ) )
			{
				path = path.substr( 0, path.rfind( "/" ) + 1 );
			}
			else
			{
				path = m_source_path;
			}
		}
	
		if( delimit == '<' )
			delimit = '>';
		else if( delimit != '"' )
			goto error;
		
		while( *mark != delimit )
		{
			if( *mark == '\n' )
				goto error;
			if( mark[ 0 ] == '.' &&
				mark[ 1 ] == '.' &&
				mark[ 2 ] == '/' )
			{
				path = path.substr( 0,
					path.rfind( "/", path.length() - 2 ) );
				path.append( "/" );
				mark += 3;
			}
			else
			{
				path.append( 1, *mark );
				mark++;
			}
		}
	
		mark++;
		while( isspace( *mark ) && *mark != '\n' )
			mark++;
		if( *mark != '\n' )
			goto error;
						
		//m_reader->push_file( path.c_str() );
			
		return;
	}
	
error:
	compile_error( fpos, ERR_ILLEGAL_DIRECTIVE );
}

END_COMPILER_NAMESPACE
