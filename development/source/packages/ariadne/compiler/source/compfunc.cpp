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
//	compfunc.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "class_t.h"
#include "stmt_t.h"
#include "makecode.h"
#include "inst_t.h"
#include "parser.h"
#include "scope_t.h"
#include "expr_t.h"
#include "type_t.h"
#include "xalloc.h"
#include "output.h"
#include "xhtab_t.h"
#include "compiler_t.h"
#include "compbreak.h"
#include "lifetime.h"
#include "templates.h"

#include "tmpvar.h"

BEGIN_COMPILER_NAMESPACE

enum {
	L_CONTINUE = 0,
	L_BREAK = 1
};

struct stmt_block_t {
	label_t*			label[ 2 ];
	switch_code_t*		switchcode;
};

stmt_block_t*			s_block = 0;

#define PUSH_BLOCK(cl,bl,sc)										\
	stmt_block_t*		old_block = s_block;						\
	stmt_block_t		new_block;									\
	new_block.label[ L_CONTINUE ] = (cl);							\
	new_block.label[ L_BREAK ] = (bl);								\
	new_block.switchcode = (sc);									\
	s_block = &new_block;
	
#define POP_BLOCK													\
	s_block = old_block;

static void
put_block_label(
	int					index )
{
	stmt_block_t*		block = s_block;
	
	if( !block )
		return;
	
	label_t*			label = block->label[ index ];
	
	if( label )
		put_label( label );
}

// ---------------------------------------------------------------------------

func_t*					s_compiled_func = 0;

unsigned long			s_local_stack_size = 0;

bool					s_init_declaration = false;

static scope_t*			s_param_scope = 0;

static label_t*			s_function_exit;

extern bool				g_template_compiled;

// ---------------------------------------------------------------------------

static void
update_local_stack_size(
	scope_t*			scope )
{
	unsigned long		stack_size = scope->offset;
	
	if( stack_size > s_local_stack_size )
		s_local_stack_size = stack_size;
}

static void
parse_stmt(
	stmt_t*				stmt );

static void
tear_down_local_scope(
	scope_t*			scope );

static void
parse_expr(
	expr_t*				expr )
{
	push_tmp_var_frame();
	
	mark_breakpoint( &expr->filepos );

	code_t*				node;
		
	node = make_rval_code( expr );

	inst_code( node );
	
	code_t*				code;
	
	code = pop_tmp_var_frame();
	
	if( code )
		inst_code( code );
}

static void
parse_expr_stmt(
	expr_stmt_t*		stmt )
{
	expr_t*				expr;
	
	if( ( expr = stmt->expr ) == 0 )
		return;
	
	if( type_expr( expr ) == false )
		return;

	expr = eval_expr( expr );

	parse_expr( expr );
}

static void
parse_if_stmt(
	if_stmt_t*			stmt )
{
	expr_t*				expr = stmt->expr;
	
	if( type_expr( expr ) == false )
		return;
		
	if( is_type_boolean( expr->type ) == false )
	{
		compile_error( &expr->filepos,
			ERR_ILLEGAL_TYPE );
		return;
	}

	expr = eval_expr( expr );

	label_t*			labelfalse;
	label_t*			labeldone;
	
	labelfalse = make_label();
	labeldone = make_label();

	inst_logical_expr( expr, labelfalse, false );

	parse_stmt( stmt->stmt_true );
	inst_bra( INST_BRA, labeldone );
	
	put_label( labelfalse );
	parse_stmt( stmt->stmt_false );
	
	put_label( labeldone );
}

static void
parse_cond_stmt(
	cond_stmt_t*		stmt )
{
	expr_t*				expr = stmt->expr;
	
	if( type_expr( expr ) == false )
		return;
	
	if( is_type_boolean( expr->type ) == false )
	{
		compile_error( &expr->filepos,
			ERR_ILLEGAL_TYPE );
		return;
	}

	expr = eval_expr( expr );
	
	bool				is_while;
	
	is_while = ( stmt->code == STMT_WHILE );
	
	label_t*			labelcond = 0;
	label_t*			labelstart;
	
	if( is_while )
		labelcond = make_label();

	labelstart = make_label();
	
	PUSH_BLOCK( labelcond, 0, 0 )
	
	if( is_while )
		inst_bra( INST_BRA, labelcond );

	put_label( labelstart );
	parse_stmt( stmt->stmt );

	if( is_while )
		put_label( labelcond );
	else
		put_block_label( L_CONTINUE );
		
	inst_logical_expr( expr, labelstart, true );
	
	put_block_label( L_BREAK );
	POP_BLOCK
}

static void
parse_for_stmt(
	for_stmt_t*			stmt )
{
	expr_t*				init = stmt->init;
	expr_t*				iter = stmt->iter;
	expr_t*				cond = stmt->cond;

	if( ( init && type_expr( init ) == false ) ||
		( iter && type_expr( iter ) == false ) )
		return;

	if( cond )
	{
		if( type_expr( cond ) == false )
			return;
		
		if( is_type_boolean( cond->type ) == false )
		{
			compile_error( &cond->filepos,
				ERR_ILLEGAL_TYPE );
			return;
		}
		
		cond = eval_expr( cond );
	}
	
	if( init )
		init = eval_expr( init );
	if( iter )
		iter = eval_expr( iter );
	
	label_t*			labelcond;
	label_t*			labelstart;
	
	labelstart = make_label();

	PUSH_BLOCK( 0, 0, 0 )

	if( cond )
	{
		labelcond = make_label();
		
		if( init )
			parse_expr( init );
		inst_bra( INST_BRA, labelcond );
		
		mark_breakpoint( &stmt->stmt->filepos );
		put_label( labelstart );
		parse_stmt( stmt->stmt );
		
		put_block_label( L_CONTINUE );
		if( iter )
			parse_expr( iter );
		put_label( labelcond );
		
		mark_breakpoint(  &cond->filepos );
		inst_logical_expr( cond, labelstart, true );
	}
	else
	{
		if( init )
			parse_expr( init );
		put_label( labelstart );
		parse_stmt( stmt->stmt );
		
		put_block_label( L_CONTINUE );
		if( iter )
			parse_expr( iter );

		inst_bra( INST_BRA, labelstart );
	}
	
	put_block_label( L_BREAK );
	POP_BLOCK
}

static stmt_t*
deep_first_stmt(
	stmt_t*				stmt )
{
	while( stmt )
	{
		if( stmt->code == STMT_BLOCK )
			stmt = ( (block_stmt_t*)stmt )->stmt;
		else
			break;
	}
	
	return stmt;
}

static void
parse_switch_stmt(
	cond_stmt_t*		stmt )
{
	expr_t*				expr = stmt->expr;

	if( type_expr( expr ) == false )
		return;

	switch_code_t*		switchcode;
	code_t*				node;
	label_t*			exitlabel;

	exitlabel = make_label();
	mark_breakpoint( &expr->filepos );
	node = make_rval_code( expr );
	switchcode = make_switch_code(
		node, expr->type );
	inst_code( switchcode );

	PUSH_BLOCK( 0, exitlabel, switchcode )
	
	stmt_t*				casecode;
	
	casecode = deep_first_stmt( stmt->stmt );

	if( casecode &&
		casecode->code != STMT_CASE &&
		casecode->code != STMT_DEFAULT )
	{
		compile_error( &casecode->filepos,
			ERR_CASE_EXPECTED );
		return;
	}

	parse_stmt( casecode );
	put_block_label( L_BREAK );
	
	POP_BLOCK

	if( switchcode->defaultlabel == 0 )
		switchcode->defaultlabel = exitlabel;
}

static void
parse_case_stmt(
	case_stmt_t*		stmt )
{
	expr_t*				expr = stmt->expr;
	
	if( expr == 0 )
	{
		compile_error( &stmt->filepos,
			ERR_SYNTAX_ERROR );
		return;
	}
	
	if( type_expr( expr ) == false )
		return;
			
	expr = eval_expr( expr );

	if( expr->code != EXPR_ICONST )
	{
		compile_error( &expr->filepos,
			ERR_EXPECTED_INTEGER_CONSTANT );
		return;
	}

	iconst_expr_t*		iexpr = (iconst_expr_t*)expr;
	intval_t			ival = iexpr->val;
	caseval_t			casevalue = (caseval_t)ival;
			
	if( casevalue != ival )
	{
		compile_error( &expr->filepos,
			ERR_INTEGER_CONSTANT_TOO_BIG );
	}

	switch_code_t*		switchcode = s_block->switchcode;
	
	if( switchcode == 0 )
	{
		compile_error( &stmt->filepos, ERR_CASE_WITHOUT_SWITCH );
		return;
	}
	
	label_t*			label = make_label();
	
	put_label( label );
	
	add_switch_node(
		switchcode, label, casevalue, &stmt->filepos );
}

static void
parse_default_stmt(
	stmt_t*				stmt )
{
	switch_code_t*		switchcode = s_block->switchcode;
	
	if( switchcode == 0 )
	{
		compile_error( &stmt->filepos, ERR_CASE_WITHOUT_SWITCH );
		return;
	}

	if( switchcode->defaultlabel )
	{
		compile_error( &stmt->filepos, ERR_DEFAULT_REDEFINED );
		return;
	}
	
	label_t*			label = make_label();
	
	put_label( label );
	
	switchcode->defaultlabel = label;
}

static void
parse_break_continue(
	stmt_t*				stmt,
	int					index )
{
	stmt_block_t*		block = s_block;
	
	if( !block )
	{
		compile_error( &stmt->filepos,
			ERR_SYNTAX_ERROR );
		return;
	}

	if( index == L_CONTINUE && block->switchcode )
	{
		compile_error( &stmt->filepos,
			ERR_CONTINUE_IN_SWITCH_BLOCK );
		return;
	}
	
	label_t*			label = block->label[ index ];
	
	if( !label )
	{
		label = make_label();
		block->label[ index ] = label;
	}

	inst_bra( INST_BRA, label );
}

static void
call_class_scope_cd(
	scope_t*			scope,
	int					index )
{
	xhtab_t*			t = scope->variables;
	
	if( !xhfirst( t ) )
		return;

	do {	
		variable_t*		var;
		
		var = (variable_t*)xhstuff( t );

		if( var->modifiers & MOD_STATIC )
			continue;

		type_t*			type = var->decl->type;
	
		if( type->code != TYPE_CLASS )
			continue;
	
		inst_variable_cd_code( var, index );

	} while( xhnext( t ) );
}

static void
call_inherited_cd(
	class_t*			clss,
	int					index )
{
	qualhead_t*			qualhead = clss->base;
	
	while( qualhead )
	{
		class_t*		baseclss;
		
		type_t*			type = qualhead->type;
	
		baseclss = ( (class_type_t*)type )->clss;
	
		inst_inherited_cd_code( clss, baseclss, index );
	
		qualhead = qualhead->next;
	}
}

static bool
call_decl_constructor(
	decl_t*				decl )
{
	type_t*				type;
	
	if( ( type = decl->type ) == 0 )
	{
		compile_error(
			&decl->spec->filepos, ERR_INTERNAL );
		return false;
	}

	if( is_type_abstract( type ) )
	{
		compile_error(
			&decl->spec->filepos, ERR_ABSTRACT_VARIABLE );
		return false;
	}			

	if( type->code != TYPE_CLASS )
		return true;

	// do the error reporting for constructor as well
	// as destructor access failures here
	for( int cd = CONSTRUCTOR; cd <= DESTRUCTOR; cd++ )
	{
		if( check_if_cd_access_valid(
			&decl->spec->filepos, s_compiled_func,
			type, cd ) == false )
		{
			return false;
		}
	}
				
	variable_t*		var = decl->var;
		
	if( var == 0 )
	{
		compile_error( 0, ERR_INTERNAL );
		return false;
	}
	
	inst_variable_cd_code( var, CONSTRUCTOR );

	return true;
}

static void
parse_return_stmt(
	expr_stmt_t*		stmt )
{
	expr_t*				expr = stmt->expr;
	
	type_t*				rtype;

	mark_breakpoint( &stmt->filepos );

	rtype = reduce_type(
		 s_compiled_func->decl->type );
	
	if( expr )
	{
		if( type_expr( expr ) == false )
			return;

		if( can_assign_type( expr->type, rtype ) == false )
		{
			caster_t*	caster;
		
			if( scope_find_caster( cur_scope(),
				expr->type, rtype,
				&expr->filepos, SCOPE_ALL, &caster ) )
			{
				func_expr_t*	conv;
				func_t*			func = caster->func;
			
				conv = (func_expr_t*)make_func_expr( 0, 0, expr );
				conv->func = func;
				conv->type = caster->to;
				expr = conv;
			}
			else
			{
				compile_error( &stmt->filepos,
					ERR_TYPE_MISMATCH );
				return;
			}
		}
		
		expr = eval_expr( expr );
		
		code_t*			node;
		
		push_tmp_var_frame();

		node = make_rval_code( expr );
	
		node = make_pass_parameter_code(
			node, expr->type, rtype, &expr->filepos );

		code_t*			code;

		code = pop_tmp_var_frame();
		if( code )
			node = make_seq_code( node, code );

		node = make_ret_code( node );
		inst_code( node );
	}
	else if( rtype->code != TYPE_VOID )
	{
		compile_error( &stmt->filepos,
			ERR_TYPE_MISMATCH );
		return;
	}
	
	scope_t*			scope = cur_scope();
		
	while( scope && scope != s_param_scope )
	{
		tear_down_local_scope( scope );
		scope = scope->up;
	}
	
	// TODO handle scopes that actually are try-catch blocks
	// TODO handle destruction of all pending temporary variables

	inst_bra( INST_BRA, s_function_exit );
}

static bool
inst_array_decl(
	expr_t*				array,
	decl_init_t*		init,
	type_t*				type );

static void
inst_elem_decl(
	expr_t*				array,
	long				index,
	decl_init_t*		init,
	type_t*				elemtype )
{
	expr_t*				elem;
	
	elem = make_binary_expr( EXPR_ACCESS_ARRAY,
		array, make_iconst_expr( index,
		make_int_type( TYPE_INT, false ) ) );

	expr_t*				expr = init->expr;
	
	if( expr == 0 )
	{
		inst_array_decl( elem, init->list, elemtype );
		return;
	}

	expr = make_binary_expr( EXPR_INITIALIZE,
		elem, expr );

	bool				success;

	s_init_declaration = true;
	success = type_expr( expr );
	s_init_declaration = false;
	
	if( success == false )
		return;

	expr = eval_expr( expr );

	parse_expr( expr );
}

static bool
inst_array_decl(
	expr_t*				array,
	decl_init_t*		init,
	type_t*				type )
{
	if( !init )
		return true;
		
	if( type->code != TYPE_POINTER )
	{
		compile_error( &init->filepos, ERR_SYNTAX_ERROR );
		return false;
	}
		
	ptr_type_t*			ptrtype = (ptr_type_t*)type;
	type_t*				elemtype = ptrtype->down;
	expr_t*				sizeexpr = ptrtype->array_expr;
	long				elemcount;
	
	if( !sizeexpr )
		return false;
		
	if( sizeexpr->code != EXPR_ICONST )
	{
		compile_error( 0, ERR_INTERNAL );
		return false;
	}
		
	elemcount = ( (iconst_expr_t*)sizeexpr )->val;

	long				index = 0;
		
	while( init )
	{
		inst_elem_decl( array, index, init, elemtype );
		index++;
		
		if( index > elemcount )
		{
			compile_error( &init->filepos,
				ERR_TOO_MANY_INITIALIZERS );
			return false;
		}
		
		init = init->next;
	}

	return true;
}

static bool
inst_decl(
	decl_t*				decl )
{	
	if( call_decl_constructor( decl ) == false )
		return false;

	decl_init_t*		init = decl->init;

	variable_t*			var = decl->var;
	
	if( var == 0 )
	{
		if( init )
			compile_error( &init->filepos, ERR_INTERNAL );
		else
			compile_error( 0, ERR_INTERNAL );
		return false;
	}
	
	if( init )
	{	
		mark_breakpoint( &init->filepos );

		expr_t*			expr = init->expr;
		
		if( expr == 0 )
		{
			inst_array_decl(
				make_variable_expr( var ),
				init->list, decl->type );
			return true;
		}
	
		expr_t*			var_expr;

		var_expr = make_variable_expr( var );
		var_expr->filepos = expr->filepos;

		expr = make_binary_expr( EXPR_INITIALIZE,
			var_expr, expr );
	
		bool			success;

		s_init_declaration = true;
		success = type_expr( expr );
		s_init_declaration = false;
		
		if( success == false )
			return false;

		expr = eval_expr( expr );

		expr->filepos = decl->spec->filepos;

		parse_expr( expr );
	}
	else if( decl->type->code == TYPE_REFERENCE )
	{
		if( ( var->modifiers & MOD_PARAM ) == 0 )
		{
			compile_error( &decl->spec->filepos,
				ERR_UNINITIALIZED_REFERENCE );
			return false;
		}
	}

	return true;
}

static void
local_declaration(
	decl_t*				decl,
	int					amod,
	int					imod )
{
	// only for non-static variables
	
	parse_declaration( decl, amod, imod, true );

	for( ; decl; decl = decl->next )
	{
		variable_t*		var;
		
		 if( ( var = decl->var ) == 0 )
		 {
			compile_error(
				&decl->spec->filepos, ERR_INTERNAL );
			// decl->var is set in
			// scope_declare_variable
			continue;
		 }
		
		var->lifetime = variable_created(
			decl->name, decl->type, var->offset );
	}
}

static void
tear_down_local_scope(
	scope_t*			scope )
{
	xhtab_t*			t = scope->variables;
	
	if( !xhfirst( t ) )
		return;

	do {	
		variable_t*		var;
		
		var = (variable_t*)xhstuff( t );

		if( var->modifiers & MOD_STATIC )
			continue;

		variable_destroyed( var->lifetime );

		type_t*			type = var->decl->type;
	
		if( type->code != TYPE_CLASS )
			continue;
	
		inst_variable_cd_code( var, DESTRUCTOR );

	} while( xhnext( t ) );
}

static void
parse_decl_stmt(
	decl_stmt_t*		stmt )
{	
	// aside from initialization, no code is generated
	// for declarations, therefore mark no breakpoint

	decl_t*				decl;
	
	if( ( decl = stmt->decl ) == 0 )
		return;
	
	if( decl->spec->modifiers & MOD_STATIC )
	{
		// copy variables from volatile function memory xalloc
		// frame to save global memory frame (so we can later
		// access them when handling static initialization)
	
		close_xalloc_frame();

		parse_declaration( decl, MOD_LOCAL,
			MOD_TYPEDEF | MOD_NATIVE | MOD_ABSTRACT, true );
	
		func_t*			func = s_compiled_func;
		class_t*		clss = func->scope->clss;
		decl_t*			node = decl;
	
		while( true )
		{
			variable_t*	var;
			
			var = (variable_t*)xalloc(
				sizeof( variable_t ) );
			
			*var = *node->var;
			node->var = var;
	
			decl_t*		next;
	
			next = node->next;
			if( next == 0 )
				break;
		
			node = next;
		}
	
		open_xalloc_frame();
	
		node->next = clss->funcstat;
		clss->funcstat = decl;
	}
	else
	{
		local_declaration( decl,
			MOD_LOCAL, MOD_TYPEDEF | MOD_NATIVE | MOD_ABSTRACT );

		do {
			inst_decl( decl );
			decl = decl->next;
		} while( decl );
	}
}

static void
parse_block_stmt(
	stmt_t*				stmt )
{
	scope_t*			embedding = cur_scope();

	scope_t*			scope;
	
	scope = push_scope();
	
	if( embedding != s_param_scope )
	{
		scope->up = embedding;	
		scope->offset = embedding->offset;
	}
		
	parse_stmt( stmt );
		
	update_local_stack_size( scope );
				
	tear_down_local_scope( scope );
		
	pop_scope();

	if( embedding == s_param_scope )
		put_label( s_function_exit );
}

static void
parse_try_stmt(
	try_stmt_t*			stmt )
{
	try_code_t*			code = make_try_code();
	label_t*			exit = make_label();

	inst_code( code );
	parse_block_stmt( stmt->stmt );
	inst_code( make_unary_code( INST_LEAVE_TRY, OPS_PTR, 0 ) );
	inst_bra( INST_BRA, exit );
	
	catch_stmt_t*		catchstmt = stmt->catchstmt;

	while( catchstmt )
	{
		decl_t*			decl = catchstmt->decl;
	
		if( finish_decl( decl, true ) == false )
			return;
	
		type_t*			type = pure_type( decl->type );
		class_t*		clss;

		if( type->code != TYPE_CLASS )
		{
			compile_error( &decl->spec->filepos,
				ERR_EXPECTED_CLASS );
			return;
		}
		
		clss = ( (class_type_t*)type )->clss;
	
		label_t*		label;
	
		label = make_label();
		put_label( label );
		add_try_catch( code, label, clss );
		
		stmt_t*			inner;
		decl_init_t*	init;
		
		init = make_decl_init();
		init->expr = make_thrown_object_expr(
			make_class_type( clss ) );
		decl->init = init;
		
		inner = make_decl_stmt( decl );
		inner->next = catchstmt->stmt;
		
		parse_block_stmt( inner );

		inst_code( make_unary_code( INST_LEAVE_CATCH, OPS_PTR, 0 ) );
		inst_bra( INST_BRA, exit );
		
		catchstmt = (catch_stmt_t*)catchstmt->next;
	}
	
	put_label( exit );
}

static void
parse_throw_stmt(
	expr_stmt_t*		stmt )
{
	expr_t*				expr = stmt->expr;
	
	mark_breakpoint( &expr->filepos );

	if( type_expr( expr ) == false )
		return;

	expr = eval_expr( expr );
	
	type_t*				type = pure_type( expr->type );
	class_t*			clss;
	
	if( type->code != TYPE_CLASS )
	{
		compile_error( &expr->filepos,
			ERR_EXPECTED_CLASS );
		return;
	}	

	clss = ( (class_type_t*)type )->clss;

	code_t*				node;
		
	node = make_rval_code( expr );
	node = make_seq_code( node, make_iconst_code(
		OPS_QUAD, false, get_class_size( clss ) ) );
	node = make_unary_code( INST_THROW, OPS_PTR, node );

	inst_code( node );
}

static void
parse_stmt(
	stmt_t*				stmt )
{
	while( stmt )
	{
		switch( stmt->code )
		{
			case STMT_NOP:
				break;
		
			case STMT_EXPR:
				parse_expr_stmt( (expr_stmt_t*)stmt );
				break;
				
			case STMT_IF:
				parse_if_stmt( (if_stmt_t*)stmt );
				break;
				
			case STMT_WHILE:
			case STMT_DO:
				parse_cond_stmt( (cond_stmt_t*)stmt );
				break;
				
			case STMT_FOR:
				parse_for_stmt( (for_stmt_t*)stmt );
				break;

			case STMT_SWITCH:
				parse_switch_stmt( (cond_stmt_t*)stmt );
				break;
				
			case STMT_CASE:
				parse_case_stmt( (case_stmt_t*)stmt );
				break;
				
			case STMT_DEFAULT:
				parse_default_stmt( stmt );
				break;
								
			case STMT_CONTINUE:
				parse_break_continue( stmt, L_CONTINUE );
				break;

			case STMT_BREAK:
				parse_break_continue( stmt, L_BREAK );
				break;
				
			case STMT_RETURN:
				parse_return_stmt( (expr_stmt_t*)stmt );
				break;
				
			case STMT_DECL:
				parse_decl_stmt( (decl_stmt_t*)stmt );
				break;
				
			case STMT_BLOCK:
				parse_block_stmt( ( (block_stmt_t*)stmt )->stmt );
				break;
				
			/*case STMT_TRY:
				parse_try_stmt( (try_stmt_t*)stmt );
				break;
				
			case STMT_THROW:
				parse_throw_stmt( (expr_stmt_t*)stmt );
				break;*/
				
			default:
				compile_error( &stmt->filepos, ERR_NOT_IMPLEMENTED );
				break;
		}
	
		stmt = stmt->next;
	}
}

static void
compile_code_func(
	class_t*			clss,
	func_t*				func )
{
	s_local_stack_size = 0;

	push_xalloc_frame();

	init_breakpoints();
	init_lifetimes();
	
	init_inst();

	s_function_exit = make_label();

	scope_t*			scope;

	scope = s_param_scope = push_scope();
	
	scope->up = clss->scope;
	scope->offset = (u32)-16;
	
	decl_t*				param;
	
	if( ( param = func->param ) != 0 )
	{
		local_declaration( param,
			MOD_PARAM, MOD_TYPEDEF | MOD_STATIC |
			MOD_NATIVE | MOD_ABSTRACT );
	}
	
	scope->offset = 0;
		
	if( func == clss->cd_func[ CONSTRUCTOR ] )
	{
		call_class_scope_cd( clss->scope, CONSTRUCTOR );
		call_inherited_cd( clss, CONSTRUCTOR );
	}

	parse_stmt( func->stmt );
	
	tear_down_local_scope( s_param_scope );

	if( func == clss->cd_func[ DESTRUCTOR ] )
	{
		call_class_scope_cd( clss->scope, DESTRUCTOR );
		call_inherited_cd( clss, DESTRUCTOR );
	}
	
	if( func->decl->type->code != TYPE_VOID )
		inst_rts();

	pop_scope();

	if( compiler_t::has_errors() == false )
		finish_inst();

	out_long( s_local_stack_size );

	dump_lifetimes();

	default_breakpoint(
		&func->decl->spec->filepos );

	dump_breakpoints();
	
	pop_xalloc_frame();
}

static void
compile_nontemplate_func(
	class_t*			clss,
	func_t*				func )
{
	out_byte( 0 );
	
	s_compiled_func = func;
	compile_code_func( clss, func );
}

void
compile_template_func(
	class_t*			clss,
	func_t*				func )
{
	functemp_map_t*		map;
	tt_replacer_t		replacer;

	for( map = func->tmap; map; map = map->next )
	{
		// since new maps are always inserted at
		// the beginning, if we hit a compiled
		// map, we're done
		if( map->code )
			break;

		replacer.replace(
			map->bind, func->param );

		out_open_inline();
		output_func_id( map->func );
		compile_nontemplate_func( clss, map->func );
		map->code = out_close_inline();

		replacer.restore();

		g_template_compiled = true;
	}
}

static void
write_dll_func_info(
	func_t*				func )
{
	long				size = 0;
	decl_t*				param = func->param;

	while( param )
	{
		size += get_type_size( param->type );
		param = param->next;
	}

	out_long( get_stack_size( size ) );

	size = get_type_size( func->decl->spec->type );
	if( size <= 4 && size != 3 )
		out_long( size );
	else
		out_long( get_stack_size( size ) );
}

void
compile_func(
	class_t*			clss,
	func_t*				func )
{
	const int			modifiers = func->decl->spec->modifiers;

	if( modifiers & ( MOD_NATIVE | MOD_ABSTRACT | MOD_DLL ) )
	{	
		output_func_id( func );
		if( modifiers & MOD_NATIVE )
			out_byte( 1 );
		else if( modifiers & MOD_ABSTRACT )
			out_byte( 2 );
		else if( modifiers & MOD_DLL )
		{
			out_byte( 3 );
			write_dll_func_info( func );
		}
	}
	else
	{
		output_func_id( func );
		compile_nontemplate_func( clss, func );
	}
}

// ---------------------------------------------------------------------------

static void
init_scope_statics(
	scope_t*			scope )
{
	xhtab_t*			t = scope->variables;
	
	if( !xhfirst( t ) )
		return;

	xhtab_iterator_t	it;

	do {	
		variable_t*		var;
		
		var = (variable_t*)xhstuff( t );

		if( ( var->modifiers & MOD_STATIC ) == 0 )
			continue;
			
		xhgetiterator( t, &it );
			
		if( inst_decl( var->decl ) == false )
			return;

		xhsetiterator( t, &it );
		
	} while( xhnext( t ) );
}

static void
init_func_statics(
	class_t*			clss )
{
	decl_t*				decl = clss->funcstat;
	
	while( decl )
	{
		mark_breakpoint( &decl->spec->filepos );
	
		if( inst_decl( decl ) == false )
			return;
			
		decl = decl->next;
	}
}

void
compile_static_cd_func(
	class_t*			clss,
	int					index )
{
	scope_t*			clss_scope = clss->scope;

	decl_t*				decl;
	decl_spec_t			spec;
	
	decl = make_decl( index == CONSTRUCTOR ? "_init" : "_cleanup" );
	decl->type = make_type( TYPE_VOID );
	decl->spec = &spec;
	decl->func = make_decl_func( 0 );
	
	spec.type = decl->type;
	spec.filepos = clss->filepos;
	spec.modifiers = MOD_STATIC;

	func_t*				func;
	
	func = (func_t*)xalloc( sizeof( func_t ) );
	func->scope = clss_scope;
	func->decl = decl;
	func->access = ACC_PRIVATE;
	func->stmt = 0;
	func->same = 0;
	func->param = 0;
	func->comp = 0;

	s_compiled_func = func;
	
	output_func_id( func );
	
	out_byte( 0 );

	s_local_stack_size = 0;

	push_xalloc_frame();

	init_breakpoints();
	init_lifetimes();

	init_inst();

	scope_t*			scope;

	scope = s_param_scope = push_scope();
	
	scope->up = clss_scope;

	s_init_declaration = true;
	init_scope_statics( clss_scope );
	init_func_statics( clss );
	s_init_declaration = false;
	
	pop_scope();
		
	if( compiler_t::has_errors() == false )
		finish_inst();

	out_long( s_local_stack_size );
	
	dump_lifetimes();
	
	dump_breakpoints();
	
	pop_xalloc_frame();
}

END_COMPILER_NAMESPACE
