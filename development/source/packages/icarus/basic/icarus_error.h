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
//	icarus_error.h			   ©2000-2000 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_ERROR_H
#define ICARUS_ERROR_H

//#pragma once

BEGIN_ICARUS_NAMESPACE

#define					throw_icarus_error\
						::icarus::issue_icarus_error

void
issue_icarus_error(
	const char*			format,
	... );

void
icarus_error(
	const char*			format );

void
icarus_trace(
	const char*			err );

void
icarus_fatal(
	const char*			err );

END_ICARUS_NAMESPACE

#endif

