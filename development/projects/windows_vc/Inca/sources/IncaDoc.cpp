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

// IncaDoc.cpp

#include "stdafx.h"
#include "Inca.h"

#include "IncaDoc.h"

#include "IncaView.h"
#include "code_view_t.h"
using namespace aklabeth;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIncaDoc

IMPLEMENT_DYNCREATE(CIncaDoc, CDocument)

BEGIN_MESSAGE_MAP(CIncaDoc, CDocument)
END_MESSAGE_MAP()


// creation

CIncaDoc::CIncaDoc()
{
	mCodeView = new code_view_t;

	CWnd* mainWindow = AfxGetMainWnd();
	CDC* pDC = mainWindow->GetDC();

	mFont.CreatePointFont( 100, "Trebuchet MS", pDC );
	CFont* oldFont = pDC->SelectObject( &mFont );

	typeface_t typeface;
	typeface.init( pDC->m_hDC );
	mCodeView->init( &typeface );

	pDC->SelectObject( oldFont );
}

CIncaDoc::~CIncaDoc()
{
	delete mCodeView;
}

BOOL CIncaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}




void CIncaDoc::Serialize(CArchive& ar)
{
	if( ar.IsStoring() )
	{
		memory_buffer_t buffer;
		mCodeView->save( &buffer );
		buffer.seek( 0, seek_mode_begin );
		fpos_t length = buffer.length();

		while( length > 0 )
		{
			UINT chunk;
			if( length > 4096 )
				chunk = 4096;
			else
				chunk = (UINT)length;
			ar.Write( buffer.access(), chunk );
			length -= chunk;
			buffer.seek( chunk, seek_mode_relative );
		}
	}
	else
	{
		memory_buffer_t stream;

		const int buffer_size = 4096;
		u08* buffer = new u08[ buffer_size ];

		int length;
		do {
			length = ar.Read( buffer, buffer_size );
			stream.write_bytes( buffer, length );
		} while( length == buffer_size );
		delete[] buffer;
		
		stream.seek( 0, seek_mode_begin );
		mCodeView->load( &stream );
	}
}


#ifdef _DEBUG
void CIncaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIncaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

