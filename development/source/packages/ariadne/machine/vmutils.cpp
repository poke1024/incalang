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
//	vmutils.cp				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "vmutils.h"
#include "machine_class_t.h"

BEGIN_MACHINE_NAMESPACE

native_func_t*
register_native_function(
	machine_t&			m,
	class_t*			clss,
	const char*			name,
	native_proc_ptr_t	proc,
	int					flags )
{
	func_id_t			id;
	func_t*				func;
	
	id = m.find_or_create_func_id( name, istrlen( name ) );
	func = clss->find_func_by_id( id, 0 );
	
	if( !func )
	{
#if SAFE_MACHINE
		char			s[ 128 + 1 ];
		
		isnprintf( s, 128, "function %s not found\n", name );
		throw_icarus_error( s );
#endif
		exit( 1 );
		return 0;
	}

	if( func->get_type() != func_type_native )
		throw_icarus_error( "expected native function" );
		
	native_func_t*		native_func;
	
	native_func =  (native_func_t*)func;
		
	native_func->set_proc_ptr( proc );
	native_func->set_flags( flags );
	
	return native_func;
}

END_MACHINE_NAMESPACE
