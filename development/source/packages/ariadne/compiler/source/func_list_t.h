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
//	func_list_t.h			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"

BEGIN_COMPILER_NAMESPACE

struct func_t;

struct func_list_t {
	func_t*				head;
	func_t**			link;
};

void
func_list_init(
	func_list_t*		list );
	
void
func_list_add(
	func_list_t*		list,
	func_t*				func );

inline func_t*
func_list_head(
	func_list_t*		list )
{
	return list->head;
}	
	
END_COMPILER_NAMESPACE
