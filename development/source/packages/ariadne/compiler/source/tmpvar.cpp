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
//	tmpvar.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "tmpvar.h"
#include "type_t.h"
#include "scope_t.h"
#include "xalloc.h"

#include "class_t.h"
#include "code_t.h"
#include "parser.h"

BEGIN_COMPILER_NAMESPACE

// one place where we use temporary variables is in functions returning
// classes and passing these as reference parameter to other functions
// like calling f( g() ) in
//
// class A { }
// A g() { }
// void f( A& a )

struct tmp_variable_t : variable_t {
	tmp_variable_t*		next;
	tmp_variable_t*		prev;
	long				size;
	char*				name;
	decl_t				fakedecl;
};

struct frame_t {
	tmp_variable_t*		free;
	tmp_variable_t*		used;
};

static frame_t			s_frame[ 32 ];
static int				s_frame_index;

void
init_tmp_vars()
{
	s_frame_index = 0;
	s_frame[ 0 ].free = 0;
	s_frame[ 0 ].used = 0;
}

variable_t*
take_tmp_var(
	type_t*				type )
{
	frame_t*			frame = &s_frame[ s_frame_index ];

	long				size = get_type_size( type );
	tmp_variable_t*		var;

	if( frame->free )
	{
		var = frame->free;
		frame->free = var->next;
		
		if( size > var->size )
			var->size = size;
	}
	else
	{
		var = (tmp_variable_t*)xalloc( sizeof( tmp_variable_t ) );
		var->decl = &var->fakedecl;
		var->name = 0;
		var->modifiers = MOD_TEMP;
		var->offset = 0;
		var->size = size;
		
		var->fakedecl.name = "";
		var->fakedecl.var = var;
	}

	var->decl->type = type;
	
	tmp_variable_t*		first = frame->used;
	
	var->next = first;
	var->prev = 0;
	if( first )
		first->prev = var;
	
	frame->used = var;

	return var;
}

static code_t*
make_destructor_call(
	tmp_variable_t*		var )
{
	type_t*				type = var->decl->type;
	
	type = resolve_type( type );
	type = pure_type( type );
	
	if( type->code != TYPE_CLASS )
		return 0;

	class_t*			clss;
	func_t*				func;
	
	clss = ( (class_type_t*)type )->clss;
	func = clss->cd_func[ DESTRUCTOR ];
	if( !func )
		return 0;
			
	code_t*				instance;
	code_t*				node;

	instance = make_reg_code(
		OPS_PTR, REG_LOCAL, 0 );
	instance = make_binary_code(
		INST_ADD, OPS_PTR, instance,
		make_tvar_code( var ) );
	instance->addr = true;
	
	node = make_func_code(
		INST_CRF, func, instance, 0 );

	return node;
}

void
free_tmp_var(
	variable_t*			var )
{
	if( !( var->modifiers & MOD_TEMP ) )
		return;

	frame_t*			frame = &s_frame[ s_frame_index ];
			
	/*code_t*				code = make_destructor_call( var );
			
	if( code )
		compile_error( 0, ERR_INTERNAL );*/
			
	tmp_variable_t*		tvar = (tmp_variable_t*)var;
	tmp_variable_t*		prev = tvar->prev;
	tmp_variable_t*		next = tvar->next;
	
	if( prev )
		prev->next = next;
	else
		frame->used = next;
	
	if( next )
		next->prev = prev;
	
	tvar->next = frame->free;
	frame->free = tvar;
}

static code_t*
free_all_tmp_vars()
{
	frame_t*			frame = &s_frame[ s_frame_index ];

	tmp_variable_t*		var = frame->used;
	code_t*				node = 0;

	while( var )
	{
		code_t*			code = make_destructor_call( var );
		
		if( node == 0 )
			node = code;
		else
			node = make_seq_code( node, code );
	
		tmp_variable_t*	next = var->next;
		var->next = frame->free;
		frame->free = var;
		var = next;
	}

	return node;
}

void
push_tmp_var_frame()
{
	// take the free list from the current frame and open up
	// a new frame having the same free list.

	int					index = s_frame_index;

	frame_t*			parent = &s_frame[ index ];

	index += 1;
	if( index >= 32 )
	{
		compile_error( 0, ERR_INTERNAL );
		return;
	}

	frame_t*			frame = &s_frame[ index ];

	frame->free = parent->free;
	frame->used = 0;
	
	s_frame_index = index;
}

code_t*
pop_tmp_var_frame()
{
	// release the temporary variables of the current frame and
	// make them the parent frame's free list.

	code_t*				code;

	code = free_all_tmp_vars();

	int					index = s_frame_index;

	index -= 1;
	if( index < 0 )
	{
		compile_error( 0, ERR_INTERNAL );
		return 0;
	}

	frame_t*			frame = &s_frame[ index ];
	
	frame->free = frame[ 1 ].free;
	
	s_frame_index = index;
	
	return code;
	
}

extern unsigned long	s_local_stack_size;

void
locate_tmp_vars()
{
#if ICARUS_SAFE
	if( s_frame_index != 0 )
		throw_icarus_error( "internal error" );
#endif

	frame_t*			frame = &s_frame[ s_frame_index ];
	tmp_variable_t*		var = frame->free;
	
	unsigned long		offset = s_local_stack_size;
	
	while( var )
	{	
		if( offset & 3 )
			offset += 4 - ( offset & 3 );

		var->offset = offset;
		offset += var->size;
		var = var->next;
	}

	if( offset & 3 )
		offset += 4 - ( offset & 3 );
	
	s_local_stack_size = offset;
}

END_COMPILER_NAMESPACE
