// ===========================================================================
//	debugger_t.cp			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "debugger_t.h"
#include "bytecode_func_t.h"
#include "native_func_t.h"
#include "machine_class_t.h"
#include "machine_t.h"
#include "htab_t.h"
//#include "environment_t.h"
#include "safe_memory_t.h"
#include "machine_error.h"
#include "thread_t.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

#if DEBUG_MACHINE

BEGIN_MACHINE_NAMESPACE

using namespace std;

// ---------------------------------------------------------------------------

#define VM_STORE_SP					\
	m_machine->m_stack_pointer = sp;

#define VM_LOAD_SP					\
	sp = m_machine->m_stack_pointer;

#define VM_DONE						\
	m_done = true;					\
	printf( "<done>\n" );			\
	m_state.sp = sp;				\
	goto done;
	
#define VM_ENVIRONMENT				\
	m_machine->m_debugger_environment
	
#define VM_MACHINE					\
	m_machine

#define VM_MEMORY					\
	m_machine->get_memory()
	
#include "bytecode.h"
#include "vmperfdefs.h"

// ---------------------------------------------------------------------------

debugger_t::debugger_t(
	machine_t*			machine,
	instance_t*			self,
	bytecode_func_t*	func ) :
	
	disassembler_t( machine )
{
	m_state.instance = self;
	m_state.func = func;
	m_state.code = func->m_code_data;
	
	u08*				sp = machine->m_stack_pointer;
	const u08*			code = m_state.code;
	
	PUSH_PTR( 0 )
	PUSH_PTR( 0 )
	PUSH_PTR( 0 )
	PUSH_PTR( 0 )
	
	m_state.localsp = sp;
	m_state.sp = sp + func->m_locals_size;
	m_state.condflags = 0;
	
	m_done = false;
}

debugger_t::~debugger_t()
{
}

void
debugger_t::commandline()
{
	debugger_environment_t*	env =
		(debugger_environment_t*)VM_ENVIRONMENT;

#if TARGET_OS_MAC
	ShowCursor();
#endif

	while( !env->m_quit_debugger )
	{
		printf( "> " );
		
		char			s[ 256 ];
		
		gets( s );
		
		if( !strcmp( s, "q" ) )
		{
			env->m_quit_debugger = true;
			return;
		}
		
		command( s );
		
		if( m_done )
		{
			m_done = false;
			break;
		}
	}
}

void
debugger_t::command(
	const char*			s )
{
	if( !strcmp( s, "r" ) )
	{
		while( !m_done )
			step( 100000 );
	}
	else if( !strcmp( s, "s" ) )
		step( 1 );
	else if( !memcmp( s, "s ", 2 ) )
		step( atol( &s[ 2 ] ) );
	else if( !strcmp( s, "d" ) )
		d( 1 );
	else if( !memcmp( s, "d ", 2 ) )
		d( atol( &s[ 2 ] ) );
	else if( !strcmp( s, "df" ) )
		df( m_state.func );
	else if( !strcmp( s, "dall" ) )
		dall();
	else if( !memcmp( s, "dc ", 3 ) )
		dc( &s[ 3 ] );
		
	else if( !strcmp( s, "stack" ) )
	{
		const u08* sp = m_state.sp - 4;
		for( int i = 0; i < 8; i++ )
		{
			printf( "%.4lx: %.4lx\n", (long)sp, (long)*(u32*)sp );
			sp -= 4;
		}
	}
		
	else if( !memcmp( s, "ll", 2 ) )
	{
		const u08* sp = m_state.localsp + atol( &s[ 2 ] );
		printf( "%.4lx\n", (long)*(u32*)sp );
	}
	else if( !memcmp( s, "ls", 2 ) )
	{
		const u08* sp = m_state.localsp + atol( &s[ 2 ] );
		printf( "%.2x\n", (short)*(u16*)sp );
	}
	else if( !memcmp( s, "lb", 2 ) )
	{
		const u08* sp = m_state.localsp + atol( &s[ 2 ] );
		printf( "%.1x\n", (char)*(u08*)sp );
	}
	
	else if( !memcmp( s, "sstr", 4 ) )
	{
		const u08* sp = m_state.sp - atol( &s[ 4 ] ) - 4;
		printf( "'%s'\n", (char*)*(u32*)sp );
	}
	else if( !memcmp( s, "sl", 2 ) )
	{
		const u08* sp = m_state.sp - atol( &s[ 2 ] ) - 4;
		printf( "%.4lx\n", (long)*(u32*)sp );
	}
	else if( !memcmp( s, "ss", 2 ) )
	{
		const u08* sp = m_state.sp - atol( &s[ 2 ] ) - 4;
		printf( "%.2x\n", (short)*(u16*)sp );
	}
	else if( !memcmp( s, "sb", 2 ) )
	{
		const u08* sp = m_state.sp - atol( &s[ 2 ] ) - 4;
		printf( "%.1x\n", (char)*(u08*)sp );
	}
}

void
debugger_t::d(
	long				count )
{
	vm_state_t			state;
	
	bytecode_func_t*	func = m_state.func;
	
	state.self = 0;
	state.func = func;
	state.code = m_state.code;
	state.sp = state.localsp = 0;
	state.condflags = 0;
	
	const u08*			codeend;
	
	codeend = func->m_code_data + func->m_code_size;
	
	while( count-- && state.code < codeend )
		disassemble( &state );
}

void
debugger_t::dc(
	const char*			name )
{
	class_t*			clss;
	clss = m_machine->get_root_class();
	
	htab_t*				t = clss->m_class_htab;

	if( hfirst( t ) )
	{
		do {
			htab_iterator_t	it;
			class_t*		subclss;
			
			hgetiterator( t, &it );
			subclss = (class_t*)hstuff( t );
			
			string			s;
			class_t::get_full_name( subclss, s );
			
			if( strstr( s.c_str(), name ) )
				dclass( subclss );
			
			hsetiterator( t, &it );
		} while( hnext( t ) );
	}
}

void
debugger_t::step(
	long				count )
{
	if( m_done )
		return;

	bytecode_func_t*	func = m_state.func;
	instance_t*			self = m_state.self;
	u08*				sp = m_state.sp;
	u08*				localsp = m_state.localsp;
	const u08*			code = m_state.code;
	int					condflags = m_state.condflags;

#if SAFE_MACHINE
	u08*				memlo = VM_MEMORY->m_memlo;
	u08*				memhi = VM_MEMORY->m_memhi;
#endif

	try
	{
		while( count-- )
		{	
			#include "vmperf.h"
		}
	}
	catch( exception& e )
	{
		printf( "exception [%s]\n", e.what() );
		m_done = true;
	}
	
done:
	m_state.func = func;
	m_state.self = self;
	m_state.sp = sp;
	m_state.localsp = localsp;
	m_state.code = code;
	m_state.condflags = condflags;
}

END_MACHINE_NAMESPACE

#endif
