%{

/* ------------------------------------------------------------------
   Initial code (copied verbatim to the output file)
   ------------------------------------------------------------------*/

#if __VISC__
#include <malloc.h>
#define alloca _alloca
#endif

//#define YYDEBUG 1

#include "expr_t.h"
#include "type_t.h"
#include "stmt_t.h"
#include "scope_t.h"
#include "class_t.h"

#include "parser.h"
#include "parse_number.h"
#include "xalloc.h"

#include "icarus.h"
#include "stdio.h"

using namespace compiler;

int yylex();

void yyerror( char *msg );

#define YYSTACK_ALLOC imalloc
#define YYSTACK_FREE ifree

%}

/* ------------------------------------------------------------------
   Yacc declarations
   ------------------------------------------------------------------*/

%union {
	int							m_flags;
	const char*					m_text;

	expr_t*						m_expr;
	e_expr_code					m_expr_code;

	decl_t*						m_decl;
	decl_spec_t*				m_decl_spec;
	decl_init_t*				m_decl_init;
	array_decl_t*				m_array_decl;
	enum_list_t*				m_enum_list;
	typename_list_t*			m_typename_list;

	type_t*						m_type;
	type_seq_t*					m_type_seq;
	qualname_t*					m_qualname;
	qualhead_t*					m_qualhead;
	
	class_spec_t*				m_clss_spec;
	
	stmt_t*						m_stmt;
	int							m_none;
	int							m_qualifiers;
	
	class_t*					m_class;
}

%token UNSIGNED SIGNED BOOL CHAR DOUBLE FLOAT INT LONG
%token BYTE WIDE SHORT VOID TYPENAME CAST
%token CONST IDENTIFIER MODULE NAMESPACE TEMPLATE USING
%token HEX_CONSTANT BIN_CONSTANT OCT_CONSTANT INT_CONSTANT
%token CHR_CONSTANT FLT_CONSTANT
%token STRING_LITERAL IMPORT_DLL COMPILER_SETTING
%token INC_OP DEC_OP PTR_OP SIZEOF TRY CATCH THROW
%token SHL_OP SHR_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN
%token SHL_ASSIGN SHR_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPEDEF STATIC NATIVE ABSTRACT OPERATOR
%token PUBLIC PRIVATE PROTECTED FRIEND ENUM OF
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR
%token GOTO CONTINUE BREAK RETURN CLASS STRUCT EXTENDS NEW DELETE
%token SCOPE_OP NULLTOKEN THISTOKEN TRUETOKEN FALSETOKEN NOT

%type <m_decl>					compilation_unit

%type <m_expr>					primary_expression
%type <m_expr>					postfix_expression
%type <m_expr>					argument_expression_list
%type <m_expr>					unary_expression
%type <m_expr_code>				unary_operator
%type <m_expr>					cast_expression
%type <m_expr>					multiplicative_expression
%type <m_expr>					additive_expression
%type <m_expr>					shift_expression
%type <m_expr>					relational_expression
%type <m_expr>					equality_expression
%type <m_expr>					and_expression
%type <m_expr>					exclusive_or_expression
%type <m_expr>					inclusive_or_expression
%type <m_expr>					logical_and_expression
%type <m_expr>					logical_or_expression
%type <m_expr>					assignment_expression
%type <m_expr>					stmt_assignment_expression
%type <m_expr_code>				assignment_operator
%type <m_expr>					expression
%type <m_expr>					stmt_expression
%type <m_expr>					constant_expression

%type <m_type>					type_name
%type <m_type_seq>				type_name_seq
%type <m_decl>					declaration
%type <m_flags>					modifiers
%type <m_flags>					modifier
%type <m_decl_spec>				declaration_specifiers
%type <m_decl>					init_declarator_list
%type <m_decl>					init_declarator
%type <m_type>					primitive_type
%type <m_qualname>				simple_qualified_name
%type <m_qualname>				qualified_name
%type <m_qualname>				simple_qualified_type
%type <m_qualname>				qualified_type
%type <m_type>					type_specifier
%type <m_decl>					declarator
%type <m_array_decl>			array_declarator
%type <m_decl>					direct_declarator
%type <m_decl>					abstract_declarator
%type <m_decl>					direct_abstract_declarator
%type <m_qualifiers>			qualifiers
%type <m_type>					pointer
%type <m_decl>					parameter_type_list
%type <m_decl>					parameter_list
%type <m_decl>					parameter_declaration
%type <m_decl_init>				initializer
%type <m_decl_init>				initializer_list

%type <m_stmt>					statement
%type <m_stmt>					labeled_statement
%type <m_stmt>					compound_statement
%type <m_stmt>					statement_list
%type <m_stmt>					expression_statement
%type <m_stmt>					selection_statement
%type <m_stmt>					iteration_statement
%type <m_stmt>					jump_statement
%type <m_stmt>					declaration_statement
%type <m_stmt>					try_statement
%type <m_stmt>					catch_statement
%type <m_stmt>					catch_statement_list
%type <m_stmt>					throw_statement

%type <m_none>					member_declaration
%type <m_none>					member_declaration_list
%type <m_none>					function_definition
%type <m_clss_spec>				class_prefix
%type <m_clss_spec>				namespace_prefix
%type <m_qualhead>				qualifed_type_list
%type <m_none>					class_declaration
%type <m_none>					access_declaration
%type <m_enum_list>				enumerator_list
%type <m_none>					enum_declaration
%type <m_typename_list>				typename_list

%type <m_text>					IDENTIFIER
%type <m_text>					TYPENAME
%type <m_text>					HEX_CONSTANT
%type <m_text>					BIN_CONSTANT
%type <m_text>					OCT_CONSTANT
%type <m_text>					INT_CONSTANT
%type <m_text>					CHR_CONSTANT
%type <m_text>					FLT_CONSTANT
%type <m_text>					STRING_LITERAL
%type <m_text>					operator_identifier

%start compilation_unit

%expect 23

%%

/* ------------------------------------------------------------------
   Yacc grammar rules
   ------------------------------------------------------------------*/

compilation_unit
	: member_declaration_list { }
	;

primary_expression
	: qualified_name {
		$$ = make_id_expr( $1 ); }
	| HEX_CONSTANT {
		$$ = parse_hex_const( $1 ); }
	| BIN_CONSTANT {
		$$ = parse_bin_const( $1 ); }
	| OCT_CONSTANT {
		$$ = parse_oct_const( $1 ); }
	| INT_CONSTANT {
		$$ = parse_int_const( $1 ); }
	| CHR_CONSTANT {
		$$ = parse_chr_const( $1 ); }
	| FLT_CONSTANT {
		$$ = parse_flt_const( $1 ); }
	| STRING_LITERAL {
		$$ = make_str_expr( $1 ); }
	| NULLTOKEN {
		$$ = make_null_expr(); }
	| THISTOKEN {
		$$ = make_this_expr(); }
	| TRUETOKEN {
		$$ = make_iconst_expr( 1, make_type( TYPE_BOOL ) ); }
	| FALSETOKEN {
		$$ = make_iconst_expr( 0, make_type( TYPE_BOOL ) ); }
	| '(' expression ')' {
		$$ = $2; }
	| NEW type_name {
		$$ = make_new_expr( $2 ); }
	| NEW '<' type_name '>' '(' expression ')'  {
		$$ = make_dtnew_expr( $3, $6 ); }
	;

postfix_expression
	: primary_expression {
		$$ = $1; }
	| postfix_expression '[' expression ']' {
		$$ = make_binary_expr( EXPR_ACCESS_ARRAY, $1, $3 ); }
	| postfix_expression '(' ')' {
		$$ = make_func_expr( 0, $1, 0 ); }
	| postfix_expression '(' argument_expression_list ')'{
		$$ = make_func_expr( 0, $1, $3 ); }
	| postfix_expression OF '<' type_name_seq '>' '(' ')' {
		$$ = make_tmpl_func_expr( 0, $4, $1, 0 ); }
	| postfix_expression OF '<' type_name_seq '>' '(' argument_expression_list ')'{
		$$ = make_tmpl_func_expr( 0, $4, $1, $7 ); }
	| postfix_expression '.' qualified_name {
		$$ = make_binary_expr( EXPR_ACCESS_DOT, $1, make_id_expr( $3 ) ); }
	| postfix_expression PTR_OP qualified_name {
		$$ = make_binary_expr( EXPR_ACCESS_PTR, $1, make_id_expr( $3 ) ); }
	| postfix_expression INC_OP {
		$$ = make_unary_expr( EXPR_POSTINC, $1 ); }
	| postfix_expression DEC_OP {
		$$ = make_unary_expr( EXPR_POSTDEC, $1 ); }
	;

argument_expression_list
	: assignment_expression {
		$$ = $1; }
	| assignment_expression ',' argument_expression_list  {
		$$ = make_binary_expr( EXPR_COMMA, $1, $3 ); }
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression {
		$$ = make_unary_expr( EXPR_PREINC, $2 ); }
	| DEC_OP unary_expression {
		$$ = make_unary_expr( EXPR_PREDEC, $2 ); }
	| unary_operator cast_expression {
		if( $1 != EXPR_NOP ) { $$ = make_unary_expr( $1, $2 ); }
		else { $$ = $2; } }
	| SIZEOF unary_expression {
		type_t* sizeoftype = make_int_type( TYPE_LONG, false );
		expr_t* expr = make_unary_expr( EXPR_SIZEOF, $2 );
		expr->type = sizeoftype; $$ = expr; }
	| SIZEOF '<' type_name '>' {
		expr_t* childexpr = make_iconst_expr( 0, $3 );
		type_t* sizeoftype = make_int_type( TYPE_LONG, false );
		expr_t* expr = make_unary_expr( EXPR_SIZEOF, childexpr );
		expr->type = sizeoftype; $$ = expr; }
	;

unary_operator
	: '&' {
		$$ = EXPR_ADDRESS_OF; }
	| '*' {
		$$ = EXPR_DEREFERENCE; }
	| '+' {
		$$ = EXPR_NOP; }
	| '-' {
		$$ = EXPR_NEGATE; }
	| '~' {
		$$ = EXPR_BITWISE_NOT; }
	| NOT {
		$$ = EXPR_LOGICAL_NOT; }
	;

cast_expression
	: unary_expression {
		$$ = $1; }
	| CAST '<' type_name '>' cast_expression {
		type_t* type = $3;
		expr_t* expr = make_unary_expr( EXPR_TYPECAST, $5 );
		expr->type = type; $$ = expr; }
	| '(' type_name ')' cast_expression {
		type_t* type = $2;
		expr_t* expr = make_unary_expr( EXPR_TYPECAST, $4 );
		expr->type = type; $$ = expr; }
	;

multiplicative_expression
	: cast_expression {
		$$ = $1; }
	| multiplicative_expression '*' cast_expression {
		$$ = make_binary_expr( EXPR_MUL, $1, $3 ); }
	| multiplicative_expression '/' cast_expression {
		$$ = make_binary_expr( EXPR_DIV, $1, $3 ); }
	| multiplicative_expression '%' cast_expression {
		$$ = make_binary_expr( EXPR_MOD, $1, $3 ); }
	;

additive_expression
	: multiplicative_expression {
		$$ = $1; }
	| additive_expression '+' multiplicative_expression {
		$$ = make_binary_expr( EXPR_ADD, $1, $3 ); }
	| additive_expression '-' multiplicative_expression {
		$$ = make_binary_expr( EXPR_SUB, $1, $3 ); }
	;

shift_expression
	: additive_expression {
		$$ = $1; }
	| shift_expression SHL_OP additive_expression {
		$$ = make_binary_expr( EXPR_SHL, $1, $3 ); }
	| shift_expression SHR_OP additive_expression {
		$$ = make_binary_expr( EXPR_SHR, $1, $3 ); }
	;

relational_expression
	: shift_expression {
		$$ = $1; }
	| relational_expression '<' shift_expression {
		$$ = make_binary_expr( EXPR_LT, $1, $3 ); }
	| relational_expression '>' shift_expression {
		$$ = make_binary_expr( EXPR_GT, $1, $3 ); }
	| relational_expression LE_OP shift_expression {
		$$ = make_binary_expr( EXPR_LE, $1, $3 ); }
	| relational_expression GE_OP shift_expression {
		$$ = make_binary_expr( EXPR_GE, $1, $3 ); }
	;

equality_expression
	: relational_expression {
		$$ = $1; }
	| equality_expression EQ_OP relational_expression {
		$$ = make_binary_expr( EXPR_EQ, $1, $3 ); }
	| equality_expression NE_OP relational_expression {
		$$ = make_binary_expr( EXPR_NE, $1, $3 ); }
	;

and_expression
	: equality_expression {
		$$ = $1; }
	| and_expression '&' equality_expression {
		$$ = make_binary_expr( EXPR_BITWISE_AND, $1, $3 ); }
	;

exclusive_or_expression
	: and_expression {
		$$ = $1; }
	| exclusive_or_expression '^' and_expression {
		$$ = make_binary_expr( EXPR_BITWISE_XOR, $1, $3 ); }
	;

inclusive_or_expression
	: exclusive_or_expression {
		$$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression {
		$$ = make_binary_expr( EXPR_BITWISE_OR, $1, $3 ); }
	;

logical_and_expression
	: inclusive_or_expression {
		$$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression {
		$$ = make_binary_expr( EXPR_LOGICAL_AND, $1, $3 ); }
	;

logical_or_expression
	: logical_and_expression {
		$$ = $1; }
	| logical_or_expression OR_OP logical_and_expression {
		$$ = make_binary_expr( EXPR_LOGICAL_OR, $1, $3 ); }
	;

/*	the following splitting of assignment_expression is a work
	around for a serious reduce/reduce conflict that occurs if
	something like "a * b" occurs inside a function. it could be a
	multiplication of variables a and b, or it could be a declaration
	of a variable b with type a*. we prefer the latter here. */

assignment_expression
	: logical_or_expression {
		$$ = $1; }
	| assignment_expression assignment_operator logical_or_expression {
		$$ = make_binary_expr( $2, $1, $3 ); }
	;
	
stmt_assignment_expression
	: assignment_expression {
		$$ = $1; }
	;
	
	/*unary_expression {
		$$ = $1; }
	| unary_expression assignment_operator assignment_expression {
		$$ = make_binary_expr( $2, $1, $3 ); }
	| shift_expression SHL_OP additive_expression {
		$$ = make_binary_expr( EXPR_SHL, $1, $3 ); }
	;*/
	
assignment_operator
	: '=' {
		$$ = EXPR_ASSIGN; }
	| MUL_ASSIGN {
		$$ = EXPR_MUL_ASSIGN; }
	| DIV_ASSIGN {
		$$ = EXPR_DIV_ASSIGN; }
	| MOD_ASSIGN {
		$$ = EXPR_MOD_ASSIGN; }
	| ADD_ASSIGN {
		$$ = EXPR_ADD_ASSIGN; }
	| SUB_ASSIGN {
		$$ = EXPR_SUB_ASSIGN; }
	| SHL_ASSIGN {
		$$ = EXPR_SHL_ASSIGN; }
	| SHR_ASSIGN {
		$$ = EXPR_SHR_ASSIGN; }
	| AND_ASSIGN {
		$$ = EXPR_AND_ASSIGN; }
	| XOR_ASSIGN {
		$$ = EXPR_XOR_ASSIGN; }
	| OR_ASSIGN {
		$$ = EXPR_OR_ASSIGN; }
	;

expression
	: assignment_expression {
		$$ = $1; }
	| expression ',' assignment_expression {
		$$ = make_binary_expr( EXPR_COMMA, $1, $3 ); }
	;

stmt_expression
	: stmt_assignment_expression {
		$$ = $1; }
	| stmt_expression ',' assignment_expression {
		$$ = make_binary_expr( EXPR_COMMA, $1, $3 ); }
	| DELETE expression {
		$$ = make_delete_expr( $2 ); }
	;
	
constant_expression
	: logical_or_expression {
		$$ = $1; }
	;		

type_name
	: type_specifier {
		$$ = $1; }
	| type_specifier abstract_declarator {
		decl_spec_t* spec = make_decl_spec( $1, 0 );
		decl_t* decl = $2;
		decl->spec = spec;
		if( finish_decl( decl, false ) )
			$$ = decl->type;
		else
			$$ = make_type( TYPE_NONE ); }
	;

type_name_seq
	: type_name {
		$$ = make_type_seq( 0, $1 ); }
	| type_name_seq ',' type_name {
		$1->next = make_type_seq( 0, $3 );
		$$ = $1; }
	;

declaration
	: declaration_specifiers init_declarator_list ';' {
		decl_t* decl = $2;
		$$ = decl;
		decl_spec_t* spec = $1;
		while( decl ) {
			decl->spec = spec;
			decl = decl->next;
		} }
	| declaration_specifiers '<' type_name_seq '>' init_declarator_list ';' {
		decl_t* decl = $5;
		decl->tmpltype = $3;
		$$ = decl;
		decl_spec_t* spec = $1;
		while( decl ) {
			decl->spec = spec;
			decl = decl->next;
		} }
	;

modifiers
	: modifier {
		$$ = $1; }
	| modifiers modifier {
		$$ = union_flags( $1, $2 ); }	
	;
	
modifier
	: TYPEDEF {
		$$ = MOD_TYPEDEF; }
	| STATIC {
		$$ = MOD_STATIC; }
	| CONST {
		$$ = MOD_CONST; }
	| NATIVE {
		$$ = MOD_NATIVE; }
	| ABSTRACT {
		$$ = MOD_ABSTRACT; }
	;

declaration_specifiers
	: modifiers type_specifier {
		$$ = make_decl_spec( $2, $1 ); }
	| type_specifier {
		$$ = make_decl_spec( $1, 0 ); }
	;

init_declarator_list
	: init_declarator {
		$$ = $1; }
	| init_declarator ',' init_declarator_list  {
		decl_t* decl = $1;
		decl->next = $3;
		$$ = decl; }
	;

init_declarator
	: declarator {
		$$ = $1; }
	| declarator '=' initializer {
		decl_t* decl = $1;
		decl->init = $3;
		$$ = decl; }
	;


primitive_type
	: VOID {
		$$ = make_type( TYPE_VOID ); }
	| BOOL {
		$$ = make_type( TYPE_BOOL ); }
	| BYTE {
		$$ = make_int_type( TYPE_BYTE, false ); }
	| CHAR {
		$$ = make_int_type( TYPE_CHAR, true ); }
	| SHORT {
		$$ = make_int_type( TYPE_SHORT, true ); }
	| INT {
		$$ = make_int_type( TYPE_INT, true ); }
	| LONG {
		$$ = make_int_type( TYPE_LONG, true ); }
	| FLOAT {
		$$ = make_type( TYPE_FLOAT ); }
	| DOUBLE {
		$$ = make_type( TYPE_DOUBLE ); }
	;
	
simple_qualified_name
	: IDENTIFIER {
		$$ = make_qualname( $1 ); }
	| simple_qualified_name SCOPE_OP IDENTIFIER {
		qualname_t* downname;
		downname = make_qualname( $3 );
		qualname_t* oldname = $1;
		oldname->down = downname;
		$$ = oldname; }
	| simple_qualified_type SCOPE_OP IDENTIFIER {
		 qualname_t* downname;
		downname = make_qualname( $3 );
		qualname_t* oldname = $1;
		oldname->down = downname;
		$$ = oldname; }
	;
	
qualified_name
	: simple_qualified_name {
		qualname_t* name = $1;
		name->scope = 0;
		$$ = name; }
	| SCOPE_OP simple_qualified_name {
		qualname_t* name = $2;
		name->scope = global_scope();
		$$ = name; }
	;

simple_qualified_type
	: TYPENAME {
		$$ = make_qualname( $1 ); }
	| simple_qualified_type SCOPE_OP TYPENAME {
		qualname_t* downname;
		downname = make_qualname( $3 );
		qualname_t* oldname = $1;
		oldname->down = downname;
		$$ = oldname; }
	;
	
qualified_type
	: simple_qualified_type {
		qualname_t* name = $1;
		name->scope = cur_scope();
		$$ = name; }
	| SCOPE_OP simple_qualified_type {
		qualname_t* name = $2;
		name->scope = global_scope();
		$$ = name; }
	;

type_specifier
	: primitive_type {
		type_t* type = $1;
		$$ = type; }
	| SIGNED primitive_type {
		type_t* type = $2;
		if( !is_type_integral( type ) )
			compile_error( 0, ERR_SYNTAX_ERROR );
		( (int_type_t*)type )->is_signed = true;
		$$ = type; }
	| UNSIGNED primitive_type {
		type_t* type = $2;
		if( !is_type_integral( type ) )
			compile_error( 0, ERR_SYNTAX_ERROR );
		( (int_type_t*)type )->is_signed = false;
		$$ = type; }
	| qualified_type {
		$$ = make_unresolved_type( $1 ); }
	;

declarator
	: pointer direct_declarator {
		decl_t* decl = $2;
		decl->prefix = $1;
		$$ = decl; }
	| pointer {
		decl_t* decl = make_decl( "" );
		decl->prefix = $1;
		$$ = decl; }
	| direct_declarator {
		$$ = $1; }
	;

operator_identifier
	: OPERATOR '[' ']' {
		$$ = "#[]"; }
	| OPERATOR '.' {
		$$ = "#."; }
	| OPERATOR '&' {
		$$ = "#&"; }
	| OPERATOR NOT {
		$$ = "#!"; }
	| OPERATOR AND_OP {
		$$ = "#&&"; }
	| OPERATOR OR_OP {
		$$ = "#||"; }
	| OPERATOR '~' {
		$$ = "#~"; }
	| OPERATOR '^' {
		$$ = "#^"; }
	| OPERATOR '|' {
		$$ = "#|"; }
	| OPERATOR '+' {
		$$ = "#+"; }
	| OPERATOR '-' {
		$$ = "#-"; }
	| OPERATOR '*' {
		$$ = "#*"; }
	| OPERATOR '/' {
		$$ = "#/"; }
	| OPERATOR '%' {
		$$ = "#%"; }
	| OPERATOR SHL_OP {
		$$ = "#<<"; }
	| OPERATOR SHR_OP {
		$$ = "#>>"; }
	| OPERATOR '<' {
		$$ = "#<"; }
	| OPERATOR '>' {
		$$ = "#>"; }
	| OPERATOR LE_OP {
		$$ = "#<="; }
	| OPERATOR GE_OP {
		$$ = "#>="; }
	| OPERATOR EQ_OP {
		$$ = "#=="; }
	| OPERATOR NE_OP {
		$$ = "#!="; }
	| OPERATOR '=' {
		$$ = "#="; }
	| OPERATOR MUL_ASSIGN {
		$$ = "#*="; }
	| OPERATOR DIV_ASSIGN {
		$$ = "#/="; }
	| OPERATOR MOD_ASSIGN {
		$$ = "#%="; }
	| OPERATOR ADD_ASSIGN {
		$$ = "#+="; }
	| OPERATOR SUB_ASSIGN {
		$$ = "#-="; }
	| OPERATOR SHL_ASSIGN {
		$$ = "#<<="; }
	| OPERATOR SHR_ASSIGN {
		$$ = "#>>="; }
	| OPERATOR AND_ASSIGN {
		$$ = "#&="; }
	| OPERATOR XOR_ASSIGN {
		$$ = "#^="; }
	| OPERATOR OR_ASSIGN {
		$$ = "#|="; }
	| OPERATOR INC_OP {
		$$ = "#++"; }
	| OPERATOR DEC_OP {
		$$ = "#--"; }
	| OPERATOR CAST {
		$$ = "#c"; }
	;

array_declarator
	: '[' expression ']' {
		$$ = make_array_decl( $2 ); }
	| array_declarator '[' expression ']' {
		array_decl_t* decl = make_array_decl( $3 );
		array_decl_t* node = $1; $$ = node;
		array_decl_t* tail = node->tail;
		decl->up = tail;
		tail->down = decl;
		node->tail = decl; }
	;

direct_declarator
	: IDENTIFIER {
		$$ = make_decl( $1 ); }
	| TYPENAME {
		$$ = make_decl( $1 ); }
	| operator_identifier {
		$$ = make_decl( $1 ); }
	| direct_declarator array_declarator {
		$$ = combine_array_decl( $1, $2 ); }
	| direct_declarator '(' parameter_type_list ')' {
		decl_t* decl = $1;
		decl->func = make_decl_func( $3 );
		$$ = decl; }
	| direct_declarator '(' ')' {
		decl_t* decl = $1;
		decl->func = make_decl_func( 0 );
		$$ = decl; }
	;

abstract_declarator
	: pointer {
		decl_t* decl = make_decl( "" );
		decl->prefix = $1;
		$$ = decl; }
	| direct_abstract_declarator {
		$$ = $1; }
	| pointer direct_abstract_declarator {
		decl_t* decl = $2;
		decl->prefix = $1;
		$$ = decl; }
	;

direct_abstract_declarator
	: array_declarator {
		$$ = combine_array_decl( make_decl( "" ), $1 ); }
	| direct_abstract_declarator array_declarator {
		$$ = combine_array_decl( $1, $2 ); }
	| '(' parameter_type_list ')' {
		decl_t* decl = make_decl( "" );
		decl->func = make_decl_func( $2 );
		$$ = decl; }
	| '(' ')' {
		decl_t* decl = make_decl( "" );
		decl->func = make_decl_func( 0 );
		$$ = decl; }
	| direct_abstract_declarator '(' parameter_type_list ')' {
		decl_t* decl = $1;
		decl->func = make_decl_func( $3 );
		$$ = decl; }
	| direct_abstract_declarator '(' ')' {
		decl_t* decl = $1;
		decl->func = make_decl_func( 0 );
		$$ = decl; }
	;

qualifiers
	: CONST {
		$$ = TC_CONST; }
	;
	
pointer
	: '*' {
		$$ = make_ptr_type( 0, 0 ); }
	| pointer '*' {
		$$ = make_ptr_type( $1, 0 ); }
	| '&' {
		$$ = make_ref_type( 0, 0 ); }
	| pointer '&' {
		$$ = make_ref_type( $1, 0 ); }
	| qualifiers '*'  {
		$$ = make_ptr_type( 0, $1 ); }
	| pointer qualifiers '*' {
		$$ = make_ptr_type( $1, $2 ); }
	| qualifiers '&' {
		$$ = make_ref_type( 0, $1 ); }
	| pointer qualifiers '&' {
		$$ = make_ref_type( $1, $2 ); }
	;

parameter_type_list
	: parameter_list {
		$$ = $1; }
	;

parameter_list
	: parameter_declaration {
		$$ = $1; }
	| parameter_declaration ',' parameter_list {
		decl_t* decl = $1;
		decl->next = $3;
		$$ = decl; }
	;

parameter_declaration
	: declaration_specifiers declarator '=' initializer {
		decl_t* decl = $2;
		decl->spec = $1;
		decl->init = $4;
		$$ = decl; }
	| declaration_specifiers declarator {
		decl_t* decl = $2;
		decl->spec = $1;
		$$ = decl; }
	| declaration_specifiers {
		decl_t* decl = make_decl( "" );
		decl->spec = $1;
		$$ = decl; }
	;

initializer
	: assignment_expression {
		expr_t* expr = $1;
		decl_init_t* init = make_decl_init();
		init->expr = expr;
		$$ = init; }
	| '{' initializer_list '}' {
		decl_init_t* init = make_decl_init();
		init->list = $2;
		$$ = init; }
	;

initializer_list
	: initializer {
		$$ = $1; }
	| initializer ',' initializer_list {
		decl_init_t* init = $1;
		init->next = $3;
		$$ = init;
		}
	;

statement
	: labeled_statement {
		$$ = $1; }
	| compound_statement {
		$$ = $1; }
	| expression_statement {
		$$ = $1; }
	| selection_statement {
		$$ = $1; }
	| iteration_statement {
		$$ = $1; }
	| jump_statement {
		$$ = $1; }
	| declaration_statement {
		$$ = $1; }
	| try_statement {
		$$ = $1; }
	| throw_statement {
		$$ = $1; }
	| COMPILER_SETTING STRING_LITERAL ';' {
		compile_setting( $2 );
		$$ = 0; }
	;

catch_statement
	: CATCH '(' declaration_specifiers declarator ')' compound_statement {
		decl_t* decl = $4; decl->spec = $3;
		$$ = make_catch_stmt( decl, $6 ); }
	;
	
catch_statement_list
	: catch_statement {
		$$ = $1; }
	| catch_statement_list catch_statement {
		stmt_t* stmt = $1; stmt->next = $2; $$ = stmt; }
	;

try_statement
	: TRY compound_statement catch_statement_list {
		$$ = make_try_stmt( $2, (catch_stmt_t*)$3 ); }
	;

throw_statement
	: THROW expression ';' {
		$$ = make_throw_stmt( $2 ); }
	;

labeled_statement
	: IDENTIFIER ':' {
		$$ = make_ident_stmt( STMT_LABEL, $1 ); }
	| CASE constant_expression ':' {
		$$ = make_case_stmt( $2 ); }
	| DEFAULT ':' {
		$$ = make_stmt( STMT_DEFAULT ); }
	;

compound_statement
	: '{' '}' {
		$$ = make_stmt( STMT_NOP ); }
	| '{' statement_list '}' {
		$$ = make_block_stmt( $2 ); }
	;

statement_list
	: statement {
		$$ = $1; }
	| statement statement_list {
		stmt_t* stmt = $1;
		stmt->next = $2;
		$$ = stmt; }
	;

expression_statement
	: ';' {
		$$ = make_stmt( STMT_NOP ); }
	| stmt_expression ';' {
		$$ = make_expr_stmt( STMT_EXPR, $1 ); }
	;

selection_statement
	: IF '(' expression ')' statement {
		$$ = parse_if_stmt( $3, $5, 0 ); }
	| IF '(' expression ')' statement ELSE statement {
		$$ = parse_if_stmt( $3, $5, $7 ); }
	| SWITCH '(' expression ')' statement {
		$$ = parse_cond_stmt( STMT_SWITCH, $3, $5 ); }
	;

iteration_statement
	: WHILE '(' expression ')' statement {
		$$ = parse_cond_stmt( STMT_WHILE, $3, $5 ); }
	| DO statement WHILE '(' expression ')' ';' {
		$$ = parse_cond_stmt( STMT_DO, $5, $2 ); }
	| FOR '(' ';' ';' ')' statement {
		$$ = parse_expr_for_stmt( 0, 0, 0, $6 ); }
	| FOR '(' expression ';' ';' ')' statement {
		$$ = parse_expr_for_stmt( $3, 0, 0, $7 ); }
	| FOR '(' expression ';' ';' expression ')' statement {
		$$ = parse_expr_for_stmt( $3, 0, $6, $8 ); }
	| FOR '(' expression ';' expression ';' ')' statement {
		$$ = parse_expr_for_stmt( $3, $5, 0, $8 ); }
	| FOR '(' expression ';' expression ';' expression ')' statement {
		$$ = parse_expr_for_stmt( $3, $5, $7, $9 ); }
	| FOR '(' declaration ';' ')' statement {
		$$ = parse_decl_for_stmt( $3, 0, 0, $6 ); }
	| FOR '(' declaration ';' expression ')' statement {
		$$ = parse_decl_for_stmt( $3, 0, $5, $7 ); }
	| FOR '(' declaration expression ';' ')' statement {
		$$ = parse_decl_for_stmt( $3, $4, 0, $7 ); }
	| FOR '(' declaration expression ';' expression ')' statement {
		$$ = parse_decl_for_stmt( $3, $4, $6, $8 ); }
	;

jump_statement
	: GOTO IDENTIFIER ';' {
		$$ = make_ident_stmt( STMT_GOTO, $2 ); }
	| CONTINUE ';' {
		$$ = make_stmt( STMT_CONTINUE ); }
	| BREAK ';' {
		$$ = make_stmt( STMT_BREAK ); }
	| RETURN ';' {
		$$ = make_expr_stmt( STMT_RETURN, 0 ); }
	| RETURN expression ';' {
		$$ = make_expr_stmt( STMT_RETURN, $2 ); }
	;
	
declaration_statement
	: declaration {
		$$ = make_decl_stmt( $1 ); }
	;

member_declaration
	: function_definition
	| declaration {
		parse_declaration( $1, 0, 0, false ); }
	| class_declaration
	| access_declaration
	| enum_declaration
	| MODULE TYPENAME '{' member_declaration_list '}' {
		$$ = 0; }
	| COMPILER_SETTING STRING_LITERAL ';' {
		compile_setting( $2 );
		$$ = 0;
	}
	;

member_declaration_list
	: member_declaration
	| member_declaration member_declaration_list
	;

function_definition
	: declaration_specifiers declarator compound_statement {
		decl_t* decl = $2;
		decl->spec = $1;
		parse_function( decl, $3 ); }
	;

class_prefix
	: CLASS TYPENAME {
		$$ = make_class_spec( $2, ACC_PRIVATE ); }
	| STRUCT TYPENAME {
		$$ = make_class_spec( $2, ACC_PUBLIC ); }
	| CLASS IDENTIFIER {
		$$ = make_class_spec( $2, ACC_PRIVATE );
		istring name( $2 ); make_type_name( name );
		compile_error( 0, ERR_ILLEGAL_TYPENAME, name.c_str(), $2 ); }
	| STRUCT IDENTIFIER {
		$$ = make_class_spec( $2, ACC_PUBLIC );
		istring name( $2 ); make_type_name( name );
		compile_error( 0, ERR_ILLEGAL_TYPENAME, name.c_str(), $2 ); }
	;

namespace_prefix
	: NAMESPACE TYPENAME {
		$$ = make_class_spec( $2, ACC_PUBLIC ); }
	| NAMESPACE IDENTIFIER {
		$$ = make_class_spec( $2, ACC_PUBLIC );
		istring name( $2 ); make_type_name( name );
		compile_error( 0, ERR_ILLEGAL_TYPENAME, name.c_str(), $2 ); }
	;
	
qualifed_type_list
	: qualified_type {
		$$ = make_qualhead( $1 ); }
	| qualified_type ',' qualifed_type_list {
		qualhead_t *qualhead = make_qualhead( $1 );
		qualhead->next = $3;
		$$ = qualhead; }
	;
	
class_declaration
	: class_prefix '{' {
		begin_class( $1, 0 ); }
	member_declaration_list '}' {
		end_class(); }
	| class_prefix EXTENDS qualifed_type_list '{' {
		begin_class( $1, $3 ); }
	member_declaration_list '}' {
		end_class(); }
	| class_prefix '{' '}' {
		begin_class( $1, 0 ); end_class(); }
	| class_prefix EXTENDS qualifed_type_list '{' '}' {
		begin_class( $1, $3 ); end_class(); }
	| namespace_prefix '{' {
		class_t* clss = begin_class( $1, 0 );
		scope_make_namespace( clss->scope ); }
	member_declaration_list '}' {
		end_class(); }
	| FRIEND CLASS qualified_type ';' {
		add_class_friend( $3 ); }
	| USING NAMESPACE qualified_type ';' {
		fileref_t filepos; get_filepos( &filepos );
		scope_add_unresolved_using( cur_scope(), $3, &filepos ); }
	| TEMPLATE typename_list {
		scope_t* frame = cur_scope();
		class_spec_t* spec;
		class_t* clss;
		spec = make_class_spec(
			template_class_name(), ACC_PUBLIC );
		clss = begin_class( spec, 0 );
		scope_add_using( frame, clss->scope );
		scope_make_template( clss->scope, $2 ); }
	member_declaration {
		end_class(); }
	| IMPORT_DLL STRING_LITERAL '{' {
		scope_t* frame = cur_scope();
		class_spec_t* spec = make_class_spec( $2, ACC_PUBLIC );	
		class_t* clss = begin_class( spec, 0 );
		scope_add_using( frame, clss->scope );
		scope_make_dll( clss->scope ); }
	member_declaration_list '}' {
		end_class(); }
	;

access_declaration
	: PUBLIC {
		declare_scope_access( ACC_PUBLIC ); }
	| PRIVATE {
		declare_scope_access( ACC_PRIVATE ); }
	| PROTECTED {
		declare_scope_access( ACC_PROTECTED ); }
	;

typename_list
	: TYPENAME {
		$$ = make_typename_list( 0, $1 ); }
	| typename_list ',' TYPENAME {
		$1->next = make_typename_list( 0, $3 ); $$ = $1; }
	;

enumerator_list
	: IDENTIFIER {
		enum_list_t* list = make_enum_list(); $$ = list;
		append_enumerator( list, $1, 0 ); }
	| IDENTIFIER '=' constant_expression {
		enum_list_t* list = make_enum_list(); $$ = list;
		append_enumerator( list, $1, $3 ); }
	| enumerator_list ',' IDENTIFIER {
		enum_list_t* list = $1; $$ = list;
		append_enumerator( list, $3, 0 ); }
	| enumerator_list ',' IDENTIFIER '=' constant_expression {
		enum_list_t* list = $1; $$ = list;
		append_enumerator( list, $3, $5 ); }
	;

enum_declaration
	: ENUM '{' enumerator_list '}' {
		fileref_t filepos; get_filepos( &filepos );
		declare_enum( 0, $3, &filepos ); }
	| ENUM TYPENAME '{' enumerator_list '}' {
		fileref_t filepos; get_filepos( &filepos );
		declare_enum( $2, $4, &filepos ); }
	;

%%
