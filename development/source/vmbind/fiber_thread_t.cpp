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
//	fiber_thread_t.cp	 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "fiber_thread_t.h"

#include "lib_protos.h"
#include "machine_class_t.h"
#include "fiber_t.h"
#include "machine_error.h"

BEGIN_MACHINE_NAMESPACE

fiber_thread_t::fiber_thread_t(
	machine_t*			machine,
	instance_t*			instance ) :
	
	ithread_t( "", 0, 64L * 1024L ),
	
	m_machine( machine ),
	m_fiber( 0 ),
	m_instance( instance )
{
}

fiber_thread_t::~fiber_thread_t()
{
}

void
fiber_thread_t::run()
{
	block();

	try {

	try
	{
		m_fiber = new fiber_t( m_machine, 8192 );
		m_fiber->set_refcon( this );
	
		class_t*		clss = CLASS_HEADER( m_instance )->clss;
		machine_t&		m = *m_machine;
		
		func_id_t		id = m.find_or_create_func_id( "run", 3 );
		unsigned long	offset = 0;
		func_t*			func = clss->find_func_by_id( id, &offset );
	
		if( not func )
			m_fiber->fatal_error( ERR_UNKNOWN );
	
		m_fiber->call(
			(instance_t*)( (u08*)m_instance + offset ), func );
	}
	catch( kill_thread_t )
	{
		return;
	}
	catch( std::bad_alloc& e )
	{
		m_fiber->fatal_error( ERR_OUT_OF_MEMORY );
	}
	catch( std::exception& e )
	{
		m_fiber->fatal_error( ERR_EXCEPTION, e.what() );
	}
	catch(...)
	{
		m_fiber->fatal_error( ERR_UNKNOWN );
	}

	}
	catch(...)
	{
		MessageBox( NULL, "Inca VM closed with fatal error.",
			"IncaVM", MB_OK );
		exit( EXIT_FAILURE );
	}
}

END_MACHINE_NAMESPACE
