// ===========================================================================
//	os_file_mac.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "os_file.h"

#include <Processes.h>
#include <string.h>
#include <string>

#include "MoreFiles.h"
#include "MoreFilesExtras.h"
#include "FullPath.h"
#include "IterateDirectory.h"

#include "NavigationUtils.h"

BEGIN_ICARUS_NAMESPACE

static void
os_convert_filepath(
	const char*				path,
	std::string&			local )
{
	local.clear();
	local.reserve( std::strlen( path ) + 1 );
	
	if( path[ 0 ] == '/' )
		path += 1;
	else
		local.append( 1, ':' );

	const char*				s = path;
	
	while( true )
	{
		char				c = *s++;
		
		if( c == '\0' )
			break;
		else if( c == '/' )
			local.append( 1, ':' );
		else
			local.append( 1, c );
	}
}

static OSErr
spec_to_path(
	const FSSpec*			spec,
	std::string&			path )
{
	short					appPathLength;
	Handle					appPath;

	OSErr					err;

	err = FSpGetFullPath(
		spec, &appPathLength, &appPath );
		
	if( err != noErr )
		return err;
		
	if( appPath == nil )
		return fnfErr;
		
	int				pathLen = GetHandleSize( appPath );
		
	path = "/";
		
	HLock( appPath );
	path.append( *appPath, pathLen );
	HUnlock( appPath );
	
	path.erase(
		path.rfind( ":" ) + 1 );
	
	DisposeHandle( appPath );
	
	return noErr;
}

static void
get_full_app_path(
	std::string&			path )
{
	static std::string		sPath;

	if( sPath.length() == 0 )
	{
		OSErr				err;
		
		Str255				processName;
		ProcessSerialNumber	processSerial;
		ProcessInfoRec		processInfo;
		FSSpec				spec;
		
		err = GetCurrentProcess( &processSerial );
		if( err != noErr )
			throw_icarus_error( "error getting app path" );
		
		processInfo.processInfoLength = sizeof( processInfo );
		processInfo.processName = processName;
		processInfo.processAppSpec = &spec;
		
		err = GetProcessInformation(
			&processSerial, &processInfo );
		
		if( err != noErr )
			throw_icarus_error( "error getting app path" );
		
		if( spec_to_path( &spec, sPath ) != noErr )
			throw_icarus_error( "error getting app path" );			
	}

	path = sPath;
}

static OSErr
path_to_spec(
	const char*				path,
	FSSpec*					spec )
{
	std::string				buf;

	get_full_app_path( buf );
	
	buf.append( path );
	
	for( int i = 0; i < buf.length(); i++ )
		if( buf[ i ] == '/' )
			buf[ i ] = ':';
			
	OSErr				err;
	
	err = FSpLocationFromFullPath(
		buf.length(), buf.data(), spec );

	return err;
}

// ---------------------------------------------------------------------------

os_file_t
os_fopen(
	const char*				filename,
	const char*				permission )
{
	std::string				localpath;
	
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
#if ICARUS_OS_MACOS
	// workaround for MW CodeWarrior bug in fread
	const int				step = 16;
	long					read = 0;

	while( count > step )
	{
		read += std::fread( buffer, sizeof( u08 ),
			step, file );
		buffer = (u08*)buffer + step;
		count -= step;
	}
	if( count )
	{
		read += std::fread( buffer, sizeof( u08 ),
			count, file );
	}
	return read;
#else
	return std::fread( buffer, sizeof( u08 ),
		count, file );
#endif
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
	return std::ferror( file );
}

void
os_fdelete(
	const char*				filename )
{
	std::string				localpath;

	os_convert_filepath( filename, localpath );

	std::remove( localpath.c_str() );
}

bool
os_fexists(
	const char*				filename )
{
	std::string				localpath;
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
	std::string				buf = path;
	std::string				name = path;
	
	int						pos;

	pos = buf.rfind( "/" );
	if( pos >= 0 )
	{
		name = buf.substr( pos + 1 );
		buf.erase( pos + 1 );
	}

	FSSpec					spec;

	if( path_to_spec( buf.c_str(), &spec ) != noErr )
		throw_icarus_error( "error creating directory" );
		
	long					dirID;
	Boolean					isDirectory;
	OSErr					err;
	
	err = GetDirectoryID( spec.vRefNum, spec.parID,
		spec.name, &dirID, &isDirectory );
		
	if( err != noErr || not isDirectory )
		throw_icarus_error( "error creating directory" );
		
	long					newDirID;
	Str255					newDirName;
		
	newDirName[ 0 ] = name.length();
	std::memcpy( &newDirName[ 1 ], name.data(),
		std::min( 255UL, name.length() ) );
		
	err = DirCreate(
		spec.vRefNum, dirID, newDirName, &newDirID );

	if( err != noErr )
		throw_icarus_error( "error creating directory" );
	
	return true;	
}

void
os_remove_dir(
	const char*				path )
{
	std::string				buf = path;
	
	buf.append( "/" );

	FSSpec					spec;
	OSErr					err;

	if( path_to_spec( buf.c_str(), &spec ) != noErr )
		throw_icarus_error( "error removing directory" );	

	err = DeleteDirectory(
		spec.vRefNum, spec.parID, spec.name );

	if( err != noErr )
		throw_icarus_error( "error removing directory" );
}

bool
os_dir_exists(
	const char*				path )
{
	std::string				buf = path;
	
	buf.append( "/" );

	FSSpec					spec;
	OSErr					err;

	err = path_to_spec( buf.c_str(), &spec );
	
	if( err == noErr )
		return true;
	else if( err != dirNFErr && err != nsvErr )
		throw_icarus_error( "file system error" );

	// in some cases we get nsvErr instead of a dirNFErr
	// due to a strange behaviour of MoreFiles

	return false;
}

struct iterate_data_t {
	iterate_proc_t			callback;
	void*					refcon;
};

static pascal void
iterate_filter(
	const CInfoPBRec* const	cpbPtr,
	Boolean*				/*quitFlag*/,
	void*					dataPtr )
{
	iterate_data_t*			data;
	
	data = (iterate_data_t*)dataPtr;
	
	std::string				name;
	
	name.assign(
		(char*)&cpbPtr->hFileInfo.ioNamePtr[ 1 ],
		cpbPtr->hFileInfo.ioNamePtr[ 0 ] );
	
	data->callback( name.c_str(), data->refcon );
}	 

void
os_iterate_dir(
	const char*			path,
	iterate_proc_t		callback,
	void*				refcon )
{
	std::string			buf = path;
	
	buf.append( "/" );

	FSSpec				spec;

	if( path_to_spec( buf.c_str(), &spec ) != noErr )
		throw_icarus_error( "error iterating directory" );
	
	iterate_data_t		data;
	
	data.callback = callback;
	data.refcon = refcon;
	
	FSpIterateDirectory( &spec, 1,
		iterate_filter, &data );
}

// ---------------------------------------------------------------------------

static void
setup_navigation_param(
	NavigationParams&	params )
{
	OSErr				err;
		
	Str255				processName;
	ProcessSerialNumber	processSerial;
	ProcessInfoRec		processInfo;
	FSSpec				spec;
		
	err = GetCurrentProcess( &processSerial );
	if( err != noErr )
		throw_icarus_error( "error getting app name" );
		
	processInfo.processInfoLength = sizeof( processInfo );
	processInfo.processName = processName;
	processInfo.processAppSpec = &spec;
		
	err = GetProcessInformation(
		&processSerial, &processInfo );
		
	if( err != noErr )
		throw_icarus_error( "error getting app name" );

	BlockMoveData( processName,
		params.applicationName, sizeof( Str63 ) );
	params.applicationCreator =
		processInfo.processSignature;
}

bool
os_choose_get_file(
	std::string&		path,
	const char*			title,
	int					type_count,
	const u32*			type_list )
{
	NavigationParams	params;
	
	setup_navigation_param( params );
	params.eventCallback = kFakeEventCallBack;
	
	params.displayPreview = false;
	params.numTypes = type_count;
	
	for( int i = 0; i < std::min(
		kNavMaxTypes, type_count ); i++ )
	{
		params.typeList[ i ] = type_list[ i ];
	}
	
	params.filterCallback = nil;
	
	Str255				ptitle;
	
	ptitle[ 0 ] = std::min(
		(long)std::strlen( title ), 255L );
	BlockMoveData( title, &ptitle[ 1 ], ptitle[ 0 ] );
	
	if( NavigationGetFile( &params, ptitle ) != noErr )
		return false;
		
	if( spec_to_path( &params.outFile, path ) != noErr )
		throw_icarus_error( "cannot find file" );

	path.append(
		(const char*)&params.outFile.name[ 1 ],
		params.outFile.name[ 0 ] );
	
	for( int i = 0; i < path.length(); i++ )
		if( path[ i ] == ':' )
			path[ i ] = '/';
	
	return true;
}

bool
os_choose_put_file(
	std::string&		path,
	const char*			title,
	u32					type )
{
	NavigationParams	params;
	
	setup_navigation_param( params );
	params.eventCallback = kFakeEventCallBack;
	
	params.inFileType = type;

	Str255				ptitle;
	
	ptitle[ 0 ] = std::min(
		(long)std::strlen( title ), 255L );
	BlockMoveData( title, &ptitle[ 1 ], ptitle[ 0 ] );
	
	std::string			name;
	Str255				pname;
	
	if( path.rfind( "/" ) >= 0 )
		name = path.substr( path.rfind( "/" ) + 1 );
	else
		name = path;

	pname[ 0 ] = std::min(
		(long)name.length(), 255L );
	BlockMoveData( name.data(), &pname[ 1 ], pname[ 0 ] );

	if( NavigationPutFile( &params, ptitle, pname ) != noErr )
		return false;

	if( spec_to_path( &params.outFile, path ) != noErr )
		throw_icarus_error( "cannot find file" );
		
	path.append(
		(const char*)&params.outFile.name[ 1 ],
		params.outFile.name[ 0 ] );

	for( int i = 0; i < path.length(); i++ )
		if( path[ i ] == ':' )
			path[ i ] = '/';
		
	return true;
}

END_ICARUS_NAMESPACE
