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

// IncaView.h

#pragma once

#include "code_view_t.h"
#include "memory_buffer_t.h"
#include "compiler_t.h"
#include "VMManager.h"

using namespace aklabeth;

typedef struct {
	char* text;
	long where;
} error_desc_t;

class CChildFrame;

class CIncaView : public CView, public VMManager
{
protected:
	CIncaView();
	DECLARE_DYNCREATE(CIncaView)

public:
	CIncaDoc* GetDocument() const;

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg void OnDraw(CDC* pDC);

	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags,CPoint point);

	afx_msg void OnMouseMove(UINT nFlags,CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags,short zDelta,CPoint pt);

	afx_msg BOOL OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message);

	afx_msg void OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	afx_msg void OnSize( UINT nType, int cx, int cy );

	afx_msg LONG OnFindReplace(WPARAM wParam, LPARAM lParam);

	void OnUndo();
	void OnRedo();
	void OnUpdateUndo(CCmdUI* pCmdUI);
	void OnUpdateRedo(CCmdUI* pCmdUI);

	void OnFind();
	void OnReplace();

	void OnCut();
	void OnCopy();
	void OnPaste();
	void OnClear();
	void OnSelectAll();
	void OnUpdateCut(CCmdUI* pCmdUI);
	void OnUpdateCopy(CCmdUI* pCmdUI);
	void OnUpdatePaste(CCmdUI* pCmdUI);
	void OnUpdateClear(CCmdUI* pCmdUI);
	void OnUpdateSelectAll(CCmdUI* pCmdUI);

	void OnCheckSyntax();
	void OnRun();
	void OnStop();
	void OnPause();
	void OnStepOver();
	void OnStepInto();
	void OnDisassemble();

	void OnUpdateRun(CCmdUI* pCmdUI);
	void OnUpdateStop(CCmdUI* pCmdUI);
	void OnUpdatePause(CCmdUI* pCmdUI);	
	void OnUpdateStepOver(CCmdUI* pCmdUI);
	void OnUpdateStepInto(CCmdUI* pCmdUI);
	void OnUpdateDisassemble(CCmdUI* pCmdUI);

	void Resume( long message );

	afx_msg LONG OnProgramPaused(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnProgramTerminated(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnFrameChanged(WPARAM wParam, LPARAM lParam);

protected:
	virtual void OnKillVM();

	afx_msg void OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnChar(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);

	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	CChildFrame* GetChildFrm();

public:
	virtual ~CIncaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	friend class CIncaDoc;

	void SetStatusText( const char* s );

	bool MakeProgram();
	bool Compile( error_desc_t* error_desc );

	HCURSOR mArrowCursor;
	HCURSOR mBeamCursor;
	UINT_PTR mTimer;
	CFindReplaceDialog* m_pFindDialog;
	bool m_bFindOnly;
	CWnd* m_pStackFrameWnd;
	CWnd* m_pVarCrawlWnd;
	bool mPaused;
	POINT mMouseLocation;
	long mStackSize;

	code_view_t* mCodeView;
	bool mCodeViewInit;

	memory_buffer_t mProgram;

protected:

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG 
inline CIncaDoc* CIncaView::GetDocument() const
   { return reinterpret_cast<CIncaDoc*>(m_pDocument); }
#endif
