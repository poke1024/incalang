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
//	machine_func_t.h		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef MACHINE_FUNC_H
#define MACHINE_FUNC_H

//#pragma once

#include "machine.h"
#include "input_stream_t.h"

BEGIN_MACHINE_NAMESPACE

class machine_t;

struct variable_t {
	const char*			name;
	const char*			type;
	s32					offset;
};

typedef unsigned long func_id_t;

enum func_type_t {
#if DEBUG_ABSTRACT_FUNCTIONS
	func_type_abstract,
#endif
	func_type_bytecode,
	func_type_native,
	func_type_dll
};

class class_t;

class func_t {
public:						
	virtual				~func_t();

	virtual void		finalize(
							machine_t*			vm );

	inline func_id_t	get_id() const
							{ return m_id; }
		
	inline func_type_t	get_type() const
							{ return m_type; }

	inline class_t*		owner() const
							{ return m_class; }
		
protected:
						func_t(
							func_id_t			id,
							func_type_t			type,
							class_t*			owner );

	func_id_t			m_id;
	func_type_t			m_type;
	class_t*			m_class;
};

END_MACHINE_NAMESPACE

#endif

