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
//	lib_input_stream_op.cp	   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "vmutils.h"
#include "string_t.h"
#include "bigint_t.h"

#include "lib_string.h"
#include "lib_bigint.h"
#include "machine_class_t.h"

#include "lib_utils.h"

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

static func_id_t
init_read_chunk_func_id(
	machine_t&			m )
{
	func_id_t			id;
	
	id = m.find_or_create_func_id( "readChunk", 9 );
	m.m_lib_static.read_chunk_func_id = id;
	
	return id;
}

#define CALL_STREAM_READ_CHUNK												\
	class_header_t*		header;												\
	class_t*			clss;												\
	func_id_t			id;													\
	machine_t&			m = *f.machine();									\
																			\
	header = CLASS_HEADER( (instance_t*)instance );							\
	clss = header->clss;													\
																			\
	if( ( id = m.m_lib_static.read_chunk_func_id ) == 0 )					\
		id = init_read_chunk_func_id( m );									\
																			\
	unsigned long		offset = 0;											\
	func_t*				func = clss->find_func_by_id( id, &offset );		\
																			\
	if( func )																\
	{																		\
		f.call( offset_instance(											\
			instance, offset - header->offset ), func );					\
	}

// ---------------------------------------------------------------------------

#define CHECK_TEXT( text ) {												\
	long length;															\
	if( ( length = text->length ) != 0 )									\
		f.check_access( text->text, length );								\
	}


STDLIB_FUNCTION( istream_char )
	instance_t*			instance = f.pop_instance();
	char*				val = (char*)f.pop_ptr();
				
	f.check_access( val, sizeof( char ) );
				
	CALL_STREAM_READ_CHUNK

	string_t*			text;

	text = INSTANCE_SELF(
		string_t, (u08*)f.pop_block( STRING_CLASS_SIZE ) );
	CHECK_TEXT( text );
		
	if( text->length < 1 )
		*val = '\0';
	else
		*val = text->text[ 0 ];

	string_dispose( m, text );
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( istream_int )
	instance_t*			instance = f.pop_instance();
	s32*				val = (s32*)f.pop_ptr();
				
	f.check_access( val, sizeof( s32 ) );
				
	CALL_STREAM_READ_CHUNK

	string_t*			text;

	text = INSTANCE_SELF(
		string_t, (u08*)f.pop_block( STRING_CLASS_SIZE ) );
	CHECK_TEXT( text );
		
	*val = string_ival( m, text );
	string_dispose( m, text );
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( istream_long )
	instance_t*			instance = f.pop_instance();
	s64*				val = (s64*)f.pop_ptr();
				
	f.check_access( val, sizeof( s64 ) );
				
	CALL_STREAM_READ_CHUNK

	string_t*			text;

	text = INSTANCE_SELF(
		string_t, (u08*)f.pop_block( STRING_CLASS_SIZE ) );
	CHECK_TEXT( text );
	
	*val = string_ival( m, text );
	string_dispose( m, text );
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( istream_float )
	instance_t*			instance = f.pop_instance();
	float*				val = (float*)f.pop_ptr();
				
	f.check_access( val, sizeof( float ) );
				
	CALL_STREAM_READ_CHUNK

	string_t*			text;

	text = INSTANCE_SELF(
		string_t, (u08*)f.pop_block( STRING_CLASS_SIZE ) );
	CHECK_TEXT( text );
	
	*val = string_val( m, text );
	string_dispose( m, text );
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( istream_double )
	instance_t*			instance = f.pop_instance();
	double*				val = (double*)f.pop_ptr();
				
	f.check_access( val, sizeof( double ) );
				
	CALL_STREAM_READ_CHUNK

	string_t*			text;

	text = INSTANCE_SELF(
		string_t, (u08*)f.pop_block( STRING_CLASS_SIZE ) );
	CHECK_TEXT( text );
	
	*val = string_val( m, text );
	string_dispose( m, text );
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( istream_string )
	instance_t*			instance = f.pop_instance();
	string_t*			string;
	
	string = INSTANCE_SELF( string_t, f.pop_instance() );
			
	CALL_STREAM_READ_CHUNK

	string_t*			text;

	text = INSTANCE_SELF(
		string_t, (u08*)f.pop_block( STRING_CLASS_SIZE ) );
	CHECK_TEXT( text );
	
	string_assign( m, string, text );
	string_dispose( m, text );
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( istream_bigint )
	instance_t*			instance = f.pop_instance();
	bigint_t*			bigint = pop_bigint( f );
			
	CALL_STREAM_READ_CHUNK

	string_t*			text;

	text = INSTANCE_SELF(
		string_t, (u08*)f.pop_block( STRING_CLASS_SIZE ) );
	CHECK_TEXT( text );
	
	bigint->set_string( text->text, text->length );
	string_dispose( m, text );
	
	f.push_ptr( instance );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_input_stream_operators(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "#>>;InputStream::&;c&",
		istream_char, native_func_can_block );

	register_native_function(
		m, rootclss, "#>>;InputStream::&;i&",
		istream_int, native_func_can_block );

	register_native_function(
		m, rootclss, "#>>;InputStream::&;l&",
		istream_long, native_func_can_block );

	register_native_function(
		m, rootclss, "#>>;InputStream::&;f&",
		istream_float, native_func_can_block );

	register_native_function(
		m, rootclss, "#>>;InputStream::&;d&",
		istream_double, native_func_can_block );

	register_native_function(
		m, rootclss, "#>>;InputStream::&;String::&",
		istream_string, native_func_can_block );

	register_native_function(
		m, rootclss, "#>>;InputStream::&;BigInt::&",
		istream_bigint, native_func_can_block );
}

END_MACHINE_NAMESPACE
