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
//	bytecode_func_t.cp		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "bytecode_func_t.h"
#include "machine_class_t.h"
#include "machine_t.h"
#include "input_stream_t.h"
#include "vmutils.h"
#include "safe_memory_t.h"

#ifdef VM_PROJECT
#include "string_t.h"
#endif

#include "vmdefs.h"
#include "bytecode.h"

BEGIN_MACHINE_NAMESPACE

bytecode_func_t::bytecode_func_t(
	func_id_t			id,
	class_t*			owner ) :
	
	func_t( id, func_type_bytecode, owner )
{
	m_code_size = 0;
	m_code_data = 0;
	
	m_quad_table = 0;
	m_octa_table = 0;
	m_float_table = 0;
	m_double_table = 0;
	
#if ARIADNE_STRING_OBJECTS
	m_string_pool = 0;
	m_string_count = 0;
#endif
	m_string_table = 0;
	m_string_data = 0;
	
	m_call_table = 0;
	m_call_count = 0;
	m_class_table = 0;
	
	m_lifetime_count = 0;
	m_lifetime_table = 0;
	
	m_sourcemap_count = 0;
	m_sourcemap_table = 0;
	
	m_breakpoints = 0;
	
	m_locals_size = 0;
}

bytecode_func_t::~bytecode_func_t()
{
	if( m_code_data )
		delete[] m_code_data;
		
	/*if( m_quad_table )
		delete[] m_quad_table;

	if( m_octa_table )
		delete[] m_octa_table;

	if( m_float_table )
		delete[] m_float_table;

	if( m_double_table )
		delete[] m_double_table;*/
		
#if ARIADNE_STRING_OBJECTS
	//if( m_string_pool )
	//	delete[] m_string_pool;
#endif

	if( m_string_table )
		delete[] m_string_table;
		
	//if( m_string_data )
	//	delete[] m_string_data;
		
	if( m_call_table )
		delete[] m_call_table;
		
	if( m_class_table )
		delete[] m_class_table;

	if( m_lifetime_table )
	{
		/*lifetime_t*		lifetime = m_lifetime_table;
		long			count = m_lifetime_count;
	
		do {
			delete[] lifetime->name;
			delete[] lifetime->type;
			lifetime++;
		} while( --count );*/
		
		delete[] m_lifetime_table;
	}

	if( m_sourcemap_table )
		delete[] m_sourcemap_table;

	while( m_breakpoints )
	{
		breakpoint_t*	next = m_breakpoints->next;
		delete m_breakpoints;
		m_breakpoints = next;
	}
}

void
bytecode_func_t::import(
	input_stream_t*		stream,
	machine_t*			vm )
{
	m_code_size = stream->read_u32();
	m_code_data = new u08[ m_code_size ];
	stream->read_bytes( m_code_data, m_code_size );
	
	import_long_table( stream, vm );
	import_wide_table( stream, vm );
	import_float_table( stream, vm );
	import_double_table( stream, vm );
	import_string_table( stream, vm );
	
	import_call_table( stream, vm );
	import_class_table( stream, vm );
	
	m_locals_size = stream->read_u32();
	
	import_lifetimes( stream, vm );
	import_sourcemaps( stream );
}

void
bytecode_func_t::finalize(
	machine_t*			vm )
{
	call_t*				call = m_call_table;
	unsigned long		call_count = m_call_count;
	
	while( call_count-- )
	{
		class_t*		clss;
		unsigned long	offset = 0;
		func_t*			func;
		
		clss = m_class_table[ call->clssindx ];
		func = clss->find_func_by_id( call->funcid, &offset );
		if( !func || offset )
			throw_icarus_error( "regular function not found" );
		call->regufunc = func;
		
		call++;
	}
	
	setup_string_pool( vm );
}

void
bytecode_func_t::import_long_table(
	input_stream_t*		stream,
	machine_t*			vm )
{
	unsigned int		count = stream->read_u16();
	
	if( !count )
		return;
	
	m_quad_table = (u32*)vm->xalloc( count * sizeof( u32 ) );
	
	u32*				entry = m_quad_table;
	
	while( count-- )
		*( entry++ ) = stream->read_u32();
}

void
bytecode_func_t::import_wide_table(
	input_stream_t*		stream,
	machine_t*			vm )
{
	unsigned int		count = stream->read_u16();
	
	if( !count )
		return;
	
	m_octa_table = (u64*)vm->xalloc( count * sizeof( u64 ) );
	
	u64*				entry = m_octa_table;
	
	while( count-- )
		*( entry++ ) = stream->read_u64();
}

void
bytecode_func_t::import_float_table(
	input_stream_t*		stream,
	machine_t*			vm )
{
	unsigned int		count = stream->read_u16();
	
	if( !count )
		return;

	m_float_table = (f32*)vm->xalloc( count * sizeof( f32 ) );

	f32*				entry = m_float_table;
	
	while( count-- )
		*( entry++ ) = stream->read_f32();
}

void
bytecode_func_t::import_double_table(
	input_stream_t*		stream,
	machine_t*			vm )
{
	unsigned int		count = stream->read_u16();
	
	if( !count )
		return;

	m_double_table = (f64*)vm->xalloc( count * sizeof( f64 ) );
	
	f64*				entry = m_double_table;
	
	while( count-- )
		*( entry++ ) = stream->read_f64();
}

void
bytecode_func_t::import_string_table(
	input_stream_t*		stream,
	machine_t*			vm )
{
	unsigned int		count = stream->read_u16();
	
#if ARIADNE_STRING_OBJECTS
	m_string_count = count;
#endif

	if( count )
	{
		m_string_table = new u32[ count ];
	
		u32*			entry = m_string_table;
	
		while( count-- )
			*( entry++ ) = stream->read_u32();
	}
		
	unsigned long		size = stream->read_u32();
	
	if( size )
	{
		memory_t*		memory = vm->get_memory();
	
		m_string_data = (char*)memory->alloc(
			size * sizeof( char ) );
		stream->read_bytes( m_string_data, size );
	}
}

void
bytecode_func_t::import_call_table(
	input_stream_t*		stream,
	machine_t*			vm )
{
	unsigned int		call_count;
	
	call_count = stream->read_u16();
	
	m_call_count = call_count;
	
	if( !call_count )
		return;
	
	m_call_table = new call_t[ call_count ];
	
	call_t*				call = m_call_table;
	
	while( call_count-- )
	{
		call->clssindx = stream->read_u16();
		call->funcid = vm->read_func_id( stream );
		call++;
	}
}

void
bytecode_func_t::import_class_table(
	input_stream_t*		stream,
	machine_t*			vm )
{
	unsigned int		class_count;
	
	class_count = stream->read_u16();
	
	if( !class_count )
		return;
	
	m_class_table = new class_t*[ class_count ];
	
	class_t**			clssp = m_class_table;
	
	while( class_count-- )
	{
		*clssp = vm->read_class_id( stream );
		clssp++;
	}
}

long
bytecode_func_t::find_source_location(
	long				offset )
{
	sourcemap_t*		map = find_sourcemap( offset );

	if( map )
		return map->srcpos - 1;
	
	return -1;
}

long
bytecode_func_t::find_code_location(
	long				srcpos )
{
	//Debugger();
	// not tested

	sourcemap_t*		map = inverse_sourcemap( srcpos );

	if( map == 0 )
		return -1;
		
	return map->offset;
}

long
bytecode_func_t::get_code_offset(
	const u08*			code ) const
{
	return code - m_code_data;
}

const u08*
bytecode_func_t::find_break_location(
	const u08*			code ) const
{
	long				offset = code - m_code_data;
	
	/*if( offset > 0 )
		offset -= 1;*/
	
	sourcemap_t*		map = find_sourcemap( offset );

	if( map == 0 )
		return 0;
		
	if( map - m_sourcemap_table >= m_sourcemap_count - 1 )
		return 0;
		
	map++;
			
	return map->offset + m_code_data;
}

static char*
read_string(
	input_stream_t*		stream,
	machine_t*			vm )
{
	char*				text;
	long				length;
		
	length = stream->read_u16();
	text = (char*)vm->xalloc( ( length + 1 ) * sizeof( char ) );
	stream->read_bytes( text, length * sizeof( char ) );
	text[ length ] = '\0';
		
	return text;
}

void
bytecode_func_t::import_lifetimes(
	input_stream_t*		stream,
	machine_t*			vm )
{
	long				count;
	
	count = stream->read_u32();
	m_lifetime_count = count;

	if( !count )
		return;
		
	lifetime_t*			lifetime;
	
	lifetime = new lifetime_t[ count ];
	m_lifetime_table = lifetime;

	do {
		lifetime->name = read_string( stream, vm );
		lifetime->type = read_string( stream, vm );
		lifetime->offset = stream->read_u32();
		lifetime->create = stream->read_u32();
		lifetime->destroy = stream->read_u32();
		
		lifetime++;
	} while( --count );
}

void
bytecode_func_t::import_sourcemaps(
	input_stream_t*		stream )
{
	long				count;
	
	count = stream->read_u32();
	m_sourcemap_count = count;

	if( !count )
		return;

	sourcemap_t*		sourcemap;
	
	sourcemap = new sourcemap_t[ count ];
	m_sourcemap_table = sourcemap;
	
	do {
		sourcemap->offset = stream->read_u32();
		sourcemap->srcpos = stream->read_u32();
		sourcemap++;
	} while( --count );
}

void
bytecode_func_t::setup_string_pool(
	machine_t*			vm )
{
#ifdef VM_PROJECT
#if ARIADNE_STRING_OBJECTS
	long				count;

	count = m_string_count;

	if( count == 0 )
		return;

	long				spacing;
	class_t*			string_class;

	string_class = vm->get_string_class();

	if( not string_class )
		throw_icarus_error( "no string class" );

	spacing = string_class->size();

	memory_t*			memory = vm->get_memory();

	m_string_pool = memory->alloc( count * spacing );

	u32*				table;
	char*				data;
	
	u08*				instance;
	
	table = m_string_table;
	data = m_string_data;

	instance = (u08*)m_string_pool;

	for( long i = 0; i < count; i++ )
	{	
		char*			text = data + table[ i ];
		u08*			block;
	
		string_class->init_instance( instance );

		block = instance + CLASS_HEADER_SIZE;
	
		string_t*		string = (string_t*)block;
		
		string->text = text;
		string->length = istrlen( text );

		instance += spacing;
	}	
#endif
#endif
}

// ---------------------------------------------------------------------------

sourcemap_t*
bytecode_func_t::find_sourcemap(
	long				offset ) const
{
	sourcemap_t*		table = m_sourcemap_table;
	long				count = m_sourcemap_count;

	long				left = 0;
	long				right = count - 1;
	long				current = 0;
	long				comparison = 0;
	
	while( left <= right )
	{
		current = ( left + right ) / 2;
		
		sourcemap_t*	sourcemap = &table[ current ];
		
		comparison = sourcemap->offset - offset;
		
		if( comparison == 0 )
			return sourcemap;
		else if( comparison > 0 )
			right = current - 1;
		else
			left = current + 1;
	}
	
	if( comparison > 0 && current > 0 )
		current--;
	
	if( current < count )
		return &table[ current ];
	
	return 0;
}

sourcemap_t*
bytecode_func_t::inverse_sourcemap(
	long				srcpos ) const
{
	long				count = m_sourcemap_count;

	if( count < 1 )
		return 0;

	sourcemap_t*		table = m_sourcemap_table;

	if( srcpos < table[ 0 ].srcpos ||
		srcpos > table[ count - 1 ].srcpos )
	{
		return 0;
	}
	
	long				low, high;
	
	for( low = 0; low < count; low++ )
	{
		sourcemap_t*	map = &table[ low ];
		
		if( srcpos <= map->srcpos )
			break;
	}

	for( high = count - 1; high >= 0; high-- )
	{
		sourcemap_t*	map = &table[ high ];
		
		if( srcpos >= map->srcpos )
			break;
	}
	
	if( low <= high )
	{
		long			index;

		index = ( low + high ) / 2 + 1;
		if( index < count )
			return &table[ index ];
	}

	return 0;
}

// ---------------------------------------------------------------------------

void
bytecode_func_t::add_breakpoint(
	long				offset,
	int					breakcode )
{
	if( offset < 0 || offset + 2 > m_code_size )
		return;

	breakpoint_t*		breakpoint = m_breakpoints;
	
	while( breakpoint )
	{
		if( breakpoint->offset == offset )
			return;
		breakpoint = breakpoint->next;
	}

	u08*				code = m_code_data + offset;

	breakpoint = new breakpoint_t;
	
	breakpoint->offset = offset;
	breakpoint->saved = *code;
	
	breakpoint->next = m_breakpoints;
	m_breakpoints = breakpoint;
	
	*code = breakcode;
}

void
bytecode_func_t::remove_breakpoint(
	long				offset )
{
	if( offset < 0 || offset + 2 > m_code_size )
		return;

	breakpoint_t**		link = &m_breakpoints;
	breakpoint_t*		breakpoint = *link;
	
	while( breakpoint )
	{
		if( breakpoint->offset == offset )
		{
			*link = breakpoint->next;
			
			u08*		code = m_code_data + breakpoint->offset;
			
			*code = breakpoint->saved;
			
			delete breakpoint;
			break;
		}
		
		link = &breakpoint->next;
		breakpoint = *link;
	}
}

u08
bytecode_func_t::hide_breakpoint(
	long				offset )
{
	breakpoint_t*		breakpoint = m_breakpoints;
	
	while( breakpoint )
	{
		if( breakpoint->offset == offset )
			return breakpoint->saved;
		breakpoint = breakpoint->next;
	}

	throw_icarus_error( "illegal breakpoint" );
}

END_MACHINE_NAMESPACE
