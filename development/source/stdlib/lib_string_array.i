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
//	lib_string_array.i		   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#define ELEM_SIZE		STRING_CLASS_SIZE
#define FUNC_NAME(s)	string_array_##s

inline string_t*
nth_string(
	void*				data,
	u32					n )
{
	u08*				p = (u08*)data;
	p += STRING_CLASS_SIZE * n;
	return INSTANCE_SELF( string_t, p );
}

inline string_t*
next_string(
	string_t*			s )
{
	u08*				p = (u08*)s;
	p += STRING_CLASS_SIZE;
	return (string_t*)p;
}

static void
init_string_array(
	machine_t&			m,
	u08*				block,
	long				count )
{
	for( long i = 0; i < count; i++ )
	{
		init_string( block, m );
		block += STRING_CLASS_SIZE;
	}
}

ARRAY_FUNCTION( create )
	machine_t&			m = *f.machine();
	memory_t*			memory = m.get_memory();
	self->count = 0;
	self->alloc = INIT_ALLOC;
	self->data = memory->alloc( INIT_ALLOC * ELEM_SIZE );
	init_string_array( m, (u08*)self->data, INIT_ALLOC );
END_STDLIB_FUNCTION

ARRAY_FUNCTION( destroy )
	machine_t&			m = *f.machine();
	memory_t*			memory = m.get_memory();
	u32					count = self->count;
	string_t*			node = nth_string( self->data, 0 );
	for( long i = 0; i < count; i++ )
	{
		string_dispose( m, node );
		node = next_string( node );
	}
	memory->free( self->data );
END_STDLIB_FUNCTION

ARRAY_FUNCTION( append )
	machine_t&			m = *f.machine();
	string_t*			param = pop_string( f );
	u32					count = self->count;
	if( count == self->alloc )
	{
		long			alloc = self->alloc;
		array_expand( &m, self, ELEM_SIZE );
		init_string_array( m,
			(u08*)self->data + alloc * ELEM_SIZE,
			self->alloc - alloc );
	}
	string_create( m,
		nth_string( self->data, count ),
		param->text, param->length );
	self->count = count + 1;
END_STDLIB_FUNCTION

ARRAY_FUNCTION( insert )
	machine_t&			m = *f.machine();
	u32					index = f.pop_int();
	string_t*			param = pop_string( f );
	u32					count = self->count;
	if( index > count )
		f.fatal_error( ERR_ILLEGAL_ARRAY_INDEX );
	if( count == self->alloc )
	{
		long			alloc = self->alloc;
		array_expand( &m, self, ELEM_SIZE );
		init_string_array( m,
			(u08*)self->data + alloc * ELEM_SIZE,
			self->alloc - alloc );
	}
	u08*				p;
	p = (u08*)self->data;
	p += index * ELEM_SIZE;
	memmove( p + ELEM_SIZE, p,
		( count - index ) * ELEM_SIZE );
	string_create( m,
		nth_string( self->data, index ),
		param->text, param->length );
	self->count = count + 1;
END_STDLIB_FUNCTION

ARRAY_FUNCTION( remove )
	machine_t&			m = *f.machine();
	u32					index = f.pop_int();
	u32					amount = f.pop_int();
	u32					count = self->count;
	if( index > count || index + amount > count )
		f.fatal_error( ERR_ILLEGAL_ARRAY_INDEX );
	u08*				p;
	p = (u08*)self->data;
	p += index * ELEM_SIZE;

	string_t*			node = INSTANCE_SELF( string_t, p );
	for( long i = 0; i < amount; i++ )
	{
		string_dispose( m, node );
		node = next_string( node );
	}

	memmove( p, p + amount * ELEM_SIZE,
		( count - ( index + amount ) ) * ELEM_SIZE );
	self->count = count - amount;
END_STDLIB_FUNCTION

ARRAY_FUNCTION( count )
	f.push_int( self->count );
END_STDLIB_FUNCTION

ARRAY_FUNCTION( search )
	machine_t&			m = *f.machine();
	string_t*			what = pop_string( f );
	u32					index = f.pop_int();
	u32					count = self->count;
	string_t*			p;
	s32					found = -1;

	if( index > count )
		f.fatal_error( ERR_ILLEGAL_ARRAY_INDEX );
	count -= index;
	p = nth_string( self->data, index );

	if( count )
	{
		do {
			if( string_compare( m, p, what ) == 0 )
			{
				found = index;
				break;
			}
			p = next_string( p );
			index++;
		} while( --count );
	}

	f.push_int( found );
END_STDLIB_FUNCTION

ARRAY_FUNCTION( bsearch )
	machine_t&			m = *f.machine();
	string_t*			key = pop_string( f );
	
	u32					count = self->count;
	void*				array = self->data;

	s32					left = 0;
	s32					right = count - 1;
	s32					current;
	s32					found = -1;
	int					comparison;

	while( left <= right )
	{
		current = ( left + right ) >> 1;
		comparison = string_compare( m,
			nth_string( array, current ), key );
		
		if( comparison == 0 )
		{
			found = current;
			break;
		}
		else if( comparison > 0 )
			right = current - 1;
		else
			left = current + 1;
	}

	f.push_int( found );	
END_STDLIB_FUNCTION

ARRAY_FUNCTION( binsidx )
	machine_t&			m = *f.machine();
	string_t*			key = pop_string( f );
	
	u32					count = self->count;
	void*				array = self->data;

	s32					left = 0;
	s32					right = count - 1;
	s32					current = 0;
	int					comparison;

	while( left <= right )
	{
retry:
		current = ( left + right ) >> 1;
		comparison = string_compare( m,
			nth_string( array, current ), key );

		if( comparison == 0 )
			break;
		else if( comparison > 0 )
			right = current - 1;
		else
		{
			if( ( left = current + 1 ) > right )
			{
				current += 1;
				break;
			}
			goto retry;
		}
	}

	f.push_int( current );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( FUNC_NAME( access ) )
	array_t*			a = pop_array( f, ELEM_SIZE );
	u32					index = f.pop_int();

	if( index >= a->count )
		f.fatal_error( ERR_ILLEGAL_STRING_INDEX );

	u08*				p;
	p = (u08*)a->data;
	p += ELEM_SIZE * index;
	f.push_ptr( p );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( FUNC_NAME( print ) )
	instance_t*			stream = f.pop_instance();
	array_t*			array = pop_array( f, ELEM_SIZE );
	ostream_write_chunk( f, stream, "[", 1 );	
	const long			count = array->count;
	string_t*			node = nth_string( array->data, 0 );
	
	for( long i = 0; i < count; i++ )
	{
		ostream_write_chunk( f, stream, "\"", 1 );
		ostream_write_chunk( f, stream, node->text, node->length );
		ostream_write_chunk( f, stream, "\"", 1 );
		if( i < count - 1 )
			ostream_write_chunk( f, stream, ",", 1 );
		node = next_string( node );
	}
	ostream_write_chunk( f, stream, "]", 1 );
	f.push_ptr( stream );
END_STDLIB_FUNCTION

// ===========================================================================

static void
FUNC_NAME(setup)(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	class_t*			clss;

	clss = rootclss->find_class(
		"StringArray", 11 );
	if( !clss )
		throw_icarus_error( "StringArray class not found" );

	register_native_function( m, clss,
		"create", FUNC_NAME( create ) );

	register_native_function( m, clss,
		"destroy", FUNC_NAME( destroy ) );

	register_native_function( m, clss,
		"append;.String::&", FUNC_NAME( append ) );

	register_native_function( m, clss,
		"insert;i;.String::&", FUNC_NAME( insert ) );

	register_native_function( m, clss,
		"remove;i;i", FUNC_NAME( remove ) );

	register_native_function( m, clss,
		"count", FUNC_NAME( count ) );

	register_native_function( m, clss,
		"search;.String::&;i", FUNC_NAME( search ) );

	register_native_function( m, clss,
		"bsearch;.String::&", FUNC_NAME( bsearch ) );

	register_native_function( m, clss,
		"binsidx;.String::&", FUNC_NAME( binsidx ) );

	register_native_function(
		m, rootclss, "#[];.StringArray::&;i",
		FUNC_NAME( access ) );

	register_native_function(
		m, rootclss, "#<<;OutputStream::&;.StringArray::&",
		FUNC_NAME( print ) );
}

