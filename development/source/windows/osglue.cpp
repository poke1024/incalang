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
//	osglue.cpp				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include <afxwin.h>
#include "oswrap.h"

BEGIN_OSWRAP_NAMESPACE

char*
from_pasteboard()
{
	if( !IsClipboardFormatAvailable( CF_TEXT ) ) 
		return 0;

	if( !OpenClipboard( AfxGetApp()->GetMainWnd()->m_hWnd ) ) 
        return 0; 

	HANDLE h = GetClipboardData( CF_TEXT );
	char* s = 0;

	if( h )
	{
		TCHAR* p = (TCHAR*)GlobalLock( h );
		if( p )
		{
			SIZE_T size = GlobalSize( h );
			s = (char*)imalloc( size + 1 );
			if( s )
			{
				int j = 0;
				for( long i = 0; i < size; i++ )
				{
					if( p[ i ] == '\r' &&
						i + 1 < size &&
						p[ i + 1 ] == '\n' )
					{					
						i++;
					}
					s[ j++ ] = p[ i ];
				}
				s[ j ] = '\0';
			}
			GlobalUnlock( h );
		}
	}

	CloseClipboard();

	return s;
}

void
to_pasteboard( const char* buffer, size_t length )
{
	istring data;
	long datalen;

	for( long i = 0; i < length; i++ )
	{
		if( buffer[ i ] == '\n' )
			data.append( "\r\n" );
		else
			data.append( 1, buffer[ i ] );
	}
	datalen = data.length();

	HANDLE h;

	if( !OpenClipboard( AfxGetApp()->GetMainWnd()->m_hWnd ) ) 
        return; 

	h = GlobalAlloc( GMEM_MOVEABLE, 
		( datalen + 1 ) * sizeof( TCHAR ) ); 
	if( h == NULL )
	{ 
		CloseClipboard(); 
		return; 
	} 
 
	TCHAR* p;
	p = (TCHAR*)GlobalLock( h );
	for( long i = 0; i < datalen; i++ )
		p[ i ] = data[ i ];
	p[ datalen ] = (TCHAR)0;
	GlobalUnlock( h ); 
 
	EmptyClipboard();
	SetClipboardData( CF_TEXT, h ); 

	CloseClipboard();
}

END_OSWRAP_NAMESPACE
