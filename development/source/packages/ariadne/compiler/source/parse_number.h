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
//	parse_number.h			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"

BEGIN_COMPILER_NAMESPACE

struct expr_t;

expr_t*
parse_hex_const(
	const char*				s );
	
expr_t*
parse_oct_const(
	const char*				s );
	
expr_t*
parse_bin_const(
	const char*				s );
	
expr_t*
parse_int_const(
	const char*				s );

expr_t*
parse_chr_const(
	const char*				s );

expr_t*
parse_flt_const(
	const char*				s );
	
END_COMPILER_NAMESPACE
