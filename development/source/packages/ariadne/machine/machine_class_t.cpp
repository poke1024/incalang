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
//	machine_class_t.cp		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine_class_t.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "safe_memory_t.h"

#include "input_stream_t.h"
#include "xhtab_t.h"
#include "xalloc.h"
#include <cstring>
#include <cassert>

#include "bytecode_func_t.h"
#include "native_func_t.h"
#include "abstract_func_t.h"
#include "dll_func_t.h"
#include "heap_t.h"

#include <windows.h> // FreeLibrary

BEGIN_MACHINE_NAMESPACE

using namespace std;

// ---------------------------------------------------------------------------

struct array_header_t {
	u32					arraysize;
};

static void*
alloc_array(
	memory_t*			memory,
	unsigned long		arraysize,
	unsigned long		elemcount )
{
	u08*				block;
	array_header_t*		header;
	
	block = memory->alloc(
		elemcount * arraysize + sizeof( array_header_t ) );
	
	header = (array_header_t*)block;
	block += sizeof( array_header_t );

	header->arraysize = arraysize;
	
	return block;
}

static void
free_array(
	memory_t*			memory,
	void*				array )
{
	u08*				block = (u08*)array;

	block -= sizeof( array_header_t );
	memory->free( block );
}

static unsigned long
get_array_size(
	void*				array )
{
	u08*				block = (u08*)array;
	array_header_t*		header;
	
	block -= sizeof( array_header_t );
	header = (array_header_t*)block;
	
	return header->arraysize;
}

// ---------------------------------------------------------------------------

class_t::class_t(
	machine_t*			machine,
	const char*			name,
	long				name_length,
	class_t*			up ) :
	
	m_machine( machine ),
	m_name( name ),
	m_name_length( name_length ),
	m_up( up )	
{
	m_constructor = 0;
	m_destructor = 0;

	m_base_count = 0;
	m_base_table = 0;

	m_class_htab = hcreate( 1 );
	
	m_func_count = 0;
	m_func_table = 0;
	
	m_class_headers = 0;
	
	m_size = 0;
	m_partial_size = 0;
	m_static_size = 0;
	
	m_local_var_count = 0;
	m_static_var_count = 0;
	m_local_vars = 0;
	m_static_vars = 0;
	
	m_instance_pool = 0;
	m_static_data = 0;

	m_dll = 0;
}

class_t::~class_t()
{
	if( m_base_table )
		delete[] m_base_table;

	if( m_func_table )
	{
		long			count = m_func_count;
		func_t**		table = m_func_table;

		for( long i = 0; i < count; i++ )
			delete table[ i ];

		delete[] table;
	}
		
	if( m_class_headers )
		delete[] m_class_headers;
		
	//if( m_static_data )
	//	delete[] m_static_data;
		
	if( m_class_htab )
	{
		htab_t*			t = m_class_htab;
		class_t*		clss;

		if( hfirst( t ) )
		{
			do {			
				clss = ( (class_t*)hstuff( t ) );
				delete clss;
			} while( hnext( t ) );
		}
	}

	if( m_local_vars )
		delete[] m_local_vars;
	
	if( m_static_vars )
		delete[] m_static_vars;

#ifdef VM_PROJECT
	free_pool();
#endif

	hdispose( m_class_htab );

	if( m_dll )
		FreeLibrary( (HMODULE)m_dll );
}

void
class_t::import(
	input_stream_t*		stream )
{	
	m_size = stream->read_u32();
	m_partial_size = stream->read_u32();

	unsigned int		basecount;
	
	basecount = stream->read_u16();
	m_base_count = basecount;
	
	if( basecount )
	{	
		m_base_table = new class_t*[ basecount ];

		class_t**		baseentry = m_base_table;
	
		while( basecount-- )
		{
			*baseentry = m_machine->read_class_id( stream );
			baseentry++;
		}
	}

	import_all_functions( stream );

	m_static_size = stream->read_u32();

	if( m_static_size )
		m_static_data = m_machine->get_memory()->alloc( m_static_size );
		
	import_variables( stream );

	// check for special system classes

#if ARIADNE_STRING_OBJECTS
	if( m_name_length == s_string_name_length &&
		imemcmp( s_string_name, m_name, m_name_length ) == 0 )
	{
		m_machine->set_string_class( this );
		finalize();
	}
#endif
}

void
class_t::finalize()
{
	if( m_class_headers )
		return;

	machine_t*			vm = m_machine;

	// finalize embedded classes

	htab_t*				t = m_class_htab;
	
	if( hfirst( t ) )
	{
		do {
			( (class_t*)hstuff( t ) )->finalize();
		} while( hnext( t ) );
	}
	
	// finalize class headers

	m_class_header_count = count_class_headers();
	m_class_headers = new class_header_t[ m_class_header_count ];

	unsigned long		offset = 0;
	setup_class_headers( this, &offset, m_class_headers );

	// finalize functions
	
	unsigned long		func_count = m_func_count;
	func_t**			func_entry = m_func_table;
	
	while( func_count-- )
	{
		( *func_entry )->finalize( vm );
		func_entry++;
	}
}

#ifdef VM_PROJECT
void
class_t::init(
	fiber_t*			fiber,
	func_id_t			id )
{
	func_t*				func;

	func = find_func_by_id_flat( id );

	if( func )
		fiber->call( 0, func );
	
	// init embedded classes

	htab_t*				t = m_class_htab;
	
	if( hfirst( t ) )
	{
		do {
			( (class_t*)hstuff( t ) )->init( fiber, id );
		} while( hnext( t ) );
	}
}
#endif

class_t*
class_t::find_class(
	const char*			name,
	const long			length )
{
	htab_t*				t = m_class_htab;

	if( hfind( t, (u08*)name, length ) )
		return (class_t*)hstuff( t );
	
	return 0;
}

class_t*
class_t::find_or_create_class(
	const char*			name,
	const long			length )
{
	htab_t*				t = m_class_htab;

	if( hfind( t, (u08*)name, length ) )
		return (class_t*)hstuff( t );
	
	char*				newname;

	newname = (char*)m_machine->xalloc(
		length * sizeof( char ) );
	imemcpy( newname, name, length * sizeof( char ) );

	class_t*			clss = new class_t( m_machine, newname, length, this );

	hadd( t, (u08*)newname, length, clss );
	
	return clss;
}

func_t*
class_t::find_func_by_id(
	func_id_t			id,
	unsigned long*		offset ) const
{
	func_t**			table = m_func_table;
	
	long				left = 0;
	long				right = m_func_count - 1;
	
	while( left <= right )
	{
		long			current;
	
		current = ( left + right ) / 2;
		
		func_t*			func = table[ current ];
		
		long			comparison;
		
		comparison = func->get_id() - id;
		
		if( comparison == 0 )
			return func;
		else if( comparison > 0 )
			right = current - 1;
		else
			left = current + 1;
	}
	
	if( offset )
		*offset += m_partial_size;
	
	unsigned int		basecount = m_base_count;
	class_t**			baseentry = m_base_table;
	
	while( basecount-- )
	{
		func_t*			func;
		
		func = ( *baseentry )->find_func_by_id( id, offset );
		if( func )
			return func;
		baseentry++;
	}
	
	return 0;
}

bool
class_t::isa(
	class_t*			clss ) const
{
	if( clss == this )
		return true;
		
	unsigned int		count = m_base_count;
	
	if( count )
	{
		class_t**		clssp = m_base_table;
	
		do
		{
			if( ( *clssp )->isa( clss ) )
				return true;
			clssp++;
		} while( --count );
	}
		
	return false;
}

found_t
class_t::get_base_class_offset(
	class_t*			baseclss,
	unsigned long*		outoffset )
{
	if( baseclss == this )
	{
		*outoffset = 0;
		return FOUND;
	}
		
	unsigned long		offset = m_partial_size;

	unsigned int		count = m_base_count;
	bool				wasfound = false;
	
	if( count )
	{
		class_t**		clssp = m_base_table;
	
		do
		{
			unsigned long	partialoffset;
			found_t			found;
			
			found = ( *clssp )->get_base_class_offset(
				baseclss, &partialoffset );

			offset += partialoffset;
			
			if( found == FOUND )
			{
				if( wasfound )
					return AMBIVALENT;
			
				*outoffset = offset;
				wasfound = true;
			}
			else if( found == AMBIVALENT )
				return AMBIVALENT;
						
			clssp++;
		} while( --count );
	}
	
	if( wasfound )
		return FOUND;
	
	*outoffset = offset;
	return NOT_FOUND;
}

void
class_t::import_all_functions(
	input_stream_t*		stream )
{
	unsigned int		count;
	
	count = stream->read_u16();
	
	heap_t				heap( count );
	func_t*				func;
	
	try
	{
		for( long index = 0; index < count; index++ )
		{
			func = import_function( stream );
			heap.enqueue( -func->get_id(), func );
		}
	}
	catch(...)
	{
		while( heap.count() > 0 )
			delete (func_t*)heap.dequeue();
		throw;
	}
	
	m_func_count = count;
	m_func_table = new func_t*[ count ];
	
	func_t**			table = m_func_table;

	for( long index = 0; index < count; index++ )
		table[ index ] = (func_t*)heap.dequeue();
	
	unsigned long		offset;
	
	offset = 0;
	m_constructor = find_func_by_id(
		m_machine->get_constructor_func_id(), &offset );
	if( m_constructor ) { assert( offset == 0 ); }

	offset = 0;
	m_destructor = find_func_by_id(
		m_machine->get_destructor_func_id(), &offset );
	if( m_destructor ) { assert( offset == 0 ); }
}

func_t*
class_t::import_function(
	input_stream_t*		stream )
{
	unsigned int		len;
		
	len = stream->read_u16();

	if( len < 1 )
		throw_icarus_error( "corrupt function name" );
	
	char*				buffer = (char*)obtain_buffer(
							len * sizeof( char ) );

	stream->read_bytes( buffer, len );

	func_id_t			id;
	
	id = m_machine->find_or_create_func_id( buffer, len );

	func_t*				func;

	switch( stream->read_u08() )
	{
		case 0:
			func = new bytecode_func_t( id, this );
			( (bytecode_func_t*)func )->import( stream, m_machine );
			break;
			
		case 1:
			func = new native_func_t( id, this, m_machine );
			break;
			
		case 2:
#if DEBUG_ABSTRACT_FUNCTIONS
			func = new abstract_func_t( id, this );
#else
			func = 0;
#endif
			break;

		case 3:
			func = new dll_func_t( id, this );
			( (dll_func_t*)func )->import( stream, m_machine );
			break;

		default:
			throw_icarus_error( "illegal function type" );
	}

	
	return func;
}

#ifdef VM_PROJECT

static void
call_array_cd(
	fiber_t*			fiber,
	instance_t*			instance,
	unsigned long		arraysize,
	const long			clsssize,
	func_t*				func,
	int					ext )
{
	int					cdext = fiber->get_cdext();

	fiber->set_cdext( ext );

	unsigned char*		ptr = (unsigned char*)instance;
	
	while( arraysize-- )
	{
		fiber->call( (instance_t*)ptr, func );
		ptr += clsssize;
	}
	
	fiber->set_cdext( cdext );
}

instance_t*
class_t::create_instance(
	fiber_t*			fiber,
	int					ext )
{
	instance_t*			instance = alloc_instance();
	
	if( m_constructor )
	{
		int				cdext = fiber->get_cdext();

		fiber->set_cdext( ext );
		fiber->call( instance, m_constructor );
		fiber->set_cdext( cdext );
	}
	
	return instance;
}

instance_t*
class_t::create_instance_array(
	fiber_t*			fiber,
	unsigned long		arraysize,
	int					ext )
{
	if( arraysize == 1 )
		return create_instance( fiber, ext );

	instance_t*			instance = alloc_instance_array( arraysize );
	
	if( m_constructor )
		call_array_cd( fiber, instance, arraysize,
			m_size, m_constructor, ext );
	
	return instance;
}

void
class_t::destroy_instance(
	fiber_t*			fiber,
	instance_t*			instance,
	int					ext )
{
	unsigned long		arraysize;

	arraysize = get_array_size( instance );
	
	if( m_destructor )
		call_array_cd( fiber, instance, arraysize,
			m_size, m_destructor, ext );
		
	if( arraysize == 1 )
		free_instance( instance );
	else
		free_instance_array( instance );
}

instance_t*
class_t::alloc_instance()
{
	instance_t*			instance = m_instance_pool;
	
	if( instance )
	{
		m_instance_pool = instance->next;
		instance->header = (class_header_ptr_t)m_class_headers;
	}
	else
	{
		u08*			block;
	
		block = (u08*)alloc_array(
			m_machine->get_memory(), 1, m_size );
		init_instance( block );
		instance = (instance_t*)block;
	}

	return instance;
}

void
class_t::free_instance(
	instance_t*			instance )
{
	instance->next = m_instance_pool;
	m_instance_pool = instance;
}

void
class_t::free_pool()
{
	instance_t*			instance = m_instance_pool;
	instance_t*			next;
	
	while( instance )
	{
		next = instance->next;
		free_array( m_machine->get_memory(), instance );
		instance = next;
	}
	
	m_instance_pool = 0;
}

instance_t*
class_t::alloc_instance_array(
	unsigned long		arraysize )
{
	const long			size = m_size;
	u08*				block;
	instance_t*			instance;

	block = (u08*)alloc_array(
		m_machine->get_memory(), arraysize, size );
	instance = (instance_t*)block;
	
	for( long i = 0; i < arraysize; i++ )
	{
		init_instance( block );
		block += size;
	}

	return instance;
}

void
class_t::free_instance_array(
	instance_t*			instance )
{
	free_array( m_machine->get_memory(), instance );
}
	
void
class_t::init_instance(
	u08*				block )
{
	class_header_t*		header = m_class_headers;
	unsigned long		count = m_class_header_count;
	
	if( !header )
		throw_icarus_error( "invalid class instance" );
	
	do {
		u08*			ptr = block + header->offset;
		
		*(class_header_t**)ptr = header;
		
		header++;
	} while( --count );
}

#endif

unsigned long
class_t::count_class_headers() const
{
	unsigned long		header_count = 1;
	
	unsigned int		count = m_base_count;
	
	if( count )
	{
		class_t**		clssp = m_base_table;
	
		do
		{
			header_count += ( *clssp )->count_class_headers();
			clssp++;
		} while( --count );
	}
	
	return header_count;
}

class_header_t*
class_t::setup_class_headers(
	class_t*			clss,
	unsigned long*		offset,
	class_header_t*		header )
{
	header->clss = clss;
	header->baseclss = this;
	header->offset = *offset;
	header++;
	
	if( offset )
		*offset += m_partial_size;
	
	unsigned int		count = m_base_count;
	
	if( count )
	{
		class_t**		clssp = m_base_table;
	
		do
		{
			header = ( *clssp )->setup_class_headers(
				clss, offset, header );
			clssp++;
		} while( --count );
	}
	
	return header;
}

void
class_t::get_full_name(
	class_t*			clss,
	istring&			name )
{
	name.clear();

	istring				s;
		
	while( clss )
	{
		s.assign( clss->m_name, clss->m_name_length );
		if( name.length() )
		{
			s.append( "::" );
			s.append( name );
		}
		name = s;

		clss = clss->m_up;
	}
}

void
class_t::get_name(
	istring&			name ) const
{
	name.assign( m_name, m_name_length );
}

#if VM_PROJECT
void
class_t::get_type_name(
	class_t*			clss,
	istring&			s )
{
	s.clear();
		
	while( clss )
	{
		long			length = clss->m_name_length;
		const char*		name = clss->m_name;

		if( length == 4 &&
			memcmp( name, "root", 4 * sizeof( char ) ) == 0 )
			;
		else
			s.append( name, length );
		s.append( ":" );
		clss = clss->m_up;
	}
}

u08*
class_t::get_static_data() const
{
	return m_static_data;
}

sourcemap_t*
class_t::inverse_sourcemap(
	long				srcpos,
	bytecode_func_t**	mfunc )
{
	long				count = m_func_count;
	func_t**			node = m_func_table;
	func_t*				func;
	sourcemap_t*		map;

	for( long i = 0; i < count; i++ )
	{
		func = *node++;
		if( func->get_type() != func_type_bytecode )
			continue;
		map = ( (bytecode_func_t*)func
			)->inverse_sourcemap( srcpos );
		if( map )
		{
			*mfunc = (bytecode_func_t*)func;
			return map;
		}
	}

	htab_t*				t = m_class_htab;
	
	if( hfirst( t ) )
	{
		do {
			map = ( (class_t*)hstuff( t ) )->
				inverse_sourcemap( srcpos, mfunc );
			if( map )
				return map;
		} while( hnext( t ) );
	}

	return 0;

}

#endif

void
class_t::export_reference(
	output_stream_t&	stream )
{
	class_t*			chain[ 32 ];
	int					index = 0;
	
	class_t*			clss = this;
	
	while( clss->m_up )
	{
		if( index >= 32 )
			throw_icarus_error( "class chain too deep" );
	
		chain[ index++ ] = clss;
		clss = clss->m_up;
	}
	
	while( --index >= 0 )
	{
		clss = chain[ index ];
	
		stream.write_u16( clss->m_name_length );
		stream.write_bytes( clss->m_name,
			clss->m_name_length * sizeof( char ) );
	}

	stream.write_u16( 0 );
}

class_t*
class_t::import_reference(
	input_stream_t&		stream )
{
	class_t*			clss = this;
	char				name[ 256 ];

	while( true )
	{
		u16					length = stream.read_u16();
		
		if( !length )
			return clss;
					
		if( length > 256 )
			throw_icarus_error( "class name too long" );
			
		stream.read_bytes( name, length * sizeof( char ) );
			
		clss = clss->find_or_create_class( name, length );
	}
}

static char*
read_string(
	machine_t*			vm,
	input_stream_t*		stream )
{
	char*				text;
	long				length;
		
	length = stream->read_u16();
	text = (char*)vm->xalloc( ( length + 1 ) * sizeof( char ) );
	stream->read_bytes( text, length * sizeof( char ) );
	text[ length ] = '\0';
		
	return text;
}

inline void
import_variable_array(
	machine_t*			vm,
	input_stream_t*		stream,
	long				count,
	variable_t**		variableptr )
{
	variable_t*			variable;

	if( count )
	{
		variable = new variable_t[ count ];
		*variableptr = variable;
	
		do {
			variable->name = read_string( vm, stream );
			variable->type = read_string( vm, stream );
			variable->offset = stream->read_u32();
			variable++;
		} while( --count );
	}
}

void
class_t::import_variables(
	input_stream_t*		stream )
{
	m_local_var_count = stream->read_u16();
	m_static_var_count = stream->read_u16();

	import_variable_array(
		m_machine,
		stream, m_local_var_count,
		&m_local_vars );

	import_variable_array(
		m_machine,
		stream, m_static_var_count,
		&m_static_vars );
}

func_t*
class_t::find_func_by_id_flat(
	func_id_t			id ) const
{
	func_t**			table = m_func_table;
	
	long				left = 0;
	long				right = m_func_count - 1;
	
	while( left <= right )
	{
		long			current;
	
		current = ( left + right ) / 2;
		
		func_t*			func = table[ current ];
		
		long			comparison;
		
		comparison = func->get_id() - id;
		
		if( comparison == 0 )
			return func;
		else if( comparison > 0 )
			right = current - 1;
		else
			left = current + 1;
	}
	
	return 0;
}

END_MACHINE_NAMESPACE
