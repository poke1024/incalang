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
//	CStackCrawlWnd.h		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "VMThread.h"

BEGIN_AKLABETH_NAMESPACE

class CStackCrawlWnd : public CDialog {
public:
						CStackCrawlWnd();

	virtual				~CStackCrawlWnd();

	void				Setup(
							CWnd*				view,
							ThreadState*		threadState,
							int					threadIndex );

protected:
	afx_msg int			OnCreate(
							LPCREATESTRUCT		lpCreateStruct );

	afx_msg void		OnSize(
							UINT				nType,
							int					cx,
							int					cy );

	afx_msg LONG		OnFrameChanged(
							WPARAM				wParam,
							LPARAM				lParam );

	virtual BOOL 		OnInitDialog();

	CListCtrl*			mList;

	CWnd*				mView;
	ThreadState*		mThreadState;
	int					mThreadIndex;

	DECLARE_MESSAGE_MAP()
};

END_AKLABETH_NAMESPACE
