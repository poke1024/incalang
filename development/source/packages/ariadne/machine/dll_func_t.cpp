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
//	dll_func_t.cpp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "dll_func_t.h"
#include "machine_class_t.h"
#include "machine_t.h"
#include <windows.h>

BEGIN_MACHINE_NAMESPACE

dll_func_t::dll_func_t(
	func_id_t			id,
	class_t*			owner ) :

	func_t( id, func_type_dll, owner ),
	m_proc( 0 )
{
}

void
dll_func_t::call(
	fiber_t&			fiber )
{
#if VM_PROJECT
	dll_proc_t			proc;
	
	if( ( proc = m_proc ) == 0 )
	{
		load( fiber );
		proc = m_proc;
	}
	
	long				size = m_param_size;
	void*				fiberstack;
	u08*				stack;

	fiberstack = fiber.free_stack(
		size );
	_asm { mov stack,esp }
	stack -= size;
	memcpy( stack, fiberstack, size );
	_asm { mov esp,stack }

	u08					val08;
	u16					val16;
	u32					val32;
	u32					val64lo;
	u32					val64hi;
	u64					val64;

	switch( m_return_size )
	{
		case 0:
			( *proc )();
			break;

		case 1:
			( *proc )();
			_asm { mov val08, al }
			fiber.push_byte( val08 );
			break;

		case 2:
			( *proc )();
			_asm { mov val16, ax }
			fiber.push_short( val16 );
			break;

		case 4:
			( *proc )();
			_asm { mov val32, eax }
			fiber.push_int( val32 );
			break;

		case 8:
			( *proc )();
			_asm { mov val64lo, eax }
			_asm { mov val64hi, edx }
			val64 = val64lo + ( ( (u64)val64hi ) << 32 );
			fiber.push_long( val64 );
			break;

		default:
			fiber.fatal_error( ERR_EXCEPTION,
				"unsupported return type" );
	}

	_asm { mov stack,esp }
	stack += size;
	_asm { mov esp,stack }

#endif
}

void
dll_func_t::import(
	input_stream_t*		stream,
	machine_t*			m )
{
	m_param_size = stream->read_u32();
	m_return_size = stream->read_u32();
}

void
dll_func_t::load(
	fiber_t&			fiber )
{
#if VM_PROJECT
	istring				name;
	istring				s;

	if( m_class->m_dll == 0 )
	{
		name.assign( m_class->m_name,
			m_class->m_name_length );
		m_class->m_dll = LoadLibrary( name.c_str() );
		if( m_class->m_dll == 0 )
		{
			s = "dll '";
			s.append( name );
			s.append( "' could not be loaded" );

			fiber.fatal_error(
				ERR_EXCEPTION,
				s.c_str() );
		}
	}

	FARPROC				proc;

	fiber.machine()->func_id_to_name( m_id, name );

	char*				t;
	t = strstr( name.c_str(), ";" );
	if( t )
		*t = '\0';

	proc = GetProcAddress(
		(HMODULE)m_class->m_dll, name.c_str() );
	if( proc == NULL )
	{
		s = "dll function '";
		s.append( name );
		s.append( "' could not be found" );

		fiber.fatal_error(
			ERR_EXCEPTION,
			s.c_str() );
	}

	m_proc = (dll_proc_t)proc;
#endif
}

END_MACHINE_NAMESPACE
