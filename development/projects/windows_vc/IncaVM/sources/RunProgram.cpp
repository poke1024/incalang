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

#include "stdafx.h"
#include "program_thread_t.h"
#include "buffered_file_stream_t.h"
#include "pipe_input_stream_t.h"
#include "scheduler_t.h"
#include <stdio.h>
#include "vmglobals.h"
#include "vmbind.h"
#include "sgl.h"
#include <windows.h>

using namespace machine;

static void
run_program()
{
	HANDLE				hPipeRead;

	hPipeRead = GetStdHandle( STD_INPUT_HANDLE );
	if( hPipeRead == INVALID_HANDLE_VALUE )
	{
		MessageBox( NULL, "Problem opening pipe",
			"IncaVM Error", MB_OK );
		exit( EXIT_FAILURE );
	}

	pipe_input_stream_t*	pipestream;
	
	pipestream = new pipe_input_stream_t( hPipeRead );

	u32					partition_size;
	
#if RUN_FROM_PIPE
	char*				basepath;	
	u32					length;
	
	length = pipestream->read_u32();
	basepath = new char[ length + 1 ];
	pipestream->read_bytes( basepath, length );
	basepath[ length ] = '\0';

	//pipestream->read_u32(); // ignore partition_size
#else
	const char*			basepath = "";
#endif
	memory_t*			memory = new memory_t;
	
	machine_t*			machine = new machine_t(
							memory, basepath );

	input_stream_t*		stream;
	program_thread_t*	thread;

#if RUN_FROM_PIPE
	delete[] basepath;
	length = pipestream->read_u32();
	stream = pipestream;

	/*{
		file_stream_t		output( "temp.o", "wb" );
		for( long i = 0; i < length; i++ )
			output.write_u08( stream->read_u08() );
	}
	exit( 0 );*/
#else
	stream = new file_stream_t( "temp.o", "rb" );
#endif
	
	thread = new program_thread_t( machine, stream );
	
	rt_enter_threads();
}

void
enter_program()
{
	try
	{
		icarus_init();
		fizban::sgl_init();
		vm_bind_init();
	
		run_program();

		vm_bind_cleanup();
		fizban::sgl_cleanup();
	}
	catch( exception& e )
	{
		MessageBox( NULL, e.what(), "Unexpected Error", MB_OK );
		exit( 1 );
	}
}
