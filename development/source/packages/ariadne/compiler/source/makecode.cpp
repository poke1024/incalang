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
//	makecode.cp				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "makecode.h"
#include "inst_t.h"
#include "type_t.h"
#include "expr_t.h"
#include "scope_t.h"
#include "parser.h"
#include "class_t.h"
#include "tmpvar.h"
#include <cassert>

#include "func_list_t.h"
#include "pickfunc.h"

BEGIN_COMPILER_NAMESPACE

static code_t*
make_class_cast_code(
	code_t*				node,
	class_t*			fromclass,
	class_t*			toclass );

static code_t*
pass_reference(
	code_t*				node,
	code_t*				destroy,
	fileref_t*			filepos );

static code_t*
make_load_code(
	e_operand_size		size,
	code_t*				code )
{
	code_t*				node;

	switch( size )
	{
		case OPS_SINGLE:
		case OPS_DOUBLE:
			node = make_unary_code( INST_FLOAD, size, code );
			break;
			
		default:
			node = make_unary_code( INST_LOAD, size, code );
			break;
	}
	
	return node;
}

static code_t*
make_store_code(
	e_operand_size		size,
	code_t*				lval,
	code_t*				rval )
{
	code_t*				node;

	switch( size )
	{
		case OPS_SINGLE:
		case OPS_DOUBLE:
			node = make_binary_code( INST_FSTORE, size, lval, rval );
			break;
			
		default:
			node = make_binary_code( INST_STORE, size, lval, rval );
			break;
	}
	
	return node;
}

code_t*
lval_to_rval_code(
	code_t*				code,
	type_t*				type )
{
	if( code->addr )
	{

retry:
		switch( type->code )
		{
			case TYPE_REFERENCE:
				type = ( (ref_type_t*)type )->down;
				goto retry;
			
			case TYPE_CLASS:
				code = make_xload_code(
					(class_type_t*)type, code );
				break;
				
			case TYPE_TYPEDEF:
				type = ( (typedef_type_t*)type )->type;
				goto retry;
		
			case TYPE_POINTER:
				if( ( (ptr_type_t*)type )->is_array )
					break;
			
			// fall through
				
			default:
				code = make_load_code(
					get_type_operand_size( type ), code );
				break;
		}
	
		code->addr = false;
	}
	
	return code;
}

code_t*
make_rval_code(
	expr_t*				expr )
{
	code_t*				code = make_code( expr );
	
	return lval_to_rval_code( code, expr->type );
}

code_t*
promote_code_int_type(
	code_t*				code,
	e_operand_size		fromsize,
	e_operand_size		tosize,
	bool				is_signed )
{
	switch( code->inst )
	{
		case INST_LDI:
			code->size = tosize;
			return code;

		case INST_FLDI:
		{
			intval_t	val;
		
			if( is_signed )
				val = (s64)( (fconst_code_t*)code )->val;
			else
				val = (u64)( (fconst_code_t*)code )->val;
			
			return make_iconst_code( tosize, is_signed, val );
		}
	}

	if( (int)fromsize < (int)tosize )
	{
		e_inst_code		opcode;

		opcode = is_signed ? INST_EXTS : INST_EXTU;

		int				size = (int)fromsize + 1;
		
		while( (int)tosize >= size )
		{
			code = make_unary_code( opcode,
				(e_operand_size)size, code );
			size += 1;
		}
	}
	else if( (int)fromsize > (int)tosize )
	{
		int				size = (int)fromsize - 1;
		
		while( (int)tosize <= size )
		{
			code = make_unary_code( INST_RDC,
				(e_operand_size)size, code );
			size -= 1;
		}
	}
	
	return code;
}

static code_t*
promote_code_flt_type(
	code_t*				code,
	e_operand_size		fromsize,
	e_operand_size		tosize )
{	
	switch( code->inst )
	{
		case INST_FLDI:
			code->size = tosize;
			return code;
	
		case INST_LDI:
		{
			fltval_t	val;
		
			if( ( (iconst_code_t*)code )->is_signed )
				val = (s64)( (iconst_code_t*)code )->val;
			else
				val = (u64)( (iconst_code_t*)code )->val;
			
			return make_fconst_code( tosize, val );
		}
	}

	if( fromsize == OPS_SINGLE )
	{
		if( tosize == OPS_DOUBLE )
			code = make_unary_code( INST_FEXT, OPS_DOUBLE, code );
	}
	else if( fromsize == OPS_DOUBLE )
	{
		if( tosize == OPS_SINGLE )
			code = make_unary_code( INST_FEXT, OPS_SINGLE, code );
	}
	
	return code;
}

static code_t*
promote_code_ptr_type(
	code_t*				code,
	type_t*				downfromtype,
	type_t*				downtotype )
{
	if( downfromtype->code == TYPE_CLASS &&
		downtotype->code == TYPE_CLASS )
	{
		class_t*		fromclss;
		class_t*		toclss;
		
		fromclss = ( (class_type_t*)downfromtype )->clss;
		toclss = ( (class_type_t*)downtotype )->clss;
		
		code = make_class_cast_code( code, fromclss, toclss );
	}
	
	return code;
}

code_t*
promote_code_type(
	code_t*				code,
	type_t*				fromtype,
	type_t*				totype )
{
	// note that int->bool and flt->bool conversions should've
	// been handeled in the type_expr module by generating
	// EXPR_EQ nodes with zero. we should not end up here in
	// these cases.

	fromtype = pure_type( fromtype );
	totype = pure_type( totype );
	
	if( fromtype->code == TYPE_CLASS )
	{
		// should not happen here. passing class instances down
		// as parameters in function should have been handled in 
		// make_func_code() and copy_class().

		compile_error( 0, ERR_INTERNAL );
		return code;
	}
	
	if( code->addr )
	{
		// we should only deal with rvalues here
		
		compile_error( 0, ERR_INTERNAL );
		return code;
	}
	
	bool				fromtypeint = is_type_int_operand( fromtype );
	bool				totypeint = is_type_int_operand( totype );

	if( fromtypeint && !totypeint )
	{	
		if( code->inst == INST_LDI )
		{
			code = promote_code_flt_type(
				code, OPS_DOUBLE, get_type_operand_size( totype ) );
		}
		else
		{
			e_inst_code	opcode;
	
			if( is_type_signed( fromtype ) )
				opcode = INST_ITOF;
			else
				opcode = INST_UTOF;
	
			code = make_unary_code( opcode,
				get_type_operand_size( fromtype ), code );
		
			code = promote_code_flt_type(
				code, OPS_DOUBLE, get_type_operand_size( totype ) );
		}
	}
	else if( !fromtypeint && totypeint )
	{
		if( code->inst == INST_FLDI )
		{
			code = promote_code_int_type(
				code, get_type_operand_size( fromtype ),
				get_type_operand_size( totype ),
				is_type_signed( totype ) );
		}
		else
		{
			e_inst_code	opcode;

			code = promote_code_flt_type(
				code, get_type_operand_size( fromtype ),
				OPS_DOUBLE );
				
			if( is_type_signed( totype ) )
				opcode = INST_FTOI;
			else
				opcode = INST_FTOU;
		
			code = make_unary_code( opcode,
				get_type_operand_size( totype ), code );
		}
	}
	else if( fromtypeint && totypeint )
	{
		if( fromtype->code == TYPE_POINTER &&
			totype->code == TYPE_POINTER )
		{
			code = promote_code_ptr_type(
				code, ( (ptr_type_t*)fromtype )->down,
				( (ptr_type_t*)totype )->down );
		}
		else
		{
			code = promote_code_int_type(
				code, get_type_operand_size( fromtype ),
				get_type_operand_size( totype ),
				is_type_signed( fromtype ) );
		}
	}
	else
	{
		code = promote_code_flt_type(
			code, get_type_operand_size( fromtype ),
			get_type_operand_size( totype ) );
	}
	
	return code;
}

static void
promoted_type_code(
	binary_expr_t*		expr,
	code_t**			lcode,
	code_t**			rcode,
	type_t*				type )
{
	expr_t*				left = expr->child[ 0 ];
	expr_t*				right = expr->child[ 1 ];
		
	type_t*				ltype = left->type;
	type_t*				rtype = right->type;
		
	code_t*				leftcode;
	code_t*				rightcode;
	
	leftcode = make_rval_code( left );
	leftcode = promote_code_type( leftcode, ltype, type );

	rightcode = make_rval_code( right );
	rightcode = promote_code_type( rightcode, rtype, type );
	
	*lcode = leftcode;
	*rcode = rightcode;
}

static void
common_type_code(
	binary_expr_t*		expr,
	code_t**			lcode,
	code_t**			rcode )
{
	promoted_type_code(
		expr, lcode, rcode, expr->type );
}

static type_t*
common_cmp_type_code(
	binary_expr_t*		expr,
	code_t**			lcode,
	code_t**			rcode )
{
	expr_t*				left = expr->child[ 0 ];
	expr_t*				right = expr->child[ 1 ];

	type_t*				ctype;

	ctype = common_type( left->type, right->type );
	
	if( !ctype )
	{
		compile_error( &expr->filepos, ERR_TYPE_MISMATCH );
		return make_type( TYPE_NONE );
	}

	promoted_type_code(
		expr, lcode, rcode, ctype );
		
	return ctype;
}

static code_t*
make_addsub_code(
	e_inst_code			iopcode,
	e_inst_code			fopcode,
	binary_expr_t*		expr )
{
	expr_t*				lexpr = expr->child[ 0 ];
	type_t*				ltype = lexpr->type;

	if( ltype->code == TYPE_POINTER )
	{
		expr_t*			rexpr = expr->child[ 1 ];

		code_t*			left;
		code_t*			right;

		left = make_rval_code( lexpr );

		right = make_rval_code( rexpr );
		right = promote_code_int_type( right,
			get_type_operand_size( rexpr->type ), OPS_PTR, false );
		
		code_t*			node;
		size_t			size;
		
		size = get_type_size( ( (ptr_type_t*)ltype )->down );
		
		if( size != 1 )
		{
			node = make_iconst_code(
				OPS_PTR, false, size );
		
			right = make_binary_code( INST_MUL, OPS_PTR,
				right, node );
		}
	
		return make_binary_code( iopcode, OPS_PTR, left, right );
	}
	else
		return make_arith_code( iopcode, fopcode, expr );
}

code_t*
make_arith_code(
	e_inst_code			iopcode,
	e_inst_code			fopcode,
	binary_expr_t*		expr )
{
	code_t*				left;
	code_t*				right;
	
	common_type_code( expr, &left, &right );

	type_t*				type = expr->type;
		
	e_inst_code			opcode;
	
	if( is_type_int_operand( type ) )
		opcode = iopcode;
	else
		opcode = fopcode;

	e_operand_size		size = get_type_operand_size( type );
	
	return make_binary_code( opcode, size, left, right );
}

code_t*
make_cmp_code(
	binary_expr_t*		expr )
{
	code_t*				left;
	code_t*				right;
	type_t*				type;
	
	type = common_cmp_type_code( expr, &left, &right );

	e_inst_code			opcode;
	
	if( is_type_int_operand( type ) )
		opcode = is_type_signed( type ) ? INST_CMP : INST_CMPU;
	else
		opcode = INST_FCMP;

	e_operand_size		size = get_type_operand_size( type );
	
	return make_binary_code( opcode, size, left, right );
}

static code_t*
make_incdec_code(
	unary_expr_t*		expr,
	long				val,
	bool				pre )
{
	code_t*				left;
	code_t*				right;
	code_t*				node;
	
	expr_t*				child = expr->child;
	
	left = make_code( child );

	type_t*				type = pure_type( child->type );
	e_operand_size		opsize = get_type_operand_size( type );
	code_t*				value;
	
	if( pre == 0 )
	{
		if( is_type_int_operand( type ) )
			value = make_unary_code( INST_DUP, opsize,
				make_load_code( opsize, left ) );
		else
			value = make_unary_code( INST_FDUP, opsize,
				make_load_code( opsize, left ) );
	}
	else
		value = make_load_code( opsize, left );
		
	if( is_type_int_operand( type ) )
	{
		right = make_iconst_code( opsize, false, val );

		node = make_binary_code( INST_ADD,
			opsize, right, value );

		node = make_binary_code( INST_STORE,
			opsize, left, node );
	}
	else
	{
		right = make_fconst_code( opsize, val );

		node = make_binary_code( INST_FADD,
			opsize, right, value );

		node = make_binary_code( INST_FSTORE,
			opsize, left, node );
	}

	if( pre == 0 )
	{
		size_t			popsize;
	
		popsize = get_type_stack_size( type );
		
		node = make_pop_code( node, popsize );
	}
	
	return node;
}

static long
get_type_inc_size(
	type_t*				type )
{
	type = pure_type( type );

	if( type->code != TYPE_POINTER )
		return 1;
		
	return get_type_size( ( (ptr_type_t*)type )->down );
}

static code_t*
make_array_code(
	binary_expr_t*		expr )
{
	code_t*				array;
	code_t*				index;
	
	array = make_code( expr->child[ 0 ] );
	index = make_rval_code( expr->child[ 1 ] );

	ptr_type_t*			arraytype = (ptr_type_t*)expr->child[ 0 ]->type;
	assert( arraytype->code == TYPE_POINTER );
	
	size_t				elemsizei;
	bool				zero_index = false;
	
	elemsizei = get_type_array_size( arraytype->down );
	
	if( index->inst == INST_LDI )
	{
		s64				indexi;
	
		indexi = (s64)( (iconst_code_t*)index )->val;
		
		if( indexi == 0 )
			zero_index = true;
		else
			( (iconst_code_t*)index )->val = indexi * elemsizei;	
	}
	else
	{
		index = promote_code_int_type(
			index, get_type_operand_size( expr->child[ 1 ]->type ),
			OPS_PTR, false );
		
		if( elemsizei != 1 )
		{
			code_t*		elemsize;
		
			elemsize = make_iconst_code(
				OPS_PTR, false, elemsizei );
		
			index = make_binary_code( INST_MUL, OPS_PTR,
				index, elemsize );
		}
	}
	
	if( !arraytype->is_array && array->addr )
		array = make_load_code( OPS_PTR, array );
	
	code_t*				node;
	
	if( zero_index )
		node = array;
	else
		node = make_binary_code( INST_ADD, OPS_PTR, array, index );
	
	node->addr = true;
	
	return node;
}

code_t*
make_var_code(
	variable_t*			var,
	class_t*			clss )
{
	const int			modifiers = var->modifiers;

	unsigned long		intoffset = 0;
		
	if( clss && !( modifiers & MOD_STATIC ) )
	{
		class_t*		baseclss = var->scope->clss;
		
		if( baseclss )
		{
			// var's not a local, but a class variable

			if( get_base_class_offset( clss,
				baseclss, &intoffset ) != FOUND )
			{
				compile_error( 0, ERR_INTERNAL );
			}
		}
	}

	intoffset += var->offset;

	code_t*				offset = 0;
	
	if( var->modifiers & MOD_TEMP )
		offset = make_tvar_code( var );
	else if( intoffset )
		offset = make_iconst_code( OPS_PTR, false, intoffset );

	int					reg;
	void*				regdata;

	if( modifiers & MOD_STATIC )
	{
		reg = REG_STATIC;
		regdata = var->scope;
	}
	else if( modifiers & ( MOD_LOCAL | MOD_PARAM | MOD_TEMP ) )
	{
		reg = REG_LOCAL;
		regdata = 0;
	}
	else
	{
		reg = REG_SELF;
		regdata = 0;
	}

	code_t*				node;
	
	node = make_reg_code( OPS_PTR, reg, regdata );
	if( offset )
		node = make_binary_code( INST_ADD, OPS_PTR, node, offset );
		
	node->addr = true;
	
	// dereference variables with reference types so that we
	// really have their actual addresses on the stack
	
	type_t*				type = var->decl->type;
	e_type_code			typecode = type->code;

	while( typecode == TYPE_REFERENCE )
	{
		node = make_load_code(
			get_type_operand_size( type ), node );
		node->addr = true;
		type = ( (ref_type_t*)type )->down;
		typecode = type->code;
	}
	
	return node;
}

static code_t*
make_access_code(
	binary_expr_t*		expr,
	bool				deref )
{
	id_expr_t*			member = (id_expr_t*)expr->child[ 1 ];
	assert( member->code == EXPR_ID );
	
	variable_t*			var = member->var;
	scope_t*			scope = member->scope;

	unsigned long		intoffset;

	if( get_base_class_offset( scope->clss,
		var->scope->clss, &intoffset ) != FOUND )
		compile_error( 0, ERR_INTERNAL );
	
	intoffset += var->offset;

	code_t*				offset = 0;
	
	if( intoffset )
		offset = make_iconst_code( OPS_PTR, false, intoffset );

	code_t*				node;

	if( var->modifiers & MOD_STATIC )
	{
		node = make_reg_code( OPS_PTR, REG_STATIC, var->scope );
	}
	else
	{
		expr_t*			left = expr->child[ 0 ];
	
		node = make_code( left );
		
		if( node->addr == 0 )
			node = pass_reference( node, 0, &expr->filepos );
		
		if( deref )
			node = make_load_code( OPS_PTR, node );						
	}
	
	if( offset )
		node = make_binary_code( INST_ADD, OPS_PTR, node, offset );
	
	node->addr = true;
	
	return node;
}

static code_t*
copy_class(
	code_t*				node,
	type_t*				type,
	fileref_t*			filepos )
{
	// situation: calling f() from g() in
	//
	// struct t { }
	// f( t ) {Ê}
	// g( t a; f( a ); }

	// check special cases how we came here. basically we
	// are only interested in the case in which a class
	// is located on top of the stack, i.e. the last instruction
	// was an xload.
	
	switch( node->inst )
	{
		case INST_CRF:
		case INST_CVF:
		case INST_CSF:
			// a function was called that returns a whole class
			// on the stack. the function should have taken
			// care of all the necessary processing already.
			return node;
			
		case CODE_DESTROY:
			// FIXME!!!
			compile_error( 0, ERR_INTERNAL );
			return node;
		
		case INST_XLOAD:
			break;
						
		default:
			compile_error( 0, ERR_INTERNAL );
			return node;
	}

	// look for a copy function of the form create( const object& )

	class_t*			clss = ( ( class_type_t*)type )->clss;
	func_t*				func = 0;
	func_list_t			func_list;

	func_list_init( &func_list );

	if( scope_find_functions( clss->scope,
		s_constructor_name, filepos, 0, &func_list ) )
	{
		type_t*			const_type;
		
		const_type = copy_type_shallow( type );
		const_type->quals |= TC_CONST;
	
		type_list_t		type_list;
	
		type_list.next = 0;
		type_list.type = make_ref_type( const_type, 0 );

		func = pick_function( 0, func_list.head,
			&type_list, 0, filepos, PICKFUNC_QUIET );
	}
	
	if( func == 0 )
	{
		// no copy function found. that means we're done here,
		// since we do the default byte level copy.
		
		return node;
	}	

	// if we end up here, a typical situation might look
	// like this:
	
	// struct t { create( *t ) {Ê} }
	// void f( t a ) { }
	// void g() { t b; f( b ); }
	
	// as we call f from g, a new instance of t has to be
	// synthesized for the duration of f calling g. we
	// do this by creating a temporary variable for b/a.

	code_t*				instance;

	// we will call the create( object* ) function here
	// to copy the object. instance will contain the
	// address of the existing object. param will point
	// to the space, where the new (copied) object should
	// be materialized.
			
	instance = ( (unary_code_t*)node )->child;

	size_t				typesize = get_type_size( type );
	
	variable_t*			tmpvar;
	code_t*				tmpvarcode;
	
	tmpvar = take_tmp_var( type );
		
	tmpvarcode = make_reg_code(
		OPS_PTR, REG_LOCAL, 0 );
	tmpvarcode = make_binary_code(
		INST_ADD, OPS_PTR, tmpvarcode,
		make_tvar_code( tmpvar ) );
	tmpvarcode->addr = true;
	
	/*param = make_salloc_code(
		get_class_size( clss ) );*/

	/*code_t*				offset;

	param = make_reg_code(
		OPS_PTR, REG_STACK, 0 );
	offset = make_iconst_code(
		OPS_PTR, false, get_class_size( clss ) );
	param = make_binary_code(
		INST_SUB, OPS_PTR, param, offset );*/

	node = make_rtinfo_code( tmpvarcode, clss );

	node = make_seq_code( node, make_func_code(
		INST_CRF, func, tmpvarcode, instance ) );
		
	code_t*				load;
	
	if( type->code == TYPE_CLASS )
		load = make_xload_code(
			(class_type_t*)type, tmpvarcode );
	else
		load = make_load_code(
			get_type_operand_size( type ), tmpvarcode );
		
	node = make_seq_code( node, load );
	
	// the class is now located on the stack (loaded via xload or
	// load). this means we can free the temporary variable
	// immediately.
	
	free_tmp_var( tmpvar );
	
	return node;
}

static code_t*
copy_to_temporary(
	code_t*				node,
	code_t*				param,
	type_t*				type,
	code_t*				destroy )
{
	// this function copies a class object to a temporary
	// variable. the address of the class object is in param.
	// the type of the class object is in type. destroy
	// contains any destroy objectives to be applied, before
	// the copy takes place (order is very important, since we
	// might copy into a temporary that was actually just released
	// by one of the destroy objectives). the whole sequence is
	// appended to node, and node will give you the address
	// of the temporary variable on return.

	size_t				typesize = get_type_size( type );

	variable_t*			tmpvar;
	code_t*				tmpvarcode;
	
	tmpvar = take_tmp_var( type );
		
	tmpvarcode = make_reg_code(
		OPS_PTR, REG_LOCAL, 0 );
	tmpvarcode = make_binary_code(
		INST_ADD, OPS_PTR, tmpvarcode,
		make_tvar_code( tmpvar ) );
		
	tmpvarcode->addr = true;
	
	bool				hint = false;
	code_t*				addr = node;
	
	code_t*				temp;
	size_t				popsize = typesize;
	
	if( type->code == TYPE_CLASS )
	{
		class_type_t*	clsstype = (class_type_t*)type;
		class_t*		clss = clsstype->clss;
		
		if( clss->cd_func[ DESTRUCTOR ] == 0 )
			hint = true;
	
		temp = make_xstore_code(
			clsstype, tmpvarcode,
			make_xload_code( clsstype, param ) );
	}
	else
	{
		temp = make_store_code(
			get_type_operand_size( type ), tmpvarcode, param );
	}

	// also pop value from stack left over from store
	popsize += get_type_stack_size( type );

	temp = make_pop_code( temp, popsize );
	
	if( destroy )
		node = make_destroy_code( node, destroy );
			
	temp = make_seq_code( CODE_SEQ_NEUTRAL, node, temp );
	
	node = make_seq_code( temp, tmpvarcode );

	if( hint )
		node = make_hint_code( CODE_STACKHINT, node, addr );

	//node = make_free_tvar_code( tmpvar, node );
	
	return node;
}

static code_t*
pass_stack_reference(
	code_t*				node,
	type_t*				rtype,
	code_t*				destroy )
{
	// situation: an object that exists only temporarily
	// is passed as a reference to another function, like
	// in function h in
	//
	// t f() {...}
	// void g( t& a ) {...}
	// void h() { g( f() ); }
	//
	// we cannot leave the object on the stack, since other
	// operations might take place, which don't know about
	// the object located there, so we copy it into a
	// temporary variable and use a pointer to the latter.

	//func_t*				func;
				
	//func = ( (func_code_t*)node )->func;

	//type_t*				rtype = func->decl->type;

	if( rtype->code == TYPE_VOID )
		return node;

	code_t*				param;
	code_t*				offset;
	size_t				rtypesize = get_type_size( rtype );
	
	param = make_reg_code(
		OPS_PTR, REG_STACK, 0 );
	offset = make_iconst_code(
		OPS_PTR, true, -rtypesize );
	param = make_binary_code(
		INST_ADD, OPS_PTR, offset, param );

	node = copy_to_temporary(
		node, param, rtype, destroy );
		
	return node;
}

static code_t*
pass_reference(
	code_t*				node,
	code_t*				destroy,
	fileref_t*			filepos )
{
	switch( node->inst )
	{
		case INST_LOAD:
		case INST_FLOAD:
		case INST_XLOAD:
			node = ( (unary_code_t*)node )->child;
			if( destroy )
				node = make_destroy_code( node, destroy );
			break;

		case INST_STORE:
		case INST_FSTORE:
		case INST_XSTORE:
			node = make_seq_code(
				node,
				pass_reference(
					( (xstore_code_t*)node )->child[ 1 ], 0, filepos ) );
					
			if( destroy )
				node = make_destroy_code( node, destroy );
			break;
			
		case INST_CRF:
		case INST_CSF:
		case INST_CVF:
			// function should have taken care of providing
			// us with the address of a temporary variable
			// containing the class on return.
			
			if( destroy )
				node = make_destroy_code( node, destroy );
			break;
			
		case CODE_DESTROY:
		{
			binary_code_t*		current;
			code_t*				right;
			
			current = (binary_code_t*)node;
			right = current->child[ 1 ];
			
			if( destroy == 0 )
				destroy = right;
			else
				destroy = make_seq_code( destroy, right );
				
			node = pass_reference(
				current->child[ 0 ], destroy, filepos );
				
			break;
		}
				
		case CODE_SEQ_POSTOP:
		{
			// special case. see make_postop_code()
			
			if( destroy )
				node = make_destroy_code( node, destroy );
			break;
		}
							
		default:
			compile_error( filepos,
				ERR_EXPECTED_LVALUE );
			break;
	}
	
	return node;
}

code_t*
make_pass_parameter_code(
	code_t*				node,
	type_t*				fromtype,
	type_t*				totype,
	fileref_t*			filepos )
{
	if( totype->code == TYPE_REFERENCE )
	{
		node = pass_reference(
			node, 0, filepos );
		
		fromtype = pure_type( fromtype );
		totype = pure_type( totype );
		
		node = promote_code_ptr_type(
			node, fromtype, totype );
	}
	else
	{
		fromtype = pure_type( fromtype );
		
		if( fromtype->code == TYPE_CLASS )
		{
			// type checking should have
			// assured, that fromtype and totype
			// are classes of the same type
			node = copy_class( node,
				fromtype, filepos );
		}	
		else
		{
			// promote integer and float types
			node = promote_code_type( node,
				fromtype, totype );
		}
	}
		
	return node;
}

static decl_t*
make_func_param_code(
	expr_t*				expr,
	code_t**			code,
	decl_t*				param )
{
	if( !expr )
		return param;

	if( expr->code == EXPR_COMMA )
	{
		decl_t*			next;
	
		next = make_func_param_code(
			( (binary_expr_t*)expr )->child[ 1 ], code, param->next );
		make_func_param_code(
			( (binary_expr_t*)expr )->child[ 0 ], code, param );
	
		return next;
	}
	else
	{
		code_t*			node;
		
		node = make_rval_code( expr );
		
		type_t*			fromtype = expr->type;
		type_t*			totype = param->type;
		
		node = make_pass_parameter_code(
			node, fromtype, totype, &expr->filepos );
		
		code_t*			prev = *code;
		
		if( prev == 0 )
			*code = node;
		else
			*code = make_param_code( prev, node );
	}
	
	return param->next;
}

static code_t*
add_regular_func_offset(
	func_expr_t*		expr,
	func_t*				func,
	code_t*				node )
{
	scope_t*			scope = expr->scope;
	
	// find class scope, out of which function is called
	while( scope && !scope->clss )
		scope = scope->up;

	if( !scope )
	{
		compile_error( 0, ERR_INTERNAL );
		return node;
	}

	class_t*			baseclss = func->scope->clss;
	code_t*				offset;
	unsigned long		intoffset;

	if( get_base_class_offset(
		scope->clss, baseclss, &intoffset ) != FOUND )
	{
		compile_error( 0, ERR_INTERNAL );
		return node;
	}
		
	offset = make_iconst_code( OPS_PTR, false, intoffset );
	node = make_binary_code( INST_ADD, OPS_PTR, node, offset );
	
	return node;
}

static code_t*
make_func_default_param_code(
	code_t*				param,
	decl_t*				paramdecl )
{
	while( paramdecl )
	{
		code_t*			node;
		decl_init_t*	init = paramdecl->init;
		
		if( init == 0 )
		{	
			compile_error( 0, ERR_INTERNAL );
			break;
		}
		
		node = make_rval_code( init->expr );
		node = promote_code_type(
			node, init->expr->type, paramdecl->type );
	
		if( param == 0 )
			param = node;
		else
			param = make_param_code( node, param );
			
		paramdecl = paramdecl->next;
	}
	
	return param;
}

static code_t*
make_func_code(
	func_expr_t*		expr )
{
	push_tmp_var_frame();

	func_t*				func = expr->func;
	code_t*				param = 0;
	decl_t*				paramdecl = func->param;
	
	paramdecl = make_func_param_code(
		expr->child[ 1 ], &param, paramdecl );
	
	param = make_func_default_param_code( param, paramdecl );
	
	code_t*				node;
	
	if( func->decl->spec->modifiers & MOD_STATIC )
	{
		node = make_func_code( INST_CSF,
			func, 0, param );
	}
	else
	{
		expr_t*			qual = expr->qual;
	
		if( qual == 0 )
			node = make_reg_code( OPS_PTR, REG_SELF, 0 );
		else
			node = make_rval_code( qual );
	
		e_inst_code		inst;
		
		if( expr->virtcall )
			inst = INST_CVF;
		else
		{
			inst = INST_CRF;
			node = add_regular_func_offset( expr, func, node );
		}
	
		node = make_func_code(
			inst, func, node, param );
	}

	code_t*				code;
	
	code = pop_tmp_var_frame();
	
	type_t*				rtype = func->decl->type;
	
	rtype = reduce_type( rtype );
	
	if( rtype->code == TYPE_CLASS )
	{
		// if we return a whole class we do this by
		// copying it to a temporary variable and
		// taking it off the stack.
		node = pass_stack_reference( node, rtype, code );
	}
	else if( code )
		node = make_destroy_code( node, code );
	
	// if a function returns a reference, signalize
	// that we're dealing with an address
	if( rtype->code == TYPE_REFERENCE )
		node->addr = true;

	return node;
}


static code_t*
make_addr_code(
	unary_expr_t*		expr )
{
	code_t*				node = make_code( expr->child );
	
	if( node->addr )
		node->addr = false;
	else
	{
		switch( node->inst )
		{
			case INST_CRF:
			case INST_CSF:
			case INST_CVF:
				compile_error( &expr->filepos,
					ERR_EXPECTED_LVALUE );
				break;
			
			default:
				node = pass_reference(
					node, 0, &expr->filepos );
				break;
		}
	}
	
	return node;
}

static code_t*
make_deref_code(
	unary_expr_t*		expr )
{
	code_t*				node = make_rval_code( expr->child );
	
	//node = make_load_code(
	//	get_type_operand_size( expr->type ), node );
	
	node->addr = true;
	
	return node;
}

static code_t*
make_class_cast_code(
	code_t*				node,
	class_t*			fromclass,
	class_t*			toclass )
{
	unsigned long		intoffset;
	code_t*				offset;

	if( get_base_class_offset( fromclass, toclass, &intoffset ) == FOUND )
	{
		if( intoffset )
		{
			offset = make_iconst_code( OPS_PTR, false, intoffset );
			node = make_binary_code( INST_ADD, OPS_PTR, node, offset );
		}
	}
	else
	{
		if( get_base_class_offset( toclass, fromclass, &intoffset ) != FOUND )
			compile_error( 0, ERR_INTERNAL );
			
		node = make_dynamic_cast_code(
			toclass, intoffset, node );
	}
		
	return node;
}

static code_t*
make_ptrcast_code(
	code_t*				node,
	ptr_type_t*			fromtype,
	ptr_type_t*			totype )
{
	type_t*				fromdown = fromtype->down;
	type_t*				todown = totype->down;

	const e_type_code	fromdowncode = fromdown->code;
	const e_type_code	todowncode = todown->code;

	if( fromdowncode == TYPE_CLASS &&
		todowncode == TYPE_CLASS )
	{
		class_t*		fromclass = ( (class_type_t*)fromdown )->clss;
		class_t*		toclass = ( (class_type_t*)todown )->clss;
		
		if( fromclass != toclass )
		{
			node = make_class_cast_code(
				node, fromclass, toclass );
		}
	}
	
	return node;
}

static code_t*
make_cast_code(
	unary_expr_t*		expr )
{
	code_t*				node = make_rval_code( expr->child );

	type_t*				fromtype = expr->child->type;
	type_t*				totype = expr->type;
	
	fromtype = reduce_type( fromtype );
	totype = reduce_type( totype );

	switch( fromtype->code )
	{
		TYPE_CASE_ARITHMETIC
		case TYPE_BOOL:
		
			node = promote_code_type(
				node, fromtype, totype );
			break;
			
		case TYPE_POINTER:
			
			switch( totype->code )
			{
				case TYPE_BOOL:
					node = promote_code_type(
						node, fromtype, totype );
					break;
			
				case TYPE_POINTER:
					node = make_ptrcast_code(
						node, (ptr_type_t*)fromtype,
						(ptr_type_t*)totype );
			}
			break;
			
		case TYPE_CLASS:
			break;
	}
	
	return node;
}

static code_t*
make_bitnot_code(
	unary_expr_t*		expr )
{
	code_t*				node = make_rval_code( expr->child );

	return make_unary_code( INST_NOT,
		get_type_operand_size( expr->type ), node );
}

static code_t*
make_neg_code(
	unary_expr_t*		expr )
{
	code_t*				node = make_rval_code( expr->child );
	e_operand_size		size = get_type_operand_size( expr->type );

	switch( size )
	{
		case OPS_SINGLE:
		case OPS_DOUBLE:
			node = make_unary_code( INST_FNEG, size, node );
			break;
			
		default:
			node = make_unary_code( INST_NEG, size, node );
			break;
	}

	return node;
}

static code_t*
make_shift_code(
	e_inst_code			sopcode,
	e_inst_code			uopcode,
	binary_expr_t*		expr )
{
	expr_t*				lexpr = expr->child[ 0 ];
	expr_t*				rexpr = expr->child[ 1 ];

	type_t*				ltype = lexpr->type;
	type_t*				rtype = rexpr->type;

	code_t*				left;
	code_t*				right;
	
	left = make_rval_code( lexpr );
	right = make_rval_code( rexpr );
	
	right = promote_code_int_type( right,
		get_type_operand_size( rtype ), OPS_BYTE, false );

	e_inst_code			opcode;

	if( is_type_signed( ltype ) )
		opcode = sopcode;
	else
		opcode = uopcode;

	return make_binary_code( opcode,
		get_type_operand_size( ltype ),
		left, right );
}

static code_t*
make_assign_code(
	expr_t*				lexpr,
	expr_t*				rexpr )
{	
	type_t*				ltype = reduce_type( lexpr->type );
	type_t*				rtype = rexpr->type;

	code_t*				left;
	code_t*				right;
	
	left = make_code( lexpr );
	right = make_rval_code( rexpr );
	
	ltype = pure_type( ltype );
	e_type_code			lcode = ltype->code;
	
	code_t*				code;
		
	if( lcode == TYPE_CLASS )
	{
		code = make_xstore_code(
			(class_type_t*)ltype, left, right );
	}
	else
	{	
		right = promote_code_type( right, rtype, ltype );

		code = make_store_code(
			get_type_operand_size( ltype ), left, right );
	}
	
	return code;
}

static code_t*
make_initialize_code(
	expr_t*				lexpr,
	expr_t*				rexpr )
{
	type_t*				ltype = reduce_type( lexpr->type );
	
	if( ltype->code != TYPE_REFERENCE )
		return make_assign_code( lexpr, rexpr );

	// initialize a reference like in int& a = b by
	// copying the address of b into variable a

	code_t*				left;
	code_t*				right;
	code_t*				code;

	left = make_code( lexpr );
	right = make_code( rexpr );

	type_t*				type = ltype;

	while( type->code == TYPE_REFERENCE )
	{
		if( left->inst != INST_LOAD )
		{
			compile_error( 0, ERR_INTERNAL );
			return right;
		}
		
		left = ( (unary_code_t*)left )->child;
		type = ( (ref_type_t*)type )->down;
	}

	code = make_store_code(
		OPS_PTR, left, right );
	
	return code;
}

static code_t*
make_assignop_code(
	e_inst_code			iopcode,
	e_inst_code			fopcode,
	binary_expr_t*		expr )
{
	expr_t*				lexpr = expr->child[ 0 ];
	expr_t*				rexpr = expr->child[ 1 ];

	type_t*				ltype = lexpr->type;
	type_t*				rtype = rexpr->type;

	code_t*				leftl;
	code_t*				leftr;
	code_t*				right;
	
	leftl = make_code( lexpr );
	
	ltype = pure_type( ltype );

	e_operand_size		lsize = get_type_operand_size( ltype );

	leftr = make_load_code( lsize, leftl );
	
	right = make_rval_code( rexpr );
	right = promote_code_type( right, rtype, ltype );
	
	e_inst_code			opcode;
	
	if( is_type_int_operand( ltype ) )
		opcode = iopcode;
	else
		opcode = fopcode;

	code_t*				node;
	
	node = make_binary_code( opcode, lsize, leftr, right );	
	
	return make_store_code( lsize, leftl, node );
}

static code_t*
make_assignmod_code(
	binary_expr_t*		expr )
{
	expr_t*				lexpr = expr->child[ 0 ];
	expr_t*				rexpr = expr->child[ 1 ];

	type_t*				ltype = lexpr->type;
	type_t*				rtype = rexpr->type;

	code_t*				leftl;
	code_t*				leftr;
	code_t*				right;
	
	leftl = make_code( lexpr );

	ltype = pure_type( ltype );

	e_operand_size		lsize = get_type_operand_size( ltype );

	leftr = make_load_code( lsize, leftl );
	
	right = make_rval_code( rexpr );
	right = promote_code_type( right, rtype, ltype );
	
	code_t*				node;
	
	node = make_binary_code( INST_DIV, lsize, leftr, right );
	node = make_binary_code( INST_MUL, lsize, node, right );
	node = make_binary_code( INST_SUB, lsize, leftr, node );
	
	return make_store_code( lsize, leftl, node );
}

static code_t*
make_assignshift_code(
	e_inst_code			sopcode,
	e_inst_code			uopcode,
	binary_expr_t*		expr )
{
	expr_t*				lexpr = expr->child[ 0 ];
	expr_t*				rexpr = expr->child[ 1 ];

	type_t*				ltype = lexpr->type;
	type_t*				rtype = rexpr->type;

	code_t*				leftl;
	code_t*				leftr;
	code_t*				right;
	
	leftl = make_code( lexpr );

	ltype = pure_type( ltype );

	e_operand_size		lsize = get_type_operand_size( ltype );

	leftr = make_load_code( lsize, leftl );
	
	right = make_rval_code( rexpr );
	right = promote_code_int_type( right,
		get_type_operand_size( rtype ), OPS_QUAD, false );

	e_inst_code			opcode;

	if( is_type_signed( ltype ) )
		opcode = sopcode;
	else
		opcode = uopcode;
	
	code_t*				node;
	
	node = make_binary_code( opcode, lsize, leftr, right );
	
	return make_store_code( lsize, leftl, node );
}

static code_t*
make_id_code(
	id_expr_t*			expr )
{
	scope_t*			scope = expr->scope;
	class_t*			clss;
	
	while( scope )
	{
		clss = scope->clss;
		if( clss )
			break;
		scope = scope->up;
	}

	return make_var_code( expr->var, clss );
}

static code_t*
make_postop_code(
	expr_t*				expr )
{
	code_t*				node = make_rval_code( expr );
	type_t*				type = expr->type;

	if( expr->code != EXPR_CALL_FUNC ||
		type->code != TYPE_REFERENCE )
	{
		compile_error( &expr->filepos, ERR_INTERNAL );
		return node;
	}
		
	type = ( (ref_type_t*)type )->down;
	if( type->code != TYPE_CLASS )
	{
		compile_error( &expr->filepos, ERR_INTERNAL );
		return node;
	}

	func_expr_t*		funcexpr = (func_expr_t*)expr;
	
	/*compile_error( &expr->filepos, ERR_NOT_IMPLEMENTED );
	return node;*/
		
	code_t*				param;

	param = pass_stack_reference( copy_class(
		make_rval_code( funcexpr->child[ 1 ] ),
		type, &expr->filepos ), type, 0 );

	node = make_seq_code( CODE_SEQ_POSTOP, param,
		make_pop_code( node, 4 ) );
	
	return node;
}

code_t*
make_code(
	expr_t*				expr )
{
	// note that this function returns lval code (address of value),
	// to obtain rval code, use make_rval_code

	switch( expr->code )
	{
		case EXPR_ICONST: {
			type_t* type = expr->type;
			return make_iconst_code(
				get_type_operand_size( type ),
				is_type_signed( type ),
				( (iconst_expr_t*)expr )->val );
		}

		case EXPR_FCONST:
			return make_fconst_code(
				get_type_operand_size( expr->type ),
				( (fconst_expr_t*)expr )->val );
		
		case EXPR_ID:
		case EXPR_VARIABLE:
			return make_id_code( (id_expr_t*)expr );
		
		case EXPR_STR:
			return make_str_code( ( (str_expr_t*)expr )->str );
			
		case EXPR_NULL:
			return make_iconst_code( OPS_PTR, false, 0 );
			
		case EXPR_THIS:
			return make_reg_code( OPS_PTR, REG_SELF, 0 );
				
		case EXPR_ACCESS_ARRAY:
			return make_array_code( (binary_expr_t*)expr );
			
		case EXPR_ACCESS_DOT:
			return make_access_code( (binary_expr_t*)expr, false );
			
		case EXPR_ACCESS_PTR:
			return make_access_code( (binary_expr_t*)expr, true );
		
		case EXPR_CALL_FUNC:
			return make_func_code( (func_expr_t*)expr );
			
		case EXPR_NEW:
			return make_new_code(
				( (new_expr_t*)expr )->newtype,
				( (new_expr_t*)expr )->elemtype );
			
		case EXPR_DTNEW:
			return make_dtnew_code(
				( (new_expr_t*)expr )->newtype,
				( (new_expr_t*)expr )->elemtype,
				( (dtnew_expr_t*)expr )->newname );
				
		case EXPR_DELETE:
			return make_delete_code(
				( (delete_expr_t*)expr )->elemtype,
				( (delete_expr_t*)expr )->delobject );
		
		case EXPR_ADDRESS_OF:
			return make_addr_code( (unary_expr_t*)expr );
			
		case EXPR_DEREFERENCE:
			return make_deref_code( (unary_expr_t*)expr );

		case EXPR_TYPECAST:
			return make_cast_code( (unary_expr_t*)expr );
			
		case EXPR_PREINC:
			return make_incdec_code( (unary_expr_t*)expr,
				get_type_inc_size( expr->type ), true );

		case EXPR_PREDEC:
			return make_incdec_code( (unary_expr_t*)expr,
				-get_type_inc_size( expr->type ), true );

		case EXPR_POSTINC:
			return make_incdec_code( (unary_expr_t*)expr,
				get_type_inc_size( expr->type ), false );

		case EXPR_POSTDEC:
			return make_incdec_code( (unary_expr_t*)expr,
				-get_type_inc_size( expr->type ), false );

		case EXPR_LOGICAL_NOT:
		case EXPR_LOGICAL_AND:
		case EXPR_LOGICAL_OR:
		case EXPR_EQ:
		case EXPR_NE:
		case EXPR_LT:
		case EXPR_GT:
		case EXPR_LE:
		case EXPR_GE:
			return make_var_code( inst_arith_expr( expr ), 0 );

		case EXPR_BITWISE_NOT:
			return make_bitnot_code( (unary_expr_t*)expr );

		case EXPR_BITWISE_AND:
			return make_arith_code(
				INST_AND, INST_AND, (binary_expr_t*)expr );

		case EXPR_BITWISE_OR:
			return make_arith_code(
				INST_OR, INST_OR, (binary_expr_t*)expr );

		case EXPR_BITWISE_XOR:
			return make_arith_code(
				INST_XOR, INST_XOR, (binary_expr_t*)expr );
			
		case EXPR_ADD:
			return make_addsub_code(
				INST_ADD, INST_FADD, (binary_expr_t*)expr );
	
		case EXPR_SUB:
			return make_addsub_code(
				INST_SUB, INST_FSUB, (binary_expr_t*)expr );
	
		case EXPR_NEGATE:
			return make_neg_code( (unary_expr_t*)expr );
	
		case EXPR_MUL:
			return make_arith_code(
				INST_MUL, INST_FMUL, (binary_expr_t*)expr );
	
		case EXPR_DIV:
			return make_arith_code(	
				INST_DIV, INST_FDIV, (binary_expr_t*)expr );
	
		case EXPR_MOD:
			return make_arith_code(
				INST_MOD, INST_FMOD, (binary_expr_t*)expr );
	
		case EXPR_SHL:
			return make_shift_code(
				INST_SHL, INST_SHL, (binary_expr_t*)expr );

		case EXPR_SHR:
			return make_shift_code(
				INST_ASR, INST_SHR, (binary_expr_t*)expr );
				
		case EXPR_ASSIGN:
			return make_assign_code(
				( (binary_expr_t*)expr )->child[ 0 ],
				( (binary_expr_t*)expr )->child[ 1 ] );

		case EXPR_INITIALIZE:
			return make_initialize_code(
				( (binary_expr_t*)expr )->child[ 0 ],
				( (binary_expr_t*)expr )->child[ 1 ] );

		case EXPR_MUL_ASSIGN:
			return make_assignop_code(
				INST_MUL, INST_FMUL, (binary_expr_t*)expr );

		case EXPR_DIV_ASSIGN:
			return make_assignop_code(
				INST_DIV, INST_FDIV, (binary_expr_t*)expr );

		case EXPR_ADD_ASSIGN:
			return make_assignop_code(
				INST_ADD, INST_FADD, (binary_expr_t*)expr );

		case EXPR_SUB_ASSIGN:
			return make_assignop_code(
				INST_SUB, INST_FSUB, (binary_expr_t*)expr );
		
		case EXPR_AND_ASSIGN:
			return make_assignop_code(
				INST_AND, INST_AND, (binary_expr_t*)expr );

		case EXPR_XOR_ASSIGN:
			return make_assignop_code(
				INST_XOR, INST_XOR, (binary_expr_t*)expr );

		case EXPR_OR_ASSIGN:
			return make_assignop_code(
				INST_OR, INST_OR, (binary_expr_t*)expr );

		case EXPR_MOD_ASSIGN:
			return make_assignmod_code( (binary_expr_t*)expr );
		
		case EXPR_SHL_ASSIGN:
			return make_assignshift_code(
				INST_SHL, INST_SHL, (binary_expr_t*)expr );
		
		case EXPR_SHR_ASSIGN:
			return make_assignshift_code(
				INST_ASR, INST_SHR, (binary_expr_t*)expr );

		case EXPR_PREUNA_OPERATOR:
			return make_code(
				( (unary_expr_t*)expr )->child );
				
		case EXPR_POSTUNA_OPERATOR:
			return make_postop_code(
				( (unary_expr_t*)expr )->child );
	
		case EXPR_BIN_OPERATOR:
			return make_code(
				( (binary_expr_t*)expr )->child[ 0 ] );
				
		case EXPR_THROWN_OBJECT:
			return make_unary_code(
				INST_ENTER_CATCH, OPS_PTR, 0 );

		case EXPR_COMMA:
			return make_comma_code(
				make_code( ( (binary_expr_t*)expr )->child[ 0 ] ),
				make_code( ( (binary_expr_t*)expr )->child[ 1 ] ) );
	}

	compile_error( 0, ERR_INTERNAL );
	return make_iconst_code( OPS_NONE, false, 0 );
}

END_COMPILER_NAMESPACE
