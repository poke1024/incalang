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
//	inst_t.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "inst_t.h"
#include "xalloc.h"
#include "expr_t.h"
#include "makecode.h"
#include "scope_t.h"
#include "type_t.h"
#include "backend.h"
#include "tmpvar.h"
#include "parser.h"
#include "class_t.h"
#include "compbreak.h"

BEGIN_COMPILER_NAMESPACE

static block_t*			s_first_block;
static block_t*			s_block;
static block_t**		s_blink;
static inst_t**			s_ilink;

extern func_t*			s_compiled_func;

static block_t*
new_block();

void
init_inst()
{
	init_tmp_vars();
	push_tmp_var_frame();
	
	s_block = 0;
	s_blink = &s_first_block;
	new_block();
	mark_codepoint();
}

static void
append_rts()
{
	block_t*			block = s_first_block;
	
	if( !block )
	{
		inst_rts();
		return;
	}
	
	while( block->next && block->next->inst )
		block = block->next;
		
	inst_t*				inst = block->inst;
	
	if( !inst )
	{
		inst_rts();
		return;
	}

	while( inst->next )
		inst = inst->next;

	if( inst->code != INST_RTS )
		inst_rts();
}

enum {
	BLOCK_VISIT_CHECK_RTS = 1,
	BLOCK_VISIT_OPTIMIZE_BRA,
	BLOCK_VISIT_OPTIMIZE_TC
};

static bool
check_rts(
	block_t*			block )
{
	while( true )
	{
next:
		if( !block )	
			break;	
			
		if( block->visit == BLOCK_VISIT_CHECK_RTS )		
			return true;

		block->visit = BLOCK_VISIT_CHECK_RTS;

		inst_t*				inst = block->inst;
		
		while( inst )
		{
			switch( inst->code )
			{
				case INST_TREE:
				{
					code_t*	node = ( (code_inst_t*)inst )->tree;
			
retry:
					switch( node->inst )
					{
						case CODE_RET:
							return true;
							
						case CODE_DESTROY:
							node = ( (binary_code_t*)node )->child[ 0 ];
							goto retry;
					}
			
					break;
				}
			
				case INST_BEQ:
				case INST_BNE:
				case INST_BGT:
				case INST_BLT:
				case INST_BGE:
				case INST_BLE:
					if( !check_rts( block->next ) )
						return false;
						
					// fall through
			
				case INST_BRA:
					block = ( (bra_inst_t*)inst )->target->block;
					goto next;
			}
		
			inst = inst->next;
		}
		
		block = block->next;
	}
	
	return false;
}

static bool
is_return_block(
	block_t*			block,
	inst_t*				inst )
{
	while( block )
	{
		if( inst == 0 )
		{
			block = block->next;
			if( block == 0 )
				break;
			inst = block->inst;
		}
		else switch( inst->code )
		{
			case INST_BRA:
				block = ( (bra_inst_t*)inst )->target->block;
				inst = block->inst;
				break;

			case INST_RTS:
				return true;
		
			default:
				return false;
		}
	}

	return true;
}

static code_t*
find_tail_call_node(
	block_t*			block,
	inst_t*				inst,
	code_t*				node )
{
	func_t*				func;
	int					modifiers;

	switch( node->inst )
	{
		case CODE_RET:
			node = ( (unary_code_t*)node )->child;
			if( node )
				return find_tail_call_node(
					block, inst, node );
			break;
		
		case INST_CSF:
			if( is_return_block(
				block->next, inst->next ) == false )
				break;

			func = ( (func_code_t*)node )->func;

			if( is_same_type( func->decl->type,
				s_compiled_func->decl->type ) == false )
				break;

			modifiers = func->decl->spec->modifiers;

			if( modifiers & MOD_NATIVE )
				break;

			if( modifiers & MOD_DLL )
				break;

			return node;
	}

	return 0;
}

static void
optimize_tail_call(
	block_t*			block )
{
	while( block )
	{
		inst_t**			link = &block->inst;
		inst_t*				inst = *link;
		
		while( inst )
		{
			if( inst->code == INST_TREE )
			{
				code_t*		node;

				node = ( (code_inst_t*)inst )->tree;
	
				node = find_tail_call_node(
					block, inst, node );

				if( node )
				{
					node->inst = INST_TCSF;
				}
			}

			link = &inst->next;
			inst = *link;
		}
		
		block = block->next;
	}		
}

static void
optimize_bra(
	block_t*			block )
{
	while( true )
	{
		if( !block )	
			break;	
			
		/*if( block->visit == BLOCK_VISIT_OPTIMIZE_BRA )		
			return;

		block->visit = BLOCK_VISIT_OPTIMIZE_BRA;*/

		inst_t**			link = &block->inst;
		inst_t*				inst = *link;
		
		while( inst )
		{
			switch( inst->code )
			{
				case INST_BEQ:
				case INST_BNE:
				case INST_BGT:
				case INST_BLT:
				case INST_BGE:
				case INST_BLE:
				case INST_BRA:
				{
					block_t*	tblock;
					
					tblock = ( (bra_inst_t*)inst )->target->block;
					if( tblock == block->next )
						*link = inst->next;
					else
					{
						inst_t*	tinst = tblock->inst;
						if( tinst && tinst->code == INST_BRA )
						{
							( (bra_inst_t*)inst )->target =
								( (bra_inst_t*)tinst )->target;
						}
					}
					
					goto next;
					//block = block->next;
				}
			}
		
			link = &inst->next;
			inst = *link;
		}

next:
		block = block->next;
	}
}

void
finish_inst()
{
	code_t*				code;
	
	code = pop_tmp_var_frame();
	if( code )
		inst_code( code );

#if ICARUS_SAFE
	if( code )
		throw_icarus_error( "internal error in finish_inst()" );
	// the tmpvar frame here should be empty. all tmp vars
	// should be enclosed in the frame needing them. the one and
	// only use for tmp vars are functions returning classes and
	// giving them as reference parameters to other functions.
	// the make_func_code routine should have taken care of this.
#endif

	*s_blink = 0;
	*s_ilink = 0;

	if( s_compiled_func->decl->type->code == TYPE_VOID )
		append_rts();
	else if( !check_rts( s_first_block ) )
	{
		compile_error(
			&s_compiled_func->decl->spec->filepos,
			ERR_EXPECTED_RETURN_VALUE,
			s_compiled_func->decl->name );
		
		return;
	}
		
	optimize_bra( s_first_block );
	
	optimize_tail_call( s_first_block );

	locate_tmp_vars();
	
	gencode( s_first_block );
}

e_operand_size
get_type_operand_size(
	type_t*				type )
{
	type = reduce_type( type );

	switch( type->code )
	{
		case TYPE_VOID:
			return OPS_NONE;
	
		case TYPE_BOOL:
		case TYPE_BYTE:
		case TYPE_CHAR:
			return OPS_BYTE;
			
		case TYPE_SHORT:
			return OPS_WORD;
			
		case TYPE_INT:
		case TYPE_POINTER:
		case TYPE_REFERENCE:
		case TYPE_NULL:
		case TYPE_ENUM:
			return OPS_QUAD;

		case TYPE_LONG:
			return OPS_OCTA;
			
		case TYPE_WIDE:
			return OPS_WIDE;
			
		case TYPE_FLOAT:
			return OPS_SINGLE;
			
		case TYPE_DOUBLE:
			return OPS_DOUBLE;
	}
	
	compile_error( 0, ERR_INTERNAL );
	return OPS_NONE;
}

bool
is_type_int_operand(
	type_t*				type )
{
	type = reduce_type( type );

	switch( type->code )
	{
		TYPE_CASE_INTEGER
		case TYPE_BOOL:
		case TYPE_ENUM:
		case TYPE_POINTER:
		case TYPE_REFERENCE:
		case TYPE_NULL:
			return true;
	}

	return false;
}

static inst_t*
alloc_inst(
	e_inst_code			code,
	size_t				memsize )
{
	inst_t*				inst;
	
	inst = (inst_t*)xalloc( memsize );
	inst->code = code;
	inst->next = 0;
	
	*s_ilink = inst;
	s_ilink = &inst->next;
	
	return inst;
}

static block_t*
new_block()
{
	block_t*			block;

	block = s_block;
	if( block && block->inst == 0 )
		return block;
	
	block = (block_t*)xalloc( sizeof( block_t ) );
	block->inst = 0;
	block->next = 0;
	block->visit = 0;
	
	block->outoffset = -1;
	
	s_ilink = &block->inst;
	*s_blink = block;
	s_blink = &block->next;
	
	s_block = block;
	
	return block;
}

void
inst_code(
	code_t*				tree )
{
	code_inst_t*		inst;
	
	inst = (code_inst_t*)alloc_inst(
		INST_TREE, sizeof( code_inst_t ) );
			
	inst->tree = tree;
	
	inst->codepoint = fetch_codepoint();
}

void
inst_bra(
	e_inst_code			code,
	label_t*			target )
{
	bra_inst_t*			inst;
	
	inst = (bra_inst_t*)alloc_inst(
		code, sizeof( bra_inst_t ) );

	inst->target = target;

	new_block();
}

void
inst_rts()
{
	inst_t*				inst;
	
	inst = (bra_inst_t*)alloc_inst(
		INST_RTS, sizeof( inst_t ) );

	new_block();
}

label_t*
make_label()
{
	label_t*			label;
	
	label = (label_t*)xalloc( sizeof( label_t ) );
	label->block = 0;
	
	return label;
}

void
put_label(
	label_t*			label )
{
	label->block = new_block();
}

// ---------------------------------------------------------------------------

static void
inst_logical_and_expr(
	binary_expr_t*		expr,
	label_t*			labelfalse,
	bool				negate )
{
	inst_logical_expr( expr->child[ 0 ], labelfalse, negate );
	inst_logical_expr( expr->child[ 1 ], labelfalse, negate );
}

static void
inst_logical_or_expr(
	binary_expr_t*		expr,
	label_t*			labelfalse,
	bool				negate )
{
	label_t*			labelttrue;

	labelttrue = make_label();
	inst_logical_expr( expr->child[ 0 ], labelttrue, !negate );
	inst_logical_expr( expr->child[ 1 ], labelfalse, negate );
	put_label( labelttrue );
}

static void
inst_logical_cmp_expr(
	expr_t*				expr,
	label_t*			labelfalse,
	e_inst_code			opcode )
{
	code_t*				code;

	code = make_cmp_code( (binary_expr_t*)expr );
	
	inst_code( code );
	inst_bra( opcode, labelfalse );
}

void
inst_logical_expr(
	expr_t*				expr,
	label_t*			labelfalse,
	bool				negate )
{
	switch( expr->code )
	{
		case EXPR_LOGICAL_NOT:
			inst_logical_expr(
				( (unary_expr_t*)expr )->child,
				labelfalse, !negate );
			break;
	
		case EXPR_LOGICAL_AND:
			if( !negate )
				inst_logical_and_expr(
					(binary_expr_t*)expr, labelfalse, false );
			else
				inst_logical_or_expr(
					(binary_expr_t*)expr, labelfalse, true );
			break;
			
		case EXPR_LOGICAL_OR:
			if( !negate )
				inst_logical_or_expr(
					(binary_expr_t*)expr, labelfalse, false );
			else
				inst_logical_and_expr(
					(binary_expr_t*)expr, labelfalse, true );
			break;
	
		case EXPR_EQ:
			inst_logical_cmp_expr( expr, labelfalse,
				!negate ? INST_BNE : INST_BEQ );
			break;
			
		case EXPR_NE:
			inst_logical_cmp_expr( expr, labelfalse,
				!negate ? INST_BEQ : INST_BNE );
			break;
			
		case EXPR_LT:
			inst_logical_cmp_expr( expr, labelfalse,
				!negate ? INST_BGE : INST_BLT );
			break;
			
		case EXPR_GT:
			inst_logical_cmp_expr( expr, labelfalse,
				!negate ? INST_BLE : INST_BGT );
			break;
	
		case EXPR_LE:
			inst_logical_cmp_expr( expr, labelfalse,
				!negate ? INST_BGT : INST_BLE );
			break;
			
		case EXPR_GE:
			inst_logical_cmp_expr( expr, labelfalse,
				!negate ? INST_BLT : INST_BGE );
			break;
	
		default:
		{
			if( expr->code == EXPR_ICONST )
			{
				if( ( (iconst_expr_t*)expr )->val )
				{
					if( negate )
						inst_bra( INST_BRA, labelfalse );
				}
				else if( negate == false )
					inst_bra( INST_BRA, labelfalse );
				break;
			}
		
			variable_t*	var;
					
			var = inst_arith_expr( expr );

			type_t*		type = var->decl->type;
			
			code_t*		node;
			code_t*		left;
			code_t*		right;
	
			left = make_var_code( var, 0 );
			left = lval_to_rval_code( left, type );
		
			e_operand_size	opsize;
			
			opsize = get_type_operand_size( type );
				
			if( is_type_int_operand( type ) )
			{
				right = make_iconst_code( opsize, false, 0 );
			
				node = make_binary_code( INST_CMP,
					opsize, left, right );
			}
			else
			{
				right = make_fconst_code( opsize, 0 );
				
				node = make_binary_code( INST_FCMP,
					opsize, left, right );
			}
			
			inst_code( node );
			
			free_tmp_var( var );
			
			inst_bra( !negate ? INST_BEQ : INST_BNE, labelfalse );
			
			break;
		}
	}
}

static void
inst_load_const(
	variable_t*			var,
	type_t*				type,
	int					val )
{
	e_operand_size		size;
	
	size = get_type_operand_size( type );

	e_inst_code			inst;
	code_t*				left;
	code_t*				right;

	if( is_type_int_operand( type ) )
	{
		inst = INST_STORE;
		left = make_var_code( var, 0 );
		right = make_iconst_code( size, false, val );
	}
	else
	{
		inst = INST_FSTORE;
		left = make_var_code( var, 0 );
		right = make_fconst_code( size, val );
	}

	code_t*				node;
	
	node = make_binary_code( inst, size, left, right );

	inst_code( node );	
}

variable_t*
inst_arith_expr(
	expr_t*				expr )
{
	variable_t*			var;

	switch( expr->code )
	{
		case EXPR_LOGICAL_NOT:
		case EXPR_LOGICAL_AND:
		case EXPR_LOGICAL_OR:
		case EXPR_EQ:
		case EXPR_NE:
		case EXPR_LT:
		case EXPR_GT:
		case EXPR_LE:
		case EXPR_GE:
		{
			label_t*	labelfalse;
			label_t*	labeldone;
		
			labelfalse = make_label();
			labeldone = make_label();
			
			inst_logical_expr( expr, labelfalse, false );
			
			type_t*		type = expr->type;
			
			var = take_tmp_var( type );
			
			inst_load_const( var, type, 1 );
			inst_bra( INST_BRA, labeldone );
			
			put_label( labelfalse );
			inst_load_const( var, type, 0 );
			
			put_label( labeldone );
			
			break;
		}
		
		case EXPR_ID:
			return ( (id_expr_t*)expr )->var;
			
		default:
		{
			code_t*		node;
			type_t*		type = expr->type;
		
			node = make_rval_code( expr );
			var = code_tmp_var( &node, type );
						
			inst_code( node );

			break;
		}
	}
	
	return var;
}

void
inst_variable_cd_code(
	variable_t*			var,
	int					index )
{
	type_t*				type = var->decl->type;
	
	if( type->code != TYPE_CLASS )
		return;
		
	class_t*			clss;
	
	clss = ( (class_type_t*)type )->clss;

	code_t*				instance;

	instance = make_var_code( var, 0 );
	
	if( index == CONSTRUCTOR )
	{
		code_t*			node;

		node = make_rtinfo_code( instance, clss );
		inst_code( node );
	}

	func_t*				func = clss->cd_func[ index ];
	
	if( !func )
		return;

	code_t*				node;
	
	node = make_func_code( INST_CRF,
		func, instance, 0 );
	inst_code( node );
}

void
inst_inherited_cd_code(
	class_t*			clss,
	class_t*			baseclss,
	int					index )
{
	func_t*				func = baseclss->cd_func[ index ];
	
	if( !func )
		return;
	
	code_t*				node;
	code_t*				offset;
	unsigned long		intoffset;
	
	if( get_base_class_offset( clss, baseclss, &intoffset ) != FOUND )
		compile_error( 0, ERR_INTERNAL );
	offset = make_iconst_code( OPS_PTR, false, intoffset );
	
	node = make_reg_code( OPS_PTR, REG_SELF, 0 );
	node = make_binary_code( INST_ADD, OPS_PTR, node, offset );
	node = make_func_code( INST_CRF, func, node, 0 );
	inst_code( node );
}

END_COMPILER_NAMESPACE
