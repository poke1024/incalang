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
//	machine_error.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_MACHINE_ERROR_H
#define ARIADNE_MACHINE_ERROR_H

//#pragma once

#include "machine.h"
#include <stdexcept>

BEGIN_MACHINE_NAMESPACE

enum {
	ERR_NONE,
	
	ERR_ILLEGAL_ACCESS,
	ERR_ILLEGAL_STRING_INDEX,
	ERR_ILLEGAL_ARRAY_INDEX,
	ERR_ILLEGAL_PARAMETER,
	ERR_STACK_OVERFLOW,
	ERR_OUT_OF_MEMORY,
	ERR_NO_G2WORLD_SET,
	ERR_NO_G3WORLD_SET,
	ERR_ILLEGAL_PIXEL_FORMAT,
	ERR_EMPTY_PIXMAP,
	ERR_ARITHMETIC,
	ERR_NON_RT_FUNCTION,

	ERR_FILE_NOT_OPEN,
	ERR_IO,
	
	ERR_EXCEPTION,
	ERR_UNKNOWN
};

/*class vm_runtime_error_t : public std::runtime_error {
public:
						vm_runtime_error_t(
							int					err,
							func_t*				func,
							long				offset );
							
	const char*			what() const;

	long				where();

	int					m_err;
	func_t*				m_func;
	long				m_offset;
};	

void
throw_vm_runtime_error(
	int					err,
	func_t*				func,
	long				offset );*/

struct stop_state_t;

void
error_string(
	const stop_state_t&	state,
	istring&			text );

END_MACHINE_NAMESPACE

#endif

