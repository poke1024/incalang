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
//	lib_storage_array.i		   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

ARRAY_FUNCTION( create )
	machine_t&			m = *f.machine();
	memory_t*			memory = m.get_memory();
	self->count = 0;
	self->alloc = INIT_ALLOC;
	self->data = memory->alloc( INIT_ALLOC * ELEM_SIZE );
END_STDLIB_FUNCTION

ARRAY_FUNCTION( destroy )
	machine_t&			m = *f.machine();
	memory_t*			memory = m.get_memory();
	memory->free( self->data );
END_STDLIB_FUNCTION

ARRAY_FUNCTION( append )
	u32					count = self->count;
	if( count == self->alloc )
		array_expand( f.machine(), self, ELEM_SIZE );
	( (ELEM_TYPE*)self->data )[ count ] = f.ELEM_POP();
	self->count = count + 1;
END_STDLIB_FUNCTION

ARRAY_FUNCTION( insert )
	u32					index = f.pop_int();
	u32					count = self->count;
	if( index > count )
		f.fatal_error( ERR_ILLEGAL_ARRAY_INDEX );
	if( count == self->alloc )
		array_expand( f.machine(), self, ELEM_SIZE );
	ELEM_TYPE*			p;
	p =	&( (ELEM_TYPE*)self->data )[ index ];
	memmove( p + 1, p,
		( count - index ) * ELEM_SIZE );
	( (ELEM_TYPE*)self->data )[ index ] = f.ELEM_POP();
	self->count = count + 1;
END_STDLIB_FUNCTION

ARRAY_FUNCTION( remove )
	u32					index = f.pop_int();
	u32					amount = f.pop_int();
	u32					count = self->count;
	if( index > count || index + amount > count )
		f.fatal_error( ERR_ILLEGAL_ARRAY_INDEX );
	ELEM_TYPE*			p;
	p =	&( (ELEM_TYPE*)self->data )[ index ];
	memmove( p, p + amount,
		( count - ( index + amount ) ) * ELEM_SIZE );
	self->count = count - amount;
END_STDLIB_FUNCTION

ARRAY_FUNCTION( count )
	f.push_int( self->count );
END_STDLIB_FUNCTION

ARRAY_FUNCTION( search )
	ELEM_TYPE			what = f.ELEM_POP();
	u32					index = f.pop_int();
	u32					count = self->count;
	const ELEM_TYPE*	q = (const ELEM_TYPE*)self->data;
	const ELEM_TYPE*	p = p;
	s32					found = -1;

	if( index > count )
		f.fatal_error( ERR_ILLEGAL_ARRAY_INDEX );
	count -= index;
	p += index;

	if( count )
	{
		do {
			if( *p++ == what )
			{
				found = (u32)( p - q - 1 );
				break;
			}
		} while( --count );
	}

	f.push_int( found );
END_STDLIB_FUNCTION

ARRAY_FUNCTION( bsearch )
	ELEM_TYPE			key = f.ELEM_POP();
	ELEM_TYPE			val;
	
	u32					count = self->count;
	const ELEM_TYPE*	array = (const ELEM_TYPE*)self->data;

	s32					left = 0;
	s32					right = count - 1;
	s32					current;
	s32					found = -1;

	while( left <= right )
	{
		current = ( left + right ) >> 1;
		val = (ELEM_TYPE)array[ current ];
		if( val == key )
		{
			found = current;
			break;
		}
		else if( val > key )
			right = current - 1;
		else
			left = current + 1;
	}

	f.push_int( found );	
END_STDLIB_FUNCTION

ARRAY_FUNCTION( binsidx )
	ELEM_TYPE			key = f.ELEM_POP();
	ELEM_TYPE			val;
	
	u32					count = self->count;
	const ELEM_TYPE*	array = (const ELEM_TYPE*)self->data;

	s32					left = 0;
	s32					right = count - 1;
	s32					current = 0;

	while( left <= right )
	{
retry:
		current = ( left + right ) >> 1;
		val = (ELEM_TYPE)array[ current ];
		if( val == key )
			break;
		else if( val > key )
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
		f.fatal_error( ERR_ILLEGAL_ARRAY_INDEX );

	f.push_ptr( &( (ELEM_TYPE*)a->data )[ index ] );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( FUNC_NAME( print ) )
	instance_t*			stream = f.pop_instance();
	array_t*			array = pop_array( f, ELEM_SIZE );
	ostream_write_chunk( f, stream, "[", 1 );	
	const long			count = array->count;
	ELEM_TYPE*			data = (ELEM_TYPE*)array->data;

{
	st_stack_chars		space( f, 32 + 1 + 1 );
	char*				buf = space();
	
	for( long i = 0; i < count; i++ )
	{
		ELEM_PRINT( buf, 32, data[ i ] );
		if( i < count - 1 )
			strcat( buf, "," );
		ostream_write_chunk( f, stream, buf, strlen( buf ) );
	}
	ostream_write_chunk( f, stream, "]", 1 );	
}

	f.push_ptr( stream );
END_STDLIB_FUNCTION

// ===========================================================================

STDLIB_FUNCTION( FUNC_NAME( sort ) )
	ELEM_TYPE*			array = (ELEM_TYPE*)f.pop_ptr();
	long				count = f.pop_int();
	long*				index_table = (long*)f.pop_ptr();

	for( long current = 1; current < count; current++ )
	{
		long			parent = current >> 1;

		//  move last element up in heap till in correct place
		while( current > 0 )
		{
			//  compare current and parent
			ELEM_TYPE*	p = &array[ parent ];
			ELEM_TYPE*	c = &array[ current ];

			ELEM_TYPE	a = *p;
			ELEM_TYPE	b = *c;

			if( a >= b )
				break;
			else
			{			
				*p = b;
				*c = a;

				if( index_table )
					iswap( index_table[ parent ],
						index_table[ current ] );
		
				current = parent;
				parent = parent >> 1;
			}
		}
	}

	for( long bottom = count - 2; bottom >= 0; bottom-- )
	{
		ELEM_TYPE		top_value = array[ 0 ];
		ELEM_TYPE*		bottom_ptr = &array[ bottom + 1 ];
		ELEM_TYPE		bottom_value = *bottom_ptr;

		array[ 0 ] = bottom_value;
		*bottom_ptr = top_value;

		if( index_table )
			iswap( index_table[ 0 ],
				index_table[ bottom + 1 ] );

		long			root2;
		long			max_child;
		long			root = 0;
		
		root2 = root << 1;

		// process until the root value is in its correct place
		while( root2 <= bottom )
		{
			// calculate index of the child with the larger value		
			if( root2 == bottom )
				// only one child 
				max_child = root2;
			else
			{
				// select the greater of the 2 children
				if( array[ root2 ] >			// left child 
					array[ root2 + 1 ] )		// right child
					max_child = root2;
				else
					max_child = root2 + 1;
			}
	
			// if heap property violated, swap values
		
			ELEM_TYPE*	r = &array[ root ];
			ELEM_TYPE*	c = &array[ max_child ];

			ELEM_TYPE	a = *r;
			ELEM_TYPE	b = *c;

			if( a < b )
			{
				*r = b;
				*c = a;

				if( index_table )
					iswap( index_table[ root ],
						index_table[ max_child ] );
			
				root = max_child;
				root2 = root << 1;
			}
			else
				break;
		}
	}
END_STDLIB_FUNCTION

// ===========================================================================

static void
FUNC_NAME(setup)(
	machine_t&			m,
	const char**		names )
{
	class_t*			rootclss = m.get_root_class();

	class_t*			clss;

	clss = rootclss->find_class(
		names[ 0 ], strlen( names[ 0 ] ) );
	if( !clss )
		throw_icarus_error( "Array class not found" );

	register_native_function( m, clss,
		"create", FUNC_NAME( create ) );

	register_native_function( m, clss,
		"destroy", FUNC_NAME( destroy ) );

	register_native_function( m, clss,
		names[ 1 ], FUNC_NAME( append ) );

	register_native_function( m, clss,
		names[ 2 ], FUNC_NAME( insert ) );

	register_native_function( m, clss,
		"remove;i;i", FUNC_NAME( remove ) );

	register_native_function( m, clss,
		"count", FUNC_NAME( count ) );

	register_native_function( m, clss,
		names[ 3 ], FUNC_NAME( search ) );

	register_native_function( m, clss,
		names[ 4 ], FUNC_NAME( bsearch ) );

	register_native_function( m, clss,
		names[ 5 ], FUNC_NAME( binsidx ) );

	register_native_function(
		m, rootclss, names[ 6 ],
		FUNC_NAME( access ) );

	register_native_function(
		m, rootclss, names[ 7 ],
		FUNC_NAME( print ) );

	register_native_function(
		m, rootclss, names[ 8 ],
		FUNC_NAME( sort ) );
}

