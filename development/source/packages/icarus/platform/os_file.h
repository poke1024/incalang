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
//	os_file.h				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef OS_FILE_H
#define OS_FILE_H

//#pragma once

#include "icarus.h"
#include <cstdio>

BEGIN_ICARUS_NAMESPACE

typedef std::FILE*		os_file_t;

os_file_t
os_fopen(
	const char*			filename,
	const char*			permission );

void
os_fclose(
	os_file_t			file );

void
os_fseek(
	os_file_t			file,
	long				offset,
	int					mode );
	
long
os_ftell(
	os_file_t			file );
	
long
os_fread(
	void*				buffer,
	long				count,
	os_file_t			file );

long
os_fwrite(
	const void*			buffer,
	long				count,
	os_file_t			file );

void
os_fflush(
	os_file_t			file );
	
int
os_ferror(
	os_file_t			file );
	
bool
os_fexists(
	const char*			filename );

void
os_fdelete(
	const char*			filename );

bool
os_create_dir(
	const char*			path );

void
os_remove_dir(
	const char*			path );

bool
os_dir_exists(
	const char*			path );

typedef	void
(*iterate_proc_t)(
	const char*			name,
	void*				refcon );

void
os_iterate_dir(
	const char*			path,
	iterate_proc_t		callback,
	void*				refcon );

bool
os_choose_get_file(
    istring&		       path,
	const char*			title,
	int					type_count,
	const u32*			type_list );

bool
os_choose_put_file(
    istring&	       	path,
	const char*			title,
	u32					type );

END_ICARUS_NAMESPACE

#endif

