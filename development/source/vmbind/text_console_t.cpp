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
//	text_console_t.cp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "text_console_t.h"
#include "vmbind.h"

BEGIN_MACHINE_NAMESPACE

text_console_t::text_console_t()
{
	m_buffer_size = 4096;
	m_buffer = new char[ m_buffer_size ];
	m_buffer_index = 0;

	AllocConsole();

	//mInHandle = GetStdHandle( STD_INPUT_HANDLE );
	//mOutHandle = GetStdHandle( STD_OUTPUT_HANDLE );

	m_in_handle = CreateFile(
		"CONIN$", GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_ALWAYS, 0, NULL );


	m_out_handle = CreateFile(
		"CONOUT$", GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_ALWAYS, 0, NULL );

	//SetConsoleMode( mInHandle, ENABLE_LINE_INPUT );

	m_sbi_dirty = true;
}

text_console_t::~text_console_t()
{
	delete[] m_buffer;

	FreeConsole();
}

s32
text_console_t::inkey()
{
	INPUT_RECORD		buffer;
	DWORD				dwRead;
	DWORD				dwCount;

	if( not GetNumberOfConsoleInputEvents(
		m_in_handle, &dwCount ) )
		return 0;

	for( int i = 0; i < dwCount; i++ )
	{
		ReadConsoleInput( m_in_handle, &buffer,
			1, &dwRead );
		if( buffer.EventType == KEY_EVENT &&
			buffer.Event.KeyEvent.bKeyDown )
		{
			char c = buffer.Event.KeyEvent.uChar.AsciiChar;
			if( c )
				return c;

			return -(int)buffer.Event.KeyEvent.wVirtualKeyCode;
		}
	}

	return 0;
}

struct console_read_t {
	HANDLE				inHandle;
	istring*			text;
	bool				done;
};

static DWORD WINAPI
consoleRead(
	LPVOID				lp )
{
	console_read_t*		data = (console_read_t*)lp;

	DWORD				dwBytesRead;
	char				temp;

	while( true )
	{
		ReadFile( data->inHandle, &temp, 1,
			&dwBytesRead, NULL );
		if( temp == '\r' )
			break;
		data->text->append( 1, temp );
	}

	data->done = true;

	return 0;
}

void
text_console_t::input(
	istring&			text )
{
	flush();

	DWORD				ID;
	console_read_t		data;

	data.inHandle = m_in_handle;
	data.text = &text;
	data.done = false;
	CreateThread( NULL, 0, consoleRead, &data, NULL, &ID );

	while( not data.done )
	{
		vm_process_os_events();
		Sleep( 100 );
	}
}

void
text_console_t::print(
	const char*			text,
	long				length )
{
	if( length < 1 )
		return;

	if( process_commands( text, length ) )
		return;

	while( true )
	{
		long			index = m_buffer_index;
		long			bufsize = m_buffer_size;

		if( index + length < bufsize )
		{
			imemcpy(
				m_buffer + index,
				text, length * sizeof( char ) );
			
			m_buffer_index = index + length;
			break;
		}
		
		flush();
		
		if( length > bufsize )
		{
			output( text, length );
			break;
		}
	}
	
	m_sbi_dirty = true;
}

void
text_console_t::flush()
{
	long				index = m_buffer_index;
	
	if( m_buffer_index < 1 )
		return;
		
	output( m_buffer, index );
				
	m_buffer_index = 0;
}

void
text_console_t::output(
	const char*			text,
	long				length )
{
	DWORD				dwBytesWritten;

	WriteFile( m_out_handle, text,
		length, &dwBytesWritten, NULL );

	FlushFileBuffers( m_out_handle );
}

int
text_console_t::crscol()
{
	update_sbi();
	return m_sbi.dwCursorPosition.X;
}

int
text_console_t::crslin()
{
	update_sbi();
	return m_sbi.dwCursorPosition.Y;
}

void
text_console_t::htab(
	int					h )
{
	update_sbi();
	m_sbi.dwCursorPosition.X = h;
	SetConsoleCursorPosition( m_out_handle,
		m_sbi.dwCursorPosition );
}

void
text_console_t::vtab(
	int					v )
{
	update_sbi();
	m_sbi.dwCursorPosition.Y = v;
	SetConsoleCursorPosition( m_out_handle,
		m_sbi.dwCursorPosition );
}

void
text_console_t::set_forecolor(
	int					r,
	int					g,
	int					b )
{
	WORD				attributes;
	WORD				mask;

	update_sbi();

	mask = FOREGROUND_RED |
		FOREGROUND_GREEN |
		FOREGROUND_BLUE;
	attributes = m_sbi.wAttributes & ~mask;
	if( r )
		attributes |= FOREGROUND_RED;
	if( g )
		attributes |= FOREGROUND_GREEN;
	if( b )
		attributes |= FOREGROUND_BLUE;

	SetConsoleTextAttribute(
		m_out_handle, attributes );

	m_sbi_dirty = true;
}

void
text_console_t::set_backcolor(
	int					r,
	int					g,
	int					b )
{
	WORD				attributes;
	WORD				mask;

	update_sbi();

	mask = BACKGROUND_RED |
		BACKGROUND_GREEN |
		BACKGROUND_BLUE;
	attributes = m_sbi.wAttributes & ~mask;
	if( r )
		attributes |= BACKGROUND_RED;
	if( g )
		attributes |= BACKGROUND_GREEN;
	if( b )
		attributes |= BACKGROUND_BLUE;

	SetConsoleTextAttribute(
		m_out_handle, attributes );

	m_sbi_dirty = true;
}

void
text_console_t::update_sbi()
{
	if( not m_sbi_dirty )
		return;

	flush();
	GetConsoleScreenBufferInfo( m_out_handle, &m_sbi );
	m_sbi_dirty = false;
}

bool
text_console_t::process_commands(
	const char*			text,
	long				length )
{
	if( length < 4 ||
		text[ 0 ] != 0 ||
		( (u08)text[ 1 ] ) != 255 ||
		( (u08)text[ 2 ] ) != 255 )
	{
		return false;
	}

	int					cmd = text[ 3 ];

	if( length == 6 && cmd == '@' )
	{
		COORD			where;
		where.X = text[ 4 ];
		where.Y = text[ 5 ];
		SetConsoleCursorPosition( m_out_handle, where );
		m_sbi_dirty = true;
		return true;
	}

	if( length == 5 && cmd == 'f' )
	{
		int				col = text[ 4 ];
		set_forecolor( col & 1, col & 2, col & 4 );
		return true;
	}

	if( length == 5 && cmd == 'b' )
	{
		int				col = text[ 4 ];
		set_backcolor( col & 1, col & 2, col & 4 );
		return true;
	}

	return false;
}

END_MACHINE_NAMESPACE
