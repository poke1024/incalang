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

// Inca.h

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CIncaApp : public CWinApp
{
public:
	CIncaApp();

	CMultiDocTemplate* GetTextDocumentTemplate();

public:
	virtual BOOL InitInstance();

	CMultiDocTemplate* mTextDocumentTemplate;

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CIncaApp theApp;

char*
pathForResource(
	const char*			name,
	const char*			extension );
