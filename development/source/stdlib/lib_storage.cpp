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
//	lib_storage.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "os_core.h"
#include "machine_class_t.h"
#include "lib_utils.h"
#include "lib_string.h"
#include "lib_array.h"

#include <stdio.h>
#include <stdlib.h>

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

static void
array_expand(
	machine_t*			m,
	array_t*			array,
	int					size )
{
	memory_t*			memory = m->get_memory();

	u32					alloc;
	void*				data;

	alloc = array->alloc * 2;
	data = memory->alloc(
		alloc * size );
	memcpy( data, array->data,
		array->count * size );
	memory->free( array->data );
	array->data = data;
	array->alloc = alloc;
}

// ---------------------------------------------------------------------------

#define INIT_ALLOC		4
#define ARRAY_FUNCTION( funcname )\
	STDLIB_CLASS_FUNCTION( FUNC_NAME( funcname ), array_t )

// ---------------------------------------------------------------------------

#define ELEM_TYPE		u64
#define ELEM_SIZE		sizeof(ELEM_TYPE)
#define ELEM_POP		pop_long
#define ELEM_PUSH		push_long
#if __VISC__
#define ELEM_FORMAT		"%I64d"
#else
#define ELEM_FORMAT		"%lld"
#endif
#define FUNC_NAME(s)	long_array_##s

#define ELEM_PRINT(buf,n,val)\
	isnprintf( buf, n, ELEM_FORMAT, val );

#include "lib_storage_array.i"

#undef ELEM_TYPE
#undef ELEM_SIZE
#undef ELEM_POP
#undef ELEM_PUSH
#undef ELEM_FORMAT
#undef FUNC_NAME

const char*				s_long_array_names[] = {
	"LongArray",
	"append;l",
	"insert;i;l",
	"search;l;i",
	"bsearch;l",
	"binsidx;l",
	"#[];.LongArray::&;i",
	"#<<;OutputStream::&;.LongArray::&",
	"sort;l*;i;i*" };

// ---------------------------------------------------------------------------

#define ELEM_TYPE		u32
#define ELEM_SIZE		sizeof(ELEM_TYPE)
#define ELEM_POP		pop_int
#define ELEM_PUSH		push_int
#define ELEM_FORMAT		"%ld"
#define FUNC_NAME(s)	int_array_##s

#define ELEM_PRINT(buf,n,val)\
	isnprintf( buf, n, ELEM_FORMAT, val );

#include "lib_storage_array.i"

#undef ELEM_TYPE
#undef ELEM_SIZE
#undef ELEM_POP
#undef ELEM_PUSH
#undef ELEM_FORMAT
#undef FUNC_NAME

const char*				s_int_array_names[] = {
	"IntArray",
	"append;i",
	"insert;i;i",
	"search;i;i",
	"bsearch;i",
	"binsidx;i",
	"#[];.IntArray::&;i",
	"#<<;OutputStream::&;.IntArray::&",
	"sort;i*;i;i*" };

// ---------------------------------------------------------------------------

#define ELEM_TYPE		u08
#define ELEM_SIZE		sizeof(ELEM_TYPE)
#define ELEM_POP		pop_byte
#define ELEM_PUSH		push_byte
#define FUNC_NAME(s)	byte_array_##s

#define ELEM_PRINT(buf,n,val)\
	isnprintf( buf, n, "%d", (int)val );

#include "lib_storage_array.i"

#undef ELEM_TYPE
#undef ELEM_SIZE
#undef ELEM_POP
#undef ELEM_PUSH
#undef ELEM_FORMAT
#undef FUNC_NAME

const char*				s_byte_array_names[] = {
	"ByteArray",
	"append;B",
	"insert;i;B",
	"search;B;i",
	"bsearch;B",
	"binsidx;B",
	"#[];.ByteArray::&;i",
	"#<<;OutputStream::&;.ByteArray::&",
	"sort;B*;i;i*" };

// ---------------------------------------------------------------------------

#define ELEM_TYPE		void*
#define ELEM_SIZE		sizeof(ELEM_TYPE)
#define ELEM_POP		pop_ptr
#define ELEM_PUSH		push_ptr
#define FUNC_NAME(s)	pointer_array_##s

#define ELEM_PRINT(buf,n,val)\
	print_pointer( buf, n, val );

#include "lib_storage_array.i"

#undef ELEM_TYPE
#undef ELEM_SIZE
#undef ELEM_POP
#undef ELEM_PUSH
#undef ELEM_FORMAT
#undef FUNC_NAME

const char*				s_pointer_array_names[] = {
	"PointerArray",
	"append;v*",
	"insert;i;v*",
	"search;v*;i",
	"bsearch;v*",
	"binsidx;v*",
	"#[];.PointerArray::&;i",
	"#<<;OutputStream::&;.PointerArray::&",
	"sort;v**;i;i*" };

// ---------------------------------------------------------------------------

#define ELEM_TYPE		double
#define ELEM_SIZE		sizeof(ELEM_TYPE)
#define ELEM_POP		pop_double
#define ELEM_PUSH		push_double
#define ELEM_FORMAT		"%lf"
#define FUNC_NAME(s)	double_array_##s

#define ELEM_PRINT(buf,n,val)\
	isnprintf( buf, n, ELEM_FORMAT, val );

#include "lib_storage_array.i"

#undef ELEM_TYPE
#undef ELEM_SIZE
#undef ELEM_POP
#undef ELEM_PUSH
#undef ELEM_FORMAT
#undef FUNC_NAME

const char*				s_double_array_names[] = {
	"DoubleArray",
	"append;d",
	"insert;i;d",
	"search;d;i",
	"bsearch;d",
	"binsidx;d",
	"#[];.DoubleArray::&;i",
	"#<<;OutputStream::&;.DoubleArray::&",
	"sort;d*;i;i*" };

// ---------------------------------------------------------------------------

#define ELEM_TYPE		float
#define ELEM_SIZE		sizeof(ELEM_TYPE)
#define ELEM_POP		pop_float
#define ELEM_PUSH		push_float
#define ELEM_FORMAT		"%f"
#define ELEM_PTYPE		ELEM_TYPE
#define FUNC_NAME(s)	float_array_##s

#include "lib_storage_array.i"

#undef ELEM_TYPE
#undef ELEM_SIZE
#undef ELEM_POP
#undef ELEM_PUSH
#undef ELEM_FORMAT
#undef FUNC_NAME

/*
class FloatArray {
public:
	native void create();
	native void destroy();
	native void append( float );
	native void remove( int, int c = 1 );
	native int count();
	native void insert( int, float );
	native int search( float, int );
	native int bsearch( float );
	native void binsidx( float );
private:
	void* data;
	int count;
	int alloc;
}

native float operator[]( const FloatArray&, int );
native FloatArray& operator<<( OutputStream& x, const FloatArray& );
native void sort( float*, int, int* itab = null );
*/

const char*				s_float_array_names[] = {
	"FloatArray",
	"append;f",
	"insert;i;f",
	"search;f;i",
	"bsearch;f",
	"binsidx;f",
	"#[];.FloatArray::&;i",
	"#<<;OutputStream::&;.FloatArray::&",
	"sort;f*;i;i*" };

// ---------------------------------------------------------------------------

#include "lib_string_array.i"

// ---------------------------------------------------------------------------

void
init_storage(
	machine_t&			m )
{
	long_array_setup( m, s_long_array_names );
	int_array_setup( m, s_int_array_names );
	byte_array_setup( m, s_byte_array_names );
	pointer_array_setup( m, s_pointer_array_names );
	double_array_setup( m, s_double_array_names );
	float_array_setup( m, s_float_array_names );
	string_array_setup( m );
}

END_MACHINE_NAMESPACE
