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
//	operators.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"

BEGIN_COMPILER_NAMESPACE

struct scope_t;
struct expr_t;

bool
unary_overload(
	expr_t*				expr,
	expr_t*				child );

bool
binary_overload(
	expr_t*				expr,
	expr_t*				left,
	expr_t*				right );

END_COMPILER_NAMESPACE
