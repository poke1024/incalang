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
//	dll_func_t.h			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_DLL_FUNC_H
#define ARIADNE_DLL_FUNC_H

//#pragma once

#include "machine_func_t.h"
#include "fiber_t.h"

BEGIN_MACHINE_NAMESPACE

typedef void (*dll_proc_t)( void );

class dll_func_t : public func_t {
public:
						dll_func_t(
							func_id_t			id,
							class_t*			owner );

	void				call(
							fiber_t&			fiber );

	void				import(
							input_stream_t*		stream,
							machine_t*			m );

private:
	void				load(
							fiber_t&			fiber );

	dll_proc_t			m_proc;
	u32					m_param_size;
	u32					m_return_size;
};

END_MACHINE_NAMESPACE

#endif

