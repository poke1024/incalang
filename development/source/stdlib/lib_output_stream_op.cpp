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
//	lib_output_stream_op.cp	   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "string_t.h"
#include "machine_class_t.h"
#include "lib_utils.h"
#include "lib_string.h"
#include "lib_bigint.h"
#include "bigint_t.h"
#include "lib_complex.h"
#include "lib_point.h"

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

static func_id_t
init_write_chunk_func_id(
	machine_t&			m )
{
	func_id_t			id;
	
	id = m.find_or_create_func_id( "writeChunk;.c*;i", 16 );
	m.m_lib_static.write_chunk_func_id = id;
	
	return id;
}

#define CALL_STREAM_WRITE_CHUNK												\
	class_header_t*		header;												\
	class_t*			clss;												\
	func_id_t			id;													\
	machine_t&			m = *f.machine();									\
																			\
	header = CLASS_HEADER( (instance_t*)instance );							\
	clss = header->clss;													\
																			\
	if( ( id = m.m_lib_static.write_chunk_func_id ) == 0 )					\
		id = init_write_chunk_func_id( m );									\
																			\
	unsigned long		offset = 0;											\
	func_t*				func = clss->find_func_by_id( id, &offset );		\
																			\
	if( func )																\
	{																		\
		f.push_int( bufsize );												\
		f.push_ptr( (void*)buf );											\
		f.call( offset_instance(											\
			instance, offset - header->offset ), func );					\
	}

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( ostream_pointer )
	instance_t*			instance = f.pop_instance();
	void*				ptr = f.pop_ptr();
	
{
	st_stack_chars		space( f, 32 + 1 );
	int					bufsize;
	char*				buf = space();
	
	print_pointer( buf, 32, ptr );
	bufsize = istrlen( buf );
	
	CALL_STREAM_WRITE_CHUNK
}
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_bool )
	instance_t*			instance = f.pop_instance();
	bool				val = f.pop_bool();
		
{
	st_stack_chars		space( f, 8 );
	char*				buf = space();
	int					bufsize;
	
	if( val )
	{
		imemcpy( buf, "true", 4 * sizeof( char ) );
		bufsize = 4;
	}
	else
	{
		imemcpy( buf, "false", 5 * sizeof( char ) );
		bufsize = 5;
	}
	
	CALL_STREAM_WRITE_CHUNK
}	

	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_char )
	instance_t*			instance = f.pop_instance();
	char				val = f.pop_char();
	
{
	st_stack_chars		space( f, 1 + 1 );
	char*				buf = space();
	
	buf[ 0 ] = val;
	int					bufsize = 1;
	
	CALL_STREAM_WRITE_CHUNK
}
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_int )
	instance_t*			instance = f.pop_instance();
	long				val = f.pop_int();
	
{
	st_stack_chars		space( f, 32 + 1 );
	int					bufsize;
	char*				buf = space();
	
	isnprintf( buf, 32, "%ld", (long)val );
	bufsize = istrlen( buf );
	
	CALL_STREAM_WRITE_CHUNK
}
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_long )
	instance_t*			instance = f.pop_instance();
	s64					val = f.pop_long();
	
{
	st_stack_chars		space( f, 32 + 1 );
	int					bufsize;
	char*				buf = space();
	const char*			format;
	
#if __VISC__
	format = "%I64d";
#else
	format = "%lld";
#endif

	isnprintf( buf, 32, format, (s64)val );
	bufsize = istrlen( buf );
	
	CALL_STREAM_WRITE_CHUNK
}
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_float )
	instance_t*			instance = f.pop_instance();
	float				val = f.pop_float();
	
{
	st_stack_chars		space( f, 32 + 1 );
	int					bufsize;
	char*				buf = space();
	
	isnprintf( buf, 32, "%g", (float)val );
	bufsize = istrlen( buf );
	
	CALL_STREAM_WRITE_CHUNK
}
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_double )
	instance_t*			instance = f.pop_instance();
	double				val = f.pop_double();
	
{
	st_stack_chars		space( f, 32 + 1 );
	int					bufsize;
	char*				buf = space();
	
	isnprintf( buf, 32, "%lg", (double)val );
	bufsize = istrlen( buf );
	
	CALL_STREAM_WRITE_CHUNK
}
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_string )
	instance_t*			instance = f.pop_instance();
	string_t*			string = pop_string( f );

	char*				buf = string->text;
	long				bufsize = string->length;
		
	CALL_STREAM_WRITE_CHUNK
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_bigint )
	instance_t*			instance = f.pop_instance();
	bigint_t*			bigint = pop_bigint( f );
	istring				text;
	
	bigint->to_string( text, 10 );
	
	memory_t*			memory = f.machine()->get_memory();
	
	char*				buf;
	long				bufsize = text.length();
	
	buf = (char*)memory->alloc( bufsize * sizeof( char ) );	
	imemcpy( buf, text.data(), bufsize * sizeof( char ) );
		
	try {
		CALL_STREAM_WRITE_CHUNK
	} catch(...) {
		memory->free( buf );
		throw;
	}
	
	memory->free( buf );
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_complex )
	instance_t*			instance = f.pop_instance();
	complex_t*			complex = pop_complex( f );
	
{
	st_stack_chars		space( f, 64 + 1 );
	int					bufsize;
	char*				buf = space();
	
	isnprintf( buf, 64, "(%lg,%lg)",
		(double)complex->re,
		(double)complex->im );
	bufsize = istrlen( buf );
	
	CALL_STREAM_WRITE_CHUNK
}
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( ostream_point )
	instance_t*			instance = f.pop_instance();
	point_t*			point = pop_point( f );
	
{
	st_stack_chars		space( f, 128 + 1 );
	int					bufsize;
	char*				buf = space();
	
	isnprintf( buf, 128, "(%g,%g,%g)",
		(float)point->x,
		(float)point->y,
		(float)point->z );
	bufsize = istrlen( buf );
	
	CALL_STREAM_WRITE_CHUNK
}
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

void
ostream_write_chunk(
	fiber_t&			f,
	instance_t*			instance,
	const char*			buf,
	u32					bufsize )
{
	CALL_STREAM_WRITE_CHUNK
}

// ---------------------------------------------------------------------------

void
init_output_stream_operators(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;.v*", ostream_pointer );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;a", ostream_bool );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;c", ostream_char );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;i", ostream_int );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;l", ostream_long );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;f", ostream_float );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;d", ostream_double );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;.String::&", ostream_string );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;.BigInt::&", ostream_bigint );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;.Complex::&", ostream_complex );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;.Point::&", ostream_point );
}

END_MACHINE_NAMESPACE
