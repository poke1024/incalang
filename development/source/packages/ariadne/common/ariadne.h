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
//	ariadne.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_H
#define ARIADNE_H

//#pragma once

#include "icarus.h"

#define BEGIN_ARIADNE_NAMESPACE			namespace ariadne {
#define END_ARIADNE_NAMESPACE			}

BEGIN_ARIADNE_NAMESPACE

using namespace icarus;

enum err_t {
	ERR_INTERNAL
};

void
ariadne_error(
	err_t				err );

// use String objects instead of char* ?
#define ARIADNE_STRING_OBJECTS			1

// allow casts of the type void* to Object* ?
// casts the other way round are always alled
#define ARIADNE_ALLOW_VOID_CAST			1

extern const char*		s_constructor_name;
extern const char*		s_destructor_name;
extern const char*		s_string_name;
extern long				s_string_name_length;

END_ARIADNE_NAMESPACE

#endif

