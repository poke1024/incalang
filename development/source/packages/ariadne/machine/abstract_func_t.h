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
//	abstract_func_t.h		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#if DEBUG_ABSTRACT_FUNCTIONS

#include "machine_func_t.h"

BEGIN_MACHINE_NAMESPACE

class abstract_func_t : public func_t {
public:
						abstract_func_t(
							func_id_t			id,
							class_t*			owner );
};

// ---------------------------------------------------------------------------

inline
abstract_func_t::abstract_func_t(
	func_id_t			id,
	class_t*			owner ) :
	
	func_t( id, func_type_abstract, owner )
{
}

END_MACHINE_NAMESPACE

#endif
