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
//	machine_t.cp			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine_t.h"
#include "machine_class_t.h"
#include "safe_memory_t.h"

#include "xalloc.h"
#include "input_stream_t.h"
#include <cstring>
#include "fiber_t.h"

#ifdef VM_PROJECT
#include "vmbind.h"
#endif

BEGIN_MACHINE_NAMESPACE

using namespace std;

static machine_t*		s_current_machine = 0;

machine_t::machine_t(
	memory_t*			memory,
	const char*			base_path ) :
	
	m_memory( memory ),
	m_base_path( base_path )
#ifdef VM_PROJECT
	,m_lib_static( memory )
#endif
{
	s_current_machine = this;

	m_root_class = new class_t( this,
		"root", 4, 0 );
	
	m_func_htab = hcreate( 5 );
	
	m_constructor_func_id = find_or_create_func_id(
		s_constructor_name, istrlen( s_constructor_name ) );
	m_destructor_func_id = find_or_create_func_id(
		s_destructor_name, istrlen( s_destructor_name ) );
		
	m_refcon = 0;
	m_string_class = 0;

#ifdef VM_PROJECT	
	m_fibers = 0;
#endif
}

machine_t::~machine_t()
{
#ifdef VM_PROJECT
	while( m_fibers )
		delete m_fibers;
#endif

	delete m_root_class;
	
	delete m_memory;

	htab_t*				t = m_func_htab;

	if( hfirst( t ) )
	{
		do {
			delete[] hstuff( t );
		} while( hnext( t ) );
	}

	hdispose( t );

	s_current_machine = 0;
}

void
machine_t::import_class(
	input_stream_t*		stream )
{
	class_t*			clss = read_class_id( stream );
	
	clss->import( stream );
}

class_t*
machine_t::read_class_id(
	input_stream_t*		stream )
{
	unsigned int		depth;
	
	depth = stream->read_u16();

	class_t*			clss = m_root_class;

	while( depth-- )
	{
		unsigned int	len;
		
		len = stream->read_u16();
		
		char*			buffer = (char*)obtain_buffer(
							len * sizeof( char ) );
		
		stream->read_bytes( buffer, len );
		
		clss = clss->find_or_create_class(
			buffer, len );
	}
		
	return clss;
}

func_id_t
machine_t::read_func_id(
	input_stream_t*		stream )
{
	unsigned int		len;
	
	len = stream->read_u16();
	
	char*				buffer = (char*)obtain_buffer(
							len * sizeof( char ) );
	
	stream->read_bytes( buffer, len );
		
	return find_or_create_func_id( buffer, len );
}

func_id_t
machine_t::find_or_create_func_id(
	const char*			name,
	const long			length )
{
	htab_t*				t = m_func_htab;

	if( hfind( t, (u08*)name, length ) )
		return *(func_id_t*)hstuff( t );

	func_id_t			func_id = t->count + 1;
	
	u08*				block;
	
	block = new u08[ length * sizeof( char ) +
		sizeof( func_id_t ) ];
	
	char*				newname;

	newname = (char*)( block + sizeof( func_id_t ) );
	memcpy( newname, name, length * sizeof( char ) );

	func_id_t*			func_idp;
	
	func_idp = (func_id_t*)block;
	*func_idp = func_id;

	hadd( t, (u08*)newname, length, func_idp );

	return func_id;	
}

void
machine_t::func_id_to_name(
	func_id_t			func_id,
	istring&			text )
{
	htab_t*				t = m_func_htab;

	if( !hfirst( t ) )
		return;
		
	char*				name;
	long				length;
		
	do {
		if( *(func_id_t*)hstuff( t ) == func_id )
		{
			name = (char*)t->ipos->key;
			length = t->ipos->keyl;
			
			if( name[ 0 ] == '#' )
			{
				text = "operator";
				text.append( name + 1, length - 1 );
			}
			else
				text.assign( name, length );
			
			return;
		}
	} while( hnext( t ) );
}

class_t*
machine_t::get_root_class() const
{
	return m_root_class;
}

void
machine_t::set_refcon(
	void*				refcon )
{
	m_refcon = refcon;
}

istring
machine_t::file_path(
	const char*			path )
{
	if( path[ 0 ] == '/' ||
		path[ 0 ] == '\\' )
		return istring( path );

	if( path[ 1 ] == ':' &&
		isalpha( path[ 0 ] ) )
		return istring( path );
	
	istring				s;
	
	s = m_base_path;
	s.append( path );
	
	return s;
}

#if VM_PROJECT

void
machine_t::request_stop()
{
	fiber_t*			fiber = first_fiber();
	
	while( fiber )
	{
		fiber->stop();
		fiber = fiber->next_fiber();
	}
}

void
machine_t::cancel_stop()
{
	fiber_t*			fiber = first_fiber();
	
	while( fiber )
	{
		fiber->cancel_stop();
		fiber = fiber->next_fiber();
	}
}

#endif

void
machine_t::set_string_class(
	class_t*			clss )
{
	m_string_class = clss;
}

#ifdef VM_PROJECT

void
machine_t::register_fiber(
	fiber_t*			fiber )
{
	fiber->m_next = m_fibers;
	m_fibers = fiber;
}

void
machine_t::unregister_fiber(
	fiber_t*			fiber )
{
	fiber_t**			link = &m_fibers;
	fiber_t*			node = *link;
	
	while( node )
	{
		if( node == fiber )
		{
			*link = node->m_next;
			break;
		}
		node = node->m_next;
	}
}

void
machine_t::broadcast_message(
	message_t			message,
	void*				param )
{
	vm_broadcast( message, param );
}

machine_t*
machine_t::current()
{
	return s_current_machine;
}

#endif

END_MACHINE_NAMESPACE
