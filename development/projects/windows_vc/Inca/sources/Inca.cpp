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

// Inca.cpp
//

#include "stdafx.h"
#include "Inca.h"
#include "MainFrm.h"
#include "CPadDoc.h"

#include "ChildFrm.h"
#include "IncaDoc.h"
#include "IncaView.h"

#include "code_options.h"
#include "variable_t.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//char gAppPath[ _MAX_PATH + 1 ];
char *gAppPath;

// CIncaApp

BEGIN_MESSAGE_MAP(CIncaApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// file
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// printing
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

// constructor

CIncaApp::CIncaApp()
{
	const char* cmdline = GetCommandLine();
	if( strstr( cmdline, "\\" ) )
	{
		gAppPath = new char[ strlen( cmdline ) + 1 ];
		strcpy( gAppPath, cmdline );
		char* p = strrchr( gAppPath, '\\' );
		p[ 1 ] = '\0';
		while( gAppPath[ 0 ] == ' ' || gAppPath[ 0 ] == '\"' )
			gAppPath++;
	}
	else
	{
		gAppPath = new char[ _MAX_PATH + 1 ];
		_getcwd( gAppPath, _MAX_PATH );
	}

	for( int i = 0; i < strlen( gAppPath ); i++ )
		if( gAppPath[ i ] == '\\' )
			gAppPath[ i ] = '/';

	/*FILE* fp = fopen( "fullpath.txt", "w" );
	fprintf( fp, "%s", gAppPath );
	fclose( fp );*/

	// _pgmptr
	/*gAppPath = new char[ strlen( _pgmptr ) + 1 ];
	strcpy( gAppPath, _pgmptr );
	char* p = strrchr( gAppPath, '\\' );
	if( p )
		p[ 1 ] = '\0';
	p = gAppPath;*/

	code::read_options( "" );

	aklabeth::variable_t::init(
		pathForResource( "system", "txt" ) );

	mTextDocumentTemplate = NULL;
}


// the main application's instance

CIncaApp theApp;

// initialisation

BOOL CIncaApp::InitInstance()
{
	InitCommonControls();

	CWinApp::InitInstance();

	// initialize OLE
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("Vom lokalen Anwendungs-Assistenten generierte Anwendungen"));
	LoadStdProfileSettings(4);

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_IncaTYPE,
		RUNTIME_CLASS(CIncaDoc),
		RUNTIME_CLASS(CChildFrame), // overridden frame
		RUNTIME_CLASS(CIncaView));
	AddDocTemplate(pDocTemplate);

	mTextDocumentTemplate = new CMultiDocTemplate(IDR_TEXTTYPE,
		RUNTIME_CLASS(CPadDoc), RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CEditView));
	AddDocTemplate( mTextDocumentTemplate );


	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	return TRUE;
}
		
CMultiDocTemplate* CIncaApp::GetTextDocumentTemplate()
{
	return mTextDocumentTemplate;
}

// about box

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CIncaApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

char*
pathForResource(
	const char*			name,
	const char*			extension )
{
	std::string			s;
	
	s = gAppPath;

	s.append( "data/" );
	s.append( name );
	s.append( "." );
	s.append( extension );

	char*				path;
	path = (char*)imalloc(
		sizeof( char )* ( s.length() + 1 ) );
	strcpy( path, s.c_str() );
	return path;
}
