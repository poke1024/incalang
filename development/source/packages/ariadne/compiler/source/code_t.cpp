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
//	code_t.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "makecode.h"
#include "xalloc.h"
#include "type_t.h"
#include "class_t.h"
#include "expr_t.h"
#include "tmpvar.h"

#include "scope_t.h"

BEGIN_COMPILER_NAMESPACE

code_t*
make_unary_code(
	e_inst_code			inst,
	e_operand_size		size,
	code_t*				child )
{
	unary_code_t*		node;
	
	node = (unary_code_t*)xalloc( sizeof( unary_code_t ) );
	node->inst = inst;
	node->size = size;
	node->child = child;
	node->addr = child->addr;

	return node;
}

static variable_t*
extract_tmp_var(
	code_t*				node )
{
	// extract a temporary variable from a code tree
	// built in copy_class() in makecode.cp

	e_inst_code			inst = node->inst;

	switch( inst )
	{
		case CODE_SEQ:
			return extract_tmp_var(
				( (binary_code_t*)node )->child[ 1 ] );
	
		case INST_LOAD:
		case INST_XLOAD:
			break;
			
		default:
			return 0;
	}

	node = ( (unary_code_t*)node )->child;
				
	if( node->inst != INST_ADD )
		return 0;
		
	code_t*				child;
		
	child = ( (binary_code_t*)node )->child[ 0 ];
	if( child->inst != CODE_REG )
		return 0;
		
	child = ( (binary_code_t*)node )->child[ 1 ];
	if( child->inst != CODE_TVAR )
		return 0;
			
	return ( (tvar_code_t*)child )->tvar;
}

code_t*
make_ret_code(
	code_t*				child )
{
	variable_t*			tvar;
	
	tvar = extract_tmp_var( child );
		
	if( 0 && tvar )
	{
		// we end up here for statements of the form
		// t a() { t b; return b; } returning a whole class.
		// the node we receive was forged inside copy_class()
		// in makecode.cp.
		
		// what we do now is to copy the temporary onto
		// the stack and then (hopefully) return. since we
		// do not call the temporary's destructor, everything
		// should be fine
	
		/*type_t*			type;
		
		type = tvar->type;*/
	
		/*if( type->code != TYPE_CLASS )
		{
			compile_error( 0, ERR_INTERNAL );
			return child;
		}
	
		code_t*			addr = child;*/
	
		/*addr = make_reg_code(
			OPS_PTR, REG_LOCAL, 0 );
		addr = make_binary_code(
			INST_ADD, OPS_PTR, addr,
			make_tvar_code( tvar ) );*/

		/*code_t*			code;

		code = make_xload_code(
			(class_type_t*)type, addr );*/
			
		/*child = make_seq_code(
			( (free_tvar_code_t*)child )->child,
			code );*/

		//child = code;

		free_tmp_var( tvar );
	}

	unary_code_t*		node;
	
	node = (unary_code_t*)xalloc( sizeof( unary_code_t ) );
	node->inst = CODE_RET;
	node->size = OPS_NONE;
	node->child = child;
	node->addr = child->addr;
		
	return node;
}

code_t*
make_binary_code(
	e_inst_code			inst,
	e_operand_size		size,
	code_t*				left,
	code_t*				right )
{
	binary_code_t*		node;
	
	node = (binary_code_t*)xalloc( sizeof( binary_code_t ) );
	node->inst = inst;
	node->size = size;
	node->child[ 0 ] = left;
	node->child[ 1 ] = right;
	node->addr = false;

	return node;
}

code_t*
make_param_code(
	code_t*				left,
	code_t*				right )
{
	binary_code_t*		node;
	
	node = (binary_code_t*)xalloc( sizeof( binary_code_t ) );
	node->inst = CODE_PARAM;
	node->size = OPS_NONE;
	node->child[ 0 ] = left;
	node->child[ 1 ] = right;
	node->addr = false;
	
	// release of code is delayed untilfunction is set up
	// via make_func_code, which calls release_param to
	// do the work. this is done to release the temporaries
	// in the running program after the function has been
	// called.
	
	return node;
}

iconst_code_t*
make_iconst_code(
	e_operand_size		size,
	bool				is_signed,
	intval_t			val )
{
	iconst_code_t*		node;
	
	node = (iconst_code_t*)xalloc( sizeof( iconst_code_t ) );
	node->inst = INST_LDI;
	node->size = size;
	node->is_signed = is_signed;
	node->val = val;
	node->addr = false;
	
	return node;
}

fconst_code_t*
make_fconst_code(
	e_operand_size		size,
	fltval_t			val )
{
	fconst_code_t*		node;
	
	node = (fconst_code_t*)xalloc( sizeof( fconst_code_t ) );
	node->inst = INST_FLDI;
	node->size = size;
	node->val = val;
	node->addr = false;
	
	return node;
}

str_code_t*
make_str_code(
	const char*			str )
{
	str_code_t*			node;
	
	node = (str_code_t*)xalloc( sizeof( str_code_t ) );
	node->inst = CODE_STR;
	node->size = OPS_PTR;
	node->str = str;
	node->addr = true;
	
	return node;
}

tvar_code_t*
make_tvar_code(
	variable_t*			var )
{
	tvar_code_t*		node;
	
	node = (tvar_code_t*)xalloc( sizeof( tvar_code_t ) );
	node->inst = CODE_TVAR;
	node->size = OPS_PTR;
	node->tvar = var;
	node->addr = false;
	
	return node;
}

/*free_tvar_code_t*
make_free_tvar_code(
	variable_t*			var,
	code_t*				child )
{
	free_tvar_code_t*	node;
	
	node = (free_tvar_code_t*)xalloc(
		sizeof( free_tvar_code_t ) );
	node->inst = CODE_FREE_TVAR;
	node->size = OPS_PTR;
	node->child = child;
	node->tvar = var;
	node->addr = false;
	
	return node;
}*/

rtinfo_code_t*
make_rtinfo_code(
	code_t*				instance,
	class_t*			clss )
{
	rtinfo_code_t*		node;
	
	node = (rtinfo_code_t*)xalloc( sizeof( rtinfo_code_t ) );
	node->inst = INST_RTINFO;
	node->size = OPS_PTR;
	node->instance = instance;
	node->clss = clss;
	node->addr = false;
	
	return node;	
}

code_t*
make_func_code(
	e_inst_code			inst,
	func_t*				func,
	code_t*				instance,
	code_t*				param )
{
	func_code_t*		node;
	
	node = (func_code_t*)xalloc( sizeof( func_code_t ) );
	node->inst = inst;
	node->size = OPS_PTR;

	node->func = func;
	node->instance = instance;
	node->param = param;
	
	node->addr = false;

	return node;
}

reg_code_t*
make_reg_code(
	e_operand_size		size,
	short				reg,
	void*				data )
{
	reg_code_t*			node;
	
	node = (reg_code_t*)xalloc( sizeof( reg_code_t ) );
	node->inst = CODE_REG;
	node->size = size;
	node->reg = reg;
	node->data = data;
	node->addr = false;
	
	return node;
}

code_t*
make_seq_code(
	code_t*				left,
	code_t*				right )
{
	binary_code_t*		node;
	
	node = (binary_code_t*)xalloc( sizeof( binary_code_t ) );
	node->inst = CODE_SEQ;
	node->size = right->size;
	node->child[ 0 ] = left;
	node->child[ 1 ] = right;
	node->addr = right->addr;
	
	return node;
}

code_t*
make_seq_code(
	e_inst_code			code,
	code_t*				left,
	code_t*				right )
{
	// a neutral sequence is one which occupies no stack space,
	// i.e. no pops have to be generated if this node occurs as
	// code tree.

	binary_code_t*		node;
	
	node = (binary_code_t*)xalloc( sizeof( binary_code_t ) );
	node->inst = code;
	node->size = right->size;
	node->child[ 0 ] = left;
	node->child[ 1 ] = right;
	node->addr = right->addr;
	
	return node;
}

code_t*
make_comma_code(
	code_t*				left,
	code_t*				right )
{
	code_t*				node = make_seq_code( left, right );
	
	node->addr = false;
	return node;
}

code_t*
make_destroy_code(
	code_t*				left,
	code_t*				right )
{
	// right sub tree should contain commands necessary to
	// perform destruct operations. this command is basically
	// a modified seq code command. code release takes place later.

	binary_code_t*		node;
	
	node = (binary_code_t*)xalloc( sizeof( binary_code_t ) );
	node->inst = CODE_DESTROY;
	node->size = left->size;
	node->child[ 0 ] = left;
	node->child[ 1 ] = right;
	node->addr = left->addr;

	return node;
}

code_t*
make_hint_code(
	e_inst_code			code,
	code_t*				child,
	code_t*				param )
{
	binary_code_t*		node;
	
	node = (binary_code_t*)xalloc( sizeof( binary_code_t ) );
	node->inst = code;
	node->size = child->size;	
	node->child[ 0 ] = child;
	node->child[ 1 ] = param;
	node->addr = child->addr;
	
	return node;
}

code_t*
make_pop_code(
	code_t*				child,
	size_t				amount )
{
	pop_code_t*			node;
	
	if( child->inst == CODE_POP )
	{
		node = (pop_code_t*)child;
		node->amount += amount;
		return node;
	}
	
	node = (pop_code_t*)xalloc( sizeof( pop_code_t ) );
	node->inst = CODE_POP;
	node->size = OPS_NONE;
	node->child = child;
	node->amount = amount;
	node->addr = false;
	
	return node;
}

code_t*
make_xstore_code(
	class_type_t*		type,
	code_t*				left,
	code_t*				right )
{
	size_t				bytecount = get_class_size( type->clss );

	xstore_code_t*		node;
	
	node = (xstore_code_t*)xalloc( sizeof( xstore_code_t ) );
	node->inst = INST_XSTORE;
	node->size = OPS_NONE;
	node->child[ 0 ] = left;
	node->child[ 1 ] = right;
	node->addr = false;
	node->bytecount = bytecount;

	return node;
}

code_t*
make_xload_code(
	class_type_t*		type,
	code_t*				child )
{
	size_t				bytecount = get_class_size( type->clss );

	xload_code_t*		node;
	
	node = (xload_code_t*)xalloc( sizeof( xload_code_t ) );
	node->inst = INST_XLOAD;
	node->size = OPS_NONE;
	node->child = child;
	node->addr = false;
	node->bytecount = bytecount;

	return node;
}

code_t*
make_dynamic_cast_code(
	class_t*			toclass,
	unsigned long		offset,
	code_t*				child )
{
	dyncast_code_t*		node;
	
	node = (dyncast_code_t*)xalloc( sizeof( dyncast_code_t ) );
	node->inst = INST_DYNCAST;
	node->size = OPS_PTR;
	node->child = child;
	node->addr = false;
	
	node->toclass = toclass;
	node->offset = offset;

	return node;
}

static code_t*
make_arraysize_code(
	type_t*				type )
{
	code_t*				node = 0;

	while( type->code == TYPE_POINTER )
	{
		ptr_type_t*		ptype = (ptr_type_t*)type;
	
		if( !ptype->is_array )
			break;
		
		code_t*			size;
		
		expr_t*			sizeexpr = ptype->array_expr;
		
		size = make_rval_code( sizeexpr );
		size = promote_code_int_type(
			size, get_type_operand_size( sizeexpr->type ),
			OPS_QUAD, false );
			
		if( !node )
			node = size;
		else
		{
			node = make_binary_code(
				INST_MUL, OPS_QUAD, size, node );
		}
			
		type = ptype->down;
	}

	if( !node )
		node = make_iconst_code( OPS_QUAD, false, 1 );
			
	return node;
}

new_code_t*
make_new_code(
	type_t*				newtype,
	type_t*				elemtype )
{
	new_code_t*			node;
	
	node = (new_code_t*)xalloc( sizeof( new_code_t ) );
	node->inst = INST_NEW;
	node->size = OPS_PTR;
	node->addr = false;
	
	node->arraysize = make_arraysize_code( newtype );
	node->newtype = newtype;
	node->elemtype = elemtype;
	
	return node;
}

dtnew_code_t*
make_dtnew_code(
	type_t*				newtype,
	type_t*				elemtype,
	expr_t*				newname )
{
	dtnew_code_t*		node;
	
	node = (dtnew_code_t*)xalloc( sizeof( dtnew_code_t ) );
	node->inst = INST_DTNEW;
	node->size = OPS_PTR;
	node->addr = false;
	
	node->arraysize = make_arraysize_code( newtype );
	node->newtype = newtype;
	node->elemtype = elemtype;
	node->newname = make_rval_code( newname );
	
	return node;
}

delete_code_t*
make_delete_code(
	type_t*				elemtype,
	expr_t*				delexpr )
{
	delete_code_t*		node;
	
	node = (delete_code_t*)xalloc( sizeof( delete_code_t ) );
	node->inst = INST_DELETE;
	node->size = OPS_PTR;
	node->addr = false;
	
	node->delobject = make_rval_code( delexpr );
	node->elemtype = elemtype;
	
	return node;
}

switch_code_t*
make_switch_code(
	code_t*				selector,
	type_t*				type )
{
	switch_code_t*		node;

	node = (switch_code_t*)xalloc( sizeof( switch_code_t ) );
	node->inst = INST_SWITCH;
	node->size = OPS_PTR;
	node->addr = false;

	selector = promote_code_type( selector,
		type, make_int_type( TYPE_INT, false ) );
	
	node->selector = selector;
	node->defaultlabel = 0;
	node->nodes = 0;
	node->count = 0;
	
	return node;
}

bool
add_switch_node(
	switch_code_t*		code,
	label_t*			label,
	caseval_t			value,
	fileref_t*			filepos )
{
	switch_node_t*		next = code->nodes;
	
	if( next == 0 )
		code->minval = code->maxval = value;
	else
	{
		caseval_t		minval = code->minval;
		caseval_t		maxval = code->maxval;
	
		if( value >= minval && value <= maxval )
		{
			switch_node_t*	item = next;
		
			do {
				if( item->value == value )
				{
					compile_error( filepos, ERR_CASE_DEFINED_TWICE );
					return false;
				}
				item = item->next;
			} while( item );
		}
		else
		{
			code->minval = imin( minval, value );
			code->maxval = imax( maxval, value );
		}
	}

	switch_node_t*		node;

	node = (switch_node_t*)xalloc( sizeof( switch_node_t ) );
	node->label = label;
	node->value = value;
	
	node->next = next;
	code->nodes = node;
	
	if( ++code->count > 0xffffL )
	{
		compile_error( filepos, ERR_TOO_MANY_CASES );
		return false;
	}
	
	return true;
}

variable_t*
code_tmp_var(
	code_t**			link,
	type_t*				type )
{
	// we assume that the caller will take care of
	// releasing the returned temporary variable

	code_t*				node = *link;

	/*if( node->inst == INST_STORE )
	{
		code_t*			child;
	
		child = ( (binary_code_t*)node )->child[ 0 ];
		if( child->inst == CODE_VAR )
			return ( (var_code_t*)child )->var;
	}*/
	
	variable_t*			var;
	
	var = take_tmp_var( type );
	
	code_t*				varcode;
	
	varcode = make_tvar_code( var );
	
	varcode = make_binary_code(
		INST_ADD, OPS_PTR,
		make_reg_code( OPS_PTR, REG_LOCAL, 0 ),
		varcode );
	
	node = make_binary_code( INST_STORE,
		get_type_operand_size( type ),
		varcode, node );
		
	*link = node;
	
	return var;
}

try_code_t*
make_try_code()
{
	try_code_t*			node;
	
	node = (try_code_t*)xalloc( sizeof( try_code_t ) );
	node->inst = INST_ENTER_TRY;
	node->size = OPS_NONE;
	node->addr = false;

	node->entries = 0;
	node->link = &node->entries;
	node->count = 0;

	return node;
}

void
add_try_catch(
	try_code_t*			code,
	label_t*			label,
	class_t*			clss )
{
	catch_entry_t*		entry;
	
	entry = (catch_entry_t*)xalloc( sizeof( catch_entry_t ) );
	entry->next = 0;
	entry->label = label;
	entry->clss = clss;

	*code->link = entry;
	code->link = &entry->next;
	code->count++;
}

END_COMPILER_NAMESPACE
