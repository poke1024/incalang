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
//	disassembler.cp			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "disassembler_t.h"
#include "bytecode.h"
#include "machine_t.h"
#include "machine_class_t.h"
#include "vmdefs.h"
#include "bytecode_func_t.h"
#include <cstdio>
#include <string>

#ifdef SUPPORT_DISASSEMBLE

BEGIN_MACHINE_NAMESPACE

using namespace std;

static char				intsize[ 4 ] = { 'b', 'w', 'q', 'o' };
static char				fltsize[ 4 ] = { 's', 'd', '?', '?' };

void
disassembler_t::print_opcode(
	const char*			name,
	int					opcode,
	bool				integer )
{
	const int			index = opcode >> 6;
	char				buf[ 8 + 1 ];

	print( name );

	isnprintf( buf, 8, ".%c\n",
		integer ? intsize[ index ] : fltsize[ index ] );

	print( buf );
}

const u08*
disassembler_t::print_branch(
	const char*			name,
	unsigned short		offset,
	const u08*			code )
{
	short				relative;
	
	relative = *code++;
	relative = ( relative << 8 ) | *code++;
	
	char				buf[ 32 + 1 ];

	isnprintf( buf, 32, " %.4x\n",
		(unsigned short)( offset + relative ) );

	print( name );
	print( buf );
	
	return code;
}

static unsigned long
fetch_index(
	const u08*&			code )
{
	unsigned long		index;

	index = *code++;
	if( index == 0xff )
	{
		index = *code++;
		index = ( index << 8 ) | *code++;
	}
	index = ( index << 8 ) | *code++;
	
	return index;
}

static u64
fetch_iconst(
	int					opcode,
	const u08*&			code,
	const u32*			qtable,
	const u64*			otable,
	int&				index )
{
	index = opcode >> 6;

	u64	val;

	if( index == 0 )
		val = *code++;
	else if( index == 1 )
	{
		val = *code++;
		val = ( val << 8 ) | *code++;
	}
	else if( index == 2 )
		val = qtable[ fetch_index( code ) ];
	else
	{
		if( !otable )
			throw_icarus_error( "internal error" );
			
		val = otable[ fetch_index( code ) ];
	}
	
	return val;
}

const u08*
disassembler_t::disassemble_call(
	const char*			name,
	const u08*			code,
	call_t*				call_table,
	u32					call_table_size,
	class_t**			class_table,
	bool				tail_call )
{
	unsigned int		index;
	
	index = *code++;
	index = ( index << 8 ) | *code++;
	
	istring				s;

	if( call_table && index < call_table_size )
	{
		call_t*			call = &call_table[ index ];
		class_t*		clss = class_table[ call->clssindx ];
		func_id_t		funcid = call->funcid;

		istring			funcname;

		m_machine->func_id_to_name(
			funcid, funcname );
		
		class_t::get_full_name( clss, s );
		if( s.length() )
			s.append( "::" );
	
		s.append( funcname );

	}
	else
		s = "<illegal call>";

	print( name );
	s.insert( 0, " " );

	if( tail_call )
	{
		char			buf[ 32 ];
		long			param_size;

		param_size = *code++;
		param_size = ( param_size << 8 ) | *code++;

		sprintf( buf, ",%ld", param_size );
		s.append( buf );
	}

	s.append( "\n" );
	print( s.c_str() );
	
	return code;
}

const u08*
disassembler_t::disassemble_rtinfo(
	const u08*			code,
	class_t**			class_table )
{
	unsigned long		index = fetch_index( code );
	class_t*			clss = class_table[ index ];
	istring				s;
	
	class_t::get_full_name( clss, s );

	s.append( "\n" );
	print( "rtinfo " );
	print( s.c_str() );
	
	return code;
}

const u08*
disassembler_t::disassemble_dyncast(
	const u08*			code,
	class_t**			class_table,
	unsigned long*		long_table )
{
	unsigned long		clssindex;
	
	clssindex = fetch_index( code );
	
	class_t*			clss = class_table[ clssindex ];

	//unsigned long		offsetindex;
	
	//offsetindex = fetch_index( code );

	istring				s;
	clss->get_name( s );
	
	/*char				buf[ 32 + 1 ];
	
	isnprintf( buf, 32, " (%ld)\n",
		long_table[ offsetindex ] );*/
	
	print( "dyncast " );
	print( s.c_str() );
	print( "\n" );
	//print( buf );	
	
	return code;
}

const u08*
disassembler_t::disassemble_newinstance(
	const u08*			code,
	class_t**			class_table )
{
	unsigned long		clssindex;
	
	clssindex = fetch_index( code );
	
	class_t*			clss = class_table[ clssindex ];

	istring				s;
	clss->get_name( s );

	s.append( "\n" );	
	print( "newinstance " );
	print( s.c_str() );
	
	return code;
}

const u08*
disassembler_t::disassemble_newblock(
	const u08*			code,
	unsigned long*		long_table )
{
	unsigned long		offsetindex;
	
	offsetindex = fetch_index( code );
	
	char				buf[ 32 + 1 ];
	
	isnprintf( buf, 32, "%ld\n",
		long_table[ offsetindex ] );

	print( "newblock " );
	print( buf );
	
	return code;
}

const u08*
disassembler_t::disassemble_newinstancedyn(
	const u08*			code,
	class_t**			class_table )
{
	unsigned long		clssindex;
	
	clssindex = fetch_index( code );
	
	class_t*			clss = class_table[ clssindex ];

	istring				s;
	clss->get_name( s );
	
	s.append( "\n" );
	print( "newinstancedyn " );
	print( s.c_str() );
	
	return code;
}

const u08*
disassembler_t::disassemble_delete(
	int					opcode,
	const u08*			code )
{
	if( opcode == ( BYTECODE_DELETE | OPS_WORD ) )
		print( "deleteinstance\n" );
	else
		print( "deleteblock\n" );
	
	return code;
}

const u08*
disassembler_t::disassemble_pushstatic(
	int					opcode,
	const u08*			code,
	class_t**			class_table )
{	
	int					index = opcode >> 6;
	long				offset;
	
	if( index == 0 )
	{
		offset = *code++;
		offset = (s08)offset;
	}
	else if( index == 1 )
	{
		offset = *code++;
		offset = ( offset << 8 ) | *code++;
		offset = (s16)offset;
	}

	unsigned long		clssindex;
	
	clssindex = fetch_index( code );
		
	class_t*			clss = class_table[ clssindex ];

	istring				s;
	clss->get_name( s );
	
	print( "pushstatic " );
	print( s.c_str() );
	
	if( index < 2 )
	{
		char			buf[ 32 + 1 ];
		
		isnprintf( buf, 32, " %ld", offset );
		print( buf );
	}

	print( "\n" );
	
	return code;
}

const u08*
disassembler_t::print_rts(
	int					opcode,
	const u08*			code,
	const u32*			table )
{
	int					index;
	unsigned long		paramsize;
	unsigned long		retsize;
	
	paramsize = fetch_iconst( opcode, code, table, 0, index );
	retsize = fetch_iconst( opcode, code, table, 0, index );
	
	char				buf[ 64 + 1 ];
	
	isnprintf( buf, 64, "rts %ld,%ld\n",
		paramsize, retsize  );
	
	print( buf );
	
	return code;
}

const u08*
disassembler_t::print_ldi(
	int					opcode,
	const u08*			code,
	const u32*			qtable,
	const u64*			otable )
{
	int					index;
	u64					val;

	val = fetch_iconst(
		opcode, code, qtable, otable, index );

	char				buf[ 64 + 1 ];
	
	if( index == 3 )
	{
#if __VISC__
		isnprintf( buf, 64, "ldi.%c #%I64d\n",
#else
		isnprintf( buf, 64, "ldi.%c #%lld\n",
#endif
			intsize[ index ], val  );
	}
	else
	{
		isnprintf( buf, 64, "ldi.%c #%ld\n",
			intsize[ index ], (long)val  );
	}
	
	print( buf );
	
	return code;
}

const u08*
disassembler_t::print_fldi(
	int					opcode,
	const u08*			code,
	const float*		ftable,
	const double*		dtable )
{
	const int			index = opcode >> 6;

	unsigned long		tableindex;

	tableindex = *code++;
	if( tableindex == 0xff )
	{
		tableindex = *code++;
		tableindex = ( tableindex << 8 ) | *code++;
	}
	tableindex = ( tableindex << 8 ) | *code++;

	double				val;

	if( index == 0 )
		val = ftable[ tableindex ];
	else
		val = dtable[ tableindex ];

	char				buf[ 64 + 1 ];
	
	isnprintf( buf, 64, "fldi.%c #%lf\n",
		fltsize[ index ], val  );
	
	print( buf );
	
	return code;
}

const u08*
disassembler_t::print_sldi(
	int					/*opcode*/,
	const u08*			code,
	const u32*			m_string_table,
	const char*			m_string_data )
{
	unsigned long		index;

	index = *code++;
	if( index == 0xff )
	{
		index = *code++;
		index = ( index << 8 ) | *code++;
	}
	index = ( index << 8 ) | *code++;
	
	const char*			s;
	
	unsigned long		offset;
	
	offset = m_string_table[ index ];	
	s = &m_string_data[ offset ];
	
	istring				t;
	
	while( *s )
	{
		char			c = *s++;
		
		switch( c )
		{
			case '\n':
				t.append( "\\n" );
				break;

			case '\r':
				t.append( "\\r" );
				break;

			case '\t':
				t.append( "\\t" );
				break;
				
			default:
				t.append( 1, c );
				break;
		}
	
	}
	
	print( "sldi '" );
	print( t.c_str() );
	print( "'\n" );
	
	return code;
}

const u08*
disassembler_t::print_ldv(
	const char*			name,
	int					opcode,
	const u08*			code,
	const u32*			table )
{
	const int			index = opcode >> 6;

	long				offset;

	if( index == 0 )
	{
		offset = *code++;
		offset = (signed char)offset;

	}
	else if( index == 1 )
	{
		offset = *code++;
		offset = ( offset << 8 ) | *code++;
		offset = (signed short)offset;
	}
	else
	{
		unsigned long	tableindex;
	
		tableindex = *code++;
		if( tableindex != 0xff )
			tableindex = ( tableindex << 8 ) | *code++;
		else
		{
			tableindex = *code++;
			tableindex = ( tableindex << 8 ) | *code++;
			tableindex = ( tableindex << 8 ) | *code++;
		}
	
		offset = table[ tableindex ];
	}
	
	char				buf[ 32 + 1 ];
	
	isnprintf( buf, 32, " #%ld\n", offset );
		
	print( name );
	print( buf );
		
	return code;
}

const u08*
disassembler_t::print_ival_opcode(
	const char*			name,
	int					opcode,
	const u08*			code,
	const u32*			table )
{
	int					index;
	unsigned long		bytecount;
	
	bytecount = fetch_iconst(
		opcode, code, table, 0, index );
	
	char				buf[ 32 + 1 ];
	
	isnprintf( buf, 32, " %ld\n", bytecount );
	
	print( name );
	print( buf );
	
	return code;
}

const u08*
disassembler_t::print_pushreg(
	const char*			name,
	int					opcode,
	const u08*			code )
{
	int					index = opcode >> 6;

	if( index == 2 )
	{
		print( name );
		print( "\n" );
		return code;
	}

	char				s[ 32 + 1 ];
	long				offset;
	
	if( index == 0 )
		offset = *code++;
	else if( index == 1 )
	{
		offset = *code++;
		offset = ( offset << 8 ) | *code++;
	}
	
	if( index == 0 )
		offset = (s08)offset;
	else
		offset = (s16)offset;
	
	print( name );
	isnprintf( s, 32, " %ld\n", offset );
	print( s );
	
	return code;
}

const u08*
disassembler_t::disassemble_switch_linear(
	vm_state_t*			state,
	const u08*			code )
{
	print( "switch_linear\n" );
	
	long				count;
	
	count = *code++;
	count = ( count << 8 ) | *code++;
	
	const u32*			qtable;
	
	qtable = state->func->m_quad_table;

	unsigned short		codeoffset;
	s16					offset;
	char				buf[ 64 + 1 ];

	for( long i = 0; i < count; i++ )
	{
		u32				value;
		int				index;
		
		value = fetch_iconst(
			OPS_QUAD, code, qtable, 0, index );
		
		codeoffset = code - state->func->m_code_data;
				
		offset = *( code++ );
		offset = ( offset << 8 ) | *code++;
		
		isnprintf( buf, 64, "%.4x\t\t%.8lx -> %.4lx\n",
			codeoffset, (long)value, (long)offset );
		print( buf );
	}
	
	codeoffset = code - state->func->m_code_data;
	
	offset = *( code++ );
	offset = ( offset << 8 ) | *code++;

	isnprintf( buf, 64 + 1, "%.4x\t\tdefault  -> %.4lx\n",
		codeoffset, (long)offset );
	print( buf );

	return code;
}

const u08*
disassembler_t::disassemble_enter_try(
	vm_state_t*			state,
	const u08*			code )
{
	print( "enter_try\n" );
	
	long				count;
	
	count = *code++;
	count = ( count << 8 ) | *code++;
	
	char				buf[ 64 + 1 ];
	unsigned short		codeoffset;
	
	for( long i = 0; i < count; i++ )
	{
		long			index;
		s16				offset;
	
		index = *code++;
		index = ( index << 8 ) | *code++;
		
		class_t*		clss;
		istring			clssname;
		
		clss = state->func->m_class_table[ index ];
		clss->get_name( clssname );
		
		codeoffset = code - state->func->m_code_data;
				
		offset = *( code++ );
		offset = ( offset << 8 ) | *code++;
		
		isnprintf( buf, 64, "%.4x\t\t", codeoffset );
		print( buf );

		print( clssname.c_str() );

		isnprintf( buf, 64, " -> %.4lx\n", (long)offset );
		print( buf );
	}
	
	return code;
}

void
disassembler_t::dcode(
	vm_state_t*			state )
{
	const u08*			code = state->code;
	
	unsigned short		offset = code - state->func->m_code_data;

	char				buf[ 32 + 1 ];

	isnprintf( buf, 32, "%.4x\t", offset );
	print( buf );

	int					opcode = *code++;
	
	offset++;

	switch( opcode & 63 )
	{
		case BYTECODE_ADD:
			print_opcode( "add", opcode, true );
			break;
			
		case BYTECODE_SUB:
			print_opcode( "sub", opcode, true );
			break;
		
		case BYTECODE_MUL:
			print_opcode( "mul", opcode, true );
			break;
		
		case BYTECODE_DIV:
			print_opcode( "div", opcode, true );
			break;

		case BYTECODE_MOD:
			print_opcode( "mod", opcode, true );
			break;

		case BYTECODE_NEG:
			print_opcode( "neg", opcode, true );
			break;

		case BYTECODE_CMP:
			print_opcode( "cmp", opcode, true );
			break;

		case BYTECODE_CMPU:
			print_opcode( "cmpu", opcode, true );
			break;

		case BYTECODE_FADD:
			print_opcode( "fadd", opcode, false );
			break;

		case BYTECODE_FSUB:
			print_opcode( "fsub", opcode, false );
			break;

		case BYTECODE_FMUL:
			print_opcode( "fmul", opcode, false );
			break;

		case BYTECODE_FDIV:
			print_opcode( "fdiv", opcode, false );
			break;

		case BYTECODE_FMOD:
			print_opcode( "fmod", opcode, false );
			break;

		case BYTECODE_FNEG:
			print_opcode( "fneg", opcode, false );
			break;

		case BYTECODE_FCMP:
			print_opcode( "fcmp", opcode, false );
			break;

		case BYTECODE_NOT:
			print_opcode( "not", opcode, true );
			break;

		case BYTECODE_AND:
			print_opcode( "and", opcode, true );
			break;

		case BYTECODE_OR:
			print_opcode( "or", opcode, true );
			break;

		case BYTECODE_XOR:
			print_opcode( "xor", opcode, true );
			break;

		case BYTECODE_SHL:
			print_opcode( "shl", opcode, true );
			break;

		case BYTECODE_SHR:
			print_opcode( "shr", opcode, true );
			break;

		case BYTECODE_ASR:
			print_opcode( "asr", opcode, true );
			break;

		case BYTECODE_BRA:
			code = print_branch( "bra", offset, code );
			break;

		case BYTECODE_BEQ:
			code = print_branch( "beq", offset, code );
			break;

		case BYTECODE_BNE:
			code = print_branch( "bne", offset, code );
			break;

		case BYTECODE_BGT:
			code = print_branch( "bgt", offset, code );
			break;

		case BYTECODE_BLT:
			code = print_branch( "blt", offset, code );
			break;

		case BYTECODE_BGE:
			code = print_branch( "bge", offset, code );
			break;

		case BYTECODE_BLE:
			code = print_branch( "ble", offset, code );
			break;

		case BYTECODE_RTS:
			code = print_rts( opcode, code,
				state->func->m_quad_table );
			break;

		case BYTECODE_CSF:
			code = disassemble_call( "csf", code,
				state->func->m_call_table,
				state->func->m_call_count,
				state->func->m_class_table,
				false );
			break;		

		case BYTECODE_CVF:
			code = disassemble_call( "cvf", code,
				state->func->m_call_table,
				state->func->m_call_count,
				state->func->m_class_table,
				false );
			break;
			
		case BYTECODE_CRF:
			code = disassemble_call( "crf", code,
				state->func->m_call_table,
				state->func->m_call_count,
				state->func->m_class_table,
				false );
			break;
			
		case BYTECODE_RTINFO:
			code = disassemble_rtinfo( code,
				state->func->m_class_table );
			break;
			
		case BYTECODE_LDI:
			code = print_ldi( opcode, code,
				state->func->m_quad_table,
				state->func->m_octa_table );
			break;

		case BYTECODE_FLDI:
			code = print_fldi( opcode, code,
				state->func->m_float_table,
				state->func->m_double_table );
			break;
			
		case BYTECODE_SLDI:
			code = print_sldi( opcode, code,
				state->func->m_string_table,
				state->func->m_string_data );
			break;
			
		case BYTECODE_PUSHSELF:
			print( "pushself\n" );
			break;

		case BYTECODE_PUSHLOCAL:
			code = print_pushreg( "pushlocal", opcode, code );
			break;

		case BYTECODE_PUSHSTACK:
			code = print_pushreg( "pushstack", opcode, code );
			break;

		case BYTECODE_PUSHSTATIC:
			code = disassemble_pushstatic(
				opcode, code, state->func->m_class_table );
			break;
					
		case BYTECODE_LOAD:
			print_opcode( "load", opcode, true );
			break;

		case BYTECODE_STORE:
			print_opcode( "store", opcode, true );
			break;

		case BYTECODE_FLOAD:
			print_opcode( "fload", opcode, false );
			break;

		case BYTECODE_FSTORE:
			print_opcode( "fstore", opcode, false );
			break;
			
		case BYTECODE_XSTORE:
			code = print_ival_opcode( "xstore",
				opcode, code, state->func->m_quad_table );
			break;

		case BYTECODE_XLOAD:
			code = print_ival_opcode( "xload",
				opcode, code, state->func->m_quad_table );
			break;
			
		case BYTECODE_POP:
			code = print_ival_opcode( "pop",
				opcode, code, state->func->m_quad_table );
			break;
						
		case BYTECODE_EXTS:
			print_opcode( "exts", opcode, true );
			break;

		case BYTECODE_EXTU:
			print_opcode( "extu", opcode, true );
			break;

		case BYTECODE_RDC:
			print_opcode( "rdc", opcode, true );
			break;

		case BYTECODE_FEXT:
			print_opcode( "fext", opcode, false );
			break;

		case BYTECODE_ITOF:
			print_opcode( "itof", opcode, true );
			break;

		case BYTECODE_UTOF:
			print_opcode( "utof", opcode, true );
			break;
		
		case BYTECODE_FTOI:
			print_opcode( "ftoi", opcode, true );
			break;

		case BYTECODE_FTOU:
			print_opcode( "ftou", opcode, true );
			break;
			
		case BYTECODE_DUP:
			print_opcode( "dup", opcode, true );
			break;

		case BYTECODE_FDUP:
			print_opcode( "fdup", opcode, false );
			break;
			
		case BYTECODE_NEW:
			if( opcode == ( BYTECODE_NEW | OPS_WORD ) )
			{
				code = disassemble_newinstance( code,
					state->func->m_class_table );
			}
			else
			{
				code = disassemble_newblock( code,
					state->func->m_quad_table );
			}
			break;
					
		case BYTECODE_DELETE:
			code = disassemble_delete( opcode, code );
			break;
			
		case BYTECODE_DYNCAST:
			code = disassemble_dyncast( code,
				state->func->m_class_table,
				state->func->m_quad_table );
			break;
			
		case BYTECODE_EXTRA:
			opcode = *code++;
			
			switch( opcode & 63 )
			{
				case BYTECODE_SWITCH_LINEAR:
					code = disassemble_switch_linear( state, code );
					break;

				case BYTECODE_DYNNEW:
					code = disassemble_newinstancedyn( code,
						state->func->m_class_table );
					break;

				case BYTECODE_ENTER_TRY:
					code = disassemble_enter_try( state, code );
					break;
			
				case BYTECODE_LEAVE_TRY:
					print( "leave_try\n" );
					break;

				case BYTECODE_ENTER_CATCH:
					print( "enter_catch\n" );
					break;

				case BYTECODE_LEAVE_CATCH:
					print( "leave_catch\n" );
					break;

				case BYTECODE_THROW:
					print( "throw\n" );
					break;

				case BYTECODE_TCVF:
					code = disassemble_call( "tcvf", code,
						state->func->m_call_table,
						state->func->m_call_count,
						state->func->m_class_table,
						true );
					break;

				case BYTECODE_TCRF:
					code = disassemble_call( "tcrf", code,
						state->func->m_call_table,
						state->func->m_call_count,
						state->func->m_class_table,
						true );
					break;

				case BYTECODE_TCSF:
					code = disassemble_call( "tcsf", code,
						state->func->m_call_table,
						state->func->m_call_count,
						state->func->m_class_table,
						true );
					break;
			}
			break;
			
		default:
			print( "invalid opcode #" );
			isnprintf( buf, 32, "%x\n", (short)opcode );
			print( buf );
			break;
	}

	state->code = (u08*)code;
}

END_MACHINE_NAMESPACE

#endif
