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
//	icarus_file.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "icarus_file.h"
#include "flat_file_t.h"
#include "fast_flat_container_t.h"
#include "cheap_flat_container_t.h"

BEGIN_ICARUS_NAMESPACE

struct flat_node_t {
	flat_node_t*		next;
	flat_container_t*	container;
	std::string			path;
};

flat_node_t*			s_flat_nodes = 0;

void
ifflat(
	const char*			filename,
	const char*			path,
	int					flags )
{
	flat_container_t*	container;

	if( flags & 1 )
		container = new fast_flat_container_t( filename );
	else
		container = new cheap_flat_container_t( filename );

	flat_node_t*		node = new flat_node_t;
	
	node->next = s_flat_nodes;
	node->container = container;
	node->path = path;
	
	s_flat_nodes = node;
}

file_t
ifopen(
	const char*			filename,
	const char*			permission )
{
	int					length = std::strlen( filename );
	flat_node_t*		node = s_flat_nodes;
	toc_entry_t*		entry = 0;
	
	while( node )
	{
		int				pathlength = node->path.length();
	
		if( pathlength < length && 
			not std::memcmp( node->path.data(),
				filename, pathlength ) )
		{
			entry = node->container->lookup(
				filename + pathlength );

			break;
		}
		
		node = node->next;
	}
	
	file_t				file = new file_record_t;
	
	if( entry )
	{
		file->is_flat = true;
		file->flat_file = new flat_file_t( node->container, entry );
	}
	else
	{
		file->is_flat = false;
		file->os_file = os_fopen( filename, permission );
	
		if( !file->os_file )
		{
			delete file;
			return 0;
		}
	}
	
	return file;
}

void
ifclose(
	file_t				file )
{
	if( !file )
		return;
		
	if( file->is_flat )
		delete file->flat_file;
	else
		os_fclose( file->os_file );
		
	delete file;
}

void
ifseek(
	file_t				file,
	fpos_t				offset,
	int					mode )
{
	if( file->is_flat )
		file->flat_file->seek( offset, mode );
	else
		os_fseek( file->os_file, offset, mode );
}
	
fpos_t
iftell(
	file_t				file )
{
	fpos_t				pos;

	if( file->is_flat )
		pos = file->flat_file->tell();
	else
		pos = os_ftell( file->os_file );

	return pos;
}
	
fpos_t
ifread(
	void*				buffer,
	fpos_t				count,
	file_t				file )
{
	fpos_t				avail;

	if( file->is_flat )
		avail = file->flat_file->read( buffer, count );
	else
		avail = os_fread( buffer, count, file->os_file );

	return avail;
}

fpos_t
ifwrite(
	const void*			buffer,
	fpos_t				count,
	file_t				file )
{
	if( file->is_flat )
		throw_icarus_error( "cannot write to flat file" );

	return os_fwrite( buffer, count, file->os_file );
}

void
ifflush(
	file_t				file )
{
	if( not file->is_flat )
		os_fflush( file->os_file );
}
	
int
iferror(
	file_t				file )
{
	if( file->is_flat )
		return 0;

	return os_ferror( file->os_file );
}

// ---------------------------------------------------------------------------

bool
ifexists(
	const char*			filename )
{
	return os_fexists( filename );
}

void
ifdelete(
	const char*			filename )
{
	os_fdelete( filename );
}

bool
ifcreatedir(
	const char*			path )
{
	return os_create_dir( path );
}

void
ifremovedir(
	const char*			path )
{
	os_remove_dir( path );
}

bool
ifdirexists(
	const char*			path )
{
	return os_dir_exists( path );
}

void
ifiteratedir(
	const char*			path,
	iterate_proc_t		callback,
	void*				refcon )
{
	os_iterate_dir( path, callback, refcon );
}

END_ICARUS_NAMESPACE
