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
//	lib_zlib.cpp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "file_stream_t.h"
#include "string_t.h"
#include "lib_string.h"
#include "machine_class_t.h"
#include "deflater_stream_t.h"
#include "inflater_stream_t.h"

BEGIN_MACHINE_NAMESPACE

class isw_t : public input_stream_t {
public:
						isw_t(
							machine_t&			m,
							instance_t*			instance );

	virtual fpos_t		read_some_bytes(
							void*				buffer,
							fpos_t				count );

	void				set_fiber(
							fiber_t*			fiber );

private:
	instance_t*			m_instance;
	func_t*				m_func;
	fiber_t*			m_fiber;
};

isw_t::isw_t(
	machine_t&			m,
	instance_t*			instance )
{
	func_id_t			id;
	
	id = m.find_or_create_func_id( "readSomeBytes;B*;i", 18 );

	class_header_t*		header;	
	class_t*			clss;

	header = CLASS_HEADER( instance );
	clss = header->clss;

	unsigned long		offset = 0;

	m_func = clss->find_func_by_id( id, &offset );

	m_instance = offset_instance(
		instance, offset - header->offset );

	m_fiber = 0;
}

fpos_t
isw_t::read_some_bytes(
	void*				buffer,
	fpos_t				count )
{
	if( m_func )
	{
		fiber_t&		f = *m_fiber;

		f.push_int( count );
		f.push_ptr( buffer );
		f.call( m_instance, m_func );
		return f.pop_int();
	}

	return 0;
}

inline void
isw_t::set_fiber(
	fiber_t*			fiber )
{
	m_fiber = fiber;
}

// ---------------------------------------------------------------------------

class osw_t : public output_stream_t {
public:
						osw_t(
							machine_t&			m,
							instance_t*			instance );

	virtual fpos_t		write_some_bytes(
							const void*			buffer,
							fpos_t				count );

	virtual void		flush();

	void				set_fiber(
							fiber_t*			fiber );

private:
	fiber_t*			m_fiber;

	instance_t*			m_write_instance;
	func_t*				m_write_func;

	instance_t*			m_flush_instance;
	func_t*				m_flush_func;
};

osw_t::osw_t(
	machine_t&			m,
	instance_t*			instance )
{
	func_id_t			id;
	class_header_t*		header;	
	class_t*			clss;

	header = CLASS_HEADER( instance );
	clss = header->clss;

	unsigned long		offset;

	id = m.find_or_create_func_id( "writeSomeBytes;.B*;i", 20 );
	offset = 0;
	m_write_func = clss->find_func_by_id( id, &offset );
	m_write_instance = offset_instance(
		instance, offset - header->offset );

	id = m.find_or_create_func_id( "flush", 5 );
	offset = 0;
	m_flush_func = clss->find_func_by_id( id, &offset );
	m_flush_instance = offset_instance(
		instance, offset - header->offset );

	m_fiber = 0;
}

fpos_t
osw_t::write_some_bytes(
	const void*			buffer,
	fpos_t				count )
{
	if( m_write_func )
	{
		fiber_t&		f = *m_fiber;

		f.push_int( count );
		f.push_ptr( (void*)buffer );
		f.call( m_write_instance, m_write_func );
		return f.pop_int();
	}

	return 0;
}

void
osw_t::flush()
{
	if( m_flush_func )
		m_fiber->call( m_flush_instance, m_flush_func );
}

inline void
osw_t::set_fiber(
	fiber_t*			fiber )
{
	m_fiber = fiber;
}

// ---------------------------------------------------------------------------

struct inflater_wrap_t {
	inflater_stream_t*	stream;
	isw_t*				isw;
};

static void
cleanup_inflater(
	inflater_wrap_t*	node )
{
	if( node->stream )
		delete node->stream;
	if( node->isw )
		delete node->isw;
	node->stream = 0;
	node->isw = 0;
}

#define INFLATER_FUNCTION( name )											\
	STDLIB_CLASS_FUNCTION( name, inflater_wrap_t )							\
	inflater_stream_t* stream;												\
	if( ( stream = self->stream ) == 0 )									\
		f.fatal_error( ERR_FILE_NOT_OPEN );									\
	self->isw->set_fiber( &f );

STDLIB_CLASS_FUNCTION( inflater_create, inflater_wrap_t )
	f;
	self->stream = 0;
	self->isw = 0;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( inflater_destroy, inflater_wrap_t )
	f;
	cleanup_inflater( self );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( inflater_attach, inflater_wrap_t )
	machine_t&			m = *f.machine();
	cleanup_inflater( self );
	instance_t*			instance = f.pop_instance();
	long				avail = f.pop_int();
	self->isw = new isw_t( m, instance );
	self->stream = new inflater_stream_t( *self->isw, avail );
END_STDLIB_FUNCTION

INFLATER_FUNCTION( inflater_read_some_bytes )
	u08*				buffer = (u08*)f.pop_ptr();
	long				count = f.pop_int();

	f.push_int( stream->read_some_bytes( buffer, count ) );
END_STDLIB_FUNCTION

INFLATER_FUNCTION( inflater_read_bytes )
	u08*				buffer = (u08*)f.pop_ptr();
	long				count = f.pop_int();

	if( stream->read_some_bytes( buffer, count ) != count )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

/*
class InflaterStream extends InputStream {
public:
	native void create();
	native void destroy();
	
	native void attach( InputStream*, int );

	native void readBytes( byte* a, int count );
	native int readSomeBytes( byte* a, int count );
	
private:
	void* data;
}
*/

static void
init_inflater_stream(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();
	class_t*			clss;

	clss = rootclss->find_class( "InflaterStream", 14 );
	if( !clss )
		throw_icarus_error( "InflaterStream class not found" );

	register_native_function(
		m, clss, "create", inflater_create );

	register_native_function(
		m, clss, "destroy", inflater_destroy );

	register_native_function(
		m, clss, "attach;InputStream::*;i", inflater_attach );

	register_native_function(
		m, clss, "readBytes;B*;i", inflater_read_bytes );

	register_native_function(
		m, clss, "readSomeBytes;B*;i", inflater_read_some_bytes );
}

// ---------------------------------------------------------------------------

struct deflater_wrap_t {
	deflater_stream_t*	stream;
	osw_t*				osw;
};

static void
cleanup_deflater(
	deflater_wrap_t*	node )
{
	if( node->stream )
		delete node->stream;
	if( node->osw )
		delete node->osw;
	node->stream = 0;
	node->osw = 0;
}

#define DEFLATER_FUNCTION( name )											\
	STDLIB_CLASS_FUNCTION( name, deflater_wrap_t )							\
	deflater_stream_t* stream;												\
	if( ( stream = self->stream ) == 0 )									\
		f.fatal_error( ERR_FILE_NOT_OPEN );									\
	self->osw->set_fiber( &f );

STDLIB_CLASS_FUNCTION( deflater_create, deflater_wrap_t )
	f;
	self->stream = 0;
	self->osw = 0;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( deflater_destroy, deflater_wrap_t )
	f;
	cleanup_deflater( self );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( deflater_attach, deflater_wrap_t )
	machine_t&			m = *f.machine();
	cleanup_deflater( self );
	instance_t*			instance = f.pop_instance();
	int					level = f.pop_int();
	self->osw = new osw_t( m, instance );
	self->stream = new deflater_stream_t( *self->osw, level );
END_STDLIB_FUNCTION

DEFLATER_FUNCTION( deflater_flush )
	stream->flush();
END_STDLIB_FUNCTION

DEFLATER_FUNCTION( deflater_write_some_bytes )
	const u08*			buffer = (const u08*)f.pop_ptr();
	long				count = f.pop_int();

	f.push_int( stream->write_some_bytes( buffer, count ) );
END_STDLIB_FUNCTION

DEFLATER_FUNCTION( deflater_write_bytes )
	const u08*			buffer = (const u08*)f.pop_ptr();
	long				count = f.pop_int();

	if( stream->write_some_bytes( buffer, count ) != count )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

/*
class DeflaterStream extends OutputStream {
public:
	native void create();
	native void destroy();
	
	native void attach( OutputStream*, int );
	native void flush();

	native void writeBytes( const byte* a, int count );
	native int writeSomeBytes( const byte* a, int count );
	
private:
	void* data;
}
*/

static void
init_deflater_stream(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();
	class_t*			clss;

	clss = rootclss->find_class( "DeflaterStream", 14 );
	if( !clss )
		throw_icarus_error( "DeflaterStream class not found" );

	register_native_function(
		m, clss, "create", deflater_create );

	register_native_function(
		m, clss, "destroy", deflater_destroy );

	register_native_function(
		m, clss, "attach;OutputStream::*;i", deflater_attach );

	register_native_function(
		m, clss, "flush", deflater_flush );

	register_native_function(
		m, clss, "writeBytes;.B*;i", deflater_write_bytes );

	register_native_function(
		m, clss, "writeSomeBytes;.B*;i", deflater_write_some_bytes );
}

void
init_zlib(
	machine_t&			m )
{
	init_inflater_stream( m );
	init_deflater_stream( m );
}

END_MACHINE_NAMESPACE
