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

// IncaView.cpp

#include "stdafx.h"

#include "Inca.h"
#include "CStackCrawlWnd.h"
#include "CVarCrawlWnd.h"
#include "UserMessages.h"

#include "IncaDoc.h"
#include "IncaView.h"
#include "ChildFrm.h"
#include "CPadDoc.h"
#include "Disassemble.h"

#ifndef _DEBUG
#include "Disassemble.cpp" // ***
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "qd_typeface_t.h"
#include "code_submit_t.h"
#include "input_stream_t.h"
#include "ariadne_lexer.h"
#include "pipe_output_stream_t.h"
#include <string>

#include "file_stream_t.h" // **
#include "ariadne_messages.h"

const int kDefaultStackSize = 64 * 1024L;

// CIncaView

static UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

IMPLEMENT_DYNCREATE(CIncaView, CView)

BEGIN_MESSAGE_MAP(CIncaView, CView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()

	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_TIMER()
	
	ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplace )
	ON_MESSAGE( WM_PROGRAM_PAUSED, OnProgramPaused )
	ON_MESSAGE( WM_PROGRAM_TERMINATED, OnProgramTerminated )
	ON_MESSAGE( WM_FRAME_CHANGED, OnFrameChanged )

	// undo redo
	ON_COMMAND(ID_EDIT_UNDO, CIncaView::OnUndo )
	ON_COMMAND(ID_EDIT_REDO, CIncaView::OnRedo )
	ON_UPDATE_COMMAND_UI( ID_EDIT_UNDO, CIncaView::OnUpdateUndo )
	ON_UPDATE_COMMAND_UI( ID_EDIT_REDO, CIncaView::OnUpdateRedo )

	// find replace
	ON_COMMAND(ID_EDIT_FIND, CIncaView::OnFind )
	ON_COMMAND(ID_EDIT_REPLACE, CIncaView::OnReplace )

	// copy paste
	ON_COMMAND(ID_EDIT_CUT, CIncaView::OnCut )
	ON_COMMAND(ID_EDIT_COPY, CIncaView::OnCopy )
	ON_COMMAND(ID_EDIT_PASTE, CIncaView::OnPaste )
	ON_COMMAND(ID_EDIT_CLEAR, CIncaView::OnClear )
	ON_COMMAND(ID_EDIT_SELECT_ALL, CIncaView::OnSelectAll )
	ON_UPDATE_COMMAND_UI( ID_EDIT_CUT, CIncaView::OnUpdateCut )
	ON_UPDATE_COMMAND_UI( ID_EDIT_COPY, CIncaView::OnUpdateCopy )
	ON_UPDATE_COMMAND_UI( ID_EDIT_PASTE, CIncaView::OnUpdatePaste )
	ON_UPDATE_COMMAND_UI( ID_EDIT_CLEAR, CIncaView::OnUpdateClear )
	ON_UPDATE_COMMAND_UI( ID_EDIT_SELECT_ALL, CIncaView::OnUpdateSelectAll )

	// printing
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

	// run
	ON_COMMAND(ID_PROGRAM_CHECKSYNTAX, CIncaView::OnCheckSyntax )
	ON_COMMAND(ID_PROGRAM_RUN, CIncaView::OnRun )
	ON_COMMAND(ID_PROGRAM_STOP, CIncaView::OnStop )
	ON_COMMAND(ID_PROGRAM_PAUSE, CIncaView::OnPause )
	ON_COMMAND(ID_PROGRAM_STEP, CIncaView::OnStepOver )
	ON_COMMAND(ID_PROGRAM_STEP_INTO, CIncaView::OnStepInto )
	ON_COMMAND(ID_PROGRAM_DISASSEMBLE, CIncaView::OnDisassemble )

	ON_UPDATE_COMMAND_UI( ID_PROGRAM_RUN, CIncaView::OnUpdateRun )
	ON_UPDATE_COMMAND_UI( ID_PROGRAM_STOP, CIncaView::OnUpdateStop )
	ON_UPDATE_COMMAND_UI( ID_PROGRAM_PAUSE, CIncaView::OnUpdatePause )
	ON_UPDATE_COMMAND_UI( ID_PROGRAM_STEP, CIncaView::OnUpdateStepOver )
	ON_UPDATE_COMMAND_UI( ID_PROGRAM_STEP_INTO, CIncaView::OnUpdateStepInto )
	ON_UPDATE_COMMAND_UI( ID_PROGRAM_DISASSEMBLE, CIncaView::OnUpdateDisassemble )
END_MESSAGE_MAP()

// creation

CIncaView::CIncaView()
{
	mCodeViewInit = false;

	mBeamCursor = AfxGetApp()->LoadStandardCursor( IDC_IBEAM );
	mArrowCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
	mTimer = 0;
	m_pFindDialog = 0;
	m_pStackFrameWnd = 0;
	m_pVarCrawlWnd = 0;
	mPaused = false;
	mMouseLocation.x = mMouseLocation.y = 0;
	mStackSize = kDefaultStackSize;
}

CIncaView::~CIncaView()
{
	if( m_pVarCrawlWnd )
		delete m_pVarCrawlWnd;
	if( m_pStackFrameWnd )
		delete m_pStackFrameWnd;
}

BOOL CIncaView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CIncaView::OnDraw(CDC* pDC)
{
	CIncaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if( pDoc->mCodeView == 0 )
		return;

	if( not mCodeViewInit )
	{
		pDoc->mCodeView->set_mfc_view( this );
		pDoc->mCodeView->recalc_image_size();
		pDoc->mCodeView->recalc_image_size();
		// call twice to setup scroll bars properly
		mCodeViewInit = true;
	}

	// HACK
	if( GetFocus() == this )
		pDoc->mCodeView->activate();

	pDC->SelectObject( pDoc->mFont );

	RECT r;
	GetClientRect( &r );

	point origin = pDoc->mCodeView->image_location();

	rect clip;
	clip.left = r.left + origin.h;
	clip.right = r.right + origin.h;
	clip.top = r.top + origin.v;
	clip.bottom = r.bottom + origin.v;
	
	grafport_t port( pDC, clip );

	port.set_origin(
		origin );

	port.fill_rect(
		r.left + origin.h,
		r.top + origin.v,
		r.right + origin.h,
		r.bottom + origin.v,
		make_color( 0, 255, 255, 255 ) );
	
	pDoc->mCodeView->draw( &port );
}

void CIncaView::OnLButtonDown(UINT nFlags,CPoint p)
{
	CIncaDoc* pDoc = GetDocument();
	if( pDoc->mCodeView == 0 )
		return;

	point location = pDoc->mCodeView->image_location();

	pDoc->mCodeView->mouse_down(
		p.x + location.h,
		p.y + location.v,
		mouse_button_left );

	SetCapture();
}

void CIncaView::OnLButtonDblClk(UINT nFlags,CPoint p)
{
	CIncaDoc* pDoc = GetDocument();
	if( pDoc->mCodeView == 0 )
		return;

	point location = pDoc->mCodeView->image_location();

	pDoc->mCodeView->mouse_down(
		p.x + location.h,
		p.y + location.v,
		mouse_button_left );
}

void CIncaView::OnLButtonUp(UINT nFlags,CPoint p)
{
	CIncaDoc* pDoc = GetDocument();
	if( pDoc->mCodeView == 0 )
		return;

	point location = pDoc->mCodeView->image_location();

	pDoc->mCodeView->mouse_up(
		p.x + location.h,
		p.y + location.v,
		mouse_button_left );

	/*if( mTimer )
	{
		KillTimer( mTimer );
		mTimer = 0;
	}*/
	ReleaseCapture();
}

void CIncaView::OnMouseMove(UINT nFlags,CPoint p)
{
	CIncaDoc* pDoc = GetDocument();
	if( pDoc->mCodeView == 0 )
		return;

	point location = pDoc->mCodeView->image_location();
	pDoc->mCodeView->mouse_dragged(
		p.x + location.h,
		p.y + location.v );

	mMouseLocation = p;
}

BOOL CIncaView::OnMouseWheel(UINT nFlags,short zDelta,CPoint pt)
{
	CIncaDoc* pDoc = GetDocument();
	if( pDoc->mCodeView == 0 )
		return TRUE;

	pDoc->mCodeView->pinned_scroll_by(
		0, ( -zDelta / WHEEL_DELTA ) *
			pDoc->mCodeView->line_height() * 3 );

	return TRUE;
}

void CIncaView::OnTimer(UINT nIDEvent) 
{
	CView::OnTimer( nIDEvent );
}

BOOL CIncaView::OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message)
{
	if( nHitTest == HTCLIENT &&
		mMouseLocation.x > 16 )
	{
		SetCursor( mBeamCursor );
	}
	else
		SetCursor( mArrowCursor );	
	return TRUE;
}

void CIncaView::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	CIncaDoc* pDoc = GetDocument();
	RECT r;
	GetClientRect( &r );
	const int linestep = pDoc->mCodeView->tab_width();

	switch( nSBCode )
	{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			SetScrollPos( SB_HORZ, nPos );
			pDoc->mCodeView->request_update();
			break;

		case SB_PAGELEFT:
			pDoc->mCodeView->pinned_scroll_by(
				-( r.right - r.left ), 0 );
			break;

		case SB_PAGERIGHT:
			pDoc->mCodeView->pinned_scroll_by(
				( r.right - r.left ), 0 );
			break;

		case SB_LINELEFT:
			pDoc->mCodeView->pinned_scroll_by(
				-linestep, 0 );
			break;

		case SB_LINERIGHT:
			pDoc->mCodeView->pinned_scroll_by(
				linestep, 0 );
			break;
	}
}

void CIncaView::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	CIncaDoc* pDoc = GetDocument();
	RECT r;
	GetClientRect( &r );
	const int linestep = pDoc->mCodeView->line_height();

	switch( nSBCode )
	{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			SetScrollPos( SB_VERT, nPos );
			pDoc->mCodeView->request_update();
			break;
	
		case SB_PAGEDOWN:
			pDoc->mCodeView->pinned_scroll_by(
				0, ( r.bottom - r.top ) );
			break;

		case SB_PAGEUP:
			pDoc->mCodeView->pinned_scroll_by(
				0, -( r.bottom - r.top ) );
			break;

		case SB_LINEDOWN:
			pDoc->mCodeView->pinned_scroll_by(
				0, linestep );
			break;

		case SB_LINEUP:
			pDoc->mCodeView->pinned_scroll_by(
				0, -linestep );
			break;
	}
}

void CIncaView::OnSize( UINT nType, int cx, int cy )
{
	CIncaDoc* pDoc = GetDocument();
	pDoc->mCodeView->recalc_image_size();
}

static int GetKeyModifiers()
{
	int modifiers = 0;

	if( GetKeyState( VK_SHIFT ) >> 15 )
		modifiers |= key_modifier_shift;
	if( GetKeyState( VK_CONTROL ) >> 15 )
		modifiers |= key_modifier_option;

	return modifiers;
}

void CIncaView::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	CIncaDoc* pDoc = GetDocument();

	int code = 0;
	RECT r;

	switch( nChar )
	{
		case VK_LEFT:
			code = key_arrow_left;
			break;

		case VK_RIGHT:
			code = key_arrow_right;
			break;

		case VK_UP:
			code = key_arrow_up;
			break;

		case VK_DOWN:
			code = key_arrow_down;
			break;

		case VK_DELETE:
			code = key_delete;
			break;

		case VK_PRIOR:
			GetClientRect( &r );
			pDoc->mCodeView->pinned_scroll_by( 0,
				r.top - r.bottom );
			break;

		case VK_NEXT:
			GetClientRect( &r );
			pDoc->mCodeView->pinned_scroll_by( 0,
				r.bottom - r.top );
			break;

		case VK_HOME:
			pDoc->mCodeView->pinned_scroll_by( 0,
				-pDoc->mCodeView->image_size().height );
			break;

		case VK_END:
			pDoc->mCodeView->pinned_scroll_by( 0,
				pDoc->mCodeView->image_size().height );
			break;
	}

	if( code )
		pDoc->mCodeView->key_down( code, GetKeyModifiers() );

	CView::OnKeyDown( nChar, nRepCnt, nFlags );
}

void CIncaView::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	CIncaDoc* pDoc = GetDocument();

	int modifiers = GetKeyModifiers();

	switch( nChar )
	{
		case 0x08: // backspace 
			nChar = key_backspace;
			break;

		case '\t':
			nChar = key_tabulator;
			if( modifiers == key_modifier_shift )
				modifiers = key_modifier_option;
			break;
	}

	pDoc->mCodeView->key_down( nChar, modifiers );
	CView::OnChar( nChar, nRepCnt, nFlags );

	pDoc->SetModifiedFlag( true );
}

BOOL CIncaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CIncaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CIncaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CIncaView::OnUndo()
{
	CIncaDoc* pDoc = GetDocument();
	pDoc->mCodeView->process_command( cmd_Undo, 0 );
	pDoc->SetModifiedFlag( true );
}

void CIncaView::OnRedo()
{
	CIncaDoc* pDoc = GetDocument();
	pDoc->mCodeView->process_command( cmd_Redo, 0 );
	pDoc->SetModifiedFlag( true );
}

void CIncaView::OnUpdateUndo(CCmdUI* pCmdUI)
{
	CIncaDoc* pDoc = GetDocument();
	command_status_t status;
	pDoc->mCodeView->command_status( cmd_Undo, status );
	pCmdUI->Enable( status.enabled );
}

void CIncaView::OnUpdateRedo(CCmdUI* pCmdUI)
{
	CIncaDoc* pDoc = GetDocument();
	command_status_t status;
	pDoc->mCodeView->command_status( cmd_Redo, status );
	pCmdUI->Enable( status.enabled );
}

void
CIncaView::OnFind()
{
	if( m_pFindDialog )
	{
		if( m_bFindOnly )
			return;
		delete m_pFindDialog;
		m_pFindDialog = 0;
	}
	
	CIncaDoc* pDoc = GetDocument();

	istring text;
	codepos_t from, to;
	pDoc->mCodeView->get_selection( from, to );
	pDoc->mCodeView->extract_text( from, to, text );

	m_pFindDialog = new CFindReplaceDialog;
	m_pFindDialog->Create( true, text.c_str(), NULL, FR_DOWN, this );
	m_bFindOnly = true;
}

void
CIncaView::OnReplace()
{
	if( m_pFindDialog )
	{
		if( not m_bFindOnly )
			return;
		delete m_pFindDialog;
		m_pFindDialog = 0;
	}
	
	CIncaDoc* pDoc = GetDocument();

	istring text;
	codepos_t from, to;
	pDoc->mCodeView->get_selection( from, to );
	pDoc->mCodeView->extract_text( from, to, text );

	m_pFindDialog = new CFindReplaceDialog;
	m_pFindDialog->Create( false, text.c_str(), NULL, FR_DOWN, this );
	m_bFindOnly = false;
}

LONG
CIncaView::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	CIncaDoc* pDoc = GetDocument();

	ASSERT(m_pFindDialog != NULL);

    // If the FR_DIALOGTERM flag is set,
    // invalidate the handle identifying the dialog box.
    if (m_pFindDialog->IsTerminating())
    {
        m_pFindDialog = NULL;
        return 0;
    }

	CString findName = m_pFindDialog->GetFindString();
	CString replaceName = m_pFindDialog->GetReplaceString();
	bool bMatchCase = m_pFindDialog->MatchCase() == TRUE;
	bool bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
	bool bSearchDown = m_pFindDialog->SearchDown() == TRUE;

	int flags = ( bMatchCase ? 0 : 1 ) +
		( bMatchWholeWord ? 2 : 0 );

	if( m_pFindDialog->ReplaceAll() )
	{
		long count = pDoc->mCodeView->replace_all(
			findName, replaceName, flags );
		char buf[ 64 ];
		sprintf( buf, "%ld occurences replaced.", count );
		MessageBox( buf, "Inca Editor" );

		pDoc->SetModifiedFlag( true );
	}
	else if( m_pFindDialog->ReplaceCurrent() )
	{
		codepos_t begin, end;
				
		pDoc->mCodeView->get_selection(
			begin, end );

		pDoc->mCodeView->remove_text(
			begin, end );
		pDoc->mCodeView->insert_text(
			begin, replaceName, strlen( replaceName ) );

		end.line = begin.line;
		end.offset = begin.offset + strlen( replaceName );
		pDoc->mCodeView->set_selection( begin, end );

		{
			code_submit_t submit( pDoc->mCodeView );
		}

		pDoc->SetModifiedFlag( true );
	}
	else if( m_pFindDialog->FindNext() )
    {
		if( pDoc->mCodeView->find(
			findName, flags, bSearchDown ? 1 : -1 ) )
		{
			pDoc->mCodeView->scroll_to_selection();
		}
		else
		{
			MessageBeep( MB_ICONEXCLAMATION );
		}
    }

    return 0;
}

void CIncaView::OnCut()
{
	CIncaDoc* pDoc = GetDocument();
	pDoc->mCodeView->process_command( cmd_Cut, 0 );
	pDoc->SetModifiedFlag( true );
}

void CIncaView::OnCopy()
{
	CIncaDoc* pDoc = GetDocument();
	pDoc->mCodeView->process_command( cmd_Copy, 0 );
	pDoc->SetModifiedFlag( true );
}

void CIncaView::OnPaste()
{
	CIncaDoc* pDoc = GetDocument();
	pDoc->mCodeView->process_command( cmd_Paste, 0 );
	pDoc->SetModifiedFlag( true );
}

void CIncaView::OnClear()
{
	CIncaDoc* pDoc = GetDocument();
	pDoc->mCodeView->process_command( cmd_Clear, 0 );
	pDoc->SetModifiedFlag( true );
}

void CIncaView::OnSelectAll()
{
	CIncaDoc* pDoc = GetDocument();
	pDoc->mCodeView->process_command( cmd_SelectAll, 0 );
}

void CIncaView::OnUpdateCut(CCmdUI* pCmdUI)
{
	CIncaDoc* pDoc = GetDocument();
	command_status_t status;
	pDoc->mCodeView->command_status( cmd_Cut, status );
	pCmdUI->Enable( status.enabled );
}

void CIncaView::OnUpdateCopy(CCmdUI* pCmdUI)
{
	CIncaDoc* pDoc = GetDocument();
	command_status_t status;
	pDoc->mCodeView->command_status( cmd_Copy, status );
	pCmdUI->Enable( status.enabled );
}

void CIncaView::OnUpdatePaste(CCmdUI* pCmdUI)
{
	CIncaDoc* pDoc = GetDocument();
	command_status_t status;
	pDoc->mCodeView->command_status( cmd_Paste, status );
	pCmdUI->Enable( status.enabled );
}

void CIncaView::OnUpdateClear(CCmdUI* pCmdUI)
{
	CIncaDoc* pDoc = GetDocument();
	command_status_t status;
	pDoc->mCodeView->command_status( cmd_Clear, status );
	pCmdUI->Enable( status.enabled );
}

void CIncaView::OnUpdateSelectAll(CCmdUI* pCmdUI)
{
	CIncaDoc* pDoc = GetDocument();
	command_status_t status;
	pDoc->mCodeView->command_status( cmd_SelectAll, status );
	pCmdUI->Enable( status.enabled );
}

void CIncaView::OnCheckSyntax()
{
	MakeProgram();
}

void CIncaView::Resume( long message )
{
	mVMThreadData.stream->write_u32( message );
	mVMThreadData.stream->flush();
	OnKillVM();
	mPaused = false;
	if( mVMThreadData.resumeEvent )
		SetEvent( mVMThreadData.resumeEvent );
}

void CIncaView::OnRun()
{
	if( IsVMRunning() )
	{
		if( mPaused && mVMThreadData.resume )
			Resume( msg_Continue );
		return;
	}

	mPaused = false;

	if( not MakeProgram() )
		return;

	istring error;
	CIncaDoc* pDoc = GetDocument();
	mProgram.seek( 0, seek_mode_begin );

	if( not StartVM( mProgram.access(), mProgram.length(),
		GetDocument()->GetPathName(), mStackSize,
		pDoc->mCodeView->file(), m_hWnd, error ) )
	{
		SetStatusText( error.c_str() );
	}
}

void CIncaView::OnStop()
{
	KillVM();
}

void CIncaView::OnPause()
{
	mVMThreadData.stream->write_u32( msg_Stop );
	mVMThreadData.stream->flush();
}

void CIncaView::OnStepOver()
{
	Resume( msg_StepOver );
}

void CIncaView::OnStepInto()
{
	Resume( msg_StepInto );
}

void CIncaView::OnUpdateRun(CCmdUI* pCmdUI)
{
	if( not IsVMRunning() )
		pCmdUI->Enable( true );
	else if( mPaused )
		pCmdUI->Enable( mVMThreadData.resume );
	else
		pCmdUI->Enable( FALSE );
}

void CIncaView::OnUpdateStop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsVMRunning() );
}

void CIncaView::OnUpdatePause(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsVMRunning() && not mPaused );
}

void CIncaView::OnUpdateStepOver(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsVMRunning() &&
		mPaused && mVMThreadData.resume );
}

void CIncaView::OnUpdateStepInto(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsVMRunning() &&
		mPaused && mVMThreadData.resume );
}

static void StatusBarMessage( char* fmt, ... )
{
	if( AfxGetApp() != NULL && AfxGetApp()->m_pMainWnd != NULL )
	{
		char buffer[256];
		CStatusBar* pStatus = (CStatusBar*) 
			AfxGetApp()->m_pMainWnd->GetDescendantWindow(
				AFX_IDW_STATUS_BAR);
		va_list argptr;
		va_start(argptr, fmt);
		_vsnprintf(buffer, 255, fmt, argptr);
		va_end(argptr);
		if (pStatus != NULL) {
			pStatus->SetPaneText(0, buffer);
			pStatus->UpdateWindow();
		}
	}
}

CChildFrame* CIncaView::GetChildFrm()
{
	return (CChildFrame*)GetParent();
}

void CIncaView::SetStatusText( const char* s )
{
	GetChildFrm()->GetStatusBar()->SetPaneText(
		0, s );
}

bool CIncaView::MakeProgram()
{
	CIncaDoc* pDoc = GetDocument();
	error_desc_t errorDesc;

	__try
	{
		if( Compile( &errorDesc ) )
		{
			SetStatusText( errorDesc.text );
		
			pDoc->mCodeView->select_token( errorDesc.where );
			pDoc->mCodeView->scroll_to_selection();
			ifree( errorDesc.text );
		}
		else
		{
			SetStatusText( "" );
			return true;
		}
	}
	__except( 1 )
	{
		SetStatusText( "fatal compiler error" );
		return false;
	}
	
	return false;
}

bool CIncaView::Compile( error_desc_t* error_desc )
{
	CIncaDoc*			pDoc = GetDocument();

	code_view_t*		code_view = pDoc->mCodeView;
	memory_buffer_t*	buffer = &mProgram;

	code_submit_t		submit( code_view );						
	code::file_t*		file;
		
	file = code_view->file();
	
	buffer->clear();
	buffer->seek( 0, seek_mode_begin );
	
	::init_lexer( file );
		
	compiler::compiler_t	compiler( buffer );

	compiler.set_stack_size( kDefaultStackSize );

	compiler.run();
	
	::cleanup_lexer();

	submit.release();

	compiler::error_t*		error = compiler.error();

	if( not error )
	{	
		mStackSize = compiler.get_stack_size();
		return false;
	}

	// copy error
	
	long					length = (long)error->what.length();
	
	error_desc->text = (char*)imalloc(
		sizeof( char ) * ( length + 1 ) );
	if( !error_desc->text )
		throw std::bad_alloc();
		
	istrcpy( error_desc->text,
		error->what.c_str() );
	
	error_desc->where = error->fpos.pos;

	return true;
}

void CIncaView::OnDisassemble()
{
#ifdef SUPPORT_DISASSEMBLE
	if( not MakeProgram() )
		return;

	CMultiDocTemplate*	temp;

	temp = ( (CIncaApp*)AfxGetApp() )->GetTextDocumentTemplate();

	CPadDoc*			doc;
	CFrameWnd*			frame;
	
	doc = (CPadDoc*)temp->CreateNewDocument();
	frame = temp->CreateNewFrame( doc, NULL );

	frame->BringWindowToTop();
	frame->ShowWindow( SW_SHOW );

	char*				text;

	text = DisassembleProgram( &mProgram );
	
	doc->SetText( text );
	ifree( text );
#endif
}

void CIncaView::OnUpdateDisassemble(CCmdUI* pCmdUI)
{
#ifdef SUPPORT_DISASSEMBLE
	pCmdUI->Enable( true );
#else
	pCmdUI->Enable( false );
#endif
}

#ifdef _DEBUG
void CIncaView::AssertValid() const
{
	CView::AssertValid();
}

void CIncaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIncaDoc* CIncaView::GetDocument() const // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIncaDoc)));
	return (CIncaDoc*)m_pDocument;
}
#endif //_DEBUG

void CIncaView::OnKillVM()
{
	CIncaDoc* pDoc = GetDocument();

	if( m_pVarCrawlWnd )
	{
		delete m_pVarCrawlWnd;
		m_pVarCrawlWnd = 0;
	}

	if( m_pStackFrameWnd )
	{
		delete m_pStackFrameWnd;
		m_pStackFrameWnd = 0;
	}

	pDoc->mCodeView->hilite_line( -1 );
	SetStatusText( "" );
}

LONG CIncaView::OnProgramPaused(WPARAM wParam, LPARAM lParam)
{
	AfxGetMainWnd()->SetForegroundWindow();

	OnKillVM();

	CString strMyClass;

	strMyClass = AfxRegisterWndClass(
		CS_VREDRAW | CS_HREDRAW,
		::LoadCursor( NULL, IDC_ARROW ),
		(HBRUSH) ::GetStockObject( WHITE_BRUSH ),
		::LoadIcon( NULL, IDI_APPLICATION ) );

	RECT viewRect, r;
	RECT screenRect;
	GetParent()->GetWindowRect( &viewRect );
	long viewWidth = viewRect.right - viewRect.left;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &screenRect, 0 );

	{
		r.right = viewRect.right;
		r.top = viewRect.bottom;
		r.left = r.right - viewWidth / 2;
		r.bottom = r.top + 150;
		if( r.bottom > screenRect.bottom )
			OffsetRect( &r, 0, viewRect.top - r.top );

		CVarCrawlWnd* pWnd = new CVarCrawlWnd;
		m_pVarCrawlWnd = pWnd;
		pWnd->Create( IDD_VARCRAWL );
		pWnd->Setup( this, mVMThreadData.stream );
		pWnd->MoveWindow( &r );
		pWnd->ShowWindow( SW_SHOWDEFAULT );
	}

	{
		r.left = viewRect.left;
		r.top = viewRect.bottom;
		r.right = r.left + viewWidth / 2;
		r.bottom = r.top + 150;

		if( r.bottom > screenRect.bottom )
			OffsetRect( &r, 0, viewRect.top - r.top );

		CStackCrawlWnd* pWnd = new CStackCrawlWnd;
		m_pStackFrameWnd = pWnd;
		pWnd->Create( IDD_STACKCRAWL );
		pWnd->Setup( this, &mVMThreadData.threads[ 0 ], 0 );
		pWnd->MoveWindow( &r );
		pWnd->ShowWindow( SW_SHOWDEFAULT );
	}

	SetStatusText( mVMThreadData.error.c_str() );

	mPaused = true;

	return 0;
}

LONG CIncaView::OnProgramTerminated(WPARAM wParam, LPARAM lParam)
{
	ExitVM();
	return 0;
}

LONG CIncaView::OnFrameChanged(WPARAM wParam, LPARAM lParam)
{
	StackFrame* frame;
	frame = &mVMThreadData.threads[
		wParam ].frames[ lParam ];

	CIncaDoc* pDoc = GetDocument();
	pDoc->mCodeView->select_token( frame->pos );
	pDoc->mCodeView->scroll_to_selection();
	codepos_t begin, end;
	pDoc->mCodeView->get_selection( begin, end );
	pDoc->mCodeView->hilite_line( begin.line );
	
	if( m_pVarCrawlWnd )
	{
		( (CVarCrawlWnd*)m_pVarCrawlWnd )->Load(
			&mVMThreadData.threads[ wParam ], wParam,
			lParam );
	}

	return 0;
}
