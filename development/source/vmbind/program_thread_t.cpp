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
//	program_thread_t.cp	 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "program_thread_t.h"
#include "lib_protos.h"
#include "machine_class_t.h"
#include "fiber_t.h"
#include "machine_error.h"
#include "fiber_thread_t.h"
#include "ariadne_messages.h"
#include "vmbind.h"
#include "vmglobals.h"

#include "bytecode_func_t.h"
#include "bytecode.h"
#include "vmdefs.h"

BEGIN_MACHINE_NAMESPACE

static program_thread_t*	s_instance = 0;

program_thread_t::program_thread_t(
	machine_t*			machine,
	input_stream_t*		stream ) :
	
	ithread_t( "", 0, 64L * 1024L ),
	
	m_machine( machine ),
	m_fiber( 0 ),
	m_stream( stream )
{
	m_terminate_message = msg_ProgramTerminated;
	s_instance = this;
}

program_thread_t::~program_thread_t()
{
	if( m_stream )
	{
		delete m_stream;
		m_stream = 0;
	}
	s_instance = 0;
}

machine_t*
program_thread_t::machine() const
{
	return m_machine;
}

void
program_thread_t::run()
{
	try
	{
		
	try
	{	
		u32				stack_size;

		stack_size = m_stream->read_u32();

		m_fiber = new fiber_t( m_machine, stack_size );
		m_fiber->set_refcon( this );

		load();

		machine_t&		m = *m_machine;
	
		class_t*		clss = m.get_root_class();

		clss->finalize();

		instance_t*		instance = clss->create_instance(
							m_fiber );
	
		init_lib( m );
		
		func_id_t		id = m.find_or_create_func_id( "main", 4 );
		unsigned long	offset = 0;
		func_t*			func = clss->find_func_by_id( id, &offset );
		
		if( not func )
		{
			m_terminate_message = msg_NoMainFound;
		
			cleanup();
			return;
		}

		func_id_t		initid = m.find_or_create_func_id( "_init", 5 );

		clss->init( m_fiber, initid );
		
		vm_start_background_thread();

		m_fiber->call(
			(instance_t*)( (u08*)instance + offset ), func );
	}
	catch( kill_thread_t )
	{
	}
	catch( std::bad_alloc& )
	{
		m_fiber->fatal_error( ERR_OUT_OF_MEMORY );
	}
	/*catch( icarus_exception& e )
	{
		m_fiber->fatal_error( ERR_EXCEPTION, e.what() );
	}*/
	catch( std::exception& e )
	{
		m_fiber->fatal_error( ERR_EXCEPTION, e.what() );
	}
	catch(...)
	{
		m_fiber->fatal_error( ERR_UNKNOWN );
	}
	
	}
	catch( std::exception& e )
	{
		if( m_fiber )
			m_fiber->fatal_error( ERR_EXCEPTION, e.what() );
	}
	catch(...)
	{
		MessageBox( NULL, "Inca VM closed with fatal error.",
			"IncaVM", MB_OK );
		exit( EXIT_FAILURE );
	}

	cleanup();
}

void
program_thread_t::load()
{
	long				class_count = m_stream->read_u32();
	
	for( long i = 0; i < class_count; i++ )
		m_machine->import_class( m_stream );
		
	load_breakpoints();

	delete m_stream;
	m_stream = 0;
}

void
program_thread_t::load_breakpoints()
{
//#if RUN_FROM_PIPE
	long				count = m_stream->read_u32();
	class_t*			root = m_machine->get_root_class();
	bytecode_func_t*	func;

	for( long i = 0; i < count; i++ )
	{
		long			srcpos = m_stream->read_u32();
		sourcemap_t*	map;
		map = root->inverse_sourcemap( srcpos, &func );
		if( map )
		{
			func->add_breakpoint(
				map->offset,
				BYTECODE_BREAK | OPS_BREAK_REGULAR );
		}
	}
//#endif
}

void
program_thread_t::cleanup()
{
	m_machine->broadcast_message( m_terminate_message, this );

	fiber_t*			fiber = m_machine->first_fiber();
	fiber_thread_t*		thread;
	
	while( fiber )
	{
		if( fiber != m_fiber )
		{
			thread = static_cast<fiber_thread_t*>(
				fiber->get_refcon() );
			thread->kill();
		}
		fiber = fiber->next_fiber();
	}

	delete m_machine;
}

program_thread_t*
program_thread_t::instance()
{
	return s_instance;
}

END_MACHINE_NAMESPACE
