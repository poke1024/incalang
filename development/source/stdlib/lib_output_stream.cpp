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
//	lib_output_stream.cp	   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "string_t.h"
#include "machine_class_t.h"
#include "lib_utils.h"
#include "lib_string.h"
#include "vmbind.h"
#include "text_console_t.h"

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

static func_id_t
init_write_bytes_func_id(
	machine_t&			m )
{
	func_id_t			id;
	
	id = m.find_or_create_func_id( "writeBytes;.B*;i", 16 );
	m.m_lib_static.write_bytes_func_id = id;
	
	return id;
}
#define CALL_STREAM_WRITE_BYTES( buffer, bufsize )							\
	class_header_t*		header;												\
	class_t*			clss;												\
	func_id_t			id;													\
	machine_t&			m = *f.machine();									\
																			\
	header = CLASS_HEADER( instptr );										\
	clss = header->clss;													\
																			\
	if( ( id = m.m_lib_static.write_bytes_func_id ) == 0 )					\
		id = init_write_bytes_func_id( m );									\
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

static func_id_t
init_write_some_bytes_func_id(
	machine_t&			m )
{
	func_id_t			id;
	
	id = m.find_or_create_func_id( "writeSomeBytes;.B*;i", 20 );
	m.m_lib_static.write_some_bytes_func_id = id;
	
	return id;
}
#define CALL_STREAM_WRITE_SOME_BYTES( buffer, bufsize )						\
	class_header_t*		header;												\
	class_t*			clss;												\
	func_id_t			id;													\
	machine_t&			m = *f.machine();									\
																			\
	header = CLASS_HEADER( instptr );										\
	clss = header->clss;													\
																			\
	if( ( id = m.m_lib_static.write_some_bytes_func_id ) == 0 )				\
		id = init_write_some_bytes_func_id( m );							\
																			\
	unsigned long		offset = 0;											\
	func_t*				func = clss->find_func_by_id( id, &offset );		\
																			\
	if( func )																\
	{																		\
		f.push_int( bufsize );												\
		f.push_ptr( (void*)buffer );										\
		f.call( offset_instance(											\
			instptr, offset - header->offset ), func );						\
	}

// ---------------------------------------------------------------------------

#define WRITE_TYPE_TO_STREAM( T )											\
	{																		\
		st_stack_bytes	space( f, sizeof( T ) );							\
		u08*			buf = space();										\
		*(T*)buf = val;														\
		CALL_STREAM_WRITE_BYTES( buf, sizeof( T ) );						\
	}

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( output_stream_write_char, void )
	char				val = f.pop_char();
	WRITE_TYPE_TO_STREAM( char );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_bool, void )
	u08					val = f.pop_bool();
	WRITE_TYPE_TO_STREAM( u08 );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_byte, void )
	u08				val = f.pop_int();
	WRITE_TYPE_TO_STREAM( u08 );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_word, void )
	u16				val = f.pop_int();
	WRITE_TYPE_TO_STREAM( u16 );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_quad, void )
	u32				val = f.pop_int();
	WRITE_TYPE_TO_STREAM( u32 );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_octa, void )
	u64				val = f.pop_long();
	WRITE_TYPE_TO_STREAM( u64 );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_float, void )
	float			val = f.pop_float();
	WRITE_TYPE_TO_STREAM( float );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_double, void )
	double			val = f.pop_double();
	WRITE_TYPE_TO_STREAM( double );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_string, void )
	string_t*		string = pop_string( f );
	long			length = string->length;

	{
		u32			val = length;
		WRITE_TYPE_TO_STREAM( u32 );
	}
	
	if( length )
	{
		CALL_STREAM_WRITE_BYTES(
			string->text, length * sizeof( char ) );
	}
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_pointer, void )
	void*			val = f.pop_ptr();
	WRITE_TYPE_TO_STREAM( void* );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( output_stream_write_bytes, void )
	const u08*			buf = (const u08*)f.pop_ptr();
	long				count = f.pop_int();
	
	f.check_access( buf, count );
	CALL_STREAM_WRITE_SOME_BYTES( buf, count );

	if( count != f.pop_int() )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( output_stream_write_chunk, void )
	char*				buf = (char*)f.pop_ptr();
	long				count = f.pop_int() * sizeof( char );

	f.check_access( buf, count );
	CALL_STREAM_WRITE_SOME_BYTES( buf, count );

	if( count != f.pop_int() )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( pstream_write_chunk, void )
	char*				buf = (char*)f.pop_ptr();
	long				bufsize = f.pop_int();	
	
	if( bufsize < 1 )
		return;
	
	machine_t&			m = *f.machine();
	text_console_t*		console = vm_text_console( &m );
	
	f.check_access( buf, sizeof( char ) * bufsize );
	console->print( buf, bufsize );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( pstream_write_some_bytes, void )
	u08*				buf = (u08*)f.pop_ptr();
	long				bufsize = f.pop_int();	
	
	machine_t&			m = *f.machine();
	text_console_t*		console = vm_text_console( &m );
		
	f.check_access( buf, bufsize );
	
	if( sizeof( char ) == sizeof( u08 ) )
		console->print( (char*)buf, bufsize );
	else
	{
		for( long i = 0; i < bufsize; i++ )
		{
			char			c = buf[ i ];
			console->print( &c, 1 );
		}
	}
	
	f.push_int( bufsize );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( pstream_flush, void )
	machine_t&			m = *f.machine();
	text_console_t*		console = vm_text_console( &m );
	console->flush();
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_output_stream(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	class_t*			clss;

	clss = rootclss->find_class( "OutputStream", 12 );
	if( !clss )
		throw_icarus_error( "OutputStream class not found" );

	register_native_function(
		m, clss, "writeChar;c", output_stream_write_char );

	register_native_function(
		m, clss, "writeBool;a", output_stream_write_bool );

	register_native_function(
		m, clss, "writeByte;i", output_stream_write_byte );

	register_native_function(
		m, clss, "writeWord;i", output_stream_write_word );

	register_native_function(
		m, clss, "writeQuad;I", output_stream_write_quad );

	register_native_function(
		m, clss, "writeOcta;L", output_stream_write_octa );

	register_native_function(
		m, clss, "writeFloat;f", output_stream_write_float );

	register_native_function(
		m, clss, "writeDouble;d", output_stream_write_double );

	register_native_function(
		m, clss, "writeString;.String::&", output_stream_write_string );

	register_native_function(
		m, clss, "writePointer;.v*", output_stream_write_string );

	register_native_function(
		m, clss, "writeBytes;.B*;i", output_stream_write_bytes );

	register_native_function(
		m, clss, "writeChunk;.c*;i", output_stream_write_chunk );

	clss = rootclss->find_class( "ConsoleOutputStream", 19 );
	if( !clss )
		throw_icarus_error( "ConsoleOutputStream class not found" );

	register_native_function(
		m, clss, "writeChunk;.c*;i", pstream_write_chunk );

	register_native_function(
		m, clss, "writeSomeBytes;.B*;i", pstream_write_some_bytes );

	register_native_function(
		m, clss, "flush", pstream_flush );
}

END_MACHINE_NAMESPACE
