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
//	debugger_t.h			   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "disassembler_t.h"
#include <string>

BEGIN_MACHINE_NAMESPACE

union instance_t;
class bytecode_func_t;
class machine_t;
class class_t;
struct call_t;

class debugger_t : public disassembler_t {
public:
						debugger_t(
							machine_t*			machine,
							instance_t*			self,
							bytecode_func_t*	func );
						
						~debugger_t();
	
	void				commandline();
	
	void				command(
							const char*			s );
	
protected:
	void				d(
							long				count );

	void				dc(
							const char*			name );

	void				step(
							long				count );
							
	vm_state_t			m_state;
	bool				m_done;
};

END_MACHINE_NAMESPACE
