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
//	backend.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "backend.h"
#include "inst_t.h"
#include "xhtab_t.h"
#include "xalloc.h"
#include "output.h"
#include "code_t.h"
#include "scope_t.h"
#include "parser.h"
#include "class_t.h"
#include <cstring>
#include "bytecode.h"
#include "expr_t.h"
#include "type_t.h"
#include "compbreak.h"

#include "tmpvar.h"

#if BYTECODE_LAST >= 64
#error too many byte codes
#endif

BEGIN_COMPILER_NAMESPACE

using namespace std;

static void
genpop(
	pop_code_t*			code );

static void
genlabel(
	label_t*			target );

// ---------------------------------------------------------------------------

struct const_node_t {
	size_t				offset;
	xhitem_t*			item;
	const_node_t*		next;
};

struct const_tab_t {
	xhtab_t*			htab;
	size_t				size;
	unsigned long		count;
	const_node_t*		first;
	const_node_t**		link;
};

static const_tab_t		s_const_tab[ 5 ];

#define INSERT_CONST_INTO_TABLE( kind, data, datasize,			\
	extra, copydata, string )									\
	const_tab_t*		ct = &s_const_tab[ kind ];				\
	xhtab_t*			t = ct->htab;							\
																\
	if( !t )													\
	{															\
		t = xhcreate( 4 );										\
		ct->htab = t;											\
	}															\
																\
	if( xhfind( t, (data), (datasize) ) ) {						\
		const_node_t*	node = (const_node_t*)xhstuff( t );		\
		offset = node->offset;									\
	} else {													\
		const_node_t*	node = (const_node_t*)xalloc(			\
							sizeof( const_node_t ) );			\
		u08*			storage;								\
																\
		if( copydata ) {										\
			storage = (u08*)xalloc( datasize );					\
			memcpy( storage, (data), (datasize) );				\
		} else													\
			storage = (data);									\
																\
		offset = ct->size;										\
		count = ct->count;										\
																\
		ct->size = offset + (datasize) + (extra);				\
		ct->count = count + 1;									\
																\
		if( string ) offset = count;							\
		xhadd( t, storage, (datasize), node );					\
		node->offset = offset;									\
		node->item = t->ipos;									\
		node->next = 0;											\
		*ct->link = node;										\
		ct->link = &node->next;									\
																\
	}

static unsigned long
index_qconst(
	long				val )
{
	size_t				offset;
	unsigned long		count;
	INSERT_CONST_INTO_TABLE( 0, (u08*)&val,
		sizeof( long ), 0, true, false )
	return offset / sizeof( long );
}

static unsigned long
index_oconst(
	s64					val )
{
	size_t				offset;
	unsigned long		count;
	INSERT_CONST_INTO_TABLE( 1, (u08*)&val,
		sizeof( s64 ), 0, true, false )
	return offset / sizeof( s64 );
}

static unsigned long
index_fconst(
	float				val )
{	
	size_t				offset;
	unsigned long		count;
	INSERT_CONST_INTO_TABLE( 2, (u08*)&val,
		sizeof( float ), 0, true, false )
	return offset / sizeof( float );
}

static unsigned long
index_dconst(
	double				val )
{
	size_t				offset;
	unsigned long		count;
	INSERT_CONST_INTO_TABLE( 3, (u08*)&val,
		sizeof( double ), 0, true, false )
	return offset / sizeof( double );
}

static unsigned long
index_strconst(
	const char*			str )
{
	size_t				offset;
	unsigned long		count;
	const long			len = strlen( str );

	INSERT_CONST_INTO_TABLE( 4, (u08*)str,
		len, 1, false, true )
	
	return offset;
}

static void
write_const_tables()
{
	const_tab_t*		ct;
	const_node_t*		node;
	xhitem_t*			item;
	
	ct = &s_const_tab[ 0 ];
	out_word( ct->count );
		
	for( node = ct->first; node; node = node->next )
	{	
		item = node->item;
		out_long( *(long*)item->key );
	}

	ct = &s_const_tab[ 1 ];
	out_word( ct->count );
		
	for( node = ct->first; node; node = node->next )
	{	
		item = node->item;
		out_wide( *(s64*)item->key );
	}

	ct = &s_const_tab[ 2 ];
	out_word( ct->count );
		
	for( node = ct->first; node; node = node->next )
	{	
		item = node->item;
		out_float( *(float*)item->key );
	}
	
	ct = &s_const_tab[ 3 ];
	out_word( ct->count );
		
	for( node = ct->first; node; node = node->next )
	{	
		item = node->item;
		out_double( *(double*)item->key );
	}
	
	ct = &s_const_tab[ 4 ];
	out_word( ct->count );
	
	long				offset = 0;
	for( node = ct->first; node; node = node->next )
	{
		out_long( offset );
		offset += node->item->keyl + 1;
	}

	out_long( ct->size );
		
	for( node = ct->first; node; node = node->next )
	{	
		item = node->item;
		out_block( item->key, item->keyl + 1 );
	}
}

static void
init_const_tables()
{
	for( int kind = 0; kind < 5; kind++ )
	{
		const_tab_t*	ct = &s_const_tab[ kind ];
	
		ct->htab = 0;
		ct->size = 0;
		ct->count = 0;
		ct->first = 0;
		ct->link = &ct->first;
	}
}

static void
write_index(
	unsigned long		index )
{
	if( index < 0xff00 )
		out_word( index );
	else
		out_long( 0xff000000 | index );
}

static void
write_ival(
	intval_t			val,
	e_operand_size		size )
{
	switch( size )
	{
		case OPS_BYTE:
			out_byte( val );
			break;
			
		case OPS_WORD:
			out_word( val );
			break;
			
		case OPS_QUAD:
			write_index( index_qconst( val ) );
			break;
			
		case OPS_OCTA:
			write_index( index_oconst( val ) );
			break;
			
		default:
			compile_error( 0, ERR_INTERNAL );
			return;
	}
}

static void
write_fval(
	fltval_t			val,
	e_operand_size		size )
{
	switch( size )
	{
		case OPS_SINGLE:
			write_index( index_fconst( val ) );
			break;
			
		case OPS_DOUBLE:
			write_index( index_dconst( val ) );
			break;
			
		default:
			compile_error( 0, ERR_INTERNAL );
			return;
	}
}

// ---------------------------------------------------------------------------

extern func_t*			s_compiled_func;

unsigned long			s_code_zero_offset;

static void
gentree(
	code_t*				node );

static unsigned long
register_class(
	class_t*			clss );

static void
write_inst(
	int					inst,
	e_operand_size		size )
{
	switch( size )
	{
		case OPS_BYTE:
		case OPS_SINGLE:
			break;
			
		case OPS_WORD:
		case OPS_DOUBLE:
			inst |= 1 << 6;
			break;
			
		case OPS_QUAD:
			inst |= 2 << 6;
			break;
			
		case OPS_OCTA:
			inst |= 3 << 6;
			break;
			
		default:
			compile_error( 0, ERR_INTERNAL );
			return;
	}
	
	out_byte( inst );
}

static void
geniconst(
	iconst_code_t*		node )
{
	write_inst( BYTECODE_LDI, node->size );
	write_ival( node->val, node->size );
}

static void
genfconst(
	fconst_code_t*		node )
{
	write_inst( BYTECODE_FLDI, node->size );
	write_fval( node->val, node->size );
}

static void
genstrconst(
	str_code_t*			node )
{
	write_inst( BYTECODE_SLDI, OPS_PTR );
	write_index( index_strconst( node->str ) );
}


static void
gentvaroffset(
	tvar_code_t*		node )
{
	write_inst( BYTECODE_LDI, OPS_PTR );
	write_ival( node->tvar->offset, OPS_PTR );
}

static void
genbinary(
	int					opcode,
	code_t*				node )
{
	gentree( ( (binary_code_t*)node )->child[ 1 ] );
	gentree( ( (binary_code_t*)node )->child[ 0 ] );
	write_inst( opcode, node->size );
}

static void
genunary(
	int					opcode,
	code_t*				node )
{
	gentree( ( (unary_code_t*)node )->child );
	write_inst( opcode, node->size );
}

struct call_t {
	func_t*				func;
	unsigned long		index;
	call_t*				next;
};

static call_t*			s_calls = 0;
static call_t**			s_calls_link;
static unsigned long	s_call_count = 0;

static void
write_call_table()
{
	if( s_call_count > 0xffff )
	{
		compile_error(
			&s_compiled_func->decl->spec->filepos,
			ERR_INTERNAL );
		return;
	}

	out_word( s_call_count );

	call_t*				node = s_calls;
	
	while( node )
	{
		func_t*			func = node->func;
		
		out_word( register_class(
			func->scope->clss ) );
		
		output_func_id( func );
		
		node = node->next;
	}
}

static unsigned long
register_call(
	func_code_t*		code )
{
	func_t*				func = code->func;
	call_t*				call;

	if( func->comp == s_compiled_func )
	{
		call = (call_t*)func->call;
		return call->index;
	}
	
	unsigned long		count = s_call_count;
	
	call = (call_t*)xalloc( sizeof( call_t ) );
	call->func = func;
	call->index = count;
	
	call->next = 0;
	*s_calls_link = call;
	s_calls_link = &call->next;
	
	s_call_count = count + 1;

	func->comp = s_compiled_func;
	func->call = call;
	
	return count;
}

static void
genparam(
	code_t*				param )
{
	if( param->inst == CODE_PARAM )
	{
		genparam( ( (binary_code_t*) param )->child[ 0 ] );
		genparam( ( (binary_code_t*) param )->child[ 1 ] );
	}
	else
		gentree( param );
}

static void
gencall(
	func_code_t*		node )
{
	code_t*				param = node->param;

	if( param )
		genparam( param );
	
	int					opcode;

	switch( node->inst )
	{
		case INST_CVF:
			gentree( node->instance );
			opcode = BYTECODE_CVF;
			break;
		
		case INST_CRF:
			gentree( node->instance );
			opcode = BYTECODE_CRF;
			break;
	
		case INST_CSF:
			opcode = BYTECODE_CSF;
			break;
	}

	write_inst( opcode, OPS_WORD );

	out_word( register_call( node ) );
}

static size_t
compiled_func_param_size()
{
	size_t				paramsize = 0;
	decl_t*				param = s_compiled_func->param;
	
	while( param )
	{
		paramsize += get_type_stack_size( param->type );
		param = param->next;
	}

	return paramsize;
}

static void
gentailcall(
	func_code_t*		node )
{
	code_t*				param = node->param;

	if( param )
		genparam( param );
	
	int					opcode;

	switch( node->inst )
	{
		case INST_TCVF:
			gentree( node->instance );
			opcode = BYTECODE_TCVF;
			break;

		case INST_TCRF:
			gentree( node->instance );
			opcode = BYTECODE_TCRF;
			break;

		case INST_TCSF:
			opcode = BYTECODE_TCSF;
			break;
	}

	write_inst( BYTECODE_EXTRA, OPS_PTR );
	write_inst( opcode, OPS_WORD );

	out_word( register_call( node ) );

	write_ival( compiled_func_param_size(),
		OPS_WORD );
}

static void
genrtinfo(
	rtinfo_code_t*		node )
{
	gentree( node->instance );
	write_inst( BYTECODE_RTINFO, OPS_PTR );
	write_index( register_class( node->clss ) );
}

static void
genreg(
	reg_code_t*			node )
{
	switch( node->reg )
	{
		case REG_SELF:
			write_inst( BYTECODE_PUSHSELF, OPS_PTR );
			break;
		
		case REG_LOCAL:
			write_inst( BYTECODE_PUSHLOCAL, OPS_PTR );
			break;

		case REG_STACK:
			write_inst( BYTECODE_PUSHSTACK, OPS_PTR );
			break;

		case REG_STATIC:
		{
			write_inst( BYTECODE_PUSHSTATIC, OPS_PTR );
		
			class_t*	clss;
			
			clss = ( (scope_t*)node->data )->clss;
			write_index( register_class( clss ) );
			break;
		}
		
		default:
			compile_error( 0, ERR_INTERNAL );
			break;
	}
}

static void
genxstore(
	xstore_code_t*		node )
{
	size_t				bytecount = node->bytecount;
	
	e_operand_size		size;
	
	if( bytecount <= 0xff )
		size = OPS_BYTE;
	else if( bytecount <= 0xffff )
		size = OPS_WORD;
	else
		size = OPS_QUAD;

	gentree( node->child[ 1 ] );
	gentree( node->child[ 0 ] );
	write_inst( BYTECODE_XSTORE, size );
	write_ival( bytecount, size );
}

static void
genxload(
	xload_code_t*		node )
{
	code_t*				child = node->child;

	if( child->inst == CODE_STACKHINT )
	{
		gentree( ( (binary_code_t*)child )->child[ 1 ] );
		return;
	}

	size_t				bytecount = node->bytecount;
	
	e_operand_size		size;
	
	if( bytecount <= 0xff )
		size = OPS_BYTE;
	else if( bytecount <= 0xffff )
		size = OPS_WORD;
	else
		size = OPS_QUAD;

	gentree( child );
	
	write_inst( BYTECODE_XLOAD, size );
	write_ival( bytecount, size );
}

struct class_inst_t {
	class_t*			clss;
	unsigned long		index;
	class_inst_t*		next;
};

static class_inst_t*	s_class = 0;
static class_inst_t**	s_class_link;
static unsigned long	s_class_count = 0;

static void
write_class_table()
{
	if( s_class_count > 0xffff )
	{
		compile_error(
			&s_compiled_func->decl->spec->filepos,
			ERR_INTERNAL );
		return;
	}

	out_word( s_class_count );

	class_inst_t*		node = s_class;
	
	while( node )
	{
		output_class_id( node->clss );
		node = node->next;
	}
}

static unsigned long
register_class(
	class_t*			clss )
{
	class_inst_t*		inst;

	if( clss->comp == s_compiled_func )
	{
		inst = (class_inst_t*)clss->inst;
		return inst->index;
	}
	
	unsigned long		count = s_class_count;
	
	inst = (class_inst_t*)xalloc( sizeof( class_inst_t ) );
	inst->clss = clss;
	inst->index = count;
	
	inst->next = 0;
	*s_class_link = inst;
	s_class_link = &inst->next;
	
	s_class_count = count + 1;

	clss->comp = s_compiled_func;
	clss->inst = inst;
	
	return count;
}

static void
gendyncast(
	dyncast_code_t*		node )
{
	gentree( node->child );
	write_inst( BYTECODE_DYNCAST, OPS_WORD );

	class_t*			clss = node->toclass;

	write_index( register_class( clss ) );
	
	//write_ival( node->offset, OPS_LONG );
}

static void
gennew(
	new_code_t*			node )
{
	gentree( node->arraysize );
	
	type_t*				elemtype = node->elemtype;

	if( elemtype->code == TYPE_CLASS )
	{
		class_t*		clss;
	
		clss = ( (class_type_t*)elemtype )->clss;
	
		write_inst( BYTECODE_NEW, OPS_WORD );
		write_index( register_class( clss ) );
	}
	else
	{
		long			elemsize;
		
		elemsize = get_type_size( elemtype );
	
		write_inst( BYTECODE_NEW, OPS_QUAD );
		write_ival( elemsize, OPS_QUAD );
	}
}

static void
gendtnew(
	dtnew_code_t*		node )
{
	gentree( node->arraysize );

	gentree( node->newname );
	
	type_t*				elemtype = node->elemtype;

	if( elemtype->code != TYPE_CLASS )
		compile_error( 0, ERR_INTERNAL );

	class_t*			clss;

	clss = ( (class_type_t*)elemtype )->clss;
		
	write_inst( BYTECODE_EXTRA, OPS_PTR );
	write_inst( BYTECODE_DYNNEW, OPS_WORD );
	write_index( register_class( clss ) );
}

static void
gendelete(
	delete_code_t*		node )
{
	gentree( node->delobject );
	
	type_t*				elemtype = node->elemtype;

	if( elemtype->code == TYPE_CLASS )
		write_inst( BYTECODE_DELETE, OPS_WORD );
	else
		write_inst( BYTECODE_DELETE, OPS_QUAD );
}

static void
genswitch(
	switch_code_t*		node )
{
	gentree( node->selector );

	intval_t			range;
	long				count;

	count = node->count;

	if( count < 1 )
		return;
	
	range = node->maxval - node->minval + 1;
	
	if( 0 && range * 2 <= count )
	{
	}
	else
	{
		write_inst( BYTECODE_EXTRA, OPS_PTR );
		write_inst( BYTECODE_SWITCH_LINEAR, OPS_PTR );
		out_word( count );
		
		switch_node_t*	block = node->nodes;
		
		while( block )
		{
			write_ival( block->value, OPS_QUAD );
			genlabel( block->label );
			block = block->next;
		}
		
		genlabel( node->defaultlabel );
	}
}

static void
gentry(
	try_code_t*			code )
{
	write_inst( BYTECODE_EXTRA, OPS_PTR );
	write_inst( BYTECODE_ENTER_TRY, OPS_PTR );
	
	out_word( code->count );
	
	catch_entry_t*		entry = code->entries;
	
	while( entry )
	{
		out_word( register_class( entry->clss ) );
		genlabel( entry->label );
		entry = entry->next;
	}
}

static void
genthrow(
	unary_code_t*		code )
{
	gentree( code->child );
	
	write_inst( BYTECODE_EXTRA, OPS_PTR );
	write_inst( BYTECODE_THROW, OPS_PTR );
}

struct local_access_t {
	long				offset;
	int					bytecode;
	class_t*			clss;
};

static bool
determine_local_access(
	binary_code_t*		node,
	local_access_t&		access )
{
	code_t*				child = node->child[ 0 ];
	
	if( child->inst == INST_ADD )
	{
		if( determine_local_access(
			(binary_code_t*)child,
			access ) == false )
		{
			return false;
		}
	}
	else if( child->inst != CODE_REG )
	{
		if( child->inst != INST_LDI )
			return false;
				
		access.offset += ( (iconst_code_t*)child )->val;
					
		child = node->child[ 1 ];
		if( child->inst != CODE_REG )
			return false;
			
		reg_code_t*		code = (reg_code_t*)child;
	
		if( code->reg != REG_STACK )
			return false;
			
		access.bytecode = BYTECODE_PUSHSTACK;
		
		return true;
	}
	else
	{
		reg_code_t*		code = (reg_code_t*)child;
	
		switch( code->reg )
		{
			case REG_LOCAL:
				access.bytecode = BYTECODE_PUSHLOCAL;
				break;
				
			case REG_STATIC:
				access.bytecode = BYTECODE_PUSHSTATIC;
				access.clss = ( (scope_t*)code->data )->clss;
				break;
					
			default:
				return false;
		}
	}
			
	child = node->child[ 1 ];
	switch( child->inst )
	{
		case INST_LDI:
			access.offset += ( (iconst_code_t*)child )->val;
			break;
		
		case CODE_TVAR:
			access.offset += ( (tvar_code_t*)child )->tvar->offset;
			break;
			
		default:
			return false;
	}
	
	return true;
}

static bool
optimize_local_access(
	binary_code_t*		node )
{
	local_access_t		access;
	
	access.offset = 0;
	access.clss = 0;
	
	if( determine_local_access( node, access ) == false )
		return false;

	long				offset = access.offset;
	int					bytecode = access.bytecode;
	class_t*			clss = access.clss;

	if( offset == 0 )
		write_inst( bytecode, OPS_PTR );
	else if( offset >= (-127-1) && offset <= 127 )
	{
		write_inst( bytecode, OPS_BYTE );
		out_byte( offset );
	}
	else if( offset >= (-32767-1) && offset <= 32767 )
	{
		write_inst( bytecode, OPS_WORD );
		out_word( offset );
	}
	else
		return false;

	if( clss )
		write_index( register_class( clss ) );

	return true;
}

static void
gentree(
	code_t*				node )
{
	switch( node->inst )
	{
		case INST_ADD:
			if( optimize_local_access( (binary_code_t*)node ) )
				break;
		
			genbinary( BYTECODE_ADD, node );
			break;
		
		case INST_SUB:
			genbinary( BYTECODE_SUB, node );
			break;
			
		case INST_MUL:
			genbinary( BYTECODE_MUL, node );
			break;
		
		case INST_DIV:
			genbinary( BYTECODE_DIV, node );
			break;
			
		case INST_MOD:
			genbinary( BYTECODE_MOD, node );
			break;

		case INST_NEG:
			genunary( BYTECODE_NEG, node );
			break;
		
		case INST_CMP:
			genbinary( BYTECODE_CMP, node );
			break;
			
		case INST_CMPU:
			genbinary( BYTECODE_CMPU, node );
			break;
			
		case INST_FADD:
			genbinary( BYTECODE_FADD, node );
			break;
		
		case INST_FSUB:
			genbinary( BYTECODE_FSUB, node );
			break;
		
		case INST_FMUL:
			genbinary( BYTECODE_FMUL, node );
			break;
			
		case INST_FDIV:
			genbinary( BYTECODE_FDIV, node );
			break;

		case INST_FMOD:
			genbinary( BYTECODE_FMOD, node );
			break;
			
		case INST_FNEG:
			genunary( BYTECODE_FNEG, node );
			break;
		
		case INST_FCMP:
			genbinary( BYTECODE_FCMP, node );
			break;
	
		case INST_NOT:
			genunary( BYTECODE_NOT, node );
			break;
			
		case INST_AND:
			genbinary( BYTECODE_AND, node );
			break;
		
		case INST_OR:
			genbinary( BYTECODE_OR, node );
			break;
			
		case INST_XOR:
			genbinary( BYTECODE_XOR, node );
			break;
		
		case INST_SHL:
			genbinary( BYTECODE_SHL, node );
			break;
		
		case INST_SHR:
			genbinary( BYTECODE_SHR, node );
			break;
			
		case INST_ASR:
			genbinary( BYTECODE_ASR, node );
			break;

		case INST_CSF:
		case INST_CVF:
		case INST_CRF:
			gencall( (func_code_t*)node );
			break;

		case INST_TCSF:
		case INST_TCVF:
		case INST_TCRF:
			gentailcall( (func_code_t*)node );
			break;
			
		case INST_RTINFO:
			genrtinfo( (rtinfo_code_t*)node );
			break;
	
		case INST_LDI:
			geniconst( (iconst_code_t*)node );
			break;
			
		case INST_FLDI:
			genfconst( (fconst_code_t*)node );
			break;
					
		case INST_STORE:
			genbinary( BYTECODE_STORE, node );
			break;
		
		case INST_LOAD:
			genunary( BYTECODE_LOAD, node );
			break;
			
		case INST_FSTORE:
			genbinary( BYTECODE_FSTORE, node );
			break;
		
		case INST_FLOAD:
			genunary( BYTECODE_FLOAD, node );
			break;
		
		case INST_XSTORE:
			genxstore( (xstore_code_t*)node );
			break;
						
		case INST_XLOAD:
			genxload( (xload_code_t*)node );
			break;
								
		case INST_EXTS:
			genunary( BYTECODE_EXTS, node );
			break;
		
		case INST_EXTU:
			genunary( BYTECODE_EXTU, node );
			break;

		case INST_RDC:
			genunary( BYTECODE_RDC, node );
			break;
			
		case INST_FEXT:
			genunary( BYTECODE_FEXT, node );
			break;
						
		case INST_ITOF:
			genunary( BYTECODE_ITOF, node );
			break;
			
		case INST_UTOF:
			genunary( BYTECODE_UTOF, node );
			break;
			
		case INST_FTOI:
			genunary( BYTECODE_FTOI, node );
			break;
			
		case INST_FTOU:
			genunary( BYTECODE_FTOU, node );
			break;

		case INST_DYNCAST:
			gendyncast( (dyncast_code_t*)node );
			break;
		
		case INST_NEW:
			gennew( (new_code_t*)node );
			break;

		case INST_DTNEW:
			gendtnew( (dtnew_code_t*)node );
			break;

		case INST_DELETE:
			gendelete( (delete_code_t*)node );
			break;

		case INST_SWITCH:
			genswitch( (switch_code_t*)node );
			break;

		case INST_DUP:
			genunary( BYTECODE_DUP, node );
			break;

		case INST_FDUP:
			genunary( BYTECODE_FDUP, node );
			break;
	
		case INST_ENTER_TRY:
			gentry( (try_code_t*)node );
			break;

		case INST_LEAVE_TRY:
			write_inst( BYTECODE_EXTRA, OPS_PTR );
			write_inst( BYTECODE_LEAVE_TRY, OPS_PTR );
			break;

		case INST_ENTER_CATCH:
			write_inst( BYTECODE_EXTRA, OPS_PTR );
			write_inst( BYTECODE_ENTER_CATCH, OPS_PTR );
			break;

		case INST_LEAVE_CATCH:
			write_inst( BYTECODE_EXTRA, OPS_PTR );
			write_inst( BYTECODE_LEAVE_CATCH, OPS_PTR );
			break;
	
		case INST_THROW:
			genthrow( (unary_code_t*)node );
			break;
	
		case CODE_STR:
			genstrconst( (str_code_t*)node );
			break;

		case CODE_TVAR:
			gentvaroffset( (tvar_code_t*)node );
			break;
			
		case CODE_REG:
			genreg( (reg_code_t*)node );
			break;

		case CODE_RET:
			gentree( ( (unary_code_t*)node )->child );
			break;
			
		case CODE_SEQ:
		case CODE_SEQ_NEUTRAL:
		case CODE_SEQ_POSTOP:
		case CODE_DESTROY:
			gentree( ( (binary_code_t*)node )->child[ 0 ] );
			gentree( ( (binary_code_t*)node )->child[ 1 ] );
			break;
	
		case CODE_STACKHINT:
			gentree( ( (binary_code_t*)node )->child[ 0 ] );
			break;
	
		case CODE_POP:
			genpop( (pop_code_t*)node );
			break;
							
		default:
			compile_error( 0, ERR_INTERNAL );
	}
}

struct bra_t {
	pos_t				pos;
	long				offset;
	block_t*			target;
	bra_t*				next;
};

static bra_t*			s_unresolved_bras = 0;

static void
write_bra_distance(
	long				distance )
{
	if( distance > 32767 || distance < (-32767-1) )
	{
		compile_error(
			&s_compiled_func->decl->spec->filepos,
			ERR_BRANCH_TOO_LONG );
	}

	out_word( distance );
}

static void
resolve_bras()
{
	pos_t				old_pos;
	
	out_tell_pos( &old_pos );

	bra_t*				node = s_unresolved_bras;
	
	while( node )
	{
		out_seek_pos( &node->pos );
		write_bra_distance( node->target->outoffset - node->offset );
		node = node->next;
	}
	
	out_seek_pos( &old_pos );
}

static void
genlabel(
	label_t*			target )
{
	block_t*			block = target->block;
	
	if( !block )
	{
		compile_error( 0, ERR_INTERNAL );
		return;
	}

	const long			offset = out_tell();
	
	const long			toffset = block->outoffset;
	
	if( toffset >= 0 )
	{
		write_bra_distance( toffset - offset );
	}
	else
	{
		bra_t*			node;
	
		node = (bra_t*)xalloc( sizeof( bra_t ) );
		out_tell_pos( &node->pos );
		node->offset = offset;
		node->target = block;
		node->next = s_unresolved_bras;
		s_unresolved_bras = node;
	
		out_word( 0 );
	}
}


static void
genbranch(
	int					opcode,
	inst_t*				inst )
{
	write_inst( opcode, OPS_WORD );	
	genlabel( ( (bra_inst_t*)inst )->target );
}

static void
genrts()
{
	size_t				paramsize;

	paramsize = compiled_func_param_size();
	
	size_t				retsize;
	
	retsize = get_type_stack_size( s_compiled_func->decl->type );
	
	e_operand_size		size;
	
	if( retsize <= 0xff && paramsize <= 0xff )
		size = OPS_BYTE;
	else if( retsize <= 0xffff && paramsize <= 0xffff  )
		size = OPS_WORD;
	else
		size = OPS_QUAD;

	write_inst( BYTECODE_RTS, size );
	write_ival( paramsize, size );
	write_ival( retsize, size );
}

static void
genpop(
	pop_code_t*			code )
{
	gentree( code->child );
	
	long				amount = code->amount;

	if( !amount )
		return;

	if( amount & 3 )
		amount += 4 - ( amount & 3 );

	e_operand_size		size;
	
	if( amount <= 0xff )
		size = OPS_BYTE;
	else if( amount <= 0xffff )
		size = OPS_WORD;
	else
		size = OPS_QUAD;

	write_inst( BYTECODE_POP, size );
	write_ival( amount, size );
}

static void
gendestroy(
	class_t*			clss )
{
	func_t*				func = clss->cd_func[ DESTRUCTOR ];

	if( func == 0 )
		return;
	
	code_t*				instance;
	code_t*				offset;
		
	instance = make_reg_code(
		OPS_PTR, REG_STACK, 0 );
	offset = make_iconst_code(
		OPS_PTR, true, -get_class_size( clss ) );
	instance = make_binary_code(
		INST_ADD, OPS_PTR, offset, instance );

	func_code_t*		node;

	node = (func_code_t*)make_func_code(
		INST_CRF, func, instance, 0 );

	gencall( node );
}

inline int
opstacksize(
	e_operand_size		size )
{
	switch( size )
	{
		case OPS_BYTE:
			return 1;
	
		case OPS_WORD:
			return 2;
		
		case OPS_QUAD:
		case OPS_SINGLE:
			return 4;
			
		case OPS_OCTA:
		case OPS_DOUBLE:
			return 8;
			
		case OPS_WIDE:
			return 16;
	}
	
	return 0;
}

static code_t*
popunused(
	code_t*				node )
{
	long				popsize = 0;

	switch( node->inst )
	{
		case CODE_RET:
		case INST_RTINFO:
		case INST_DELETE:
		case INST_SWITCH:

		case INST_CMP:
		case INST_CMPU:
		case INST_FCMP:
		
		case INST_ENTER_TRY:
		case INST_LEAVE_TRY:
		case INST_ENTER_CATCH:
		case INST_LEAVE_CATCH:
		case INST_THROW:

		case CODE_SEQ_NEUTRAL:
			break;

		case INST_ADD:
		case INST_MUL:
		case INST_DIV:
		case INST_MOD:
		case INST_NEG:
		case INST_FADD:
		case INST_FSUB:
		case INST_FMUL:
		case INST_FDIV:
		case INST_FMOD:
		case INST_FNEG:
		case INST_NOT:
		case INST_AND:
		case INST_OR:
		case INST_XOR:
		case INST_SHL:
		case INST_SHR:
		case INST_ASR:
		
		case INST_LDI:
		case INST_FLDI:
		case INST_LOAD:
		case INST_FLOAD:
		case INST_STORE:
		case INST_FSTORE:
		
		case INST_ITOF:
		case INST_UTOF:
		case INST_FTOI:
		case INST_FTOU:
		
		case INST_EXTS:
		case INST_EXTU:
		case INST_RDC:
		case INST_FEXT:
		
		case INST_DUP:
		case INST_FDUP:
			popsize = opstacksize( node->size );
			break;
		
		case INST_DYNCAST:
		case INST_NEW:
		case INST_DTNEW:
		case CODE_STR:
		case CODE_TVAR:
		case CODE_REG:
			popsize = opstacksize( OPS_PTR );
			break;
		
		case INST_XSTORE:
			popsize = ( (xstore_code_t*)node )->bytecount;
			break;

		case INST_XLOAD:
			//popsize = ( (xload_code_t*)node )->bytecount;
			node = ( (xload_code_t*)node )->child;
			popsize = opstacksize( OPS_PTR );
			break;
							
		case INST_CRF:
		case INST_CSF:	
		case INST_CVF:
		case INST_TCRF:
		case INST_TCSF:
		case INST_TCVF:
			popsize = get_type_size(
				( (func_code_t*)node )->func->decl->type );
			break;
		
		case CODE_SEQ:
		case CODE_SEQ_POSTOP:
			( (binary_code_t*)node )->child[ 0 ] =
				popunused( ( (binary_code_t*)node )->child[ 0 ] );
			( (binary_code_t*)node )->child[ 1 ] =
				popunused( ( (binary_code_t*)node )->child[ 1 ] );
			break;
		
		case CODE_DESTROY:
		case CODE_STACKHINT:
			( (binary_code_t*)node )->child[ 0 ] =
				popunused( ( (binary_code_t*)node )->child[ 0 ] );
			break;

		case CODE_POP:
			( (unary_code_t*)node )->child =
				popunused( ( (unary_code_t*)node )->child );
			break;
	
		default:
			compile_error( 0, ERR_INTERNAL );
			break;
	}
	
	if( popsize ) 
	{
		node = make_pop_code(
			node, get_stack_size( popsize ) );
	}

	return node;
}

static void
geninst(
	inst_t*				inst )
{
	switch( inst->code )
	{
		case INST_TREE:
		{
			code_inst_t*	node = (code_inst_t*)inst;
			code_t*			code = popunused( node->tree );
			
			update_codepoint( node->codepoint );
					
			gentree( code );
			break;
		}
	
		case INST_BRA:
			genbranch( BYTECODE_BRA, inst );
			break;
		
		case INST_BEQ:
			genbranch( BYTECODE_BEQ, inst );
			break;
			
		case INST_BNE:
			genbranch( BYTECODE_BNE, inst );
			break;
			
		case INST_BGT:
			genbranch( BYTECODE_BGT, inst );
			break;
			
		case INST_BLT:
			genbranch( BYTECODE_BLT, inst );
			break;
		
		case INST_BGE:
			genbranch( BYTECODE_BGE, inst );
			break;
		
		case INST_BLE:
			genbranch( BYTECODE_BLE, inst );
			break;
		
		case INST_RTS:
			genrts();
			break;
	}
}

static void
init_backend()
{
	init_const_tables();
	s_unresolved_bras = 0;

	s_calls = 0;
	s_calls_link = &s_calls;
	s_call_count = 0;

	s_class = 0;
	s_class_link = &s_class;
	s_class_count = 0;
}

static void
exit_backend()
{
	resolve_bras();
	
	write_const_tables();
	
	write_call_table();

	write_class_table();
}

void
gencode(
	block_t*			block )
{
	pos_t				start_pos;
	
	out_tell_pos( &start_pos );
	out_long( 0 );
	s_code_zero_offset = out_tell();

	init_backend();

	while( block )
	{
		block->outoffset = out_tell();
	
		inst_t*			inst = block->inst;
		
		while( inst )
		{
			geninst( inst );
			inst = inst->next;
		}
	
		block = block->next;
	}

	pos_t				end_pos;
	long				code_len;

	out_tell_pos( &end_pos );
	code_len = out_tell() - s_code_zero_offset;
	out_seek_pos( &start_pos );
	out_long( code_len );
	out_seek_pos( &end_pos );

	exit_backend();
	
	codepoint_t*		codepoint = fetch_codepoint();
	
	if( codepoint )
		codepoint->offset = code_len;
}

END_COMPILER_NAMESPACE
