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
//	format_token.i			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

	// [Item][Space]

	case token_Unsigned:
	case token_Signed:
	case token_Bool:
	case token_Byte:
	case token_Char:
	case token_Double:
	case token_Float:
	case token_Int:
	case token_Long:
	case token_Wide:
	case token_Short:
	case token_Void:
		TYPE_SEEN
		// fall through
	
	case token_Case:
	case token_Class:
	case token_Struct:
	case token_New:
	case token_Delete:
	case token_NotOp:
	case token_Include:
	case token_Module:
	case token_Enum:
	case token_Throw:
	case token_Namespace:
	case token_Template:
	case token_Using:
	case token_Of:
	case token_ImportDll:
	case token_CompilerSetting:
		if( last == token_Identifier )
			text.append_space();

		text.append( s_token_text[ id ] );
		text.append_space();
		break;

	// [Space][Item]
	
	case token_Operator:
		text.append_space();
		text.append( s_token_text[ id ] );
		opcount = 4;
		break;
		
	// [Item]

	case token_Sizeof:
	case token_True:
	case token_False:
	case token_Public:
	case token_Private:
	case token_Protected:
	case token_Default:
	case token_Continue:
	case token_Break:
	case token_Do:
	case token_Null:
	case token_This:
	case token_IncOp:
	case token_DecOp:
	case token_PtrOp:
	case token_Dot:
	case token_ScopeOp:
	case token_Colon:
	case token_Not:
	case token_Try:
	case token_Catch:
	case token_StrawQuotation:
	case token_Backslash:
		text.append( s_token_text[ id ] );
		break;

	// [IdentItem]

	case token_Identifier:
	{
		int flag = lexer->flag();
		const char* s = lexer->string();
		
		if( last == token_ParenOpen &&
			compiler::is_type_name( s + 1 ) )
		{
			TYPE_SEEN;
		}
		
		if( flag == 2 )
			text.append_declaration( s + 1 );
		else if( flag == 1 )
		{
			text.append_space();
			text.append( s + 1 );
		}
		else
		{
			if( last == token_Identifier )
				text.append_space();
				
			text.append( s + 1 );
		}
		break;
	}
	
	case token_HexConstant:
	case token_BinConstant:
	case token_OctConstant:
	case token_IntConstant:
	case token_ChrConstant:
	case token_FltConstant:
		text.append( lexer->string() );
		break;

	case token_StringLiteral:
		text.append( "\"" );
		text.append_literal( lexer->string() );
		text.append( "\"" );
		break;

	case token_CommentCpp:
		if( last != 0 )
		{
			text.trim_tail();
			text.append( "\t//" );
		}
		else
			text.append( "//" );
			
		text.append( lexer->string() );
		break;

	// [Space][Item][Space]
		
	case token_Typedef:
	case token_Static:
	case token_Const:
	case token_Native:
	case token_Abstract:
	case token_Friend:
	case token_Else:
	case token_Goto:
	case token_Extends:
		text.append_space();
		text.append( s_token_text[ id ] );
		text.append_space();
		break;

	case token_Cast:
		text.append_space();
		text.append( s_token_text[ id ] );
		break;

	case token_ClampClose:
		if( ( flag = lexer->flag() ) != 0 )
		{
			if( options.space_clamps )
				text.append_space();
			else
				text.trim_tail();
			text.append( s_token_text[ id ] );
			if( flag == 2 )
				text.append_space();
		}
		else
		{
			flag = options.space_operators;
			if( flag )
				text.append_space();
			text.append( s_token_text[ id ] );
			if( flag )
				text.append_space();
		}
		break;

	case token_ClampOpen:
		if( lexer->flag() )
		{	
			text.append( s_token_text[ id ] );
			flag = options.space_clamps;
			if( flag )
				text.append_space();
			break;
		}
		// fall through

	// {Space}[Operator]{Space}

	case token_AndOp:
	case token_OrOp:
	case token_ShrOp:
	case token_ShlOp:
	case token_LeOp:
	case token_GeOp:
	case token_EqOp:
	case token_NeOp:
	case token_Slash:
	case token_Percent:
	case token_Xor:
	case token_Or:
	case token_QuestionMark:
	case token_Assign:

	case token_MulAssign:
	case token_DivAssign:
	case token_ModAssign:
	case token_AddAssign:
	case token_SubAssign:
	case token_ShlAssign:
	case token_ShrAssign:
	case token_AndAssign:
	case token_XorAssign:
	case token_OrAssign:
		flag = options.space_operators;
		if( last == token_Operator )
			flag = false;
		if( flag )
			text.append_space();
		text.append( s_token_text[ id ] );
		if( flag )
			text.append_space();
		break;

	// [Item]{Space}
	case token_For:	
	case token_If:
	case token_Switch:
	case token_While:
		text.append( s_token_text[ id ] );
		flag = options.space_statements;
		if( flag )
			text.append_space();
		break;

	case token_BrackOpen:
		// handle special case
		// for arrays like int[ n ]
		if( lexer->flag() == false )
			text.trim_tail();
			
		// common case
		flag = options.space_brackets;
		text.append( s_token_text[ id ] );
		if( flag )
			text.append_space();
		break;

	case token_BrackClose:
		if( last == token_BrackOpen )
			text.trim_tail();
		else
		{
			flag = options.space_brackets;
			if( flag )
				text.append_space();
			else
				text.trim_tail();
		}
		text.append( s_token_text[ id ] );
		break;

	case token_ParenOpen:
		flag = options.space_parentheses;
		text.append( s_token_text[ id ] );
		if( flag )
			text.append_space();
		if( last != token_Identifier && opcount <= 0 &&
			last != token_For )
			state |= nofunc_state;
		else
			state &= ~nofunc_state;
		break;
				
	case token_ParenClose:
		flag = options.space_parentheses;
		if( flag &&
			last != token_ParenOpen &&
			( state & typecast_state ) == 0 )
		{
			text.append_space();
		}
		else
			text.trim_tail();
		text.append( s_token_text[ id ] );
		state &= ~typecast_state;
		break;

	case token_DelimOpen:
	case token_DelimClose:
		flag = options.space_delimiters;
		if( flag )
			text.append_space();
		text.append( s_token_text[ id ] );
		if( flag )
			text.append_space();
		break;

	case token_Star:
	{
		int flag = lexer->flag();
		
		if( state & typecast_state )
		{
			text.trim_tail();
			text.append( s_token_text[ id ] );
		}
		else if( flag )
		{
			// pointer operator as in
			// int *a
			// a = *b
	
			if( flag == 2 )
				// star preceded by identifier
				text.append_declaration( "" );			
			else if( flag == 1 )
			{
				// star preceded by identifier
				if( options.space_pointer )
					text.append_space();			
			}
			else if( flag == 4 )
				// operator*
				text.trim_tail();
			else
				// star preceded by another star
				text.trim_tail();

			text.append( s_token_text[ id ] );
			break;
		}
		else
		{
			// binary multiply operator as in
			// a = b * c
		
			flag = options.space_operators;
			if( flag )
				text.append_space();
			text.append( s_token_text[ id ] );
			if( flag )
				text.append_space();
		}
		break;
	}
	
	case token_And:
	{
		int flag = lexer->flag();
		
		if( flag )
		{
			// pointer operator as in
			// int &a
			// a = &b
	
			if( flag == 2 )
				// & preceded by identifier
				text.append_declaration( "" );			
			else if( flag == 1 )
			{
				// & preceded by identifier
				if( options.space_pointer )
					text.append_space();			
			}
			else
				// & preceded by another &
				text.append_space();

			text.append( s_token_text[ id ] );
			break;
		}
		else
		{
			// binary multiply operator as in
			// a = b & c
		
			flag = options.space_operators;
			if( flag )
				text.append_space();
			text.append( s_token_text[ id ] );
			if( flag )
				text.append_space();
		}
		break;
	}

	case token_Plus:
	case token_Minus:
		if( lexer->flag() )
		{
			// binary operator
			
			flag = options.space_operators;
			if( flag )
				text.append_space();
			text.append( s_token_text[ id ] );
			if( flag )
				text.append_space();
		}
		else
		{
			// unary operator
		
			text.append( s_token_text[ id ] );
		}
		break;

	case token_Return:
		text.append( s_token_text[ id ] );
		text.append_space();
		break;

	case token_Comma:
		text.trim_tail();
		text.append( options.comma );
		break;
		
	case token_Semicolon:
		if( last != 0 )
			text.trim_tail();
		text.append( options.semicolon );
		break;

	case token_CommentBegin:
		if( last != 0 )
		{
			text.trim_tail();
			text.append( " " );
		}
		text.append( "/* " );
		break;
	
	case token_CommentEnd:
		if( last != 0 )
		{
			text.trim_tail();
			text.append( " " );
		}
		text.append( "*/" );
		break;
