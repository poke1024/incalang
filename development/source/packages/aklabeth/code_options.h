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
//	code_options.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef CODE_OPTIONS_H
#define CODE_OPTIONS_H

//#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct code_options_t {
	// a*b				<->			a * b
	// (false)						(true)
	bool				space_operators;
	
	// for (			<->			for(
	// (true)						(false)
	bool				space_statements;
	
	// [a]				<->			[ a ]
	// (false)						(true)
	bool				space_brackets;
	
	// (a)				<->			( a )
	// (false)						(true)
	bool				space_parentheses;
	
	// a { b			<->			a{b
	// (true)						(false)
	bool				space_delimiters;

	// <a>				<->			< a >
	// (false)						(true)
	bool				space_clamps;
	
	// <a*>b			<->			<a *>b
	// (false)						(true)
	bool				space_pointer;
	
	// int* a			<->			int *a
	// (false)						(true)
	bool				bind_pointer_left;
	
	// int a			<->			int {n tabs} a
	// (0)							(n)
	int					declaration_tabs;
	
	// class {			<->			class {
	// public:						    public:
	// (0)							(1)
	int					access_modifier_tabs;
	
	// switch( a ) {	<->			switch( a ) {
	// case b:						    case b:
	// (0)							(1)
	int					switch_tabs;
	
	// a,b				<->			a , b			<->			a, b
	// (,)							( , )						(, )
	char				comma[ 4 ];
	
	// a;				<->			a ;
	// (;)							( ;)
	char				semicolon[ 4 ];
} code_options_t;

extern code_options_t	s_code_options;

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include "code.h"

BEGIN_CODE_NAMESPACE

typedef code_options_t options_t;

void
read_options(
	const char*			path );

END_CODE_NAMESPACE

#endif

#endif

