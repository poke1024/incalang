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

#include "code.h"

#include "input_stream_t.h"

BEGIN_CODE_NAMESPACE

int yylex();

int yywrap();

void
init_lexer(
	input_stream_t*		yystream );

void
cleanup_lexer();

class tag_t;

typedef union {
	char*				text;
	tag_t*				tag;
} YYSTYPE;

extern YYSTYPE yylval;

enum {
	token_EndOfLine,
	
	token_Unsigned,
	token_Signed,
	token_Bool,
	token_Byte,
	token_Char,
	token_Double,
	token_Float,
	token_Int,
	token_Long,
	token_Wide,
	token_Short,
	token_Sizeof,
	token_Void,
	token_True,
	token_False,
	
	token_Identifier,
	token_HexConstant,
	token_BinConstant,
	token_OctConstant,
	token_IntConstant,
	token_ChrConstant,
	token_FltConstant,
	token_StringLiteral,
	
	token_Typedef,
	token_Static,
	token_Const,
	token_Native,
	token_Abstract,
	
	token_Public,
	token_Private,
	token_Protected,
	token_Friend,
	
	token_Case,
	token_Default,
	token_If,
	token_Else,
	token_Switch,
	token_While,
	token_Do,
	token_For,
	token_Goto,
	token_Continue,
	token_Break,
	token_Return,
	token_Class,
	token_Struct,
	token_Extends,
	token_New,
	token_Delete,
	token_Null,
	token_This,
	token_NotOp,
	
	token_ShrOp,
	token_ShlOp,
	token_IncOp,
	token_DecOp,
	token_PtrOp,
	token_Dot,
	token_LeOp,
	token_GeOp,
	token_EqOp,
	token_NeOp,
	token_AndOp,
	token_OrOp,
	token_ScopeOp,
	
	token_MulAssign,
	token_DivAssign,
	token_ModAssign,
	token_AddAssign,
	token_SubAssign,
	token_ShlAssign,
	token_ShrAssign,
	token_AndAssign,
	token_XorAssign,
	token_OrAssign,
	
	token_BrackOpen,
	token_BrackClose,
	token_ParenOpen,
	token_ParenClose,
	token_DelimOpen,
	token_DelimClose,
	token_Star,
	token_Slash,
	token_Backslash,
	token_Percent,
	token_Plus,
	token_Minus,
	token_ClampOpen,
	token_ClampClose,
	token_And,
	token_Xor,
	token_Or,
	token_QuestionMark,
	token_Colon,
	token_Assign,
	token_Comma,
	token_Semicolon,
	token_Not,
	
	token_CommentCpp,
	token_CommentBegin,
	token_CommentEnd,
	token_Cast,
	token_Tag,
	token_Include,
	token_Module,
	token_Operator,
	token_Enum,
	token_Try,
	token_Catch,
	token_Throw,
	token_Namespace,
	token_Template,
	token_Using,
	token_Of,
	token_ImportDll,
	token_CompilerSetting,
	
	token_StrawQuotation
};

END_CODE_NAMESPACE
