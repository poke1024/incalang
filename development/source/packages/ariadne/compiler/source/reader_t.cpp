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
//	reader_t.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "reader_t.h"
//#include "source_stream_t.h"
#include "lexer.h"

BEGIN_COMPILER_NAMESPACE

reader_t::reader_t() :
	
	m_binding( 0 ),
	m_files( 0 )
{
}

reader_t::~reader_t()
{
	while( pop() )
		;
		
	while( m_files )
	{
		file_t*			next = m_files->next;
		
		delete m_files;
		m_files = next;
	}
}

char*
reader_t::submit_fileid(
	binding_t*			binding,
	char*				buffer,
	u32&				count )
{
	binding->submit = false;
		
	if( ( count -= LEX_FILEREF_SIZE ) <= 0 )
		// signal buffer overflow
		return 0;
		
	*buffer++ = LEX_CHAR_FILEREF;

	fileid_t			fileid;
	u32					offset;

	fileid = binding->fileid;
	offset = binding->mark - binding->buffer;

	static const char	hexTable[ 16 ] = {
							'0', '1', '2', '3', '4',
							'5', '6', '7', '8', '9',
							'A', 'B', 'C', 'D', 'E',
							'F' };

	for( int i = LEX_FILEID_CHARS * 4 - 4; i >= 0; i -= 4 )
		*buffer++ = hexTable[ ( fileid >> i ) & 0xf ];

	for( int i = LEX_FILEPOS_CHARS * 4 - 4; i >= 0; i -= 4 )
		*buffer++ = hexTable[ ( offset >> i ) & 0xf ];

	return buffer;
}

u32
reader_t::read_line(
	void*				buffer,
	u32					count )
{	
	u32					outcnt;
	
	if( ( outcnt = count ) == 0 )
		return 0;

	binding_t*			binding;
	char*				outbuf = (char*)buffer;	

	while( ( binding = m_binding ) != 0 )
	{	
		if( binding->submit )
		{
			outbuf = submit_fileid(
				binding, outbuf, outcnt );
				
			if( !outbuf )
				return count;
		}
	
		char*			mark = binding->mark;
	
		while( true )
		{
			char		c;
		
			if( ( c = *mark++ ) == '\0' )
			{
				pop();
				*outbuf++ = '\n';
				--outcnt;
				goto done;
			}

			*outbuf++ = c;
			if( --outcnt == 0 || c == '\n' )
			{
				binding->mark = mark;
				goto done;
			}
		}
	}

done:
	return count - outcnt;
}

void
reader_t::push(
	char*				buffer )
{
	// buffer MUST be null-terminated and allocated
	// via new[] since reader_t owns it from here on.
	
	binding_t*			binding = new binding_t;
	
	binding->buffer = buffer;
	binding->mark = buffer;
	binding->fileid = fileid_invalid;
	binding->submit = false;
	
	binding->next = m_binding;
	m_binding = binding;
}

#if 0
void
reader_t::push_file(
	const char*			path )
{
	char*				buffer = 0;
	bool				file_open = false;

	try {
		source_stream_t	stream( path, "r" );
		file_open = true;
				
		fpos_t			length = stream.length();
		fpos_t			chlength = length / sizeof( char );
		
		buffer = new char[ chlength + 1 ];
		stream.read_bytes( buffer, length );
		
		buffer[ chlength ] = '\0';
		push( buffer );
	}
	catch(...)
	{
		if( not file_open )
		{
			compile_error( 0, ERR_FILE_OPEN_FAILED );
			return;
		}
	
		if( buffer )
			delete[] buffer;
		throw;
	}

	binding_t*			binding = m_binding;
	
	binding->fileid = register_fileid( path );
	binding->submit = true;
}
#endif

bool
reader_t::lookup_fileid(
	fileid_t			fileid,
	istring&			path ) const
{
	file_t*				node = m_files;

	while( node )
	{
		if( node->id == fileid )
		{
			path = node->path;
			return true;
		}
	
		node = node->next;
	}
	
	return false;
}

bool
reader_t::pop()
{
	binding_t*			binding;
	
	if( ( binding = m_binding ) == 0 )
		return false;
	
	binding_t*			next = binding->next;
	
	delete[] binding->buffer;
	delete binding;
	
	if( next )
		next->submit = ( next->fileid != fileid_invalid );
	m_binding = next;

#if 0
	while( ( binding = m_binding ) == 0 )
	{
		game_t*			game;
		storage_t*		db;
		static_index_t*	index;

		game = compiler_t::game();
		
		if( !game )
			break;
		
		db = game->map()->script_storage();
		
		index = (static_index_t*)( db->index() );
		
		if( not index->next( m_embedded_id  ) )
			break;

		char			buf[ 32 ];
	
		std::sprintf( buf, "#%ld",
			(long)m_embedded_id );
	
		push_file( buf );
	}
#endif

	return true;
}

fileid_t
reader_t::register_fileid(
	const char*			path )
{
	file_t**			link = &m_files;
	file_t*				node = *link;
	fileid_t			id = 1;

	while( node )
	{
		if( !istrcmp( node->path.c_str(), path ) )
			return node->id;
	
		link = &node->next;
		node = *link;
		id += 1;
	}
	
	node = new file_t;
	node->next = 0;
	node->id = id;
	node->path = path;
	*link = node;	

	return id;
}

END_COMPILER_NAMESPACE
