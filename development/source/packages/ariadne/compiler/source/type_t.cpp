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
//	type_t.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "type_t.h"
#include "xalloc.h"
#include "class_t.h"
#include "output.h"
#include "scope_t.h"
#include "expr_t.h"
#include "resolve.h"

#include <iostream>
#include <string>

BEGIN_COMPILER_NAMESPACE

using namespace std;

type_list_t*
make_type_list(
	type_list_t*		next,
	type_t*				type )
{
	type_list_t*		node;
	
	node = (type_list_t*)xalloc( sizeof( type_list_t ) );
	node->next = 0;
	node->type = type;

	return node;
}

type_seq_t*
make_type_seq(
	type_seq_t*			next,
	type_t*				type )
{
	type_seq_t*			node;
	
	node = (type_seq_t*)xalloc( sizeof( type_seq_t ) );
	node->next = 0;
	node->type = type;

	return node;
}

inline long
get_ptr_type_array_size(
	ptr_type_t*			type )
{
	expr_t*				expr = type->array_expr;
	
	if( expr->code != EXPR_ICONST )
		return 0;
	
	return ( (iconst_expr_t*)expr )->val;
}

type_t*
make_type(
	e_type_code			code )
{
	type_t*				t = (type_t*)xalloc( sizeof( type_t ) );
	
	t->code = code;
	t->quals = 0;
	
	return t;
}

int_type_t*
make_int_type(
	e_type_code			code,
	bool				is_signed )
{
	int_type_t*			t = (int_type_t*)xalloc( sizeof( int_type_t ) );
	
	t->code = code;
	t->quals = 0;
	t->is_signed = is_signed;
	
	return t;
}

ptr_type_t*
make_ptr_type(
	type_t*				down,
	int					quals )
{
	ptr_type_t*			t = (ptr_type_t*)xalloc( sizeof( ptr_type_t ) );
	
	t->code = TYPE_POINTER;
	t->quals = quals;
	t->down = down;
	t->is_array = false;
		
	return t;
}

ref_type_t*
make_ref_type(
	type_t*				down,
	int					quals )
{
	ref_type_t*			t = (ref_type_t*)xalloc( sizeof( ref_type_t ) );
	
	t->code = TYPE_REFERENCE;
	t->quals = quals;
	t->down = down;
		
	return t;
}

class_type_t*
make_class_type(
	class_t*			clss )
{
	class_type_t*		t = (class_type_t*)xalloc( sizeof( class_type_t ) );
	
	t->code = TYPE_CLASS;
	t->quals = 0;
	t->clss = clss;
	
	return t;
}

enum_type_t*
make_enum_type(
	const char*			name,
	class_t*			clss )
{
	enum_type_t*		t = (enum_type_t*)xalloc( sizeof( enum_type_t ) );
	
	t->code = TYPE_ENUM;
	t->quals = 0;
	t->name = name;
	t->clss = clss;
	
	return t;
}

unresolved_type_t*
make_unresolved_type(
	qualname_t*			qualname )
{
	unresolved_type_t*	t = (unresolved_type_t*)xalloc(
							sizeof( unresolved_type_t ) );
	
	t->code = TYPE_UNRESOLVED;
	t->quals = 0;
	t->qualname = qualname;
	get_filepos( &t->filepos );
	
	return t;
}

typedef_type_t*
make_typedef_type(
	const char*			name,
	type_t*				type,
	type_seq_t*			tmpltype,
	class_t*			clss )
{
	typedef_type_t*		t = (typedef_type_t*)xalloc(
							sizeof( typedef_type_t ) );
	
	t->code = TYPE_TYPEDEF;
	t->quals = 0;
	t->name = name;
	t->type = type;
	t->tmpltype = tmpltype;
	t->clss = clss;
	
	return t;
}

template_type_t*
make_template_type(
	const char*			name,
	int					index )
{
	tt_type_t*			s = (tt_type_t*)xalloc(
							sizeof( tt_type_t ) );

	template_type_t*	t = (template_type_t*)s;
	
	t->code = TYPE_TEMPLATE;
	t->quals = 0;

	t->fcid = 0;
	t->bind = 0;
	t->name = name;
	t->index = index;

	return t;
}

bool
is_type_signed(
	type_t*				type )
{
	switch( type->code )
	{
		TYPE_CASE_INTEGER
			return ( (int_type_t*)type )->is_signed;

		case TYPE_ENUM:
			return true;

		case TYPE_REFERENCE:
			return is_type_signed(
				( (ref_type_t*)type )->down );	
			
		case TYPE_TYPEDEF:
			return is_type_signed(
				( (typedef_type_t*)type )->type );
	
		default:
			return false;
	}
}

bool
is_type_arithmetic(
	type_t*				type )
{
	switch( type->code )
	{
		TYPE_CASE_ARITHMETIC
			return true;
		
		case TYPE_REFERENCE:
			return is_type_arithmetic(
				( (ref_type_t*)type )->down );	
			
		case TYPE_TYPEDEF:
			return is_type_arithmetic(
				( (typedef_type_t*)type )->type );
	
		default:
			return false;
	}
}

bool
is_type_floating(
	type_t*				type )
{
	switch( type->code )
	{
		case TYPE_FLOAT:
		case TYPE_DOUBLE:
			return true;
	}
	
	return false;
}

bool
is_type_integral(
	type_t*				type )
{
	switch( type->code )
	{
		TYPE_CASE_INTEGER
		case TYPE_ENUM:
		case TYPE_BOOL:
			return true;

		case TYPE_REFERENCE:
			return is_type_integral(
				( (ref_type_t*)type )->down );	
	
		case TYPE_TYPEDEF:
			return is_type_integral(
				( (typedef_type_t*)type )->type );
	
		default:
			return false;
	}	
}

bool
is_type_boolean(
	type_t*				type )
{
	switch( type->code )
	{
		TYPE_CASE_ARITHMETIC
		case TYPE_BOOL:
		case TYPE_POINTER:
			return true;

		case TYPE_REFERENCE:
			return is_type_boolean(
				( (ref_type_t*)type )->down );	
			
		case TYPE_TYPEDEF:
			return is_type_boolean(
				( (typedef_type_t*)type )->type );
	
		default:
			return false;
	}
}

bool
is_type_size_known(
	type_t*				type )
{
	switch( type->code )
	{
		TYPE_CASE_ARITHMETIC
		case TYPE_VOID:
		case TYPE_BOOL:
		case TYPE_ENUM:
			return true;
		
		case TYPE_POINTER:
		{
			ptr_type_t*		ptr_type;
		
			ptr_type = (ptr_type_t*)type;
			
			if( ptr_type->is_array )
			{
				// check if array dimension is known. this could
				// be unknown due to unresolved const variables
				// used within the bounds specification.
				if( constants_resolved() == false )
					return false;
			
				return is_type_size_known( ptr_type->down );
			}
					
			return true;
		}
		
		case TYPE_REFERENCE:
			return true;
	
		case TYPE_CLASS:
			return is_class_size_known(
				( (class_type_t*)type )->clss );
		
		case TYPE_TYPEDEF:
			return is_type_size_known(
				( (typedef_type_t*)type )->type );
				
		default:
			return false;
	}
}

static long
get_ptr_type_size(
	ptr_type_t*			type )
{
	if( !type->is_array )
		return 4;	

	long				elemsize;

	elemsize = get_type_size( type->down );

	expr_t*				expr = type->array_expr;
	intval_t			elemcount;

	if( get_expr_iconst( &expr, elemcount ) == false )
	{
		compile_error( 0, ERR_INTERNAL );
		elemcount = 0;
	}
	
	type->array_expr = expr;
		
	return elemcount * elemsize;
}

long
get_type_size(
	type_t*				type )
{
	switch( type->code )
	{
		case TYPE_CHAR:
		case TYPE_BYTE:
			return 1;
		
		case TYPE_SHORT:
			return 2;
			
		case TYPE_INT:
		case TYPE_ENUM:
			return 4;
		
		case TYPE_LONG:
			return 8;
		
		case TYPE_WIDE:
			return 16;
		
		case TYPE_VOID:
			return 0;

		case TYPE_BOOL:
			return 1;
			
		case TYPE_FLOAT:
			return 4;
			
		case TYPE_DOUBLE:
			return 8;

		case TYPE_POINTER:
			return get_ptr_type_size( (ptr_type_t*)type );
				
		case TYPE_REFERENCE:
			return 4;
				
		case TYPE_CLASS:
		{
			class_t*	clss = ( (class_type_t*)type )->clss;
		
			return get_class_size( clss );
		}

		case TYPE_TYPEDEF:
			return get_type_size(
				( (typedef_type_t*)type )->type );

		default:
			compile_error( 0, ERR_ILLEGAL_TYPE );
			break;
	}
	
	return 0;
}

type_t*
common_type(
	type_t*				typeone,
	type_t*				typetwo )
{
	typeone = pure_type( typeone );
	typetwo = pure_type( typetwo );

	const e_type_code	codeone = typeone->code;
	const e_type_code	codetwo = typetwo->code;

	if( codeone == TYPE_DOUBLE )
	{
		if( is_type_integral( typetwo ) ||
			codetwo == TYPE_FLOAT ||
			codetwo == TYPE_DOUBLE )
			return typeone;
	}
	else if( codetwo == TYPE_DOUBLE )
	{
		if( is_type_integral( typeone ) ||
			codeone == TYPE_FLOAT )
			return typetwo;
	}
	
	else if( codeone == TYPE_FLOAT )
	{
		if( is_type_integral( typetwo ) ||
			codetwo == TYPE_FLOAT )
			return typeone;
	}
	else if( codetwo == TYPE_FLOAT )
	{
		if( is_type_integral( typeone ) )
			return typetwo;
	}
	else if( codeone == TYPE_BOOL && codetwo == TYPE_BOOL )
		return typeone;	
	else if( is_type_integral( typeone ) &&
		is_type_integral( typetwo ) )
	{
		e_type_code		code;
	
		if( codeone == TYPE_WIDE || codetwo == TYPE_WIDE )
			code = TYPE_WIDE;
		else if( codeone == TYPE_LONG || codetwo == TYPE_LONG )
			code = TYPE_LONG;
		else if( codeone == TYPE_INT || codetwo == TYPE_INT )
			code = TYPE_INT;
		else if( codeone == TYPE_ENUM || codetwo == TYPE_ENUM )
			code = TYPE_INT;
		else if( codeone == TYPE_SHORT || codetwo == TYPE_SHORT )
			code = TYPE_SHORT;
		else if( codeone == TYPE_CHAR || codetwo == TYPE_CHAR )
			code = TYPE_CHAR;
		else if( codeone == TYPE_BYTE && codetwo == TYPE_BYTE )
			code = TYPE_BYTE;
		else
		{
			code = TYPE_INT;
			compile_error( 0, ERR_INTERNAL );
		}
			
		int_type_t*		type;
		bool			is_signed;
			
		is_signed = ( (int_type_t*)typeone )->is_signed &&
			( (int_type_t*)typetwo )->is_signed;
			
		type = make_int_type( code, is_signed );
			
		return type;
	}
	else if( codeone == TYPE_POINTER && codetwo == TYPE_NULL )
		return typeone;
	else if( codeone == TYPE_NULL && codetwo == TYPE_POINTER )
		return typetwo;
	else if( codeone == TYPE_POINTER && codetwo == TYPE_POINTER )
	{
		if( is_same_type(
			( (ptr_type_t*)typeone )->down,
			( (ptr_type_t*)typetwo )->down ) )
		{
			return typeone;
		}
	}
	
	return 0;
}

bool
are_types_comparable_eq(
	type_t*				typeone,
	type_t*				typetwo )
{
	typetwo = pure_type( typetwo );

	const e_type_code	codeone = typeone->code;
	const e_type_code	codetwo = typetwo->code;
	
	switch( codeone )
	{
		TYPE_CASE_ARITHMETIC
		
			switch( codetwo )
			{
				TYPE_CASE_ARITHMETIC
					return true;
			}
		
			return false;
						
		case TYPE_POINTER:
			
			if( codetwo == TYPE_POINTER ||
				codetwo == TYPE_NULL )
				return true;
				
			return false;
			
		case TYPE_NULL:
		
			if( codetwo == TYPE_POINTER )
				return true;
				
			return false;

		case TYPE_ENUM:		
			if( is_same_type( typeone, typetwo ) )
				return true;
			
		case TYPE_TYPEDEF:
			return are_types_comparable_eq(
				( (typedef_type_t*)typeone )->type, typetwo );

		case TYPE_REFERENCE:
			return are_types_comparable_eq(
				( (ref_type_t*)typeone )->down, typetwo );			
				
		case TYPE_BOOL:
		
			if( codetwo == TYPE_BOOL )
				return true;
				
			return false;
	}
	
	return false;
}

bool
are_types_comparable_lg(
	type_t*				typeone,
	type_t*				typetwo )
{
	typetwo = pure_type( typetwo );

	const e_type_code	codeone = typeone->code;
	const e_type_code	codetwo = typetwo->code;
	
	switch( codeone )
	{
		TYPE_CASE_ARITHMETIC
		
			switch( codetwo )
			{
				TYPE_CASE_ARITHMETIC
					return true;
			}
		
			return false;
			
		case TYPE_POINTER:
			
			if( codetwo == TYPE_POINTER )
				return true;
				
			return false;

		case TYPE_ENUM:		
			if( is_same_type( typeone, typetwo ) )
				return true;
			
		case TYPE_TYPEDEF:
			return are_types_comparable_lg(
				( (typedef_type_t*)typeone )->type, typetwo );

		case TYPE_REFERENCE:
			return are_types_comparable_lg(
				( (ref_type_t*)typeone )->down, typetwo );			
	}
	
	return false;
}

bool
is_type_constant(
	type_t*				type )
{
	if( type->quals & TC_CONST )
		return true;

	switch( type->code )
	{
		case TYPE_REFERENCE:
			return is_type_constant(
				( (ref_type_t*)type )->down );
				
		case TYPE_TYPEDEF:
			return is_type_constant(
				( (typedef_type_t*)type )->type );
	}
	
	return false;
}

bool
can_assign_type(
	type_t*				fromtype,
	type_t*				totype )
{	
	return type_distance( fromtype, totype ) < DISTANCE_MAX;
}

static bool
can_cast_ptr_type(
	ptr_type_t*			fromtype,
	ptr_type_t*			totype )
{
	if( fromtype->is_array || totype->is_array )
		return false;
	
	type_t*				downfromtype = fromtype->down;
	type_t*				downtotype = totype->down;

	downfromtype = pure_type( downfromtype );
	downtotype = pure_type( downtotype );

	const e_type_code	downfromcode = downfromtype->code;
	const e_type_code	downtocode = downtotype->code;
		
	if( downtocode == TYPE_VOID )
		return true;
		
#if ARIADNE_ALLOW_VOID_CAST
	if( downfromcode == TYPE_VOID )
		return true;
#endif

	if( downfromcode == TYPE_CLASS &&
		downtocode == TYPE_CLASS )
	{
		class_t*		fromclss;
		class_t*		toclss;
		
		fromclss = ( (class_type_t*)downfromtype )->clss;
		toclss = ( (class_type_t*)downtotype )->clss;
	
		if( is_base_class( fromclss, toclss ) == FOUND )
			return true;
		else if( is_base_class( toclss, fromclss ) == FOUND )
			return true;
	
		return false;
	}
	
	return is_same_type( downfromtype, downtotype );
}

bool
can_cast_type(
	type_t*				fromtype,
	type_t*				totype )
{
	totype = reduce_type( totype );

	const e_type_code	fromcode = fromtype->code;
	const e_type_code	tocode = totype->code;

	switch( fromcode )
	{
		TYPE_CASE_ARITHMETIC
		case TYPE_BOOL:
			
			switch( tocode )
			{
				TYPE_CASE_ARITHMETIC
				case TYPE_BOOL:
					return true;		
			}
			
			return false;
			
		case TYPE_POINTER:
		
			switch( tocode )
			{
				case TYPE_BOOL:
					return true;
					
				case TYPE_POINTER:
					return can_cast_ptr_type(
						(ptr_type_t*)fromtype,
						(ptr_type_t*)totype );
			}
			
			return false;
			
		case TYPE_REFERENCE:
			return can_cast_type(
				( (ref_type_t*)fromtype )->down,
				totype );
	
		case TYPE_CLASS:
		
			if( tocode == TYPE_CLASS )
			{
				if( ( (class_type_t*)fromtype )->clss ==
					( (class_type_t*)totype )->clss )
					return true;
			}
			
			return false;
			
		case TYPE_TYPEDEF:
			return can_assign_type(
				( (typedef_type_t*)fromtype )->type, totype );
	}
	
	return false;
}

bool
is_same_type(
	type_t*				typeone,
	type_t*				typetwo,
	bool				ignorequals )
{
	if( ignorequals == false && typeone->quals != typetwo->quals )
		return false;

	const e_type_code	code = typeone->code;

	if( code != typetwo->code )
		return false;
	
	switch( code )
	{
		TYPE_CASE_INTEGER
			return ( (int_type_t*)typeone )->is_signed ==
				( (int_type_t*)typetwo )->is_signed;
		
		case TYPE_VOID:
		case TYPE_BOOL:
		case TYPE_FLOAT:
		case TYPE_DOUBLE:
			return true;
		
		case TYPE_POINTER:
			return is_same_type(
				( (ptr_type_t*)typeone )->down,
				( (ptr_type_t*)typetwo )->down );

		case TYPE_REFERENCE:
			return is_same_type(
				( (ref_type_t*)typeone )->down,
				( (ref_type_t*)typetwo )->down );
		
		case TYPE_CLASS:
			return ( (class_type_t*)typeone )->clss ==
				( (class_type_t*)typetwo )->clss;
	
		case TYPE_ENUM:
			return typeone == typetwo;
	
		case TYPE_TYPEDEF:
			return ( (typedef_type_t*)typeone )->type ==
				( (typedef_type_t*)typetwo )->type;
	}

	compile_error( 0, ERR_INTERNAL );
	return false;
}

static void
make_type_string(
	type_t*				type,
	const char*			name,
	istring&			str )
{		
	int					quals = type->quals;

	if( quals && str.length() > 0 )
		str.append( " " );

	if( quals & TC_CONST )
		str.append( "const " );
	if( quals & TC_VOLATILE )
		str.append( "volatile " );
		
	str.append( name );
}

void
type_to_string(
	type_t*				type,
	istring&			str )
{
	if( !type )
	{
		str.clear();
		return;
	}
	
	switch( type->code )
	{
		case TYPE_BYTE:
			make_type_string( type, 
				( (int_type_t*)type )->is_signed ?
					"signed byte" : "byte", str );
			break;
	
		case TYPE_CHAR:
			make_type_string( type, 
				( (int_type_t*)type )->is_signed ?
				"char" : "unsigned char", str );
			break;
			
		case TYPE_SHORT:
			make_type_string( type, 
				( (int_type_t*)type )->is_signed ?
					"short" : "unsigned short", str );
			break;

		case TYPE_INT:
			make_type_string( type,
				( (int_type_t*)type )->is_signed ?
					"int" : "unsigned int", str );
			break;

		case TYPE_LONG:
			make_type_string( type,
				( (int_type_t*)type )->is_signed ?
					"long" : "unsigned long", str );
			break;

		case TYPE_WIDE:
			make_type_string( type,
				( (int_type_t*)type )->is_signed ?
					"wide" : "unsigned wide", str );
			break;
		
		case TYPE_VOID:
			make_type_string( type, "void", str );
			break;
			
		case TYPE_BOOL:
			make_type_string( type, "bool", str );
			break;
			
		case TYPE_FLOAT:
			make_type_string( type, "float", str );
			break;
			
		case TYPE_DOUBLE:
			make_type_string( type, "double", str );
			break;
	
		case TYPE_POINTER:
		{
			ptr_type_t* ptr_type = (ptr_type_t*)type;
			
			type_to_string( ptr_type->down, str );
			
			if( ptr_type->is_array == false )
			{
				make_type_string( type, "*", str );
			}
			else
			{
				char s[ 32 + 1 ];

				str.append( "[" );
				isnprintf( s, 32, "%ld",
					(long)get_ptr_type_array_size( ptr_type ) );
				str.append( s );
				str.append( "]" );
			}
			
			break;
		}
		
		case TYPE_REFERENCE:
		{
			ref_type_t* ref_type = (ref_type_t*)type;
			
			type_to_string( ref_type->down, str );
			
			make_type_string( type, "&", str );
			break;
		}
		
		case TYPE_CLASS:
			make_type_string( type,
				( (class_type_t*)type )->clss->name, str );
			break;
	
		case TYPE_UNRESOLVED:
		{
			qualname_t* qualname;
			istring		name;
			
			qualname = ( (unresolved_type_t*)type )->qualname;
			
			if( qualname->name[ 0 ] == ':' )
			{
				name.append( "::" );
				qualname = qualname->down;
			}
			
			while( qualname )
			{
				name.append( qualname->name );
				qualname = qualname->down;
				if( qualname )
					name.append( "::" );
			}
			
			make_type_string( type, name.c_str(), str );
			break;
		}
			
		case TYPE_TYPEDEF:
			make_type_string( type,
				( (typedef_type_t*)type )->name, str );
			break;

		case TYPE_TEMPLATE:
			str.append(
				( (template_type_t*)type )->name );
			break;
	}
}

bool
is_type_resolved(
	type_t*				type )
{
	switch( type->code )
	{
		case TYPE_TYPEDEF:
			return is_type_resolved(
				( (typedef_type_t*)type )->type );
	
		case TYPE_POINTER:
			return is_type_resolved(
				( (ptr_type_t*)type )->down );

		case TYPE_REFERENCE:
			return is_type_resolved(
				( (ref_type_t*)type )->down );
		
		case TYPE_UNRESOLVED:
			return false;
	}
	
	return true;
}

bool
is_type_abstract(
	type_t*				type )
{
	switch( type->code )
	{
		case TYPE_TYPEDEF:
			return is_type_abstract(
				( (typedef_type_t*)type )->type );
	
		case TYPE_CLASS:
			return ( ( class_type_t*)type )->clss->is_abstract;
		
		case TYPE_UNRESOLVED:
			return true;
	}
	
	return false;
}

void
error_undeclared_type(
	type_t*				type )
{
	type = reduce_type( type );
	
	while( type->code == TYPE_POINTER )
	{
		type = ( (ptr_type_t*)type )->down;
		type = reduce_type( type );
	}

	if( type->code != TYPE_UNRESOLVED )
		compile_error( 0, ERR_INTERNAL );
	else
	{
		istring			name;
		
		qualname_to_string(
			( (unresolved_type_t*)type )->qualname,
			name );
	
		compile_error(
			&( (unresolved_type_t*)type )->filepos,
			ERR_UNDECLARED_TYPE,
			name.c_str() );
	}
}

type_t*
deep_reduce_type(
	type_t*				type )
{
	if( type->code == TYPE_POINTER )
	{
		type_t*			s = type;
		
		while( ( (ptr_type_t*)s )->down )
		{
			s = ( (ptr_type_t*)s )->down;
			if( s->code != TYPE_POINTER )
				break;
		}
		
		if( s->code != TYPE_TYPEDEF )
			return type;
	
		ptr_type_t*		t;
	
		t = (ptr_type_t*)xalloc( sizeof( ptr_type_t ) );
		*t = *(ptr_type_t*)type;
		t->down = deep_reduce_type( ( (ptr_type_t*)type )->down );
		
		return t;
	}
	else if( type->code == TYPE_REFERENCE )
	{
		type_t*			s = type;
		
		while( ( (ref_type_t*)s )->down )
		{
			s = ( (ref_type_t*)s )->down;
			if( s->code != TYPE_REFERENCE )
				break;
		}
		
		if( s->code != TYPE_TYPEDEF )
			return type;
	
		ref_type_t*		t;
	
		t = (ref_type_t*)xalloc( sizeof( ref_type_t ) );
		*t = *(ref_type_t*)type;
		t->down = deep_reduce_type( ( (ref_type_t*)type )->down );
		
		return t;
	}
	else if( type->code == TYPE_TYPEDEF )
		type = ( (typedef_type_t*)type )->type;
		
	return type;
}

static void
output_compressed_class_id(
	class_t*			clss )
{
	while( clss )
	{
		const char*		name = clss->name;
		const int		len = strlen( name );
		
		for( int i = 0; i < len; i++ )
			out_byte( name[ i ] );
		out_byte( ':' );
		
		clss = clss->embedding;
	}
}

static void
output_type_quals(
	type_t*				type )
{
	const int			quals = type->quals;

	if( quals == 0 )
		return;

	if( quals & TC_CONST )
		out_byte( '.' );
	if( quals & TC_VOLATILE )
		out_byte( ':' );
}

void
output_type_id(
	type_t*				type,
	bool				reduce )
{
	switch( type->code )
	{
		case TYPE_BYTE:
			output_type_quals( type );
			out_byte( ( (int_type_t*)type )->is_signed ?
				'b' : 'B' );
			break;

		case TYPE_CHAR:
			output_type_quals( type );
			out_byte( ( (int_type_t*)type )->is_signed ?
				'c' : 'C' );
			break;
			
		case TYPE_SHORT:
			output_type_quals( type );
			out_byte( ( (int_type_t*)type )->is_signed ?
				's' : 'S' );
			break;

		case TYPE_INT:
			output_type_quals( type );
			out_byte( ( (int_type_t*)type )->is_signed ?
				'i' : 'I' );
			break;

		case TYPE_LONG:
			output_type_quals( type );
			out_byte( ( (int_type_t*)type )->is_signed ?
				'l' : 'L' );
			break;
		
		case TYPE_WIDE:
			output_type_quals( type );
			out_byte( ( (int_type_t*)type )->is_signed ?
				'w' : 'W' );
			break;
		
		case TYPE_VOID:
			output_type_quals( type );
			out_byte( 'v' );
			break;
			
		case TYPE_BOOL:
			output_type_quals( type );
			out_byte( 'a' );
			break;
			
		case TYPE_FLOAT:
			output_type_quals( type );
			out_byte( 'f' );
			break;
			
		case TYPE_DOUBLE:
			output_type_quals( type );
			out_byte( 'd' );
			break;
	
		case TYPE_POINTER:
		{
			ptr_type_t* ptr_type = (ptr_type_t*)type;
			
			output_type_id( ptr_type->down, reduce );
			
			if( ptr_type->is_array == false )
			{
				output_type_quals( type );
				out_byte( '*' );
			}
			else
			{
				out_byte( '[' );
				out_long( get_ptr_type_array_size( ptr_type ) );
			}
			
			break;
		}
		
		case TYPE_REFERENCE:
		{
			ref_type_t* ref_type = (ref_type_t*)type;
			
			output_type_id( ref_type->down, reduce );
			
			output_type_quals( type );
			out_byte( '&' );
			break;
		}
		
		case TYPE_CLASS:
		{
			output_type_quals( type );
			class_t* clss = ( (class_type_t*)type )->clss;
			output_compressed_class_id( clss );
			break;
		}
		
		case TYPE_ENUM:
		{
			output_type_quals( type );
			
			const char* name = ( (enum_type_t*)type )->name;
			const int len = strlen( name );
			for( int i = 0; i < len; i++ )
				out_byte( name[ i ] );
			out_byte( ':' );
				
			class_t* clss = ( (enum_type_t*)type )->clss;
			output_compressed_class_id( clss );
			
			break;
		}
				
		case TYPE_TYPEDEF:
		{
			if( reduce )
			{
				output_type_id(
					( (typedef_type_t*)type )->type, reduce );
				break;
			}
		
			output_type_quals( type );

			const char* name = ( (typedef_type_t*)type )->name;
			const int len = strlen( name );
			for( int i = 0; i < len; i++ )
				out_byte( name[ i ] );
			out_byte( ':' );
				
			class_t* clss = ( (typedef_type_t*)type )->clss;
			output_compressed_class_id( clss );
			
			break;
		}

		case TYPE_UNRESOLVED:
		default:
			compile_error( 0, ERR_INTERNAL );
			break;
	}
}


void
output_type_string(
	type_t*				type,
	bool				reduce )
{
	pos_t				oldpos;
	pos_t				newpos;

	out_tell_pos( &oldpos );
	out_word( 0 );

	const long			zero_offset = out_tell();

	output_type_id( type, reduce );

	const long			typelen = out_tell() - zero_offset;

	out_tell_pos( &newpos );
	out_seek_pos( &oldpos );
	out_word( typelen );
	out_seek_pos( &newpos );
}

type_t**
find_bottom_type(
	type_t**			link )
{
	type_t*				type = *link;

	switch( type->code )
	{
		TYPE_CASE_ARITHMETIC
		case TYPE_VOID:
		case TYPE_BOOL:
		case TYPE_CLASS:
		case TYPE_UNRESOLVED:
		case TYPE_TYPEDEF:
		case TYPE_NULL:
			break;
			
		case TYPE_POINTER:
			return find_bottom_type(
				&( (ptr_type_t*)type )->down );
			break;
		
		case TYPE_REFERENCE:
			return find_bottom_type(
				&( (ref_type_t*)type )->down );
			break;
		
		default:
			compile_error( 0, ERR_INTERNAL );
			break;
	}
	
	return link;
}

int
type_record_size(
	type_t*				type )
{
	switch( type->code )	
	{
		TYPE_CASE_INTEGER
			return sizeof( int_type_t );
			
		case TYPE_VOID:
		case TYPE_BOOL:
		case TYPE_FLOAT:
		case TYPE_DOUBLE:
		case TYPE_NULL:
			return sizeof( type_t );
			
		case TYPE_POINTER:
			return sizeof( ptr_type_t );

		case TYPE_REFERENCE:
			return sizeof( ref_type_t );

		case TYPE_CLASS:
			return sizeof( class_type_t );
		
		case TYPE_ENUM:
			return sizeof( enum_type_t );

		case TYPE_UNRESOLVED:
			return sizeof( unresolved_type_t );
		
		case TYPE_TYPEDEF:
			return sizeof( typedef_type_t );

		case TYPE_TEMPLATE:
			return sizeof( template_type_t );

		default:
			compile_error( 0, ERR_INTERNAL );
			break;
	}
	
	return 0;
}

type_t*
copy_type_shallow(
	type_t*				type )
{
	int					size;
	void*				ptr;
	
	size = type_record_size( type );
	ptr = xalloc( size );
	memcpy( ptr, type, size );
	
	return (type_t*)ptr;	
}

type_t*
stripped_type(
	type_t*				type )
{
	while( true )
	{
		switch( type->code )
		{
			case TYPE_POINTER:
				type = ( (ptr_type_t*)type )->down;
				break;

			case TYPE_REFERENCE:
				type = ( (ref_type_t*)type )->down;
				break;

			default:
				return type;
		}
	}

	return type;
}

template_type_t*
template_type(
	type_t*				type )
{
	while( true )
	{
		switch( type->code )
		{
			case TYPE_POINTER:
				type = ( (ptr_type_t*)type )->down;
				break;

			case TYPE_REFERENCE:
				type = ( (ref_type_t*)type )->down;
				break;

			case TYPE_TEMPLATE:
				return (template_type_t*)type;

			default:
				return 0;
		}
	}

	return 0;
}

END_COMPILER_NAMESPACE
