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

typedef union {
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
} YYSTYPE;
#define	UNSIGNED	258
#define	SIGNED	259
#define	BOOL	260
#define	CHAR	261
#define	DOUBLE	262
#define	FLOAT	263
#define	INT	264
#define	LONG	265
#define	BYTE	266
#define	WIDE	267
#define	SHORT	268
#define	VOID	269
#define	TYPENAME	270
#define	CAST	271
#define	CONST	272
#define	IDENTIFIER	273
#define	MODULE	274
#define	NAMESPACE	275
#define	TEMPLATE	276
#define	USING	277
#define	HEX_CONSTANT	278
#define	BIN_CONSTANT	279
#define	OCT_CONSTANT	280
#define	INT_CONSTANT	281
#define	CHR_CONSTANT	282
#define	FLT_CONSTANT	283
#define	STRING_LITERAL	284
#define	IMPORT_DLL	285
#define	COMPILER_SETTING	286
#define	INC_OP	287
#define	DEC_OP	288
#define	PTR_OP	289
#define	SIZEOF	290
#define	TRY	291
#define	CATCH	292
#define	THROW	293
#define	SHL_OP	294
#define	SHR_OP	295
#define	LE_OP	296
#define	GE_OP	297
#define	EQ_OP	298
#define	NE_OP	299
#define	AND_OP	300
#define	OR_OP	301
#define	MUL_ASSIGN	302
#define	DIV_ASSIGN	303
#define	MOD_ASSIGN	304
#define	ADD_ASSIGN	305
#define	SUB_ASSIGN	306
#define	SHL_ASSIGN	307
#define	SHR_ASSIGN	308
#define	AND_ASSIGN	309
#define	XOR_ASSIGN	310
#define	OR_ASSIGN	311
#define	TYPEDEF	312
#define	STATIC	313
#define	NATIVE	314
#define	ABSTRACT	315
#define	OPERATOR	316
#define	PUBLIC	317
#define	PRIVATE	318
#define	PROTECTED	319
#define	FRIEND	320
#define	ENUM	321
#define	OF	322
#define	CASE	323
#define	DEFAULT	324
#define	IF	325
#define	ELSE	326
#define	SWITCH	327
#define	WHILE	328
#define	DO	329
#define	FOR	330
#define	GOTO	331
#define	CONTINUE	332
#define	BREAK	333
#define	RETURN	334
#define	CLASS	335
#define	STRUCT	336
#define	EXTENDS	337
#define	NEW	338
#define	DELETE	339
#define	SCOPE_OP	340
#define	NULLTOKEN	341
#define	THISTOKEN	342
#define	TRUETOKEN	343
#define	FALSETOKEN	344
#define	NOT	345


extern YYSTYPE yylval;
