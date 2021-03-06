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

/*
 *  resolve.h
 *
 *  Created by Bernhard Liebl on Wed Apr 25 2001.
 *  Copyright (c) 2001 Bernhard Liebl.
 *
 */

#ifndef ARIADNE_RESOLVE_H
#define ARIADNE_RESOLVE_H

//#pragma once

#include "compiler.h"

BEGIN_COMPILER_NAMESPACE

struct scope_t;

void
init_resolve();

bool
constants_resolved();

END_COMPILER_NAMESPACE

#endif

