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
//	vmutils.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef VMUTILS_H
#define VMUTILS_H

//#pragma once

#include "machine_t.h"
#include "native_func_t.h"

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

enum {
	CLASS_HEADER_SIZE = 4
};

#define INSTANCE_SELF( CLASSTYPE, INSTANCE )								\
	( (CLASSTYPE*)(u08*)( ( (u08*)INSTANCE ) + CLASS_HEADER_SIZE ) )

#define SELF_INSTANCE( SELF )												\
	( ( (u08*)SELF ) - CLASS_HEADER_SIZE )

#define STDLIB_FUNCTION( NAME )												\
static void																	\
NAME(																		\
	fiber_t&			f,													\
	instance_t*			 )													\
{

#define STDLIB_CLASS_FUNCTION( NAME, CLASSTYPE )							\
static void																	\
NAME(																		\
	fiber_t&			f,													\
	instance_t*			instptr )											\
{																			\
	u08*				members = ( (u08*)instptr ) + CLASS_HEADER_SIZE;	\
	CLASSTYPE*			self = (CLASSTYPE*)members;
	
#define END_STDLIB_FUNCTION													\
}

// ---------------------------------------------------------------------------

inline instance_t*
offset_instance(
	instance_t*				instance,
	long					offset )
{
	u08*					ptr = (u08*)instance;

	ptr += offset;
	
	return (instance_t*)ptr;
}

// ---------------------------------------------------------------------------

native_func_t*
register_native_function(
	machine_t&			m,
	class_t*			clss,
	const char*			name,
	native_proc_ptr_t	proc,
	int					flags = native_func_can_throw );

END_MACHINE_NAMESPACE

#endif

