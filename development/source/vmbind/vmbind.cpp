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
//	vmbind.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include <windows.h>
#include <process.h>

#include "vmbind.h"
#include "text_console_t.h"
#include "ariadne_messages.h"

#include "fiber_t.h"
#include "array_t.h"
#include "bytecode_func_t.h"
#include "machine_class_t.h"
#include "machine_t.h"
#include "vmutils.h"
#include "key_buffer_t.h"
#include "heap_t.h"
#include "input_t.h"
#include "screen_t.h"

#include "pipe_input_stream_t.h"
#include "pipe_output_stream_t.h"
#include "vmglobals.h"

machine::key_buffer_t*				g_key_buffer = 0;

extern bool							g_quit_okay;
extern bool							g_quit_issued;

BEGIN_MACHINE_NAMESPACE

USING_FIZBAN_NAMESPACE

static text_console_t*				s_text_console = 0;

void
vm_bind_init()
{
	g_key_buffer = new key_buffer_t;
}

void
vm_bind_cleanup()
{
	vm_flush();
}

s32
vm_inkey(
	machine_t*			vm )
{
	s32					c;
	
	c = g_key_buffer->fetch_key();
	if( c == 0 && s_text_console )
		c = s_text_console->inkey();

	return c;
}

long
vm_mouse_x(
	machine_t*			vm )
{
	screen_t*			screen = screen_t::current();
	if( screen )
		return screen->mouse_x();
	return -1;
}

long
vm_mouse_y(
	machine_t*			vm )
{
	screen_t*			screen = screen_t::current();
	if( screen )
		return screen->mouse_y();
	return -1;
}

long
vm_mouse_z(
	machine_t*			vm )
{
	screen_t*			screen = screen_t::current();
	if( screen )
		return screen->mouse_z();
	return 0;
}

bool
vm_mouse_button(
	machine_t*			vm,
	int					index )
{
	screen_t*			screen = screen_t::current();
	if( screen )
		return screen->mouse_button( index );
	return 0;
}

text_console_t*
vm_text_console(
	machine_t*			vm )
{
	text_console_t*		console;
	
	if( ( console = s_text_console ) == 0 )
	{
		console = new text_console_t;
		s_text_console = console;
	}

	return console;
}

bool
vm_console_open()
{
	return s_text_console;
}

void
vm_process_os_events()
{
	vm_flush();

	fizban::rt_input_update();

	MSG					msg;

	if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
    {
		if( !GetMessage( &msg, NULL, 0, 0 ) )
			return;
	            
		// If you want WM_CHAR messages, add
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

void
vm_wait_for_quit()
{
	if( not vm_console_open() )
		return;

	g_quit_okay = true;
	while( not g_quit_issued )
		vm_process_os_events();
}

// ===========================================================================

bool					g_real_time_mode = false;
static DWORD			s_old_class;
static DWORD			s_old_prio;

void
vm_enter_real_time_mode()
{
	if( g_real_time_mode )
		return;

	HANDLE				current_process;
	HANDLE				current_thread;

	current_process = GetCurrentProcess();
	current_thread = GetCurrentThread();

	s_old_class = GetPriorityClass( current_process );
	s_old_prio = GetThreadPriority( current_thread );

	SetPriorityClass( current_process, REALTIME_PRIORITY_CLASS );
	SetThreadPriority( current_thread, THREAD_PRIORITY_TIME_CRITICAL );

	g_real_time_mode = true;
}

void
vm_leave_real_time_mode()
{
	if( not g_real_time_mode )
		return;

	HANDLE				current_process;
	HANDLE				current_thread;

	current_process = GetCurrentProcess();
	current_thread = GetCurrentThread();

	SetPriorityClass( current_process, s_old_class );
	SetThreadPriority( current_thread, s_old_prio );

	g_real_time_mode = false;
}

// ---------------------------------------------------------------------------

static bool
read_memory(
	fiber_t*			fiber,
	void*				addr,
	u32					length,
	void*				buffer )
{
#if ICARUS_OS_WIN32
	SIZE_T				read = 0;

	if( ReadProcessMemory( GetCurrentProcess(),
		addr, buffer, length, &read ) != TRUE )
	{
		return false;
	}
	else if( read != length )
		return false;
	
	return true;
#else
	if( not fiber->test_access( addr, length ) )
		return false;
	
	memcpy( buffer, addr, length );

	return true;
#endif
}

// ---------------------------------------------------------------------------

static void
stack_crawl(
	fiber_t*			fiber,
	array_t&			array )
{
	machine_t&			m = *fiber->machine();
	const stop_state_t& state = fiber->state();
	
	bytecode_func_t*	func = state.func;
	u08*				localsp = state.localsp;
	u08*				sp;
	const u08*			code = state.code;
	instance_t*			instance = state.instance;
	
	while( func )
	{	
		sp = localsp;

		void*			ptr[ 4 ];

		if( not read_memory( fiber, sp - sizeof( void* ) * 4,
			sizeof( void* ) * 4, ptr ) )
		{
			break;
		}
		
		stack_frame_t	frame;
		
		frame.instance = instance;
		frame.func = func;
		frame.localsp = localsp;
		frame.code = (u08*)code;

		array.insert_at( 0, &frame );
		
		code = (const u08*)ptr[ 0 ];
		localsp = (u08*)ptr[ 1 ];
		instance = (instance_t*)ptr[ 2 ];
		func = (bytecode_func_t*)ptr[ 3 ];

		/*sp -= sizeof( void* );
		func = *(bytecode_func_t**)sp;
		sp -= sizeof( void* );
		instance = *(instance_t**)sp;
		sp -= sizeof( void* );
		localsp = *(u08**)sp;
		sp -= sizeof( void* );
		code = *(const u08**)sp;*/
	}
}

// ---------------------------------------------------------------------------

static void
expand_type_name(
	const char*			type,
	long				typelen,
	istring&			text )
{
	if( typelen == 0 )
		return;
		
	if( typelen == 1 )
	{
		switch( *type )
		{
			case 'b':
				text.append( "signed byte" );
				break;

			case 'B':
				text.append( "byte" );
				break;

			case 's':
				text.append( "short" );
				break;

			case 'S':
				text.append( "unsignd short" );
				break;
		
			case 'i':
				text.append( "int" );
				break;

			case 'I':
				text.append( "unsigned int" );
				break;

			case 'l':
				text.append( "long" );
				break;

			case 'L':
				text.append( "unsigned long" );
				break;

			case 'a':
				text.append( "bool" );
				break;

			case 'f':
				text.append( "float" );
				break;

			case 'd':
				text.append( "double" );
				break;
				
			default:
				text.append( "?" );
				break;
		}
		
		return;
	}
	
	if( type[ typelen - 1 ] == '*' )
	{
		expand_type_name( type, typelen - 1, text );
		text.append( "*" );
		return;
	}

	if( type[ typelen - 1 ] == '&' )
	{
		expand_type_name( type, typelen - 1, text );
		text.append( "&" );
		return;
	}
	
	if( type[ 0 ] == '.' )
	{
		text.append( "const " );
		expand_type_name( type + 1, typelen - 1, text );
		return;
	}

	if( type[ 0 ] == ':' )
	{
		text.append( "volatile " );
		expand_type_name( type + 1, typelen - 1, text );
		return;
	}
	
	long				index = typelen - 1;
	long				beta = index + 1;
	bool				flag = false;
	
	while( index >= 0 )
	{
		if( type[ index ] == ':' )
		{
			long		length = beta - ( index + 1 );
		
			if( flag )
				text.append( "::" );
			
			if( length > 0 )
			{
				text.append( &type[ index + 1 ], length );
				flag = true;
			}
			
			beta = index;
		}
	
		index--;
	}
	
	if( flag )
		text.append( "::" );
	
	text.append( type, beta );
}

static void
expand_function_name(
	const char*			name,
	istring&			text )
{
	char				c;

	while( true )
	{
		c = *name++;
		if( c == '\0' )
			return;
		if( c == ';' )
			break;
		text.append( 1, c );
	}
	
	text.append( "(" );
	
	while( true )
	{
		const char*		p = name;
		
		while( true )
		{
			c = *p;
			if( c == ';' || c == '\0' )
				break;
			p++;
		}
		
		expand_type_name( name, p - name, text );
		name = p + 1;
		if( c == '\0' )
			break;
			
		text.append( "," );
	}
	
	text.append( ")" );
}

struct composite_t {
	class_t*			clss;
	long				ptr;
	long				ref;
};

static bool
find_type_class(
	class_t*			clss,
	const char*			type,
	long				typelen,
	composite_t&		composite )
{
	if( typelen == 0 )
	{
		composite.clss = clss;
		return true;
	}
	
	if( typelen < 1 )
		return false;

	if( typelen >= 1 )
	{
		switch( *type )
		{
			case '.':
			case ':':
				return find_type_class(
					clss, type + 1, typelen - 1, composite );
		}
	}
	
	switch( type[ typelen - 1 ] )
	{
		case '*':
			if( composite.ptr >= 1 )
				return false;
				
			composite.ptr++;

			return find_type_class(
				clss, type, typelen - 1, composite );			

		case '&':
			composite.ref++;
			
			return find_type_class(
				clss, type, typelen - 1, composite );
	}
	
	long				index = typelen - 1;
	long				beta = index + 1;
	
	while( index >= 0 )
	{
		if( type[ index ] == ':' )
		{
			long		length = beta - ( index + 1 );
		
			if( length > 0 )
			{
				clss = clss->find_class(
					&type[ index + 1 ], length );

				if( not clss )
					return false;
			}
			
			beta = index;
		}
	
		index--;
	}
	
	clss = clss->find_class(
		&type[ index + 1 ], beta - ( index + 1 ) );
	
	composite.clss = clss;
	
	return clss != 0;
}

static void
expand_class(
	class_t*			clss,
	long&				offset,
	bool				annotate,
	output_stream_t&	stream )
{	
	stream.write_u08( 1 );

	long				count;
	variable_t*			var;
	
	count = clss->m_local_var_count;
	var = clss->m_local_vars;

	heap_t				heap( count );
	
	for( long i = 0; i < count; i++ )
	{
		heap.enqueue( -var->offset, var );
		var++;
	}
			
	if( annotate )
	{
		istring			s;
		
		clss->get_name( s );
		stream.write_string( s.c_str() );
	}
	
	stream.write_u32( count );
	
	for( long i = 0; i < count; i++ )
	{
		var = (variable_t*)heap.dequeue();
	
		stream.write_string( var->name );
		stream.write_string( var->type );
		stream.write_u32( offset + var->offset );
	}
		
	offset += clss->m_partial_size;
		
	long				basecount = clss->m_base_count;
	class_t**			basetable = clss->m_base_table;

	for( long i = 0; i < basecount; i++ )
	{
		expand_class( basetable[ i ],
			offset, annotate, stream );
	}
}

static void
expand_globals(
	class_t*			clss,
	output_stream_t&	stream )
{	
	long				count;

	count = clss->m_static_var_count;

	if( count > 0 )
	{
		variable_t*		var;
	
		var = clss->m_static_vars;

		stream.write_u08( 1 );

		heap_t			heap( count );
	
		for( long i = 0; i < count; i++ )
		{
			heap.enqueue( -var->offset, var );
			var++;
		}
			
		istring			s;
		
		clss->get_name( s );
		stream.write_string( s.c_str() );

		stream.write_ptr( clss->get_static_data() );
	
		stream.write_u32( count );
	
		for( long i = 0; i < count; i++ )
		{
			var = (variable_t*)heap.dequeue();
	
			stream.write_string( var->name );
			stream.write_string( var->type );
			stream.write_u32( var->offset );
		}
	}

	long				basecount = clss->m_base_count;
	class_t**			basetable = clss->m_base_table;

	for( long i = 0; i < basecount; i++ )
		expand_globals( basetable[ i ], stream );

	class_t*			up = clss->m_up;

	if( up )
		expand_globals( up, stream );
}

static void
get_instance_type(
	instance_t*			instance,
	istring&			type )
{
	class_header_t*		header;

	__try {
		header = CLASS_HEADER( instance );
		class_t::get_type_name(
			header->clss, type );
	} __except( 1 ) {
		type.clear();
	}
}

// ---------------------------------------------------------------------------

static HANDLE			s_background_thread = 0;
static HANDLE			s_pipe_handle = 0;

static DWORD WINAPI
vm_background_thread(
	void*				param )
{
	pipe_input_stream_t		instream( s_pipe_handle );

	while( true )
	{
		u32				selector = instream.read_u32();

		if( selector == msg_Stop )
			machine_t::current()->request_stop();
		else
			// communication protocol error
			exit( EXIT_FAILURE );
	}

	return 0;
}

void
vm_start_background_thread()
{
#if !RUN_FROM_PIPE
		return;
#endif

	if( s_background_thread )
		return;

	if( s_pipe_handle == 0 )
	{
		HANDLE			hPipeRead;

		hPipeRead = GetStdHandle( STD_INPUT_HANDLE );
		if( hPipeRead == INVALID_HANDLE_VALUE )
		{
			MessageBox( NULL, "Problem opening pipe",
				"IncaVM Error", MB_OK );
			exit( EXIT_FAILURE );
		}
		s_pipe_handle = hPipeRead;
	}

	DWORD				id;

	s_background_thread = CreateThread(
		NULL, 0, vm_background_thread, 0, 0, &id );
}

void
vm_kill_background_thread()
{
	if( s_background_thread == 0 )
		return;

	TerminateThread( s_background_thread, 0 );

	CloseHandle( s_background_thread );
	s_background_thread = 0;
}

// ---------------------------------------------------------------------------

void
vm_flush()
{
	if( s_text_console )
		s_text_console->flush();
}

void
vm_broadcast(
	int					message,
	void*				param )
{
#if !RUN_FROM_PIPE
	return;
#endif

	vm_kill_background_thread();
	machine_t::current()->request_stop();

	vm_flush();

	if( message == msg_ProgramTerminated )
		vm_wait_for_quit();

	HANDLE				hPipeRead;
	HANDLE				hPipeWrite;

	hPipeRead = GetStdHandle( STD_INPUT_HANDLE );
	hPipeWrite = GetStdHandle( STD_OUTPUT_HANDLE );
	if( hPipeRead == INVALID_HANDLE_VALUE ||
		hPipeWrite == INVALID_HANDLE_VALUE )
	{
		MessageBox( NULL, "Problem opening pipe",
			"IncaVM Error", MB_OK );
		exit( EXIT_FAILURE );
	}

	pipe_input_stream_t		instream( hPipeRead );
	pipe_output_stream_t	outstream( hPipeWrite );

	outstream.write_u32( message );
	
	fiber_t*			fiber = 0;
	array_t				frames( sizeof( stack_frame_t ) );
				
	if( message == msg_MachineStopped )
	{
		istring			text;

		fiber = (fiber_t*)param;
		stack_crawl( fiber, frames );
		
		const stop_state_t&		state = fiber->state();
		
		outstream.write_u08( state.resume );
		
		if( state.err != machine::ERR_NONE )
			error_string( state, text );

		outstream.write_string( text.c_str() );
	}
	
	bool				done = false;
	
	while( not done )
	{
		outstream.flush();
	
		u32				selector = instream.read_u32();
		
		switch( selector )
		{
			case msg_QuitMachine:
				exit( EXIT_SUCCESS );
				break;
				
			case msg_GetStackFrameDepth:
				outstream.write_u32( frames.count() );
				break;

			case msg_GetStackFrameInfo:
			{
				machine_t*	machine = fiber->machine();
				long		index = instream.read_u32();
				stack_frame_t*	frame = (stack_frame_t*)frames[ index ];				
				bytecode_func_t*	func = frame->func;
				long		pos;
				
				pos = func->find_source_location(
					func->get_code_offset( frame->code ) - 1 );

				istring		name;
				istring		text;

				class_t*	clss;
				class_t*	root;
	
				if( frame->instance == 0 )
					clss = func->owner();
				else
					clss = CLASS_HEADER( frame->instance )->clss;
				root = fiber->machine()->get_root_class();

				machine->func_id_to_name(
					func->get_id(), name );
			
				expand_function_name( name.c_str(), text );
			
				outstream.write_string( text.c_str() );
				outstream.write_u32( pos );
				outstream.write_ptr( clss );
				outstream.write_u08( clss != root );
				break;
			}
			
			case msg_GetFrameVariables:
			{
				long				index = instream.read_u32();
				stack_frame_t*		frame = (stack_frame_t*)frames[ index ];				

				// write "this" class variable

				istring				this_type;

				get_instance_type( frame->instance, this_type );

				outstream.write_ptr( frame->instance );
				outstream.write_string( "this" );
				outstream.write_string( this_type.c_str() );
				outstream.write_u32( 0 );

				// write local variables
				
				bytecode_func_t*	func = frame->func;
				const u08*			code = frame->code;
			
				long				codeoffset;
				
				codeoffset = func->get_code_offset( code ) - 1;
				
				long				ltcount = func->m_lifetime_count;
				lifetime_t*			lifetime;
				long				count = 0;
				
				lifetime = func->m_lifetime_table;
				
				for( long i = 0; i < ltcount; i++, lifetime++ )
				{
					if( codeoffset >= lifetime->create &&
						codeoffset < lifetime->destroy )
					{
						count++;
					}
				}

				outstream.write_ptr( frame->localsp );
				outstream.write_u32( count );
				lifetime = func->m_lifetime_table;
				
				for( long i = 0; i < ltcount; i++, lifetime++ )
				{
					if( codeoffset >= lifetime->create &&
						codeoffset < lifetime->destroy )
					{
						outstream.write_string( lifetime->name );
						outstream.write_string( lifetime->type );
						outstream.write_u32( lifetime->offset );
					}
				}

				break;
			}
			
			case msg_FindTypeClass:
			{
				istring			s;
				composite_t		comp;
				class_t*		root;

				root = fiber->machine()->get_root_class();

				comp.clss = 0;
				comp.ptr = comp.ref = 0;

				instream.read_string( s );
				if( not find_type_class(
					root,
					s.data(), s.length(), comp ) )
				{
					comp.clss = 0;
				}
				outstream.write_ptr( comp.clss );
				outstream.write_u32( comp.ptr + comp.ref );
				
				class_t*		clss = comp.clss;
				
				if( clss == root )
					outstream.write_u08( 0 );
				else if( clss && (
					clss->m_static_size > 0 ||
					clss->m_size > machine::CLASS_HEADER_SIZE ) )
					outstream.write_u08( 1 );
				else
					outstream.write_u08( 0 );
				
				break;
			}
			
			case msg_ExpandClass:
			{
				class_t*		clss;
				long			offset = 0;
				bool			annotate;
			
				clss = (class_t*)instream.read_ptr();
				annotate = ( clss->m_base_count > 0  );
			
				outstream.write_u08( annotate ? 1 : 0 );
				expand_class( clss, offset, annotate, outstream );
				outstream.write_u08( 0 );
				break;
			}

			case msg_ExpandGlobals:
			{
				class_t*		clss;

				clss = (class_t*)instream.read_ptr();
				expand_globals( clss, outstream );
				outstream.write_u08( 0 );
				break;
			}

			case msg_Continue:
				done = true;
				break;

			case msg_StepOver:
				fiber->step( false );
				done = true;
				break;

			case msg_StepInto:
				fiber->step( true );
				done = true;
				break;
			
			case msg_ReadByte:
			{
				void* addr = instream.read_ptr();
				u08 temp;
				if( read_memory( fiber, addr, 1, &temp ) )
				{
					outstream.write_u08( 1 );
					outstream.write_u32( temp );
				}
				else
					outstream.write_u08( 0 );
				break;
			}
			
			case msg_ReadWord:
			{
				void* addr = instream.read_ptr();
				u16 temp;
				if( read_memory( fiber, addr, 2, &temp ) )
				{
					outstream.write_u08( 1 );
					outstream.write_u32( temp );
				}
				else
					outstream.write_u08( 0 );
				break;
			}
			
			case msg_ReadQuad:
			{
				void* addr = instream.read_ptr();
				u32 temp;
				if( read_memory( fiber, addr, 4, &temp ) )
				{
					outstream.write_u08( 1 );
					outstream.write_u32( temp );
				}
				else
					outstream.write_u08( 0 );
				break;
			}
			
			case msg_ReadOcta:
			{
				void* addr = instream.read_ptr();
				u64 temp;
				if( read_memory( fiber, addr, 8, &temp ) )
				{
					outstream.write_u08( 1 );
					outstream.write_u64( temp );
				}
				else
					outstream.write_u08( 0 );
				break;
			}
			
			case msg_ReadFloat:
			{
				void* addr = instream.read_ptr();
				f32 temp;
				if( read_memory( fiber, addr, 4, &temp ) )
				{
					outstream.write_u08( 1 );
					outstream.write_f32( temp );
				}
				else
					outstream.write_u08( 0 );
				break;
			}
			
			case msg_ReadDouble:
			{
				void* addr = instream.read_ptr();
				f64 temp;
				if( read_memory( fiber, addr, 8, &temp ) )
				{
					outstream.write_u08( 1 );
					outstream.write_f64( temp );
				}
				else
					outstream.write_u08( 0 );
				break;
			}
			
			case msg_ReadPointer:
			{
				void* addr = instream.read_ptr();
				void* temp;
				if( read_memory( fiber, addr, sizeof( void* ), &temp ) )
				{
					outstream.write_u08( 1 );
					outstream.write_ptr( temp );
				}
				else
					outstream.write_u08( 0 );
				break;
			}

			case msg_ReadBlock:
			{
				void* addr = instream.read_ptr();
				u32 length = instream.read_u32();
				u08* temp = 0;
				try {
					temp = new u08[ length ];
					if( read_memory( fiber, addr, length, temp ) )
					{
						outstream.write_u08( 1 );
						outstream.write_bytes( temp, length );
					}
					else
						outstream.write_u08( 0 );
					delete[] temp;
				} catch(...) {
					if( temp )
						delete[] temp;
					else
						outstream.write_u08( 0 );
				}
				break;
			}
		}
	}

	machine_t::current()->cancel_stop();
	vm_start_background_thread();
}

END_MACHINE_NAMESPACE
