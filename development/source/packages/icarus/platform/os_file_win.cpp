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
//	os_file_win.cp			   ©2000-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "os_file.h"

BEGIN_ICARUS_NAMESPACE

static void
os_convert_filepath(
	const char*				path,
	istring&    			local )
{
	local.clear();
	local.reserve( std::strlen( path ) + 1 );

	local = path;
}

// ---------------------------------------------------------------------------

os_file_t
os_fopen(
	const char*				filename,
	const char*				permission )
{
    istring		    		localpath;
	
	os_convert_filepath( filename, localpath );

	return std::fopen( localpath.c_str(), permission );
}

void
os_fclose(
	os_file_t				file )
{
	std::fclose( file );
}

void
os_fseek(
	os_file_t				file,
	long					offset,
	int						mode )
{
	std::fseek( file, offset, mode );
}
	
long
os_ftell(
	os_file_t				file )
{
	return std::ftell( file );
}
	
long
os_fread(
	void*					buffer,
	long					count,
	os_file_t				file )
{
	return std::fread( buffer, sizeof( u08 ),
		count, file );
}

long
os_fwrite(
	const void*				buffer,
	long					count,
	os_file_t				file )
{
	return std::fwrite( buffer, sizeof( u08 ),
		count, file );
}

void
os_fflush(
	os_file_t				file )
{
	std::fflush( file );
}

int
os_ferror(
	os_file_t				file )
{
	return ferror( file );
}

void
os_fdelete(
	const char*				filename )
{
    istring                 localpath;

	os_convert_filepath( filename, localpath );

	std::remove( localpath.c_str() );
}

bool
os_fexists(
	const char*				filename )
{
    istring				    localpath;
	std::FILE*				fp;

	os_convert_filepath( filename, localpath );
	
	fp = std::fopen( localpath.c_str(), "r" );
	if( !fp )
		return false;
		
	fclose( fp );
	return true;
}

bool
os_create_dir(
	const char*				path )
{
	throw_icarus_error( "not implemented" );
	return false;
}

void
os_remove_dir(
	const char*				path )
{
	throw_icarus_error( "not implemented" );
}

bool
os_dir_exists(
	const char*				path )
{
	throw_icarus_error( "not implemented" );
	return false;
}
void
os_iterate_dir(
	const char*			path,
	iterate_proc_t		callback,
	void*				refcon )
{
	throw_icarus_error( "not implemented" );
}

// ---------------------------------------------------------------------------

bool
os_choose_get_file(
    istring&		    path,
	const char*			title,
	int					type_count,
	const u32*			type_list )
{
	throw_icarus_error( "not implemented" );
	return false;
}

bool
os_choose_put_file(
    istring&		    path,
	const char*			title,
	u32					type )
{
	throw_icarus_error( "not implemented" );
	return false;
}

END_ICARUS_NAMESPACE
