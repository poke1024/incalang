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
//	lib_file.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "file_stream_t.h"
#include "string_t.h"
#include "lib_string.h"
#include "machine_class_t.h"
#include "file_t.h"
#include "memory_buffer_t.h"

BEGIN_MACHINE_NAMESPACE

struct file_wrap_t {
	random_stream_t*	stream;
};

static void
close_file(
	file_wrap_t*		wrap )
{
	if( wrap->stream )
	{
		delete wrap->stream;
		wrap->stream = 0;
	}
}

#define FILE_FUNCTION( name )												\
	STDLIB_CLASS_FUNCTION( name, file_wrap_t )								\
	random_stream_t* stream;												\
	if( ( stream = self->stream ) == 0 )									\
		f.fatal_error( ERR_FILE_NOT_OPEN );

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( file_create, file_wrap_t )
	f;
	self->stream = 0;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( file_destroy, file_wrap_t )
	f;
	close_file( self );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( mb_create, file_wrap_t )
	f;
	self->stream = new memory_buffer_t;
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_CLASS_FUNCTION( file_open_for_reading, file_wrap_t )
	close_file( self );

	string_t*			string = pop_string( f );
	istring				path;
	machine_t&			m = *f.machine();
	
	path.assign( string->text, string->length );
	path = m.file_path( path.c_str() );

	self->stream = new file_t(
		f.m_lib_context, path.c_str(), "rb" );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( file_open_for_writing, file_wrap_t )
	close_file( self );

	string_t*			string = pop_string( f );
	istring				path;
	machine_t&			m = *f.machine();
	
	path.assign( string->text, string->length );
	path = m.file_path( path.c_str() );

	self->stream = new file_t(
		f.m_lib_context, path.c_str(), "wb" );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( file_open_for_updating, file_wrap_t )
// TODO
	close_file( self );

	string_t*			string = pop_string( f );
	istring				path;
	machine_t&			m = *f.machine();
 	
	path.assign( string->text, string->length );
	path = m.file_path( path.c_str() );

	self->stream = new file_t(
		f.m_lib_context, path.c_str(), "wb+" );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

FILE_FUNCTION( file_close )
	close_file( self );
END_STDLIB_FUNCTION

FILE_FUNCTION( file_flush )
	stream->flush();
END_STDLIB_FUNCTION

FILE_FUNCTION( file_size )
	f.push_long( stream->length() );
END_STDLIB_FUNCTION

FILE_FUNCTION( file_tell )
	f.push_long( stream->tell() );
END_STDLIB_FUNCTION

FILE_FUNCTION( file_seek_absolute )
	s64					offset = f.pop_long();
	stream->seek( offset, seek_mode_begin );
END_STDLIB_FUNCTION

FILE_FUNCTION( file_seek_relative )
	s64					offset = f.pop_long();
	stream->seek( offset, seek_mode_relative );
END_STDLIB_FUNCTION

FILE_FUNCTION( file_seek_from_end )
	s64					offset = f.pop_long();
	stream->seek( offset, seek_mode_end );
END_STDLIB_FUNCTION

FILE_FUNCTION( file_write_some_bytes )
	const u08*			buffer = (const u08*)f.pop_ptr();
	long				count = f.pop_int();

	f.push_int( stream->write_some_bytes( buffer, count ) );
END_STDLIB_FUNCTION

FILE_FUNCTION( file_write_bytes )
	const u08*			buffer = (const u08*)f.pop_ptr();
	long				count = f.pop_int();

	if( stream->write_some_bytes( buffer, count ) != count )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

FILE_FUNCTION( file_read_some_bytes )
	u08*				buffer = (u08*)f.pop_ptr();
	long				count = f.pop_int();

	f.push_int( stream->read_some_bytes( buffer, count ) );
END_STDLIB_FUNCTION

FILE_FUNCTION( file_read_bytes )
	u08*				buffer = (u08*)f.pop_ptr();
	long				count = f.pop_int();

	if( stream->read_some_bytes( buffer, count ) != count )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

/*
class File extends InputStream, OutputStream {
public:
	native void create();
	native void destroy();
	
	native void openForReading( const String& name );
	native void openForWriting( const String& name );
	native void openForUpdating( const String& name );
	native void close();
	native void flush();
	native long size();
	native long tell();
	native void seekAbsolute( long );
	native void seekRelative( long );
	native void seekFromEnd( long );

	native void writeBytes( const byte* a, int count );
	native int writeSomeBytes( const byte* a, int count );
	native void readBytes( byte* a, int count );
	native int readSomeBytes( byte* a, int count );
	
private:
	void* data;
}
*/

static void
init_file(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	class_t*			clss;

	clss = rootclss->find_class( "File", 4 );
	if( !clss )
		throw_icarus_error( "File class not found" );

	register_native_function(
		m, clss, "create", file_create );

	register_native_function(
		m, clss, "destroy", file_destroy );

	register_native_function(
		m, clss, "openForReading;.String::&", file_open_for_reading );

	register_native_function(
		m, clss, "openForWriting;.String::&", file_open_for_writing );

	register_native_function(
		m, clss, "openForUpdating;.String::&", file_open_for_updating );

	register_native_function(
		m, clss, "close", file_close );

	register_native_function(
		m, clss, "flush", file_flush );

	register_native_function(
		m, clss, "size", file_size );

	register_native_function(
		m, clss, "tell", file_tell );

	register_native_function(
		m, clss, "seekAbsolute;l", file_seek_absolute );

	register_native_function(
		m, clss, "seekRelative;l", file_seek_relative );

	register_native_function(
		m, clss, "seekFromEnd;l", file_seek_from_end );

	register_native_function(
		m, clss, "writeBytes;.B*;i", file_write_bytes );

	register_native_function(
		m, clss, "writeSomeBytes;.B*;i", file_write_some_bytes );

	register_native_function(
		m, clss, "readBytes;B*;i", file_read_bytes );

	register_native_function(
		m, clss, "readSomeBytes;B*;i", file_read_some_bytes );
}

// ---------------------------------------------------------------------------

/*
class MemoryBuffer extends InputStream, OutputStream {
public:
	native void create();
	native void destroy();
	
	native void flush();
	native long size();
	native long tell();
	native void seekAbsolute( long );
	native void seekRelative( long );
	native void seekFromEnd( long );

	native void writeBytes( const byte* a, int count );
	native int writeSomeBytes( const byte* a, int count );
	native void readBytes( byte* a, int count );
	native int readSomeBytes( byte* a, int count );
	
private:
	void* data;
}
*/

static void
init_memory_buffer(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	class_t*			clss;

	clss = rootclss->find_class( "MemoryBuffer", 12 );
	if( !clss )
		throw_icarus_error( "MemoryBuffer class not found" );

	register_native_function(
		m, clss, "create", mb_create );

	register_native_function(
		m, clss, "destroy", file_destroy );

	register_native_function(
		m, clss, "flush", file_flush );

	register_native_function(
		m, clss, "size", file_size );

	register_native_function(
		m, clss, "tell", file_tell );

	register_native_function(
		m, clss, "seekAbsolute;l", file_seek_absolute );

	register_native_function(
		m, clss, "seekRelative;l", file_seek_relative );

	register_native_function(
		m, clss, "seekFromEnd;l", file_seek_from_end );

	register_native_function(
		m, clss, "writeBytes;.B*;i", file_write_bytes );

	register_native_function(
		m, clss, "writeSomeBytes;.B*;i", file_write_some_bytes );

	register_native_function(
		m, clss, "readBytes;B*;i", file_read_bytes );

	register_native_function(
		m, clss, "readSomeBytes;B*;i", file_read_some_bytes );
}

// ---------------------------------------------------------------------------

void
init_native_streams(
	machine_t&			m )
{
	init_file( m );
	init_memory_buffer( m );
}

END_MACHINE_NAMESPACE

