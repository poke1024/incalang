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

// Disassemble.cpp

#ifdef SUPPORT_DISASSEMBLE

#include "Disassemble.h"

#include "machine_t.h"
#include "machine_class_t.h"
#include "disassembler_t.h"

using namespace machine;

class string_disassembler_t : public disassembler_t {
public:
						string_disassembler_t(
							machine_t*			machine,
							istring&			text );

	virtual void		print(
							const char*			text );
							
private:
	istring&			m_text;
};

string_disassembler_t::string_disassembler_t(
	machine_t*			machine,
	istring&			text ) :
	
	disassembler_t( machine ),
	m_text( text )
{
}

void
string_disassembler_t::print(
	const char*			text )
{
	m_text.append( text );
}

char*
DisassembleProgram(
	memory_buffer_t*	buffer )
{
	buffer->seek( 0, seek_mode_begin );

	machine_t			m( new memory_t(), "" );

	long				class_count = buffer->read_u32();

	for( long i = 0; i < class_count; i++ )
		m.import_class( buffer );
	
	class_t*			clss = m.get_root_class();

	clss->finalize();
		
	istring				text;
	string_disassembler_t	disassembler( &m, text );
	
	disassembler.dall();

	istring				dostext;
	const char*			p = text.c_str();

	dostext.reserve( text.length() );
	while( *p != '\0' )
	{
		if( *p == '\n' )
			dostext.append( "\r\n" );
		else
			dostext.append( 1, *p );
		p++;
	}

	char*				data;
	data = (char*)imalloc( sizeof( char ) *
		( dostext.length() + 1 ) );
	strcpy( data, dostext.c_str() );
	
	return data;
}

#endif