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
//	disassembler_t.cp		   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "disassembler_t.h"
#include "bytecode_func_t.h"
#include "native_func_t.h"
#include "machine_class_t.h"
#include "machine_t.h"
#include "xhtab_t.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

#ifdef SUPPORT_DISASSEMBLE

BEGIN_MACHINE_NAMESPACE

using namespace std;

disassembler_t::disassembler_t(
	machine_t*			machine )
{
	m_machine = machine;
}

disassembler_t::~disassembler_t()
{
}

void
disassembler_t::dall()
{
	class_t*			clss;
	clss = m_machine->get_root_class();
	
	dclass( clss );	
}

void
disassembler_t::dclass(
	class_t*			clss )
{
	istring				s;
	istring				r;
	
	class_t::get_full_name( clss, s );
	r = "# class ";
	r.append( s );
	r.append( "\n\n" );
	print( r.c_str() );

	for( long i = 0; i < clss->m_func_count; i++ )
	{
		func_t*			func;
		
		func = clss->m_func_table[ i ];
	
		if( !func )
			continue;
	
		func_type_t		type = func->get_type();
	
		if( type != func_type_bytecode )
			continue;
	
		istring			name;
	
		m_machine->func_id_to_name(
			func->get_id(), name );

		//if( strcmp( name.c_str(), "createLink;Element::*;a" ) == 0 )
		//		continue; // +++
	
		r = "# function ";
		r.append( name );
		r.append( "\n" );
		print( r.c_str() );

		switch( type )
		{
			case func_type_bytecode:
				df( (bytecode_func_t*)func );
				break;
				
			case func_type_native:
				print( "(native)\n" );
				break;

#if DEBUG_ABSTRACT_FUNCTIONS
			case func_type_abstract:
				print( "(abstract)\n" );
				break;
#endif

			default:
				print( "(invalid function type)\n" );
				break;
		}
		
		print( "\n" );
	}
	
	htab_t*				t = clss->m_class_htab;

	if( hfirst( t ) )
	{
		do {
			htab_iterator_t	it;
			
			hgetiterator( t, &it );
			dclass( (class_t*)hstuff( t ) );
			hsetiterator( t, &it );
		} while( hnext( t ) );
	}
}

void
disassembler_t::df(
	bytecode_func_t*	func )
{
	char				buf[ 64 ];

	isnprintf( buf, 63,
		"[ locals size %ld ]\n",
		(long)func->locals_size() );
	print( buf );

	vm_state_t			state;
	
	state.instance = 0;
	state.func = func;
	state.code = func->m_code_data;
	state.sp = state.localsp = 0;
	state.condflags = 0;
	
	const u08*			codeend;
	
	codeend = state.code + func->m_code_size;
	
	while( state.code < codeend )
		dcode( &state );
		
	dump_lifetimes( func );
}

void
disassembler_t::dump_lifetimes(
	bytecode_func_t*	func )
{
	long				count = func->m_lifetime_count;
	
	if( count < 1 )
		return;
	
	lifetime_t*			lifetime = func->m_lifetime_table;
	char				buf[ 64 ];
	
	do {
		print( "[ lifetime " );
		print( lifetime->type );
		print( " " );
		print( lifetime->name );

		isnprintf( buf, 63,
			"(%.4lx,%.4lx) ]\n",
			lifetime->create,
			lifetime->destroy );
	
		print( buf );
		
		lifetime++;
	} while( --count );
}

void
disassembler_t::print(
	const char*			text )
{
	text;
	//printf( text );
}

END_MACHINE_NAMESPACE

#endif

// work around a very strange bug in VC
#if !ICARUS_DEBUG
#include "disassemble.cpp"
#endif