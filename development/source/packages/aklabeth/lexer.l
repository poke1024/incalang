/* ------------------------------------------------------------------
   Initial code (copied verbatim to the output file)
   ------------------------------------------------------------------*/
%{

/* add to top:
#include "code.h"
BEGIN_CODE_NAMESPACE
#define YY_SKIP_YYWRAP
*/

END_CODE_NAMESPACE

#include "xalloc.h"
#include "aklabeth_lexer.h"
#include <cstring>

BEGIN_CODE_NAMESPACE

/* -----------------------------------------------------------------*/

void
c_comment();

void
cpp_comment();

void
extract_string_literal();

void
extract_tag();

/* -----------------------------------------------------------------*/

#define YY_INPUT(buf,result,max_size)\
	result = g_yystream->read_some_bytes( buf, max_size );\

#define ECHO

#define isatty(x) 0

/* -----------------------------------------------------------------*/

input_stream_t*			g_yystream = 0;

/* ------------------------------------------------------------------
   Some macros (standard regular expressions)
   ------------------------------------------------------------------*/
%}

D					[0-9]
L					[a-zA-Z_]
H					[a-fA-F0-9]
E					[Ee][+-]?{D}+
FS					(f|F|l|L)
IS					(u|U|l|L)*

CR					[\r]
LF					[\n]
FF					[\f]
TAB					[\t]
BLK					[\b]
BLANK				[ ]

TAG					[\001]

Identifier 			{L}({L}|{D})*

/* ------------------------------------------------------------------
   The lexer rules
   ------------------------------------------------------------------*/
%%

"/*"				{ return token_CommentBegin; }
"*/"				{ return token_CommentEnd; }
"//"				{ cpp_comment(); return token_CommentCpp; }

"unsigned"			{ return token_Unsigned; }
"signed"			{ return token_Signed; }
"bool"				{ return token_Bool; }
"byte"				{ return token_Byte; }
"char"				{ return token_Char; }
"double"			{ return token_Double; }
"float"				{ return token_Float; }
"int"				{ return token_Int; }
"long"				{ return token_Long; }
"wide"				{ return token_Wide; }
"short"				{ return token_Short; }
"sizeof"			{ return token_Sizeof; }
"void"				{ return token_Void; }
"true"				{ return token_True; }
"false"				{ return token_False; }

"typedef"			{ return token_Typedef; }
"static"			{ return token_Static; }
"const"				{ return token_Const; }
"native"			{ return token_Native; }
"abstract"			{ return token_Abstract; }

"public:"			{ return token_Public; }
"private:"			{ return token_Private; }
"protected:"		{ return token_Protected; }
"friend"			{ return token_Friend; }
"using"				{ return token_Using; }

"case"				{ return token_Case; }
"default"			{ return token_Default; }
"if"				{ return token_If; }
"else"				{ return token_Else; }
"switch"			{ return token_Switch; }
"while"				{ return token_While; }
"do"				{ return token_Do; }
"for"				{ return token_For; }
"goto"				{ return token_Goto; }
"continue"			{ return token_Continue; }
"break"				{ return token_Break; }
"return"			{ return token_Return; }
"struct"			{ return token_Struct; }
"class"				{ return token_Class; }
"extends"			{ return token_Extends; }
"new"				{ return token_New; }
"delete"			{ return token_Delete; }
"null"				{ return token_Null; }
"this"				{ return token_This; }
"not"				{ return token_NotOp; }
"cast"				{ return token_Cast; }
"include"			{ return token_Include; }
"module"			{ return token_Module; }
"operator"			{ return token_Operator; }
"enum"				{ return token_Enum; }
"try"				{ return token_Try; }
"catch"				{ return token_Catch; }
"throw"				{ return token_Throw; }
"namespace"			{ return token_Namespace; }
"template"			{ return token_Template; }
"of"				{ return token_Of; }
"import_dll"			{ return token_ImportDll; }
"compiler_setting"		{ return token_CompilerSetting; }

{L}({L}|{D})*		{ yylval.text = xstrdup( yytext ); return token_Identifier; }

0[xX]{H}+{IS}?		{ yylval.text = xstrdup( yytext ); return token_HexConstant; }
0[bB]{D}+{IS}?		{ yylval.text = xstrdup( yytext ); return token_BinConstant; }
0{D}+{IS}?			{ yylval.text = xstrdup( yytext ); return token_OctConstant; }
{D}+{IS}?			{ yylval.text = xstrdup( yytext ); return token_IntConstant; }
'(\\.|[^\\'])+'		{ yylval.text = xstrdup( yytext ); return token_ChrConstant; }

{D}+({E})?{FS}?			{ yylval.text = xstrdup( yytext ); return token_FltConstant; }
{D}*"."{D}+({E})?{FS}?	{ yylval.text = xstrdup( yytext ); return token_FltConstant; }
{D}+"."{D}*({E})?{FS}?	{ yylval.text = xstrdup( yytext ); return token_FltConstant; }


\"(\\.|[^\\"])*\"	{  extract_string_literal(); return token_StringLiteral; }
\"					{ return token_StrawQuotation; }


{TAG}				{ extract_tag(); return token_Tag; }

">>"				{ return token_ShrOp; }
"<<"				{ return token_ShlOp; }
"++"				{ return token_IncOp; }
"--"				{ return token_DecOp; }
"->"				{ return token_PtrOp; }
"."					{ return token_Dot; };
"<="				{ return token_LeOp; }
">="				{ return token_GeOp; }
"=="				{ return token_EqOp; }
"!="				{ return token_NeOp; }
"&&"				{ return token_AndOp; }
"||"				{ return token_OrOp; }
"::"				{ return token_ScopeOp; }

"*="				{ return token_MulAssign; }
"/="				{ return token_DivAssign; }
"%="				{ return token_ModAssign; }
"+="				{ return token_AddAssign; }
"-="				{ return token_SubAssign; }
"<<="				{ return token_ShlAssign; }
">>="				{ return token_ShrAssign; }
"&="				{ return token_AndAssign; }
"^="				{ return token_XorAssign; }
"|="				{ return token_OrAssign; }

"["					{ return token_BrackOpen; }
"]"					{ return token_BrackClose; }
"("					{ return token_ParenOpen; }
")"					{ return token_ParenClose; }
"{"					{ return token_DelimOpen; }
"}"					{ return token_DelimClose; }
"*"					{ return token_Star; }
"/"					{ return token_Slash; }
"\\"					{ return token_Backslash; }
"%"					{ return token_Percent; }
"+"					{ return token_Plus; }
"-"					{ return token_Minus; }
"<"					{ return token_ClampOpen; }
">"					{ return token_ClampClose; }
"&"					{ return token_And; }
"^"					{ return token_Xor; }
"|"					{ return token_Or; }
"?"					{ return token_QuestionMark; }
":"					{ return token_Colon; }
"="					{ return token_Assign; }
","					{ return token_Comma; }
";"					{ return token_Semicolon; }
"~"					{ return token_Not; }

{CR}   				{ }
{LF}				{ }
{FF}				{ }
{TAB}				{ }
{BLK}       	    { }
{BLANK}				{ }

%%

/* ------------------------------------------------------------------
   Additional code (again copied verbatim to the output file)
   ------------------------------------------------------------------*/

void
cpp_comment()
{
	char c;
	istring s;

	while( true )
	{
		c = yyinput();
		if( c == '\0' || c == '\n' )
			break;
		s.append( 1, c );
	}
	
	yylval.text = xstrdup( s.c_str() );
}

void
extract_string_literal()
{
	const char*			s = yytext + 1;
	const size_t		length = ( istrlen( s ) + 1 ) * sizeof( char );
	char*				t = (char*)xalloc( length );
	
	char*				p = t;
	char				c;
	bool				done = false;
	
	do {
		c = *s++;
		
		switch( c )
		{
			case '\\':
				switch( *s++ )
				{
					case 'n':
						c = '\n';
						break;

					case 't':
						c = '\t';
						break;

					case 'r':
						c = '\r';
						break;
						
					case '\"':
						c = '\"';
						break;

					case '\'':
						c = '\'';
						break;

					case '\\':
						c = '\\';
						break;
				
					default:
						s--;
						break;
				}
				break;
		
			case '\"':
				c = '\0';
				done = true;
				break;
		}

		*p++ = c;
	} while( done == false );

	yylval.text = t;
}

void
extract_tag()
{
	iptr_t				val = 0;
	
	for( int i = ICARUS_PTR_SIZE * 2 - 1; i >= 0; i-- )
	{
		char			c = yyinput();
		int				digit;
		
		if( c >= 'a' )
			digit = 10 + c - 'a';
		else
			digit = c - '0';
			
		val <<= 4;
		val |= digit;
	}

	yyinput();

	
	yylval.tag = (tag_t*)val;
}

void
init_lexer(
	input_stream_t*		yystream )
{
	g_yystream = yystream;
}

void
cleanup_lexer()
{
	g_yystream = 0;
}

END_CODE_NAMESPACE
