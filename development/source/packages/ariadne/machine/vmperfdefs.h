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

#if VM_PROJECT

#include "vmdefs.h"

// ---------------------------------------------------------------------------

#define MACHINE_RUNTIME_ERROR( error )										\
	{ VM_SAVE_FIBER_STATE													\
	VM_FIBER->m_state.resume = false;										\
	VM_FIBER->m_state.err = error;											\
	VM_MACHINE->broadcast_message( msg_MachineStopped, VM_FIBER );			\
	while( true ) { ithread_t::current()->block(); } }

#if CMA_MACHINE

#define CHECK_ACCESS( ptr, size )											\
	if( ( (u08*)ptr ) < memlo || ( (u08*)ptr ) + size > memhi )				\
		MACHINE_RUNTIME_ERROR( ERR_ILLEGAL_ACCESS );

#else

#define CHECK_ACCESS( ptr, size )

#endif

#if SAFE_MACHINE

#define CHECK_STACK_OVERFLOW( amount )										\
	if( sp + amount - VM_FIBER->m_stack_base > VM_FIBER->m_stack_size )		\
		MACHINE_RUNTIME_ERROR( ERR_STACK_OVERFLOW );
#define CHECK_INSTANCE( instance )											\
	CHECK_ACCESS( instance, sizeof( class_header_t ) );

#define VM_SAVE_FIBER_STATE													\
	VM_FIBER->m_state.instance = self;										\
	VM_FIBER->m_state.func = func;											\
	VM_FIBER->m_state.code = code;											\
	VM_FIBER->m_state.sp = sp;												\
	VM_FIBER->m_state.localsp = localsp;									\
	VM_FIBER->m_state.condflags = condflags;

#else

#define CHECK_STACK_OVERFLOW
#define CHECK_INSTANCE( instance )
#define VM_SAVE_FIBER_STATE

#endif

// ---------------------------------------------------------------------------

#define POP_UBYTE(x)\
	sp -= 4; x = *(u08*)sp;

#define POP_SBYTE(x)\
	sp -= 4; x = *(s08*)sp;

#define PUSH_BYTE(x)\
	CHECK_STACK_OVERFLOW( 4 ); *sp = (x); sp += 4;

#define POP_UWORD(x)\
	sp -= 4; x = *(u16*)sp;

#define POP_SWORD(x)\
	sp -= 4; x = *(s16*)sp;

#define PUSH_WORD(x)\
	CHECK_STACK_OVERFLOW( 4 ); *(u16*)sp = (x); sp += 4;

#define POP_UQUAD(x)\
	sp -= 4; x = *(u32*)sp;

#define POP_SQUAD(x)\
	sp -= 4; x = *(s32*)sp;

#define PUSH_QUAD(x)\
	CHECK_STACK_OVERFLOW( 4 ); *(u32*)sp = (x); sp += 4;

#define POP_UOCTA(x)\
	sp -= 8; x = *(u64*)sp;

#define POP_SOCTA(x)\
	sp -= 8; x = *(s64*)sp;

#define PUSH_OCTA(x)\
	CHECK_STACK_OVERFLOW( 8 ); *(u64*)sp = (x); sp += 8;
	
#define POP_FLOAT(x)\
	sp -= 4; x = *(float*)sp;

#define PUSH_FLOAT(x)\
	CHECK_STACK_OVERFLOW( 4 ); *(float*)sp = (x); sp += 4;

#define POP_DOUBLE(x)\
	sp -= 8; x = *(double*)sp;

#define PUSH_DOUBLE(x)\
	CHECK_STACK_OVERFLOW( 8 ); *(double*)sp = (x); sp += 8;
	
#define POP_PTR(x)\
	sp -= 4; x = *(void**)sp;
	
#define PUSH_PTR(x)\
	CHECK_STACK_OVERFLOW( 4 ); *(void**)sp = (x); sp += 4;

// ---------------------------------------------------------------------------

#define ARITH_UNABVTE(opcode,result)			\
	case (opcode) | OPS_BYTE: {					\
		int a;									\
		POP_UBYTE( a );							\
		PUSH_BYTE( result );					\
		break; }

#define ARITH_UNAWORD(opcode,result)			\
	case (opcode) | OPS_WORD: {					\
		int a;									\
		POP_UWORD( a );							\
		PUSH_WORD( result );					\
		break; }

#define ARITH_UNALONG(opcode,result)			\
	case (opcode) | OPS_QUAD: {					\
		long a;									\
		POP_UQUAD( a );							\
		PUSH_QUAD( result );					\
		break; }

#define ARITH_UNAWIDE(opcode,result)			\
	case (opcode) | OPS_OCTA: {					\
		s64 a;									\
		POP_UOCTA( a );							\
		PUSH_OCTA( result );					\
		break; }

#define ARITH_BINBVTE(opcode,result)			\
	case (opcode) | OPS_BYTE: {					\
		int a, b;								\
		POP_UBYTE( a );							\
		POP_UBYTE( b );							\
		PUSH_BYTE( result );					\
		break; }

#define ARITH_BINWORD(opcode,result)			\
	case (opcode) | OPS_WORD: {					\
		int a, b;								\
		POP_UWORD( a );							\
		POP_UWORD( b );							\
		PUSH_WORD( result );					\
		break; }

#define ARITH_BINLONG(opcode,result)			\
	case (opcode) | OPS_QUAD: {					\
		long a, b;								\
		POP_UQUAD( a );							\
		POP_UQUAD( b );							\
		PUSH_QUAD( result );					\
		break; }

#define ARITH_BINWIDE(opcode,result)			\
	case (opcode) | OPS_OCTA: {					\
		s64 a, b;								\
		POP_UOCTA( a );							\
		POP_UOCTA( b );							\
		PUSH_OCTA( result );					\
		break; }

#define ARITH_UNAINT(opcode,result)				\
	ARITH_UNABVTE(opcode,result)				\
	ARITH_UNAWORD(opcode,result)				\
	ARITH_UNALONG(opcode,result)				\
	ARITH_UNAWIDE(opcode,result)
		
#define ARITH_BININT(opcode,result)				\
	ARITH_BINBVTE(opcode,result)				\
	ARITH_BINWORD(opcode,result)				\
	ARITH_BINLONG(opcode,result)				\
	ARITH_BINWIDE(opcode,result)

#define ARITH_UNAFLOAT(opcode,result)			\
	case (opcode) | OPS_FLOAT: {				\
		float a;								\
		POP_FLOAT( a );							\
		PUSH_FLOAT( result );					\
		break; }

#define ARITH_UNADOUBLE(opcode,result)			\
	case (opcode) | OPS_DOUBLE: {				\
		double a;								\
		POP_DOUBLE( a );						\
		PUSH_DOUBLE( result );					\
		break; }

#define ARITH_BINFLOAT(opcode,result)			\
	case (opcode) | OPS_FLOAT: {				\
		float a, b;								\
		POP_FLOAT( a );							\
		POP_FLOAT( b );							\
		PUSH_FLOAT( result );					\
		break; }

#define ARITH_BINDOUBLE(opcode,result)			\
	case (opcode) | OPS_DOUBLE: {				\
		double a, b;							\
		POP_DOUBLE( a );						\
		POP_DOUBLE( b );						\
		PUSH_DOUBLE( result );					\
		break; }

#define ARITH_UNAFLT(opcode,result)				\
	ARITH_UNAFLOAT(opcode,result)				\
	ARITH_UNADOUBLE(opcode,result)

#define ARITH_BINFLT(opcode,result)				\
	ARITH_BINFLOAT(opcode,result)				\
	ARITH_BINDOUBLE(opcode,result)
	
// ---------------------------------------------------------------------------

#define ARITH_SSHIFT(opcode,result)									\
	case (opcode) | OPS_BYTE: {										\
		int a; int b; POP_SBYTE( a ); POP_UBYTE( b );				\
		PUSH_BYTE( result ); break; }								\
																	\
	case (opcode) | OPS_WORD: {										\
		int a; int b; POP_SWORD( a ); POP_UBYTE( b );				\
		PUSH_WORD( result ); break; }								\
																	\
	case (opcode) | OPS_QUAD: {										\
		long a; int b; POP_SQUAD( a ); POP_UBYTE( b );				\
		PUSH_QUAD( result ); break; }								\
																	\
	case (opcode) | OPS_OCTA: {										\
		s64 a; int b; POP_SOCTA( a ); POP_UBYTE( b );			\
		PUSH_OCTA( result ); break; }

#define ARITH_USHIFT(opcode,result)									\
	case (opcode) | OPS_BYTE: {										\
		unsigned int a; int b; POP_UBYTE( a ); POP_UBYTE( b );		\
		PUSH_BYTE( result ); break; }								\
																	\
	case (opcode) | OPS_WORD: {										\
		unsigned int a; int b; POP_UWORD( a ); POP_UBYTE( b );		\
		PUSH_WORD( result ); break; }								\
																	\
	case (opcode) | OPS_QUAD: {										\
		unsigned long a; int b; POP_UQUAD( a ); POP_UBYTE( b );		\
		PUSH_QUAD( result ); break; }								\
																	\
	case (opcode) | OPS_OCTA: {										\
		u64 a; int b; POP_UOCTA( a ); POP_UBYTE( b );\
		PUSH_OCTA( result ); break; }


#define UPDATE_COND_CMP												\
	condflags &= ~( COND_EQ | COND_GT );							\
	if( a == b ) condflags |= COND_EQ;								\
	else if( a > b ) condflags |= COND_GT;
	
#define RETURN_FROM_SUBROUTINE {									\
	sp -= retsize;													\
	u08* retsp = sp;												\
	sp -= func->m_locals_size;										\
																	\
	void* a;														\
	POP_PTR( a ); func = (bytecode_func_t*)a;						\
	POP_PTR( a ); self = (instance_t*)a;							\
	POP_PTR( a ); localsp = (u08*)a;								\
	POP_PTR( a ); code = (u08*)a;									\
																	\
	sp -= paramsize;												\
	memcpy( sp, retsp, retsize );									\
	sp += retsize;													\
																	\
	if( !code ) {													\
		VM_DONE														\
	} }
	
#define CALL_SUBROUTINE	{											\
	void* a;														\
	a = (void*)code; PUSH_PTR( a );									\
	a = (void*)localsp; PUSH_PTR( a );								\
	a = (void*)self; PUSH_PTR( a );									\
	a = (void*)func; PUSH_PTR( a );									\
																	\
	func = (bytecode_func_t*)newfunc;								\
	code = func->m_code_data;										\
	localsp = sp;													\
	self = instance;												\
																	\
	sp += func->m_locals_size; }

#define TAILCALL_SUBROUTINE {											\
	u08 buf[ 16 ]; memcpy( buf, localsp - 16, 16 );					\
																	\
	u08* param_sp = localsp + func->m_locals_size;					\
	int param_size = sp - param_sp;									\
	sp = localsp - 16 - tc_param_size;												\
	memmove( sp, param_sp, param_size );							\
	sp += param_size;												\
																	\
	memcpy( sp, buf, 16 );											\
	sp += 16;														\
																	\
	func = (bytecode_func_t*)newfunc;								\
	code = func->m_code_data;										\
	localsp = sp;													\
	self = instance;												\
																	\
	sp += func->m_locals_size; }
	
// ---------------------------------------------------------------------------

#define FETCH_SWORD(x)									\
	x = *code++; x = ( x << 8 ) | ( *code++ ); x = (s16)x;

#define FETCH_UWORD(x)									\
	x = *code++; x = ( x << 8 ) | ( *code++ ); x = (u16)x;

#define FETCH_INDEX(x)									\
	x = *code++; if( x == 0xff ) { 						\
	x = *code++; x = ( x << 8 ) | ( *code++ ); }		\
	x = ( x << 8 ) | ( *code++ );

#if YIELDED_MACHINE

#define BRANCH											\
	{ s16 offset; offset = code[ 0 ];					\
	offset = ( offset << 8 ) | code[ 1 ];				\
	code += offset; if( offset < 0 ) VM_YIELD }

#else

#define BRANCH											\
	{ int offset; offset = code[ 0 ];					\
	offset = ( offset << 8 ) | code[ 1 ];				\
	code += (s16)offset; }

#endif

inline void*
dyncast(
	fiber_t*			/*fiber*/,
	void*				instance,
	class_t*			clss )
{
	if( !instance )
		return 0;

	//CHECK_INSTANCE( instance );

	class_t*			rtclss;
	unsigned long		offset;
	class_header_t*		header;

	header = CLASS_HEADER( (instance_t*)instance );
	rtclss = header->clss;

	if( rtclss->get_base_class_offset( clss, &offset ) != FOUND )
		return 0;
		
	return ( (u08*)instance ) - header->offset + offset;
}

static void*
newinstance(
	fiber_t*			fiber,
	class_t*			clss,
	unsigned long		arraysize )
{
	instance_t*			instance;

	instance = clss->create_instance_array( fiber, arraysize );

	return instance;
}

static void*
newinstancedyn(
	fiber_t*			fiber,
	class_t*			clss,
	unsigned long		arraysize,
	const char*			clssname )
{
	instance_t*			instance;
	class_t*			rtclss;
	
	rtclss = fiber->machine()->get_root_class();

	while( true )
	{
		int				length = 0;
		char			s;
		
		while( true )
		{
			s = clssname[ length ];
			
			if( s == '\0' || s == ':' )
				break;
				
			length++;
		}
	
		rtclss = rtclss->find_class( clssname, length );
		if( !rtclss )
			return 0;
		
		if( s == '\0' )
			break;
		
		clssname += length + 1;
		if( *clssname != ':' )
			return 0;
		clssname++;
	}

	instance = rtclss->create_instance_array(
		fiber, arraysize );

	return dyncast( fiber, instance, clss );
}

static void
deleteinstance(
	fiber_t*			fiber,
	void*				instance )
{
	if( !instance )
		return;

	//CHECK_INSTANCE( instance );

	u08*				block;
	class_t*			clss;
	class_header_t*		header;
	
	header = CLASS_HEADER( (instance_t*)instance );
	block = ( (u08*)instance ) - header->offset;
	clss = header->clss;

	clss->destroy_instance(
		fiber, (instance_t*)block );
}

inline void*
newblock(
	memory_t*			memory,
	unsigned long		elemsize,
	unsigned long		arraysize )
{
	return memory->alloc( elemsize * arraysize );
}

inline void
deleteblock(
	memory_t*			memory,
	void*				block )
{
	if( !block )
		return;

	memory->free( (u08*)block );
}

// ---------------------------------------------------------------------------

#endif // VM_PROJECT
