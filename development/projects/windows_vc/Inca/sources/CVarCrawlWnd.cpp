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
//	CVarCrawlWnd.cpp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "stdafx.h"
#include "Inca.h"
#include "UserMessages.h"

#include "CVarCrawlWnd.h"
#include "variable_t.h"

BEGIN_AKLABETH_NAMESPACE

BEGIN_MESSAGE_MAP( CVarCrawlWnd, CWnd )
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE( WM_FRAME_CHANGED, OnFrameChanged )
END_MESSAGE_MAP()

// ===========================================================================

class CVarCrawlCtrl : public CListCtrl {
public:
						CVarCrawlCtrl();

	BOOL				AddVariable(
							variable_t*		var,
							int				index );
	
	virtual void		OnItemchanged(
							NMHDR*			pNMHDR,
							LRESULT*		pResult );

	virtual void		OnCustomdraw(
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

	void				ToggleExpansion(
							int				nItem );

	void				SetStream(
							pipe_stream_t*	stream );
	
	DECLARE_MESSAGE_MAP()

private:
	CBrush				mGrayBrush;
	pipe_stream_t*		mStream;
};

BEGIN_MESSAGE_MAP( CVarCrawlCtrl, CListCtrl )
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_NOTIFY_REFLECT( LVN_ITEMCHANGED, OnItemchanged )
	ON_NOTIFY_REFLECT( NM_CUSTOMDRAW, OnCustomdraw )
END_MESSAGE_MAP()

const int					BOX_SIZE = 9;
const int					IDENT_STEP = 16;

CVarCrawlCtrl::CVarCrawlCtrl()
{
	mGrayBrush.CreateSolidBrush( RGB( 128, 128, 128 ) );
	mStream = 0;
}

void
CVarCrawlCtrl::SetStream(
	pipe_stream_t*	stream )
{
	mStream = stream;
}

void
CVarCrawlCtrl::OnLButtonDown(
	UINT			nFlags,
	CPoint			point )
{
	int				nItem = HitTest( point );
	RECT			bounds;

	if( nItem == -1 )
		return;

	GetItemRect( nItem, &bounds, LVIR_BOUNDS );
	if( point.x >= bounds.left + 4 &&
		point.y >= bounds.top + 4 &&
		point.x <= bounds.left + 4 + BOX_SIZE &&
		point.y <= bounds.bottom + 4 + BOX_SIZE )
	{
		ToggleExpansion( nItem );
		return;
	}

	CListCtrl::OnLButtonDown( nFlags, point );
}

void
CVarCrawlCtrl::OnLButtonDblClk(
	UINT			nFlags,
	CPoint			point )
{
	int				nItem = HitTest( point );

	if( nItem == -1 )
		return;

	ToggleExpansion( nItem );
}

void
CVarCrawlCtrl::OnRButtonDown(
	UINT			nFlags,
	CPoint			point )
{
}

void
CVarCrawlCtrl::OnRButtonDblClk(
	UINT			nFlags,
	CPoint			point )
{
}

void
CVarCrawlCtrl::ToggleExpansion(
	int				nItem )
{
	variable_t*		var = (variable_t*)GetItemData( nItem );
		
	if( not var->is_expandable() || mStream == 0 )
		return;

	variable_array_t*	array = var->childs( mStream );
	long			count = array->count();

	if( var->expanded() )
	{
		count = var->collapse();

		for( long i = 0; i < count; i++ )
			DeleteItem( nItem + 1 );

		var->set_expanded( false );
		Invalidate();
		return;
	}
		
	for( long i = 0; i < count; i++ )
		AddVariable( array->at( i ), nItem + i + 1 );
	
	var->set_expanded( true );
	Invalidate();
}

BOOL
CVarCrawlCtrl::AddVariable(
	variable_t*		var,
	int				index )
{
	LVITEM			item;
	int				nIndex;

	nIndex = InsertItem(
		LVIF_TEXT | LVIF_STATE, index,
		"", 0, LVIS_SELECTED, 0, 0 );
	ASSERT( nIndex != -1 );
    if( nIndex < 0 )
		return FALSE;
	
	SetItemText( nIndex, 1, "" );

	SetItemData( nIndex, (DWORD_PTR)var );

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
CVarCrawlCtrl::OnItemchanged(
	NMHDR*			pNMHDR,
	LRESULT*		pResult )
{
	NM_LISTVIEW*	pNMListView = (NM_LISTVIEW*)pNMHDR;

	if( ( !IsChecked( pNMListView->uOldState ) ) &&
		( IsChecked( pNMListView->uNewState ) ) )
	{
		/*GetParent()->PostMessage( WM_FRAME_CHANGED,
			0, pNMListView->iItem );*/
	}

	*pResult = 0;
}

void
CVarCrawlCtrl::OnCustomdraw(
	NMHDR*			pNMHDR,
	LRESULT*		pResult )
{
	NMLVCUSTOMDRAW*	pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    *pResult = 0;
	
	if( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		LVITEM		rItem;
        int			nItem;
		RECT		rect;
		
		nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

		variable_t*	var = (variable_t*)GetItemData( nItem );
		const char*	s;

		CDC*		pDC = CDC::FromHandle( pLVCD->nmcd.hdc );
		CRect		bounds;

		GetItemRect( nItem, &bounds, LVIR_BOUNDS );

		if( var->is_separator() )
			pDC->FillSolidRect( &bounds, RGB( 240, 240, 240 ) );
		else
			pDC->FillSolidRect( &bounds, GetBkColor() );

		RECT		r;

		r = bounds;
		r.left += 20 + var->ident() * IDENT_STEP;
		r.right = bounds.left + GetColumnWidth( 0 );
		s = var->name();
		pDC->DrawText( s, strlen( s ), &r,
			DT_BOTTOM | DT_LEFT | DT_WORD_ELLIPSIS );

		r.left = r.right;
		r.right = bounds.right;
		s = var->value();
		pDC->DrawText( s, strlen( s ), &r,
			DT_BOTTOM | DT_LEFT | DT_WORD_ELLIPSIS );

		if( var->is_expandable() )
		{
			r = bounds;
			r.left += 4;
			r.top += 3;
			r.right = r.left + BOX_SIZE;
			r.bottom = r.top + BOX_SIZE;
			pDC->FrameRect( &r, &mGrayBrush );

			pDC->SelectObject( GetStockObject( BLACK_BRUSH ) );
			pDC->MoveTo( r.left + 2, ( r.top + r.bottom ) / 2 );
			pDC->LineTo( r.right - 2, (  r.top + r.bottom ) / 2 );

			if( not var->expanded() )
			{
				pDC->MoveTo( ( r.left + r.right ) / 2,
					r.top + 2 );
				pDC->LineTo( ( r.left + r.right ) / 2,
					r.bottom - 2 );
			}
		}

		*pResult = CDRF_SKIPDEFAULT;
	}
}

// ===========================================================================

CVarCrawlWnd::CVarCrawlWnd() :
	mList( 0 ),
	mThreadState( 0 )
{
}

CVarCrawlWnd::~CVarCrawlWnd()
{
}

BOOL 
CVarCrawlWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	mList = new CVarCrawlCtrl;

	RECT				r;

	GetClientRect( &r );

	mList->Create(
		LVS_REPORT |
		LVS_SHOWSELALWAYS |
		LVS_SINGLESEL |
		LVS_NOSORTHEADER,
		r, this, 1000 );

	mList->ShowWindow( SW_SHOW );

	mList->InsertColumn( 0, "Name" );
	mList->InsertColumn( 1, "Value" );

	mList->SetColumnWidth( 0,
		( r.right - r.left ) / 2 );
	mList->SetColumnWidth( 1, r.right - r.left -
		mList->GetColumnWidth( 0 ) );

	return TRUE;
}

void
CVarCrawlWnd::Setup(
	CWnd*				view,
	pipe_stream_t*		stream )
{
	mView = view;
	mStream = stream;
	( (CVarCrawlCtrl*)mList )->SetStream( stream );
}

void
CVarCrawlWnd::Load(
	ThreadState*		threadState,
	int					threadIndex,
	int					frameIndex )
{
	mThreadState = threadState;
	mThreadIndex = threadIndex;
	mFrameIndex = frameIndex;

	mVars.load_stack_frame( frameIndex, mStream );

	CVarCrawlCtrl*		list = (CVarCrawlCtrl*)mList;
	long				count = mVars.count();

	list->DeleteAllItems();

	// instance "this"
	if( threadState->frames[ frameIndex ].hasThis )
	{
		list->AddVariable(
			mVars.at( 0 ), 0 );
	}

	// local variables
	for( long i = count - 1; i > 0; i-- )
	{
		variable_t*		var = mVars.at( i );

		list->AddVariable(
			var, list->GetItemCount() );
	}

	// global variables
	variable_t*			globals;

	globals = threadState->frames[ frameIndex ].globals;

	if( globals && globals->is_expandable() )
		list->AddVariable( globals, 0 );
}

int
CVarCrawlWnd::OnCreate(
	LPCREATESTRUCT		lpCreateStruct )
{
	if( CDialog::OnCreate( lpCreateStruct ) )
		return -1;

	return 0;
}

void
CVarCrawlWnd::OnSize(
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
CVarCrawlWnd::OnFrameChanged(
	WPARAM				wParam,
	LPARAM				lParam )
{
	//mView->PostMessage(
	//	WM_FRAME_CHANGED, mThreadIndex, lParam );
	return 0;
}

END_AKLABETH_NAMESPACE
