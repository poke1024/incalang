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
//	lib_input.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "vmutils.h"
#include "string_t.h"

#include "lib_string.h"
#include "machine_class_t.h"

#include "lib_utils.h"
#include "machine_error.h"
#include "vmbind.h"
#include "text_console_t.h"
#include "input_t.h"
#include "thread_t.h"

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

static func_id_t
init_read_bytes_func_id(
	machine_t&			m )
{
	func_id_t			id;
	
	id = m.find_or_create_func_id( "readBytes;B*;i", 14 );
	m.m_lib_static.read_bytes_func_id = id;
	
	return id;
}
#define CALL_STREAM_READ_BYTES( buffer, bufsize )							\
	class_header_t*		header;												\
	class_t*			clss;												\
	func_id_t			id;													\
	machine_t&			m = *f.machine();									\
																			\
	header = CLASS_HEADER( instptr );										\
	clss = header->clss;													\
																			\
	if( ( id = m.m_lib_static.read_bytes_func_id ) == 0 )					\
		id = init_read_bytes_func_id( m );									\
																			\
	unsigned long		offset = 0;											\
	func_t*				func = clss->find_func_by_id( id, &offset );		\
																			\
	if( func )																\
	{																		\
		f.push_int( bufsize );												\
		f.push_ptr( buffer );												\
		f.call( offset_instance(											\
			instptr, offset - header->offset ), func );						\
	}

static void
stream_read_bytes(
	fiber_t&			f,
	instance_t*			instptr,
	void*				buffer,
	size_t				bufsize )
{
	CALL_STREAM_READ_BYTES( buffer, bufsize );
}

// ---------------------------------------------------------------------------

static func_id_t
init_read_some_bytes_func_id(
	machine_t&			m )
{
	func_id_t			id;
	
	id = m.find_or_create_func_id( "readSomeBytes;B*;i", 18 );
	m.m_lib_static.read_some_bytes_func_id = id;
	
	return id;
}
#define CALL_STREAM_READ_SOME_BYTES( buffer, bufsize )						\
	class_header_t*		header;												\
	class_t*			clss;												\
	func_id_t			id;													\
	machine_t&			m = *f.machine();									\
																			\
	header = CLASS_HEADER( instptr );										\
	clss = header->clss;													\
																			\
	if( ( id = m.m_lib_static.read_some_bytes_func_id ) == 0 )				\
		id = init_read_some_bytes_func_id( m );								\
																			\
	unsigned long		offset = 0;											\
	func_t*				func = clss->find_func_by_id( id, &offset );		\
																			\
	if( func )																\
	{																		\
		f.push_int( bufsize );												\
		f.push_ptr( buffer );												\
		f.call( offset_instance(											\
			instptr, offset - header->offset ), func );						\
	}

// ---------------------------------------------------------------------------

#define READ_TYPE_FROM_STREAM( T )											\
	T					val;												\
																			\
	{																		\
		st_stack_bytes	space( f, sizeof( T ) );							\
		u08*			buf = space();										\
		CALL_STREAM_READ_BYTES( buf, sizeof( T ) );							\
		val = *(T*)buf;														\
	}

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( input_stream_read_char, void )
	READ_TYPE_FROM_STREAM( char );
	f.push_char( val );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_bool, void )
	READ_TYPE_FROM_STREAM( u08 );
	f.push_bool( val );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_byte, void )
	READ_TYPE_FROM_STREAM( u08 );
	f.push_int( val );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_word, void )
	READ_TYPE_FROM_STREAM( u16 );
	f.push_int( val );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_quad, void )
	READ_TYPE_FROM_STREAM( u32 );
	f.push_int( val );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_octa, void )
	READ_TYPE_FROM_STREAM( u64 );
	f.push_long( val );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_float, void )
	READ_TYPE_FROM_STREAM( float );
	f.push_float( val );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_double, void )
	READ_TYPE_FROM_STREAM( double );
	f.push_double( val );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_string, void )
	u08					block[ STRING_CLASS_SIZE ];
	long				length;
		
	{
		READ_TYPE_FROM_STREAM( u32 );
		length = val;
	}
	{
		st_stack_chars	space( f, length );
		char*			buf = space();
		CALL_STREAM_READ_BYTES( buf, length * sizeof( char ) );

		init_string( block, m );
		string_create( m,
			INSTANCE_SELF( string_t, block ),
			buf, length );
	}
	
	f.push_block( block, STRING_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_pointer, void )
	READ_TYPE_FROM_STREAM( void* );
	f.push_ptr( val );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( input_stream_read_bytes, void )
	u08*				buf = (u08*)f.pop_ptr();
	long				count = f.pop_int();
	
	f.check_access( buf, count );
	CALL_STREAM_READ_SOME_BYTES( buf, count );

	if( count != f.pop_int() )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( input_stream_read_chunk, void )
	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];
	
	init_string( block, m );
	
	istring				s;
	char				c;

	stream_read_bytes( f, instptr, &c, sizeof( char ) );
	
	while( iisspace( c ) )
		stream_read_bytes( f, instptr, &c, sizeof( char ) );

	while( true )
	{	
		s.append( 1, c );
		
		stream_read_bytes( f, instptr, &c, sizeof( char ) );

		if( iisspace( c ) )
			break;
	}
		
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		s.data(), s.length() );

	f.push_block( block, STRING_CLASS_SIZE ); 	
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( console_input_stream_read_chunk, void )
	machine_t&			m = *f.machine();
	text_console_t*		console = vm_text_console( &m );
	istring				text;
	
	console->input( text );

	u08					block[ STRING_CLASS_SIZE ];
		
	init_string( block, m );
	
	string_create( m,
		INSTANCE_SELF( string_t, block ),
		text.data(), text.length() );
		
	f.push_block( block, STRING_CLASS_SIZE ); 
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( console_input_stream_read_some_bytes, void )
	f.push_int( 0 );
END_STDLIB_FUNCTION

static void
create_string_from_key(
	machine_t&			m,
	string_t*			string,
	s32					key )
{
	char				c[ 2 ];

	if( key == 0 )
		string_create( m, string, 0, 0 );
	else if( key >= 0 )
	{
		c[ 0 ] = key;
		string_create( m, string, c, 1 );
	}
	else
	{
		c[ 0 ] = 0;
		c[ 1 ] = -key;
		string_create( m, string, c, 2 );
	}
}

STDLIB_FUNCTION( method_inkey )
	machine_t&			m = *f.machine();
	s32					key = vm_inkey( &m );

	u08					block[ STRING_CLASS_SIZE ];
	string_t*			string;
	init_string( block, m );
	string = INSTANCE_SELF( string_t, block );
	create_string_from_key( m, string, key );

	f.push_block( block, STRING_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( method_waitkey )
	machine_t&			m = *f.machine();
	s32					key;
	
	while( true )
	{
		key = vm_inkey( &m );
		if( key )
			break;
		ithread_t::current()->sleep( 100L * 1000L );
		vm_process_os_events();
	}

	u08					block[ STRING_CLASS_SIZE ];
	string_t*			string;
	init_string( block, m );
	string = INSTANCE_SELF( string_t, block );
	create_string_from_key( m, string, key );
	f.push_block( block, STRING_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( method_vkey_down )
	long				keycode = f.pop_int();
	f.push_bool( fizban::rt_key_down( keycode ) );
END_STDLIB_FUNCTION

/*STDLIB_FUNCTION( method_vkey_code )
	long				keycode = f.pop_int();
#if ICARUS_OS_WIN32
	f.push_int( 0 );
#else
	f.push_int( ascii_to_virtual( keycode ) );
#endif
END_STDLIB_FUNCTION*/

// ---------------------------------------------------------------------------

void
init_input_stream(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "inkey", method_inkey );

	register_native_function(
		m, rootclss, "waitkey", method_waitkey );

	register_native_function(
		m, rootclss, "vkeyDown;i", method_vkey_down );

	//register_native_function(
	//	m, rootclss, "vkeyCode;i", method_vkey_code );

	class_t*			clss;
	
	clss = rootclss->find_class( "InputStream", 11 );
	if( !clss )
		throw_icarus_error( "InputStream class not found" );

	register_native_function(
		m, clss, "readChar", input_stream_read_char );

	register_native_function(
		m, clss, "readBool", input_stream_read_bool );

	register_native_function(
		m, clss, "readByte", input_stream_read_byte );

	register_native_function(
		m, clss, "readWord", input_stream_read_word );

	register_native_function(
		m, clss, "readQuad", input_stream_read_quad );

	register_native_function(
		m, clss, "readOcta", input_stream_read_octa );

	register_native_function(
		m, clss, "readFloat", input_stream_read_float );

	register_native_function(
		m, clss, "readDouble", input_stream_read_double );

	register_native_function(
		m, clss, "readString", input_stream_read_string );

	register_native_function(
		m, clss, "readPointer", input_stream_read_pointer );
	
	register_native_function(
		m, clss, "readBytes;B*;i", input_stream_read_bytes );

	register_native_function(
		m, clss, "readChunk", input_stream_read_chunk );
	
	clss = rootclss->find_class( "ConsoleInputStream", 18 );
	if( !clss )
		throw_icarus_error( "ConsoleInputStream class not found" );

	register_native_function(
		m, clss, "readChunk",
		console_input_stream_read_chunk,
		native_func_can_block );

	register_native_function(
		m, clss, "readSomeBytes;B*;i",
		console_input_stream_read_some_bytes );
}

END_MACHINE_NAMESPACE
