// ===========================================================================
//	main.cp					   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine.h"
#include "file_stream_t.h"
#include "machine_t.h"
#include "xalloc.h"
#include "machine_class_t.h"
#include <cstdio>
#include "environment_t.h"
#include "native_func_t.h"

BEGIN_MACHINE_NAMESPACE

void print( const char* s )
{
	std::printf( s );
}

END_MACHINE_NAMESPACE

using namespace machine;

using namespace std;

class kathrin {
public:
	char name[ 20 ];
};

class janeway {
public:
	char rank[ 10 ];
};

class voyager : public kathrin, public janeway {
public:
	char data[ 4 ];
};

void
myproc(
	environment_t*		env,
	instance_t*			self )
{
	machine_t&			m = *env->get_machine();

	int b = m.pop_int();
	m.push_int( b + 1 );
}

BEGIN_ARIADNE_NAMESPACE

void
ariadne_error(
	err_t				/*err*/ )
{
}

END_ARIADNE_NAMESPACE

int
main()
{
	/*int s;

	janeway* j;
	s = sizeof( janeway );
	j->rank[ 0 ] = 'a';

	kathrin* k;
	s = sizeof( kathrin );
	k->name[ 0 ] = 'a';

	voyager* v;
	s = sizeof( voyager );
	v->name[ 0 ] = 'a';
	v->rank[ 0 ] = 'a';
	v->data[ 0 ] = 'a';
	
	k = v;
	k->name[ 0 ] = 'a';
	j = v;
	j->rank[ 0 ] = 'a';*/
	

	push_xalloc_frame();

	file_stream_t	stream( "objects.a.o", "rb" );

	machine_t		m;

	long			class_count = stream.read_u32();

	for( long i = 0; i < class_count; i++ )
		m.import_class( &stream );
	
	class_t*		clss = m.get_root_class();

	clss->finalize();

	instance_t*		instance = clss->create_instance(
						m.get_default_environment() );
	
	m.init_stdlib();
	
	func_id_t		id = m.find_or_create_func_id( "main", 4 );
	unsigned long	offset = 0;
	func_t*			func = clss->find_func_by_id( id, &offset );
	
	/*if( !func )
		return 1;*/
	
	environment_t*	env;
	//env = m.get_default_environment();
	env = m.get_debugger_environment();
	
	try {
		env->call( (instance_t*)( (u08*)instance + offset ), func );
	} catch( std::exception& e )
	{
		printf( "%s\n", e.what() );
	}

	// set up a native func
	
	/*{
		class_t*	nodeclss = clss->find_or_create_class( "node", 4 );
		func_id_t	id = m.find_or_create_func_id( "f;i", 3 );
		func_t*		func = nodeclss->find_func_by_id( id, 0 );

		if( func->get_type() != func_type_native )
			throw runtime_error( "expected native function" );
		
		( (native_func_t*)func )->set_proc_ptr( myproc );
	}
	
	// call the fib func
	
	//"test;s;l", 8
	func_id_t		id = m.find_or_create_func_id( "fib;i", 5 );
	unsigned long	offset;
	func_t*			func = clss->find_func_by_id( id, &offset );
	
	m.clear_stack();
	m.push_int( 4 );
		
	environment_t*	env;
	
	env = m.get_debugger_environment();
	env->call( (instance_t*)( (u08*)instance + offset ), func );*/
		
	/*debugger_t		debug( &m, instance, func );
	debug.commandline();*/
		
	/*func->disassemble( &m );
	try {
		//m.call( instance, func );
	} catch( std::exception& e )
	{
		printf( "%s\n", e.what() );
	}
	
	char* a = (char*)m.pop_ptr();
	//printf( "%ld\n", (long)m.pop_int() );
	printf( "%s\n", a );*/
	
	return 0;
}
