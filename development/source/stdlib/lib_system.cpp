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
//	lib_system.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"
#include "os_core.h"
#include "vmbind.h"
#include "rt.h"
#include "scheduler_t.h"
#include "input_t.h"

#include <Windows.h>
#include <direct.h>

#include "lib_point.h"
#include "lib_string.h"
#include "lib_array.h"
#include <process.h>
#include "machine_class_t.h"

#include "text_console_t.h"
#include "port_io_t.h"

bool					g_quit_okay = false;
bool					g_quit_issued = false;

BEGIN_MACHINE_NAMESPACE

STDLIB_FUNCTION( system_exit )
	exit( f.pop_int() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_fatal )
	string_t*		s = pop_string( f );
	const char*		t = string_get_c_str( s );
	if( t == 0 )
		t = "fatal error";
	f.fatal_error( ERR_EXCEPTION, t );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_millis )
	f.push_long( rt_micros() / 1000 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_micros )
	f.push_long( rt_micros() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_task )
	if( not vm_in_real_time_mode() )
	{
		vm_process_os_events();
		f.stop_on_demand();
	}
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_enter_real_time )
	vm_enter_real_time_mode();
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_leave_real_time )
	vm_leave_real_time_mode();
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_get_latency )
	f.push_long( rt_cycles_to_micros( scheduler_latency().peak() ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_reset_latency )
	scheduler_latency().clear();
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

USING_FIZBAN_NAMESPACE

STDLIB_FUNCTION( system_enable_ports )
	long				from = f.pop_int();
	long				to = f.pop_int();
	port_io_init();
	enable_ports( from, to );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_disable_ports )
	long				from = f.pop_int();
	long				to = f.pop_int();
	port_io_init();
	disable_ports( from, to );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_inp )
	long				port = f.pop_int();
	f.push_byte( port_in( port ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_outp )
	long				port = f.pop_int();
	u08					value = f.pop_byte();
	port_out( port, value );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( system_show_cursor )
	ShowCursor( TRUE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_hide_cursor )
	ShowCursor( FALSE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_get_date )
	SYSTEMTIME			date;
	GetLocalTime( &date );

	char				buf[ 16 ];
	sprintf( buf, "%02d/%02d/%04d",
		(int)date.wMonth, (int)date.wDay, (int)date.wYear );

	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];
	string_t*			s;
	init_string( block, m );
	s = INSTANCE_SELF( string_t, block );	
	string_create( m, s, buf, 10 );
	f.push_block( block, STRING_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_get_time )
	SYSTEMTIME			date;
	GetLocalTime( &date );

	char				buf[ 16 ];
	sprintf( buf, "%02d:%02d:%02d",
		(int)date.wHour, (int)date.wMinute, (int)date.wSecond );

	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];
	string_t*			s;
	init_string( block, m );
	s = INSTANCE_SELF( string_t, block );	
	string_create( m, s, buf, 8 );
	f.push_block( block, STRING_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_bmove )
	const u08*			from = (u08*)f.pop_ptr();
	u08*				to = (u08*)f.pop_ptr();
	long				size = f.pop_int();
	if( size > 0 )
	{
		if( to + size <= from || to >= from + size )
			memmove( to, from, size );
		else
			memcpy( to, from, size );
	}
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_crscol )
	machine_t*			m = f.machine();
	text_console_t*		console = vm_text_console( m );
	f.push_int( console->crscol() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_crslin )
	machine_t*			m = f.machine();
	text_console_t*		console = vm_text_console( m );
	f.push_int( console->crslin() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_htab )
	machine_t*			m = f.machine();
	text_console_t*		console = vm_text_console( m );
	console->htab( f.pop_int() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_vtab )
	machine_t*			m = f.machine();
	text_console_t*		console = vm_text_console( m );
	console->vtab( f.pop_int() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_at )
	char				buf[ 8 ];
	buf[ 0 ] = 0;
	buf[ 1 ] = 255;
	buf[ 2 ] = 255;
	buf[ 3 ] = '@';
	buf[ 4 ] = f.pop_int();
	buf[ 5 ] = f.pop_int();

	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];
	string_t*			s;
	init_string( block, m );
	s = INSTANCE_SELF( string_t, block );	
	string_create( m, s, buf, 6 );
	f.push_block( block, STRING_CLASS_SIZE );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_fcol )
	point_t*			color = pop_point( f );

	char				buf[ 8 ];
	buf[ 0 ] = 0;
	buf[ 1 ] = 255;
	buf[ 2 ] = 255;
	buf[ 3 ] = 'f';
	buf[ 4 ] =
		( color->x > 0.5 ? 1 : 0 ) |
		( color->y > 0.5 ? 2 : 0 ) |
		( color->z > 0.5 ? 4 : 0 );

	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];
	string_t*			s;
	init_string( block, m );
	s = INSTANCE_SELF( string_t, block );	
	string_create( m, s, buf, 5 );
	f.push_block( block, STRING_CLASS_SIZE );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_bcol )
	point_t*			color = pop_point( f );

	char				buf[ 8 ];
	buf[ 0 ] = 0;
	buf[ 1 ] = 255;
	buf[ 2 ] = 255;
	buf[ 3 ] = 'b';
	buf[ 4 ] =
		( color->x > 0.5 ? 1 : 0 ) |
		( color->y > 0.5 ? 2 : 0 ) |
		( color->z > 0.5 ? 4 : 0 );

	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];
	string_t*			s;
	init_string( block, m );
	s = INSTANCE_SELF( string_t, block );	
	string_create( m, s, buf, 5 );
	f.push_block( block, STRING_CLASS_SIZE );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_get_dir )
	char*				buf;

	if( vm_in_real_time_mode() )
		f.fatal_error( ERR_NON_RT_FUNCTION );

	buf = getcwd( NULL, 0 );
	if( buf == 0 )
		f.fatal_error( ERR_OUT_OF_MEMORY );

	machine_t&			m = *f.machine();
	u08					block[ STRING_CLASS_SIZE ];
	string_t*			s;
	init_string( block, m );
	s = INSTANCE_SELF( string_t, block );	
	string_create( m, s, buf, strlen( buf ) );
	f.push_block( block, STRING_CLASS_SIZE );

	std::free( buf );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_set_dir )
	string_t*			path = pop_string( f );
	const char*			t = string_get_c_str( path );
	if( t == 0 )
		f.fatal_error( ERR_ILLEGAL_PARAMETER );

	if( not SetCurrentDirectory( t ) )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_dfree )
	DWORD				sectorsPerCluster;
	DWORD				bytesPerSector;
	DWORD				numberOfFreeClusters;
	DWORD				totalNumberOfClusters;

	string_t*			path = pop_string( f );
	const char*			t = string_get_c_str( path );

	if( not GetDiskFreeSpace( t,
		&sectorsPerCluster,
		&bytesPerSector,
		&numberOfFreeClusters,
		&totalNumberOfClusters ) )
	{
		f.fatal_error( ERR_IO );
	}
	else
	{
		f.push_long( (u64)numberOfFreeClusters *
			(u64)sectorsPerCluster *
			(u64)bytesPerSector );
	}
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_mkdir )
	string_t*			path = pop_string( f );
	const char*			t = string_get_c_str( path );
	if( t == 0 )
		f.fatal_error( ERR_ILLEGAL_PARAMETER );
	if( mkdir( t ) )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_rmdir )
	string_t*			path = pop_string( f );
	const char*			t = string_get_c_str( path );
	if( t == 0 )
		f.fatal_error( ERR_ILLEGAL_PARAMETER );
	if( rmdir( t ) )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_exist )
	string_t*			path = pop_string( f );
	const char*			t = string_get_c_str( path );
	if( t == 0 )
		f.push_bool( false );
	else
	{
		FILE*			fp;
		fp = fopen( t, "rb" );
		if( fp )
		{
			f.push_bool( true );
			fclose( fp );
		}
		else
		{
			if( errno == ENOENT )
				f.push_bool( false );
			else
				f.fatal_error( ERR_IO );
		}
	}
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_copy_file )
	string_t*			from = pop_string( f );
	string_t*			to = pop_string( f );
	const char*			s = string_get_c_str( from );
	const char*			t = string_get_c_str( to );
	if( s == 0 || t == 0 )
		f.fatal_error( ERR_ILLEGAL_PARAMETER );
	if( not CopyFile( s, t, true ) )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_move_file )
	string_t*			from = pop_string( f );
	string_t*			to = pop_string( f );
	const char*			s = string_get_c_str( from );
	const char*			t = string_get_c_str( to );
	if( s == 0 || t == 0 )
		f.fatal_error( ERR_ILLEGAL_PARAMETER );
	if( not MoveFile( s, t ) )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_unlink )
	string_t*			path = pop_string( f );
	const char*			t = string_get_c_str( path );
	if( t == 0 )
		f.fatal_error( ERR_ILLEGAL_PARAMETER );
	if( unlink( t ) )
		f.fatal_error( ERR_IO );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

struct color_chooser_t {
	int					dummy;
};

struct ofile_chooser_t {
	istring				filter;
};

struct sfile_chooser_t {
	istring				filter;
	istring				title;
};

struct file_iterator_t {
	WIN32_FIND_DATA		data;
	HANDLE				handle;
	bool				init;
	istring				filter;
};

STDLIB_CLASS_FUNCTION( choose_color, color_chooser_t* )
	point_t*			color = pop_point( f );

	CHOOSECOLOR			cc;
	static COLORREF		acrCustClr[ 16 ];

	ZeroMemory( &cc, sizeof( CHOOSECOLOR ) );
	cc.lStructSize = sizeof( CHOOSECOLOR );
	cc.hwndOwner = NULL;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = RGB(
		max( min( color->x * 255.0, 255 ), 0 ),
		max( min( color->y * 255.0, 255 ), 0 ),
		max( min( color->z * 255.0, 255 ), 0 ) );
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if( ChooseColor( &cc )== TRUE )
	{
		color->x = GetRValue( cc.rgbResult ) / 255.0;
		color->y = GetGValue( cc.rgbResult ) / 255.0;
		color->z = GetBValue( cc.rgbResult ) / 255.0;
		f.push_bool( true );
	}
	else
		f.push_bool( false );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( ofile_chooser_create, ofile_chooser_t* )
	ofile_chooser_t*	me;		
	*self = me = new ofile_chooser_t;
	me->filter = "All\0*.*\0";
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( ofile_chooser_destroy, ofile_chooser_t* )
	ofile_chooser_t*	me = *self;
	delete me;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( ofile_chooser_choose, ofile_chooser_t* )
	ofile_chooser_t*	me = *self;

	OPENFILENAME		ofn;
	char				szFile[ 260 ];
	string_t*			string = pop_string( f );
	long				szFileLength;
	istring				filter;

	filter = me->filter;
	filter.append( "\0\0" );
	
	szFileLength = min( string->length, 256 );
	strncpy( szFile, string->text, szFileLength );
	szFile[ szFileLength ] = '\0';

	ZeroMemory( &ofn, sizeof( OPENFILENAME ) );
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof( szFile );
	ofn.lpstrFilter = NULL; //filter.data();
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	machine_t&			m = *f.machine();
	bool				success;
	
	vm_kill_background_thread();
	success = ( GetOpenFileName( &ofn ) == TRUE );
	vm_start_background_thread();
	
	if( success )
	{
		const char*		path = ofn.lpstrFile;
		string_create( m, string, path, strlen( path ) );
		f.push_bool( true );
	}
	else
		f.push_bool( false );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( sfile_chooser_create, sfile_chooser_t* )
	sfile_chooser_t*	me;		
	*self = me = new sfile_chooser_t;
	me->filter = "All\0*.*\0";
	me->title = "Save File As";
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( sfile_chooser_destroy, sfile_chooser_t* )
	sfile_chooser_t*	me = *self;
	delete me;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( sfile_chooser_choose, sfile_chooser_t* )
	sfile_chooser_t*	me = *self;

	OPENFILENAME		ofn;
	char				szFile[ 260 ];
	string_t*			string = pop_string( f );
	long				szFileLength;
	char				szFileTitle[ 260 ];
	istring				filter;

	filter = me->filter;
	filter.append( "\0\0" );
	
	szFileLength = min( string->length, 256 );
	strncpy( szFile, string->text, szFileLength );
	szFile[ szFileLength ] = '\0';

	ZeroMemory( &ofn, sizeof( OPENFILENAME ) );
	ofn.lStructSize = sizeof( OPENFILENAME ); 
	ofn.hwndOwner = NULL; 
	ofn.lpstrFilter = NULL; //filter.data();
	ofn.lpstrFile = szFile; 
	ofn.nMaxFile = sizeof( szFile ); 
	ofn.lpstrFileTitle = szFileTitle; 
	ofn.nMaxFileTitle = sizeof( szFileTitle ); 
	ofn.lpstrInitialDir = (LPSTR)NULL; 
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT; 
	ofn.lpstrTitle = me->title.c_str(); 
 
	machine_t&			m = *f.machine();
	bool				success;
	
	vm_kill_background_thread();
	success = ( GetSaveFileName( &ofn ) == TRUE );
	vm_start_background_thread();

	if( success )
	{
		string_dispose( m, string );
		string_create( m, string,
			ofn.lpstrFile, strlen( ofn.lpstrFile ) );
		f.push_bool( true );
	}
	else
		f.push_bool( false );
}

STDLIB_CLASS_FUNCTION( file_iterator_create, file_iterator_t* )
	file_iterator_t*	me;	
	*self = me = new file_iterator_t;
	me->handle = 0;
	me->init = false;
	me->filter = "*.*";
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( file_iterator_destroy, file_iterator_t* )
	file_iterator_t*	me = *self;
	if( me->init && me->handle != INVALID_HANDLE_VALUE )
		FindClose( me->handle );
	delete me;
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( file_iterator_set_filter, file_iterator_t* )
	file_iterator_t*	me = *self;
	string_t*			filter = pop_string( f );
	me->filter.assign( filter->text, filter->length );
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( file_iterator_next, file_iterator_t* )
	file_iterator_t*	me = *self;

	if( not me->init )
	{
		me->init = true;
		me->handle = FindFirstFile(
			me->filter.c_str(), &me->data );
		f.push_bool(
			me->handle != INVALID_HANDLE_VALUE );
	}
	else
	{
		HANDLE			handle = me->handle;

		if( handle == INVALID_HANDLE_VALUE )
			f.push_bool( false );
		else
			f.push_bool( FindNextFile(
				handle, &me->data ) != FALSE );
	}
END_STDLIB_FUNCTION

STDLIB_CLASS_FUNCTION( file_iterator_name, file_iterator_t* )
	file_iterator_t*	me = *self;
	machine_t&			m = *f.machine();
	u08					buf[ STRING_CLASS_SIZE ];
	string_t*			string = INSTANCE_SELF( string_t, buf );
	const char*			name = me->data.cFileName;

	init_string( buf, m );
	if( me->init && me->handle != INVALID_HANDLE_VALUE )
		string_create( m, string, name, strlen( name ) );
	else
		string_create( m, string, "", 0 );
	f.push_block( buf, STRING_CLASS_SIZE );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( system_exec )
	string_t*			path = pop_string( f );
	array_t*			params = pop_array( f, STRING_CLASS_SIZE );	

	istring				pathstr;
	pathstr.assign( path->text, path->length );

	char**				paramsstr;
	paramsstr = new char*[ params->count ];
	for( int i = 0; i < params->count; i++ )
	{
		u08*			p = (u08*)params->data;
		p += i * STRING_CLASS_SIZE;
		string_t*		s = INSTANCE_SELF( string_t, p );
		long			length = s->length;
		char*			t;
		t = (char*)imalloc( sizeof( char ) * ( length + 1 ) );
		strncpy( t, s->text, length );
		t[ length ] = '\0';
		paramsstr[ i ] = t;
	}

	f.push_int( execv( pathstr.c_str(), paramsstr ) );

	for( int i = 0; i < params->count; i++ )
		ifree( paramsstr[ i ] );
	delete[] paramsstr;
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( mouse_x )
	machine_t&			m = *f.machine();
	f.push_int( vm_mouse_x( &m ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( mouse_y )
	machine_t&			m = *f.machine();
	f.push_int( vm_mouse_y( &m ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( mouse_z )
	machine_t&			m = *f.machine();
	f.push_int( vm_mouse_z( &m ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( mouse_button )
	machine_t&			m = *f.machine();
	int					index = f.pop_int();
	f.push_bool( vm_mouse_button( &m, index ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( mouse_wait_button )
	machine_t&			m = *f.machine();
	while( true )
	{
		for( int i = 0; i < 8; i++ )
		{
			if( vm_mouse_button( &m, i ) )
			{
				f.push_int( i );
				break;
			}
		}
		ithread_t::current()->sleep( 100L * 1000L );
		vm_process_os_events();
	}
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

STDLIB_FUNCTION( di_mouse_x )
	f.push_int( fizban::rt_mouse_x() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( di_mouse_y )
	f.push_int( fizban::rt_mouse_y() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( di_mouse_z )
	f.push_int( fizban::rt_mouse_z() );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( di_mouse_button )
	int					index = f.pop_int();
	f.push_bool( fizban::rt_mouse_button( index ) );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( di_update )
	fizban::rt_input_update();
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_system(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "exit;i", system_exit );

	register_native_function(
		m, rootclss, "fatal;.String::&", system_fatal );

	register_native_function(
		m, rootclss, "millis", system_millis );

	register_native_function(
		m, rootclss, "micros", system_micros );

	register_native_function(
		m, rootclss, "systemTask", system_task );

	register_native_function(
		m, rootclss, "enterRealTime", system_enter_real_time );

	register_native_function(
		m, rootclss, "leaveRealTime", system_leave_real_time );

	register_native_function(
		m, rootclss, "systemLatency", system_get_latency );
			
	register_native_function(
		m, rootclss, "resetSystemLatency", system_reset_latency );



	register_native_function(
		m, rootclss, "enablePorts;i;i", system_enable_ports );

	register_native_function(
		m, rootclss, "disablePorts;i;i", system_disable_ports );

	register_native_function(
		m, rootclss, "inp;i", system_inp );

	register_native_function(
		m, rootclss, "outp;i;B", system_outp );



	register_native_function(
		m, rootclss, "showCursor", system_show_cursor );

	register_native_function(
		m, rootclss, "hideCursor", system_hide_cursor );

	register_native_function(
		m, rootclss, "date", system_get_date );

	register_native_function(
		m, rootclss, "time", system_get_time );

	register_native_function(
		m, rootclss, "bmove;.v*;v*;i", system_bmove );

	register_native_function(
		m, rootclss, "crscol", system_crscol );
	
	register_native_function(
		m, rootclss, "crslin", system_crslin );

	register_native_function(
		m, rootclss, "htab;i", system_htab );

	register_native_function(
		m, rootclss, "vtab;i", system_vtab );

	register_native_function(
		m, rootclss, "at;i;i", system_at );

	register_native_function(
		m, rootclss, "forecolor;.Color::&", system_fcol );

	register_native_function(
		m, rootclss, "backcolor;.Color::&", system_bcol );


	register_native_function(
		m, rootclss, "mousex", mouse_x );

	register_native_function(
		m, rootclss, "mousey", mouse_y );

	register_native_function(
		m, rootclss, "mousez", mouse_z );

	register_native_function(
		m, rootclss, "mousek;i", mouse_button );

	register_native_function(
		m, rootclss, "waitmouse", mouse_wait_button );



	register_native_function(
		m, rootclss, "diMousex", di_mouse_x );

	register_native_function(
		m, rootclss, "diMousey", di_mouse_y );

	register_native_function(
		m, rootclss, "diMousez", di_mouse_z );

	register_native_function(
		m, rootclss, "diMousek;i", di_mouse_button );

	register_native_function(
		m, rootclss, "diUpdate", di_update );


	class_t*			clss;

	clss = rootclss->find_class( "ColorChooser", 12 );
	if( !clss )
		throw_icarus_error( "ColorChooser class not found" );

	register_native_function(
		m, clss, "choose;Color::&", choose_color );
	

	clss = rootclss->find_class( "OpenFileChooser", 15 );
	if( !clss )
		throw_icarus_error( "OpenFileChooser class not found" );

	register_native_function(
		m, clss, "create", ofile_chooser_create );

	register_native_function(
		m, clss, "destroy", ofile_chooser_destroy );

	register_native_function(
		m, clss, "choose;String::&", ofile_chooser_choose );




	clss = rootclss->find_class( "SaveFileChooser", 15 );
	if( !clss )
		throw_icarus_error( "SaveFileChooser class not found" );

	register_native_function(
		m, clss, "create", sfile_chooser_create );

	register_native_function(
		m, clss, "destroy", sfile_chooser_destroy );

	register_native_function(
		m, clss, "choose;String::&", sfile_chooser_choose );



	clss = rootclss->find_class( "FileIterator", 12 );
	if( !clss )
		throw_icarus_error( "FileIterator class not found" );

	register_native_function(
		m, clss, "create", file_iterator_create );

	register_native_function(
		m, clss, "destroy", file_iterator_destroy );

	register_native_function(
		m, clss, "setFilter;.String::&", file_iterator_set_filter );

	register_native_function(
		m, clss, "next", file_iterator_next );

	register_native_function(
		m, clss, "name", file_iterator_name );


	register_native_function(
		m, rootclss, "currentDir",
		system_get_dir );

	register_native_function(
		m, rootclss, "changeDir;.String::&",
		system_set_dir );

	register_native_function(
		m, rootclss, "createDir;.String::&",
		system_mkdir );	

	register_native_function(
		m, rootclss, "deleteDir;.String::&",
		system_rmdir );	

	register_native_function(
		m, rootclss, "copyFile;.String::&;.String::&",
		system_copy_file );

	register_native_function(
		m, rootclss, "moveFile;.String::&;.String::&",
		system_move_file );

	register_native_function(
		m, rootclss, "deleteFile;.String::&",
		system_unlink );

	register_native_function(
		m, rootclss, "diskFree;.String::&",
		system_dfree );	

	register_native_function(
		m, rootclss, "fileExists;.String::&",
		system_exist );

	register_native_function(
		m, rootclss, "exec;.String::&;.StringArray::&",
		system_exec );
}

END_MACHINE_NAMESPACE
