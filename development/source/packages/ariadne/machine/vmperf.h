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

int				opcode = *code++;
	
process_opcode:

switch( opcode )
{
	ARITH_BININT( BYTECODE_ADD, a + b )
	ARITH_BININT( BYTECODE_SUB, a - b )
	ARITH_BININT( BYTECODE_MUL, a * b )
	ARITH_BININT( BYTECODE_DIV, a / b )
	ARITH_BININT( BYTECODE_MOD, a % b )
	ARITH_UNAINT( BYTECODE_NEG, -a )
	
	case BYTECODE_CMP | OPS_BYTE: {
		int a, b; POP_SBYTE( a ); POP_SBYTE( b );
		UPDATE_COND_CMP; break; }

	case BYTECODE_CMP | OPS_WORD: {
		int a, b; POP_SWORD( a ); POP_SWORD( b );
		UPDATE_COND_CMP; break; }

	case BYTECODE_CMP | OPS_QUAD: {
		long a, b; POP_SQUAD( a ); POP_SQUAD( b );
		UPDATE_COND_CMP; break; }

	case BYTECODE_CMP | OPS_OCTA: {
		s64 a, b; POP_SOCTA( a ); POP_SOCTA( b );
		UPDATE_COND_CMP; break; }
	
	case BYTECODE_CMPU | OPS_BYTE: {
		int a, b; POP_UBYTE( a ); POP_UBYTE( b );
		UPDATE_COND_CMP; break; }
		
	case BYTECODE_CMPU | OPS_WORD: {
		unsigned int a, b; POP_UWORD( a ); POP_UWORD( b );
		UPDATE_COND_CMP; break; }
	
	case BYTECODE_CMPU | OPS_QUAD: {
		unsigned long a, b; POP_UQUAD( a ); POP_UQUAD( b );
		UPDATE_COND_CMP; break; }

	case BYTECODE_CMPU | OPS_OCTA: {
		u64 a, b; POP_UOCTA( a ); POP_UOCTA( b );
		UPDATE_COND_CMP; break; }
	
	ARITH_BINFLT( BYTECODE_FADD, a + b )
	ARITH_BINFLT( BYTECODE_FSUB, a - b )
	ARITH_BINFLT( BYTECODE_FMUL, a * b )
	ARITH_BINFLT( BYTECODE_FDIV, a / b )
	ARITH_BINFLT( BYTECODE_FMOD, ifmod( a, b ) )
	ARITH_UNAFLT( BYTECODE_FNEG, -a );
	
	case BYTECODE_FCMP | OPS_BYTE: {
		float a, b; POP_FLOAT( a ); POP_FLOAT( b );
		UPDATE_COND_CMP; break; }

	case BYTECODE_FCMP | OPS_WORD: {
		double a, b; POP_DOUBLE( a ); POP_DOUBLE( b );
		UPDATE_COND_CMP; break; }			
	
	ARITH_UNAINT( BYTECODE_NOT, ~a )
	ARITH_BININT( BYTECODE_AND, a & b )
	ARITH_BININT( BYTECODE_OR, a | b )
	ARITH_BININT( BYTECODE_XOR, a ^ b )
	
	ARITH_USHIFT( BYTECODE_SHL, a << b )
	ARITH_USHIFT( BYTECODE_SHR, a >> b )
	ARITH_SSHIFT( BYTECODE_ASR, a >> b )

	case BYTECODE_BRA | OPS_WORD: {
		BRANCH; break; }

	case BYTECODE_BEQ | OPS_WORD: {
		if( condflags & COND_EQ ) { BRANCH; }
		else { code += 2; } break; }

	case BYTECODE_BNE | OPS_WORD: {
		if( !( condflags & COND_EQ ) ) { BRANCH; }
		else { code += 2; } break; }

	case BYTECODE_BGT | OPS_WORD: {
		if( condflags & COND_GT ) { BRANCH; }
		else { code += 2; } break; }

	case BYTECODE_BLT | OPS_WORD: {
		if( !( condflags & ( COND_EQ | COND_GT ) ) ) { BRANCH; }
		else { code += 2; } break; }
	
	case BYTECODE_BGE | OPS_WORD: {
		if( condflags & ( COND_EQ | COND_GT ) ) { BRANCH; }
		else { code += 2; } break; }

	case BYTECODE_BLE | OPS_WORD: {
		if( !( condflags & COND_GT ) ) { BRANCH; }
		else { code += 2; } break; }
		
	case BYTECODE_RTS | OPS_BYTE: {
		int retsize, paramsize; paramsize = *code++;
		retsize = *code++; RETURN_FROM_SUBROUTINE; break; }

	case BYTECODE_RTS | OPS_WORD: {
		unsigned int retsize, paramsize; FETCH_UWORD( paramsize );
		FETCH_UWORD( retsize ); RETURN_FROM_SUBROUTINE; break; }

	case BYTECODE_RTS | OPS_QUAD: {
		unsigned long offset; unsigned long retsize, paramsize;
		FETCH_INDEX( offset ); paramsize = func->m_quad_table[ offset ];
		FETCH_INDEX( offset ); retsize = func->m_quad_table[ offset ];
		RETURN_FROM_SUBROUTINE; break; }
		
	case BYTECODE_CSF | OPS_WORD: {
		unsigned long index; FETCH_UWORD( index );
		call_t* call = &func->m_call_table[ index ];
		index = call->funcid;
		class_t* clss = func->m_class_table[ call->clssindx ];
		unsigned long offset = 0;
		func_t* newfunc = clss->find_func_by_id( index, &offset );
		if( !newfunc )
			throw_icarus_error( "function not found (ME#2)" );

		VM_YIELD

		switch( newfunc->get_type() )
		{
			case func_type_native:
			{
				native_func_t* natfunc = (native_func_t*)newfunc;
				const int flags = natfunc->flags();
				VM_ENTER_NATIVE( flags );
				natfunc->call( *VM_FIBER, 0 );
				VM_LEAVE_NATIVE( flags );
				break;
			}

			case func_type_dll:
			{
				dll_func_t* dllfunc = (dll_func_t*)newfunc;
				const int flags = native_func_can_block;
				VM_ENTER_NATIVE( flags );
				dllfunc->call( *VM_FIBER );
				VM_LEAVE_NATIVE( flags );
				break;
			}

#if DEBUG_ABSTRACT_FUNCTIONS
			default:
			case func_type_abstract:
				throw_icarus_error( "invalid function called (ME#3)" );
#endif
				
			case func_type_bytecode:
			{
				instance_t* instance = 0;
				CALL_SUBROUTINE
				break;
			}
		}
		break;
	}
		
	case BYTECODE_CVF | OPS_WORD: {
		unsigned long index; FETCH_UWORD( index );
		index = func->m_call_table[ index ].funcid;
		void* a; POP_PTR( a ); instance_t* instance = (instance_t*)a;
		CHECK_INSTANCE( instance );
		class_header_t*	header = CLASS_HEADER( instance );
		class_t* clss = header->clss;
		unsigned long offset = 0;

		VM_YIELD

		func_t* newfunc = clss->find_func_by_id( index, &offset );
		if( !newfunc )
		{
#if ICARUS_SAFE && !__VISC__
			istring name;
			VM_MACHINE->func_id_to_name( index, name );
			throw_icarus_error(
				"function '%s' not found (ME#4)",
				 name.c_str() );
#endif

			throw_icarus_error( "function not found (ME#4)" );
		}
		
		instance = (instance_t*)( (u08*)instance - header->offset + offset );
	
		switch( newfunc->get_type() )
		{
			case func_type_native:
			{
				native_func_t* natfunc = (native_func_t*)newfunc;
				const int flags = natfunc->flags();
				VM_ENTER_NATIVE( flags );
				natfunc->call( *VM_FIBER, instance );
				VM_LEAVE_NATIVE( flags );
				break;
			}

#if DEBUG_ABSTRACT_FUNCTIONS
			default:
			case func_type_abstract:
				throw_icarus_error( "invalid function called (ME#5)" );
#endif
				
			case func_type_bytecode:
				CALL_SUBROUTINE
				break;
		}
		break; }
	
	case BYTECODE_CRF | OPS_WORD: {
		unsigned long index; FETCH_UWORD( index );
		call_t* call = &func->m_call_table[ index ];
		void* a; POP_PTR( a ); instance_t* instance = (instance_t*)a;
		CHECK_INSTANCE( instance );
#if SAFE_MACHINE && !__VISC__
		class_t* funcclss = func->m_class_table[ call->clssindx ];
		if( CLASS_HEADER( instance )->baseclss != funcclss )
		{
			istring funcclssname;
			istring subclssname;
			class_t::get_full_name( funcclss, funcclssname );
			class_t::get_full_name(
				CLASS_HEADER( instance )->baseclss, subclssname );
		
			istring s;
			s = "illegal regular function call (ME#6) [";
			s.append( funcclssname );
			s.append( "/" );
			s.append( subclssname );
			s.append( "]" );
		
			throw_icarus_error( s.c_str() );
		}
#endif
		func_t* newfunc = call->regufunc;

		VM_YIELD

		switch( newfunc->get_type() )
		{
			case func_type_native:
			{
				native_func_t* natfunc = (native_func_t*)newfunc;
				const int flags = natfunc->flags();
				VM_ENTER_NATIVE( flags );
				natfunc->call( *VM_FIBER, instance );
				VM_LEAVE_NATIVE( flags );
				break;
			}
				
#if DEBUG_ABSTRACT_FUNCTIONS
			case func_type_abstract:
				throw_icarus_error( "abstract function called (ME#7)" );
#endif
				
			case func_type_bytecode:
				CALL_SUBROUTINE
				break;
		}
		break; }
		
	case BYTECODE_RTINFO | OPS_PTR: {
		void* block; POP_PTR( block );
		unsigned long index; FETCH_INDEX( index );
		class_t* clss = func->m_class_table[ index ];
		CHECK_ACCESS( block, clss->size() );
		clss->init_instance( (u08*)block );
		break; }
						
	case BYTECODE_LDI | OPS_BYTE:
		PUSH_BYTE( *code++ );
		break;

	case BYTECODE_LDI | OPS_WORD: {
		int value; FETCH_UWORD( value );
		PUSH_WORD( value ); break; }

	case BYTECODE_LDI | OPS_QUAD: {
		unsigned long offset; FETCH_INDEX( offset );
		PUSH_QUAD( func->m_quad_table[ offset ] ); break; }

	case BYTECODE_LDI | OPS_OCTA: {
		unsigned long offset; FETCH_INDEX( offset );
		PUSH_OCTA( func->m_octa_table[ offset ] ); break; }
		
	case BYTECODE_FLDI | OPS_FLOAT: {
		unsigned long offset; FETCH_INDEX( offset );
		PUSH_FLOAT( func->m_float_table[ offset ] ); break; }

	case BYTECODE_FLDI | OPS_DOUBLE: {
		unsigned long offset; FETCH_INDEX( offset );
		PUSH_DOUBLE( func->m_double_table[ offset ] ); break; }
		
	case BYTECODE_SLDI | OPS_PTR: {
		unsigned long index; FETCH_INDEX( index );
#if ARIADNE_STRING_OBJECTS
		void* s = func->m_string_pool + 
			index * VM_MACHINE->get_string_class()->size();
#else
		void* s = &func->m_string_data[
			func->m_string_table[ index ] ];
#endif
		PUSH_PTR( s ); break; }
		
	case BYTECODE_STORE | OPS_BYTE: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 1 );
		POP_UBYTE( *(u08*)p ); sp += 4; break; }

	case BYTECODE_STORE | OPS_WORD: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 2 );
		POP_UWORD( *(u16*)p ); sp += 4; break; }

	case BYTECODE_STORE | OPS_QUAD: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 4 );
		POP_UQUAD( *(u32*)p ); sp += 4; break; }

	case BYTECODE_STORE | OPS_OCTA: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 8 );
		POP_UOCTA( *(u64*)p ); sp += 8; break; }

	case BYTECODE_LOAD | OPS_BYTE: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 1 );
		PUSH_BYTE( *(u08*)p ); break; }

	case BYTECODE_LOAD | OPS_WORD: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 2 );
		PUSH_WORD( *(u16*)p ); break; }

	case BYTECODE_LOAD | OPS_QUAD: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 4 );
		PUSH_QUAD( *(u32*)p ); break; }

	case BYTECODE_LOAD | OPS_OCTA: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 8 );
		PUSH_OCTA( *(u64*)p ); break; }

	case BYTECODE_FSTORE | OPS_FLOAT: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 4 );
		POP_FLOAT( *(f32*)p ); sp += 4; break; }

	case BYTECODE_FSTORE | OPS_DOUBLE: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 8 );
		POP_DOUBLE( *(f64*)p ); sp += 8; break; }
		
	case BYTECODE_FLOAD | OPS_FLOAT: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 4 );
		PUSH_FLOAT( *(f32*)p ); break; }

	case BYTECODE_FLOAD | OPS_DOUBLE: {
		void* p; POP_PTR( p );
		CHECK_ACCESS( p, 8 );
		PUSH_DOUBLE( *(f64*)p ); break; }

	case BYTECODE_XSTORE | OPS_BYTE: {
		void* p; POP_PTR( p ); isize_t bytecount = *code++;
		CHECK_ACCESS( p, bytecount );
		memcpy( p, sp - bytecount, bytecount ); break; }

	case BYTECODE_XSTORE | OPS_WORD: {
		void* p; POP_PTR( p ); isize_t bytecount;
		FETCH_UWORD( bytecount );
		CHECK_ACCESS( p, bytecount );
		memcpy( p, sp - bytecount, bytecount ); break; }

	case BYTECODE_XSTORE | OPS_QUAD: {
		void* p; POP_PTR( p ); isize_t bytecount;
		unsigned long offset; FETCH_INDEX( offset );
		bytecount = func->m_quad_table[ offset ];
		CHECK_ACCESS( p, bytecount );
		memcpy( p, sp - bytecount, bytecount ); break; }

	case BYTECODE_XLOAD | OPS_BYTE: {
		void* p; POP_PTR( p ); isize_t bytecount = *code++;
		CHECK_ACCESS( p, bytecount );
		memcpy( sp, p, bytecount ); sp += bytecount; break; }

	case BYTECODE_XLOAD | OPS_WORD: {
		void* p; POP_PTR( p ); isize_t bytecount;
		FETCH_UWORD( bytecount );
		CHECK_ACCESS( p, bytecount );
		memcpy( sp, p, bytecount ); sp += bytecount; break; }

	case BYTECODE_XLOAD | OPS_QUAD: {
		void* p; POP_PTR( p ); isize_t bytecount;
		unsigned long offset; FETCH_INDEX( offset );
		bytecount = func->m_quad_table[ offset ];
		CHECK_ACCESS( p, bytecount );
		memcpy( sp, p, bytecount ); sp += bytecount; break; }
				
	case BYTECODE_POP | OPS_BYTE: {
		unsigned int amount = *code++; sp -= amount; break; }

	case BYTECODE_POP | OPS_WORD: {
		unsigned int amount; FETCH_UWORD( amount ); sp -= amount; break; }

	case BYTECODE_POP | OPS_QUAD: {
		unsigned long index; FETCH_INDEX( index );
		sp -= func->m_quad_table[ index ]; break; }

	case BYTECODE_PUSHSELF | OPS_PTR: {
		PUSH_PTR( self ); break; }

	case BYTECODE_PUSHLOCAL | OPS_BYTE: {
		int offset = (s08)( *code++ );
		PUSH_PTR( localsp + offset ); break; }

	case BYTECODE_PUSHLOCAL | OPS_WORD: {
		int offset; FETCH_SWORD( offset );
		PUSH_PTR( localsp + offset ); break; }

	case BYTECODE_PUSHLOCAL | OPS_PTR: {
		PUSH_PTR( localsp ); break; }
	
	case BYTECODE_PUSHSTACK | OPS_BYTE: {
		int offset = (s08)( *code++ );
		PUSH_PTR( sp + offset ); break; }

	case BYTECODE_PUSHSTACK | OPS_WORD: {
		int offset; FETCH_SWORD( offset );
		PUSH_PTR( sp + offset ); break; }

	case BYTECODE_PUSHSTACK | OPS_PTR: {
		PUSH_PTR( sp ); break; }

	case BYTECODE_PUSHSTATIC | OPS_BYTE: {
		int offset = (s08)( *code++ );
		unsigned long index; FETCH_INDEX( index );
		PUSH_PTR( func->m_class_table[ index ]->m_static_data + offset );
		break; }

	case BYTECODE_PUSHSTATIC | OPS_WORD: {
		int offset; FETCH_SWORD( offset );
		unsigned long index; FETCH_INDEX( index );
		PUSH_PTR( func->m_class_table[ index ]->m_static_data + offset );
		break; }

	case BYTECODE_PUSHSTATIC | OPS_PTR: {
		unsigned long index; FETCH_INDEX( index );
		PUSH_PTR( func->m_class_table[ index ]->m_static_data );
		break; }


	case BYTECODE_EXTS | OPS_WORD: {
		int a; POP_SBYTE( a ); PUSH_WORD( a ); break; }

	case BYTECODE_EXTS | OPS_QUAD: {
		long a; POP_SWORD( a ); PUSH_QUAD( a ); break; }

	case BYTECODE_EXTS | OPS_OCTA: {
		s64 a; POP_SQUAD( a ); PUSH_OCTA( a ); break; }

	case BYTECODE_EXTU | OPS_WORD: {
		unsigned int a; POP_UBYTE( a ); PUSH_WORD( a ); break; }

	case BYTECODE_EXTU | OPS_QUAD: {
		unsigned long a; POP_UWORD( a ); PUSH_QUAD( a ); break; }

	case BYTECODE_EXTU | OPS_OCTA: {
		u64 a; POP_UQUAD( a ); PUSH_OCTA( a ); break; }

	case BYTECODE_RDC | OPS_BYTE: {
		unsigned int a; POP_UWORD( a ); PUSH_BYTE( a ); break; }

	case BYTECODE_RDC | OPS_WORD: {
		unsigned long a; POP_UQUAD( a ); PUSH_WORD( a ); break; }

	case BYTECODE_RDC | OPS_QUAD: {
		u64 a; POP_UOCTA( a ); PUSH_QUAD( a ); break; }
		
	case BYTECODE_FEXT | OPS_FLOAT: {
		double a; POP_DOUBLE( a ); PUSH_FLOAT( a ); break; }

	case BYTECODE_FEXT | OPS_DOUBLE: {
		float a; POP_FLOAT( a ); PUSH_DOUBLE( a ); break; }
		
	case BYTECODE_ITOF | OPS_BYTE: {
		int a; POP_SBYTE( a ); PUSH_DOUBLE( a ); break; }

	case BYTECODE_ITOF | OPS_WORD: {
		int a; POP_SWORD( a ); PUSH_DOUBLE( a ); break; }

	case BYTECODE_ITOF | OPS_QUAD: {
		long a; POP_SQUAD( a ); PUSH_DOUBLE( a ); break; }

	case BYTECODE_ITOF | OPS_OCTA: {
		s64 a; POP_SOCTA( a ); PUSH_DOUBLE( a ); break; }

	case BYTECODE_UTOF | OPS_BYTE: {
		unsigned int a; POP_UBYTE( a ); PUSH_DOUBLE( a ); break; }

	case BYTECODE_UTOF | OPS_WORD: {
		unsigned int a; POP_UWORD( a ); PUSH_DOUBLE( a ); break; }

	case BYTECODE_UTOF | OPS_QUAD: {
		unsigned long a; POP_UQUAD( a ); PUSH_DOUBLE( a ); break; }

	case BYTECODE_UTOF | OPS_OCTA: {
		u64 a; POP_UOCTA( a ); PUSH_DOUBLE( a ); break; }

	case BYTECODE_FTOI | OPS_BYTE: {
		double a; POP_DOUBLE( a ); s08 b = a; PUSH_BYTE( b ); break; }

	case BYTECODE_FTOI | OPS_WORD: {
		double a; POP_DOUBLE( a ); s16 b = a; PUSH_WORD( b ); break; }

	case BYTECODE_FTOI | OPS_QUAD: {
		double a; POP_DOUBLE( a ); s32 b = a; PUSH_QUAD( b ); break; }

	case BYTECODE_FTOI | OPS_OCTA: {
		double a; POP_DOUBLE( a ); s64 b = a; PUSH_OCTA( b ); break; }

	case BYTECODE_FTOU | OPS_BYTE: {
		double a; POP_DOUBLE( a ); u08 b = a; PUSH_BYTE( b ); break; }

	case BYTECODE_FTOU | OPS_WORD: {
		double a; POP_DOUBLE( a ); u16 b = a; PUSH_WORD( b ); break; }

	case BYTECODE_FTOU | OPS_QUAD: {
		double a; POP_DOUBLE( a ); u32 b = a; PUSH_QUAD( b ); break; }

	case BYTECODE_FTOU | OPS_OCTA: {
		double a; POP_DOUBLE( a ); u64 b = a; PUSH_OCTA( b ); break; }
	
	case BYTECODE_DUP | OPS_BYTE: {
		unsigned int a; POP_UBYTE( a ); sp += 4; PUSH_BYTE( a ); break; }

	case BYTECODE_DUP | OPS_WORD: {
		unsigned int a; POP_UWORD( a ); sp += 4; PUSH_WORD( a ); break; }

	case BYTECODE_DUP | OPS_QUAD: {
		unsigned long a; POP_UQUAD( a ); sp += 4; PUSH_QUAD( a ); break; }

	case BYTECODE_DUP | OPS_OCTA: {
		u64 a; POP_UOCTA( a ); sp += 8; PUSH_OCTA( a ); break; }

	case BYTECODE_FDUP | OPS_FLOAT: {
		float a; POP_FLOAT( a ); sp += 4; PUSH_FLOAT( a ); break; }

	case BYTECODE_FDUP | OPS_DOUBLE: {
		double a; POP_DOUBLE( a ); sp += 8; PUSH_DOUBLE( a ); break; }

	case BYTECODE_DYNCAST | OPS_WORD: {
		unsigned int clssindex; FETCH_INDEX( clssindex );
		void* instance; POP_PTR( instance );
		PUSH_PTR( dyncast( VM_FIBER, instance,
			func->m_class_table[ clssindex ] ) );
		break; }
		
	case BYTECODE_NEW | OPS_WORD: {
		unsigned int clssindex; FETCH_INDEX( clssindex );
		unsigned long arraysize; POP_UQUAD( arraysize );
		VM_STORE_SP;
		PUSH_PTR( newinstance(
			VM_FIBER,
			func->m_class_table[ clssindex ],
			arraysize ) );
		break; }
		
	case BYTECODE_NEW | OPS_QUAD: {
		unsigned long offsetindex; FETCH_INDEX( offsetindex );
		unsigned long arraysize; POP_UQUAD( arraysize );
		PUSH_PTR( newblock(
			VM_MEMORY,
			func->m_quad_table[ offsetindex ],
			arraysize ) );
		break; }
				
	case BYTECODE_DELETE | OPS_WORD: {
		void* instance; POP_PTR( instance );
		VM_STORE_SP;
		deleteinstance(
			VM_FIBER,
			instance );
		break; }

	case BYTECODE_DELETE | OPS_QUAD: {
		void* block; POP_PTR( block );
		deleteblock( VM_MEMORY, block );
		break; }

	case BYTECODE_BREAK | OPS_BREAK_REGULAR:
		opcode = func->hide_breakpoint(
			func->get_code_offset( code ) - 1 );

		m_stop_flag = true;
		condflags |= COND_BREAK;
		VM_SAVE_FIBER_STATE;
		condflags &= ~COND_BREAK;
		stop_on_demand();

		goto process_opcode;

	case BYTECODE_BREAK | OPS_BREAK_STEP:
		func->remove_breakpoint(
			func->get_code_offset( code ) - 1 );

		m_stop_flag = true;
		condflags |= COND_BREAK;
		VM_SAVE_FIBER_STATE;
		condflags &= ~COND_BREAK;
		stop_on_demand();
		code -= 1;
		break;
				
	case BYTECODE_BREAK | OPS_BREAK_STEP_CVF:
	{
		func->remove_breakpoint(
			func->get_code_offset( code ) - 1 );

		unsigned long index; FETCH_UWORD( index );
		index = func->m_call_table[ index ].funcid;
		void* a; POP_PTR( a ); sp += 4;
		instance_t* instance = (instance_t*)a;
		CHECK_INSTANCE( instance );
		class_header_t*	header = CLASS_HEADER( instance );
		class_t* clss = header->clss;
		unsigned long offset = 0;
						
		func_t* newfunc = clss->find_func_by_id( index, &offset );
		if( newfunc && newfunc->get_type() == func_type_bytecode )
			( (bytecode_func_t*)newfunc )->add_breakpoint(
				0, BYTECODE_BREAK | OPS_BREAK_STEP );

		code -= 3;
				
		break;
	}

	case BYTECODE_EXTRA | OPS_PTR: {
		opcode = *code++;
		switch( opcode )
		{
			case BYTECODE_SWITCH_LINEAR | OPS_PTR:
			{
				unsigned long selector;
				POP_UQUAD( selector );
				const u32* quadtable;
				quadtable = func->m_quad_table;
			
				unsigned short count;
				unsigned long index;
				short offset;
				
				FETCH_UWORD( count );
				do {
					FETCH_INDEX( index );
					if( quadtable[ index ] == selector )
					{	
						FETCH_SWORD( offset );
						code += offset - 2;
						goto done;
					}
					code += 2;
				} while( --count );
				
				FETCH_SWORD( offset );
				code += offset - 2;

done:				
				break;
			}

			case BYTECODE_DYNNEW | OPS_WORD: {
				void* clssname; POP_PTR( clssname );
				unsigned int clssindex; FETCH_INDEX( clssindex );
				unsigned long arraysize; POP_UQUAD( arraysize );
				VM_STORE_SP;
				PUSH_PTR( newinstancedyn(
					VM_FIBER,
					func->m_class_table[ clssindex ],
					arraysize, (const char*)clssname ) );
				break; }
			
			case BYTECODE_TCSF | OPS_WORD: {
				unsigned long index; FETCH_UWORD( index );
				call_t* call = &func->m_call_table[ index ];
				index = call->funcid;
				class_t* clss = func->m_class_table[ call->clssindx ];
				unsigned long offset = 0;
				func_t* newfunc = clss->find_func_by_id( index, &offset );
				if( !newfunc )
					throw_icarus_error( "function not found (ME#2)" );
				unsigned long tc_param_size = *code++;
				tc_param_size <<= 8;
				tc_param_size |= *code++;

				VM_YIELD

				switch( newfunc->get_type() )
				{
					case func_type_bytecode:
					{
						instance_t* instance = 0;
						TAILCALL_SUBROUTINE
						break;
					}
				}
				break;
			}

			default:
				goto invalid_opcode;
		}
		break; }
		
invalid_opcode:
	default:
		invalid_opcode_error( VM_MACHINE, func, code, opcode );
		break;			 
}
