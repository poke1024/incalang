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

#include "func_list_t.h"
#include "scope_t.h"
#include "parser.h"

BEGIN_COMPILER_NAMESPACE

void
func_list_init(
	func_list_t*		list )
{
	list->head = 0;
	list->link = &list->head;
}

static bool
func_list_contains_compatible(
	func_list_t*		list,
	func_t*				func )
{
	// this is definitely not an efficient way to check if we have
	// to deal with virtual functions (of which we only want to take
	// the one entered first). however assuming that the number of
	// functions with same name but different parameters will be small
	// (and therefore the list will be small), it shouldn't be too bad.

	func_t*				node = list->head;
	
	while( node )
	{
		if( are_params_equal(
			node->param,
			func->param ) )
		{
			return true;
		}
	
		node = node->next;
	}
	
	return false;
}

void
func_list_add(
	func_list_t*		list,
	func_t*				func )
{
	if( func_list_contains_compatible( list, func ) )
		return;

	func->next = 0;

	*list->link = func;
	list->link = &func->next;
}

END_COMPILER_NAMESPACE
