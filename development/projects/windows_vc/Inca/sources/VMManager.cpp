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
//	VMManager.cpp			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#define WRITE_TEMP_O 0

#include "stdafx.h"
#include "Inca.h"
#include <process.h>
#include "VMManager.h"
#include "pipe_input_stream_t.h"
#include "pipe_output_stream_t.h"

#if WRITE_TEMP_O
#include "file_stream_t.h"
#endif

BEGIN_AKLABETH_NAMESPACE

VMManager::VMManager()
{
	m_hVMPipeReadClient = 0;
	m_hVMPipeWriteClient = 0;
	m_hVMPipeReadServer = 0;
	m_hVMPipeWriteServer = 0;

	m_hVMProcess = 0;
	m_hVMThread = 0;

	mVMReadStream = 0;
	mVMWriteStream = 0;
	mVMPipeStream = 0;

	mPartitionSize = 1024;

	InitThreadStates( &mVMThreadData );
}

VMManager::~VMManager()
{
	KillVM();
}

bool
VMManager::StartVM(
	const void*			programData,
	long				programLength,
	const CString&		basePath,
	long				stackSize,
	code::file_t*		codeFile,
	HWND				hwnd,
	istring&			error )
{
	KillVM();

	char*				szProcess;		// name of child process image
	SECURITY_ATTRIBUTES sa;				// security privileges for handles
	STARTUPINFO			sui;			// info for starting a process
	PROCESS_INFORMATION pi;				// info returned about a process
	int					iLen;			// return value
	BOOL				bTest;			// return value
	HANDLE				hPipeReadClient;	// inbound end of pipe (for client)
	HANDLE				hPipeWriteClient;
	HANDLE				hProcess;

	HANDLE				hPipeReadServer;
	HANDLE				hPipeWriteServer;

	// fill out a SECURITY_ATTRIBUTES struct so handles are inherited
	sa.nLength = sizeof( SECURITY_ATTRIBUTES );	// structure size
	sa.lpSecurityDescriptor = NULL;				// default descriptor
	sa.bInheritHandle = TRUE;					// inheritable
    
	// create the pipe
	bTest = CreatePipe(
		&hPipeReadClient,	// reading handle
		&hPipeWriteClient,	// writing handle
		&sa,				// lets handles be inherited
		0 );				// default buffer size

	// create the pipe
	if( bTest )
		bTest = CreatePipe(
			&hPipeReadServer,
			&hPipeWriteServer,
			&sa,
			0 );

	if( !bTest )			// error during pipe creation
	{
		error = "could not create pipe";
		return false;
	}
   
	// make an uninheritable duplicate of the outbound (write) handle
	bTest = DuplicateHandle(
		GetCurrentProcess(),
		hPipeWriteClient,       // original handle
		GetCurrentProcess(),
		NULL,             // don’t create new handle
		0,
		FALSE,            // not inheritable
		DUPLICATE_SAME_ACCESS );

	if( bTest )
		bTest = DuplicateHandle(
			GetCurrentProcess(),
			hPipeReadServer, 
			GetCurrentProcess(),
			NULL, 
			0,
			FALSE,
			DUPLICATE_SAME_ACCESS );

	if( !bTest )			// duplication failed
	{
		error = "could not duplicate pipe handle";
		CloseHandle( hPipeReadClient );
		CloseHandle( hPipeWriteClient );
		CloseHandle( hPipeReadServer );
		CloseHandle( hPipeWriteServer );
		return false;
   }

	// fill in the process’s startup information
	memset( &sui, 0, sizeof( STARTUPINFO ) );
	sui.cb			= sizeof( STARTUPINFO );
	sui.dwFlags		= STARTF_USESTDHANDLES;
	sui.hStdInput	= hPipeReadClient;
	sui.hStdOutput	= hPipeWriteServer;
	sui.hStdError	= GetStdHandle( STD_ERROR_HANDLE );
    
	// create the vm process
	szProcess = pathForResource( "IncaVM", "exe" );

	for( int i = 0; i < (int)strlen( szProcess ); i++ )
		if( szProcess[ i ] == '/' )
			szProcess[ i ] = '\\';

	std::string			cmdline;
	char*				szCmdLine;

	cmdline = "IncaVM.exe"; //szProcess;
	cmdline.append( " *run*" );
	szCmdLine = istrdup( cmdline.c_str() );

	bTest = CreateProcess( szProcess,      // .EXE image
						  szCmdLine,       // command line
                          NULL,            // process security
                          NULL,            // thread security
                          TRUE,            // inherit handles—yes
                          DETACHED_PROCESS, //0,               // creation flags
                          NULL,            // environment block
                          NULL,            // current directory
                          &sui,            // startup info
                          &pi );           // process info (returned)

	ifree( szProcess );
	ifree( szCmdLine );

	// did we succeed in launching the process?
	if( !bTest )
	{
		error = "could not launch vm process";
		CloseHandle( hPipeWriteClient );
		CloseHandle( hPipeReadClient );
		CloseHandle( hPipeWriteServer );
		CloseHandle( hPipeReadServer );
		return false;
	}
	else                                   // creation succeeded
	{
		hProcess = pi.hProcess;             // save new process handle
		CloseHandle( pi.hThread );          // discard new thread handle
	}

	//WaitForInputIdle( hProcess, 2000 );

	m_hVMPipeReadClient = hPipeReadClient;
	m_hVMPipeWriteClient = hPipeWriteClient;
	m_hVMPipeReadServer = hPipeReadServer;
	m_hVMPipeWriteServer = hPipeWriteServer;
	m_hVMProcess = hProcess;

	unsigned			threadID;

	try
	{
		InitThreadStates( &mVMThreadData );

		mVMReadStream = new pipe_input_stream_t( m_hVMPipeReadServer );
		mVMWriteStream = new pipe_output_stream_t( m_hVMPipeWriteClient );
		mVMPipeStream =  new pipe_stream_t(
			mVMReadStream, mVMWriteStream );

		mVMThreadData.hwnd = hwnd;
		mVMThreadData.stream = mVMPipeStream;
		mVMThreadData.paused = false;

		m_hVMThread = (HANDLE)_beginthreadex(
			NULL, 0, &VMThreadFunc, &mVMThreadData, 0,
			&threadID );

		const char*			s;
		const char*			delimiter;
		long				length;
		std::string			raw_path;

		s = (LPCTSTR)basePath;
		delimiter = strrchr( s, '\\' );
		if( delimiter )
			length = delimiter - s + 1;
		else
			length = strlen( s );

		raw_path.reserve( length );
		for( long i = 0; i < length; i++ )
		{
			char c = s[ i ];
			if( c == '\\' )
				raw_path.append( "/" );
			else
				raw_path.append( 1, c );
		}

		mVMWriteStream->write_string( raw_path.c_str() );	

		//mVMWriteStream->write_u32( mPartitionSize * 1024L );
		mVMWriteStream->write_u32( programLength );
		mVMWriteStream->write_u32( stackSize );
		mVMWriteStream->write_bytes( programData, programLength );

		codeFile->export_breakpoints( mVMWriteStream );

		mVMWriteStream->flush();

#if WRITE_TEMP_O
		icarus::file_stream_t stream( "temp.o", "wb" );
		stream.write_bytes( programData, programLength );
		codeFile->export_breakpoints( &stream );
#endif
	}
	catch(...)
	{
		KillVM();
		error = "error starting vm";
		return false;
	}

	return true;
}

void
VMManager::ExitVM()
{
	WaitForSingleObject( m_hVMProcess, INFINITE );
	CloseHandle( m_hVMProcess );
	m_hVMProcess = 0;

	KillVM();
}

void
VMManager::KillVM()
{
	OnKillVM();

	if( m_hVMThread )
	{
		TerminateThread( m_hVMThread, EXIT_FAILURE );
		CloseHandle( m_hVMThread );
		m_hVMThread = 0;
	}

	CleanupThreadStates( &mVMThreadData );

	if( mVMPipeStream )
	{
		delete mVMPipeStream;
		mVMPipeStream = 0;
	}

	if( mVMReadStream )
	{
		delete mVMReadStream;
		mVMReadStream = 0;
	}

	if( mVMWriteStream )
	{
		delete mVMWriteStream;
		mVMWriteStream = 0;
	}

	if( m_hVMPipeReadClient )
	{
		CloseHandle( m_hVMPipeReadClient );
		m_hVMPipeReadClient = 0;
	}

	if( m_hVMPipeWriteClient )
	{
		CloseHandle( m_hVMPipeWriteClient );
		m_hVMPipeWriteClient = 0;
	}

	if( m_hVMPipeReadServer )
	{
		CloseHandle( m_hVMPipeReadServer );
		m_hVMPipeReadServer = 0;
	}

	if( m_hVMPipeWriteServer )
	{
		CloseHandle( m_hVMPipeWriteServer );
		m_hVMPipeWriteServer = 0;
	}

	if( m_hVMProcess )
	{
		TerminateProcess( m_hVMProcess, 1 );
		CloseHandle( m_hVMProcess );
		m_hVMProcess = 0;
	}
}

bool
VMManager::IsVMRunning()
{
	if( m_hVMProcess == 0 )
		return false;

	DWORD				exitCode;

	if( GetExitCodeProcess(
		m_hVMProcess, &exitCode ) )
	{
		if( exitCode == STILL_ACTIVE )
			return true;

		KillVM();
		return false;
	}

	return true;
}

void
VMManager::OnKillVM()
{
}

END_AKLABETH_NAMESPACE
