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
//	VMThread.cpp 			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "stdafx.h"
#include "VMThread.h"
#include <process.h>
#include "ariadne_messages.h"
#include "UserMessages.h"
#include "variable_t.h"

BEGIN_AKLABETH_NAMESPACE

void
InitThreadStates(
	VMThreadData*		data )
{
	data->threadCount = 0;
	data->threads = 0;
	data->resumeEvent = 0;
}

void
CleanupThreadStates(
	VMThreadData*		data )
{
	long				threadCount;

	threadCount = data->threadCount;
	for( long i = 0; i < threadCount; i++ )
	{
		ThreadState*	state = &data->threads[ i ];
		long			frameCount = state->frameCount;

		for( long i = 0; i < frameCount; i++ )
			delete state->frames[ i ].globals;

		delete[] state->frames;
	}

	delete[] data->threads;

	data->threadCount = 0;
	data->threads = 0;
}

static void
ObtainThreadState(
	VMThreadData*		data,
	long				index )
{
	ThreadState*		state;

	state = &data->threads[ index ];
	state->name = "";

	pipe_stream_t*		stream = data->stream;

	long				frameCount;

	stream->write_u32(
		msg_GetStackFrameDepth );
	frameCount = stream->read_u32();

	state->frameCount = frameCount;
	state->frames = new StackFrame[ frameCount ];

	for( long i = 0; i < frameCount; i++ )
		state->frames[ i ].globals = new variable_t;

	for( long i = 0; i < frameCount; i++ )
	{
		StackFrame*		frame = &state->frames[ i ];

		stream->write_u32(
			msg_GetStackFrameInfo );
		stream->write_u32( i );
		stream->read_string( frame->name );
		frame->pos = stream->read_u32();
		frame->clss = stream->read_ptr();
		frame->hasThis = stream->read_u08();
	}

	for( long i = 0; i < frameCount; i++ )
	{
		StackFrame*		frame = &state->frames[ i ];

		frame->globals->init_as_globals(
			stream, frame->clss );
	}
}

static void
HandleMachineStopped(
	VMThreadData*		data )
{
	pipe_stream_t*		stream = data->stream;

	data->paused = true;
	data->resume = stream->read_u08();
	stream->read_string( data->error );

	CleanupThreadStates( data );

	data->threadCount = 1;
	data->threads = new ThreadState[ 1 ];

	ObtainThreadState( data, 0 );

	PostMessage( data->hwnd,
		WM_PROGRAM_PAUSED, 0, 0 );

	data->resumeEvent = CreateEvent(
		NULL, FALSE, FALSE, NULL );

	WaitForSingleObject(
		data->resumeEvent, INFINITE );

	if( data->resumeEvent )
		CloseHandle( data->resumeEvent );
	data->resumeEvent = 0;
}

unsigned __stdcall
VMThreadFunc(
	void*				pArguments )
{
	VMThreadData*		data;
	pipe_stream_t*		stream;

	data = (VMThreadData*)pArguments;
	stream = data->stream;

	try
	{
		while( true )
		{
			u32			message;

			message = stream->read_u32();

			switch( message )
			{
				case msg_ProgramTerminated:
					stream->write_u32(
						msg_QuitMachine );
					stream->flush();
					PostMessage( data->hwnd,
						WM_PROGRAM_TERMINATED, 0, 0 );
					break;

				case msg_NoMainFound:
					MessageBox( NULL, "Please specify a main method.",
						"Inca Information", MB_OK );
					stream->write_u32(
						msg_QuitMachine );
					stream->flush();
					PostMessage( data->hwnd,
						WM_PROGRAM_TERMINATED, 0, 0 );
					break;

				case msg_MachineStopped:
					HandleMachineStopped( data );
					break;
			}
		}
	}
	catch(...)
	{
		while( true )
			;
	}

	_endthreadex( 0 );
	return 0;
} 

END_AKLABETH_NAMESPACE
