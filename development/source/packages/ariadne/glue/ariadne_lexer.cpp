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
//	ariadne_lexer.cp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_group_lexer_t.h"
#include "aklabeth_lexer.h"
#include "ariadne_lexer.h"

// ---------------------------------------------------------------------------

#include "xalloc.h"
#include "scope_t.h"
#include "type_t.h"
#include "expr_t.h"
#include "stmt_t.h"
#include "class_t.h"
#include "lexer.h"
#include "parser.h"
#include <cstring>

using namespace compiler;

#include "compiler_tab.h"

// ---------------------------------------------------------------------------

typedef code::group_t group_t;
typedef code::group_lexer_t group_lexer_t;

group_lexer_t*			s_lexer = 0;

void
init_lexer(
	group_t*			group )
{
	s_lexer = new group_lexer_t( group );
}

void
cleanup_lexer()
{
	if( s_lexer )
	{
		delete s_lexer;
		s_lexer = 0;
	}
}

int
yylex()
{
	group_lexer_t*		lexer = s_lexer;

retry:

	switch( static_cast<u08>( lexer->next() ) )
	{
		case code::token_Unsigned:
			return UNSIGNED;
			
		case code::token_Signed:
			return SIGNED;

		case code::token_Bool:
			return BOOL;

		case code::token_Byte:
			return BYTE;

		case code::token_Char:
			return CHAR;

		case code::token_Double:
			return DOUBLE;

		case code::token_Float:
			return FLOAT;

		case code::token_Int:
			return INT;

		case code::token_Long:
			return LONG;

		case code::token_Wide:
			return WIDE;

		case code::token_Short:
			return SHORT;

		case code::token_Sizeof:
			return SIZEOF;

		case code::token_Void:
			return VOID;

		case code::token_True:
			return TRUETOKEN;

		case code::token_False:
			return FALSETOKEN;

		case code::token_Typedef:
			return TYPEDEF;

		case code::token_Static:
			return STATIC;

		case code::token_Const:
			return CONST;

		case code::token_Native:
			return NATIVE;

		case code::token_Abstract:
			return ABSTRACT;

		case code::token_Public:
			return PUBLIC;

		case code::token_Private:
			return PRIVATE;

		case code::token_Protected:
			return PROTECTED;

		case code::token_Friend:
			return FRIEND;

		case code::token_Case:
			return CASE;

		case code::token_Default:
			return DEFAULT;

		case code::token_If:
			return IF;

		case code::token_Else:
			return ELSE;

		case code::token_Switch:
			return SWITCH;
			
		case code::token_While:
			return WHILE;

		case code::token_Do:
			return DO;

		case code::token_For:
			return FOR;

		case code::token_Goto:
			return GOTO;

		case code::token_Continue:
			return CONTINUE;

		case code::token_Break:
			return BREAK;

		case code::token_Return:
			return RETURN;
	
		case code::token_Class:
			return CLASS;
			
		case code::token_Struct:
			return STRUCT;

		case code::token_Extends:
			return EXTENDS;

		case code::token_New:
			return NEW;

		case code::token_Delete:
			return DELETE;

		case code::token_Null:
			return NULLTOKEN;

		case code::token_This:
			return THISTOKEN;

		case code::token_Not:
			return '~';
			
		case code::token_NotOp:
			return NOT;
			
		case code::token_Identifier:
		{
			const char* text = lexer->string() + 1;
			
			// ignore identifier flag byte
			::yylval.m_text = text;
			
			if( is_type_name( text ) )
				return TYPENAME;
			else
				return IDENTIFIER;
		}
			
		case code::token_HexConstant:
			::yylval.m_text = lexer->string();
			return HEX_CONSTANT;

		case code::token_BinConstant:
			::yylval.m_text = lexer->string();
			return BIN_CONSTANT;

		case code::token_OctConstant:
			::yylval.m_text = lexer->string();
			return OCT_CONSTANT;

		case code::token_IntConstant:
			::yylval.m_text = lexer->string();
			return INT_CONSTANT;

		case code::token_ChrConstant:
			::yylval.m_text = lexer->string();
			return CHR_CONSTANT;

		case code::token_FltConstant:
			::yylval.m_text = lexer->string();
			return FLT_CONSTANT;

		case code::token_StringLiteral:
			::yylval.m_text = lexer->string();
			return STRING_LITERAL;
	
		case code::token_ShrOp:
			return SHR_OP;

		case code::token_ShlOp:
			return SHL_OP;

		case code::token_IncOp:
			return INC_OP;

		case code::token_DecOp:
			return DEC_OP;

		case code::token_PtrOp:
			return PTR_OP;

		case code::token_Dot:
			return '.';

		case code::token_LeOp:
			return LE_OP;

		case code::token_GeOp:
			return GE_OP;

		case code::token_EqOp:
			return EQ_OP;

		case code::token_NeOp:
			return NE_OP;

		case code::token_AndOp:
			return AND_OP;

		case code::token_OrOp:
			return OR_OP;

		case code::token_ScopeOp:
			return SCOPE_OP;

		case code::token_MulAssign:
			return MUL_ASSIGN;

		case code::token_DivAssign:
			return DIV_ASSIGN;

		case code::token_ModAssign:
			return MOD_ASSIGN;

		case code::token_AddAssign:
			return ADD_ASSIGN;

		case code::token_SubAssign:
			return SUB_ASSIGN;

		case code::token_ShlAssign:
			return SHL_ASSIGN;

		case code::token_ShrAssign:
			return SHR_ASSIGN;

		case code::token_AndAssign:
			return AND_ASSIGN;

		case code::token_XorAssign:
			return XOR_ASSIGN;

		case code::token_OrAssign:
			return OR_ASSIGN;

		case code::token_BrackOpen:
			return '[';

		case code::token_BrackClose:
			return ']';

		case code::token_ParenOpen:
			return '(';

		case code::token_ParenClose:
			return ')';

		case code::token_DelimOpen:
			return '{';

		case code::token_DelimClose:
			return '}';

		case code::token_Star:
			return '*';

		case code::token_Slash:
			return '/';

		case code::token_Percent:
			return '%';

		case code::token_Plus:
			return '+';

		case code::token_Minus:
			return '-';

		case code::token_ClampOpen:
			return '<';

		case code::token_ClampClose:
			return '>';

		case code::token_And:
			return '&';

		case code::token_Xor:
			return '^';

		case code::token_Or:
			return '|';

		case code::token_QuestionMark:
			return '?';

		case code::token_Colon:
			return ':';

		case code::token_Assign:
			return '=';

		case code::token_Comma:
			return ',';

		case code::token_Semicolon:
			return ';';
			
		case code::token_Operator:
			return OPERATOR;
			
		case code::token_Enum:
			return ENUM;
			
		case code::token_Try:
			return TRY;

		case code::token_Catch:
			return CATCH;

		case code::token_Throw:
			return THROW;
					
		case code::token_EndOfLine:
			return 0;

		case code::token_StrawQuotation:
			compile_error( 0, ERR_SYNTAX_ERROR );
			return 0;
	
		case code::token_Cast:
			return CAST;
	
		case code::token_Module:
			return MODULE;
			
		case code::token_Namespace:
			return NAMESPACE;

		case code::token_Template:
			return TEMPLATE;
			
		case code::token_Using:
			return USING;

		case code::token_Of:
			return OF;

		case code::token_ImportDll:
			return IMPORT_DLL;

		case code::token_CompilerSetting:
			return COMPILER_SETTING;

		case code::token_CommentBegin:
		{
			int token;
			do {
				token = lexer->next();
				if( token == code::token_EndOfLine )
				{
					compile_error( 0, ERR_UNTERMINATED_COMMENT );
					return 0;
				}
			} while( token != code::token_CommentEnd );
			
			goto retry;
		}
		
		case code::token_CommentEnd:
			goto retry;
		
		case code::token_CommentCpp:
		case code::token_Include:
			goto retry;
						
		default:
			throw_icarus_error( "illegal token" );
	}

	return 0;
}

// ---------------------------------------------------------------------------

BEGIN_COMPILER_NAMESPACE

void
get_filepos(
	fileref_t*			filepos )
{
	filepos->id = 0;
	filepos->pos = s_lexer->location();
}

END_COMPILER_NAMESPACE
