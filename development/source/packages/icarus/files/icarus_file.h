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
//	icarus_file.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_FILE_H
#define ICARUS_FILE_H

//#pragma once

#include "icarus.h"
#include "os_file.h"

BEGIN_ICARUS_NAMESPACE

class flat_file_t;

struct file_record_t {
	bool				is_flat;
	union {
		flat_file_t*	flat_file;
		os_file_t		os_file;
	};
};

typedef file_record_t*	file_t;

void
ifflat(
	const char*			filename,
	const char*			path,
	int					flags );

// ---------------------------------------------------------------------------

file_t
ifopen(
	const char*			filename,
	const char*			permission );

void
ifclose(
	file_t				file );

void
ifseek(
	file_t				file,
	fpos_t				offset,
	int					mode );
	
fpos_t
iftell(
	file_t				file );
	
fpos_t
ifread(
	void*				buffer,
	fpos_t				count,
	file_t				file );

fpos_t
ifwrite(
	const void*			buffer,
	fpos_t				count,
	file_t				file );

void
ifflush(
	file_t				file );
	
int
iferror(
	file_t				file );
	
// ---------------------------------------------------------------------------

bool
ifexists(
	const char*			filename );

void
ifdelete(
	const char*			filename );

bool
ifcreatedir(
	const char*			path );

void
ifremovedir(
	const char*			path );

bool
ifdirexists(
	const char*			path );

void
ifiteratedir(
	const char*			path,
	iterate_proc_t		callback,
	void*				refcon );

END_ICARUS_NAMESPACE

#endif

