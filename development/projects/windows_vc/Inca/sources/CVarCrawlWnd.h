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
//	CVarCrawlWnd.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "VMThread.h"
#include "variable_array_t.h"
#include "pipe_stream_t.h"

BEGIN_AKLABETH_NAMESPACE

class CVarCrawlWnd : public CDialog {
public:
						CVarCrawlWnd();

	virtual				~CVarCrawlWnd();

	void				Setup(
							CWnd*				view,
							pipe_stream_t*		stream );

	void				Load(
							ThreadState*		threadState,
							int					threadIndex,
							int					frameIndex );

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
	pipe_stream_t*		mStream;

	ThreadState*		mThreadState;
	int					mThreadIndex;
	int					mFrameIndex;

	variable_array_t	mVars;

	DECLARE_MESSAGE_MAP()
};

END_AKLABETH_NAMESPACE
