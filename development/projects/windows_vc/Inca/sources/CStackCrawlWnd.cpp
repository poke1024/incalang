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
//	CStackCrawlWnd.cpp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "stdafx.h"
#include "Inca.h"
#include "UserMessages.h"

#include "CStackCrawlWnd.h"

BEGIN_AKLABETH_NAMESPACE

BEGIN_MESSAGE_MAP( CStackCrawlWnd, CWnd )
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE( WM_FRAME_CHANGED, OnFrameChanged )
END_MESSAGE_MAP()

// ===========================================================================

class CStackCrawlCtrl : public CListCtrl {
public:
	BOOL				AddFrame(
							int				index,
							const char*		name );
	
	virtual void		OnItemchanged(
							NMHDR*			pNMHDR,
							LRESULT*		pResult );

	afx_msg void		OnLButtonDown(
							UINT			nFlags,
							CPoint			point );

	afx_msg void		OnLButtonDblClk(
							UINT			nFlags,
							CPoint			point );

	afx_msg void		OnRButtonDown(
							UINT			nFlags,
							CPoint			point );

	afx_msg void		OnRButtonDblClk(
							UINT			nFlags,
							CPoint			point );

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP( CStackCrawlCtrl, CListCtrl )
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_NOTIFY_REFLECT( LVN_ITEMCHANGED, OnItemchanged )
END_MESSAGE_MAP()

void
CStackCrawlCtrl::OnLButtonDown(
	UINT			nFlags,
	CPoint			point )
{
	if( HitTest( point ) == -1 )
		return;
	CListCtrl::OnLButtonDown( nFlags, point );
}

void
CStackCrawlCtrl::OnLButtonDblClk(
	UINT			nFlags,
	CPoint			point )
{
}

void
CStackCrawlCtrl::OnRButtonDown(
	UINT			nFlags,
	CPoint			point )
{
}

void
CStackCrawlCtrl::OnRButtonDblClk(
	UINT			nFlags,
	CPoint			point )
{
}

BOOL
CStackCrawlCtrl::AddFrame(
	int				index,
	const char*		name )
{
	int				nIndex;
	char			buf[ 32 ];

	sprintf( buf, "%d", index );
	nIndex = InsertItem(
		LVIF_TEXT | LVIF_STATE, index - 1,
		buf, 0, LVIS_SELECTED, 0, 0 );
	ASSERT( nIndex != -1 );
    if( nIndex < 0 )
		return FALSE;
	
	SetItemText( nIndex, 1, name );
	return TRUE;
}

inline BOOL
IsChecked(
	UINT			uState )
{
	return ( uState ? ( ( uState & LVIS_STATEIMAGEMASK )
		>> 12 ) - 1 : FALSE );
}

void
CStackCrawlCtrl::OnItemchanged(
	NMHDR*			pNMHDR,
	LRESULT*		pResult )
{
	NM_LISTVIEW*	pNMListView = (NM_LISTVIEW*)pNMHDR;

	if( ( !IsChecked( pNMListView->uOldState ) ) &&
		( IsChecked( pNMListView->uNewState ) ) )
	{
		GetParent()->PostMessage( WM_FRAME_CHANGED,
			0, pNMListView->iItem );
	}

	*pResult = 0;
}

// ===========================================================================

CStackCrawlWnd::CStackCrawlWnd() :
	mList( 0 ),
	mThreadState( 0 )
{
}

CStackCrawlWnd::~CStackCrawlWnd()
{
}

BOOL 
CStackCrawlWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	mList = new CStackCrawlCtrl;

	RECT				r;

	GetClientRect( &r );

	mList->Create(
		LVS_REPORT |
		LVS_SHOWSELALWAYS |
		LVS_SINGLESEL |
		LVS_NOSORTHEADER,
		r, this, 1000 );

	mList->ShowWindow( SW_SHOW );

	mList->SetExtendedStyle(
		mList->GetExtendedStyle() | LVS_EX_FULLROWSELECT );

	mList->InsertColumn( 0, "#" );
	mList->InsertColumn( 1, "Name" );

	mList->SetColumnWidth( 0, 64 );
	mList->SetColumnWidth( 1, r.right - r.left -
		mList->GetColumnWidth( 0 ) );

	return TRUE;
}

void
CStackCrawlWnd::Setup(
	CWnd*				view,
	ThreadState*		threadState,
	int					threadIndex )
{
	mView = view;
	mThreadState = threadState;
	mThreadIndex = threadIndex;

	CStackCrawlCtrl*	list = (CStackCrawlCtrl*)mList;
	
	long				count = threadState->frameCount;

	for( long i = 0; i < count; i++ )
		list->AddFrame( i + 1,
			threadState->frames[ i ].name.c_str() );

	int					nItem = mList->GetItemCount() - 1;
	mList->SetItem(
		nItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, 
		LVIS_SELECTED, 0 );
}

int
CStackCrawlWnd::OnCreate(
	LPCREATESTRUCT		lpCreateStruct )
{
	if( CDialog::OnCreate( lpCreateStruct ) )
		return -1;

	return 0;
}

void
CStackCrawlWnd::OnSize(
	UINT				nType,
	int					cx,
	int					cy )
{
	CDialog::OnSize( nType, cx, cy );

	if( mList )
	{
		RECT			r;
		GetClientRect( &r );
		mList->MoveWindow( &r );

		long			width = r.right - r.left;

		if( width < 300 )
			width = 300;

		mList->SetColumnWidth( 1, width -
			mList->GetColumnWidth( 0 ) );
	}
}

LONG
CStackCrawlWnd::OnFrameChanged(
	WPARAM				wParam,
	LPARAM				lParam )
{
	mView->PostMessage(
		WM_FRAME_CHANGED, mThreadIndex, lParam );
	return 0;
}

END_AKLABETH_NAMESPACE
