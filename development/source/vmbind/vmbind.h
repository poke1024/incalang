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
//	stdlib_bind.h			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef STDLIB_BIND_H
#define STDLIB_BIND_H

//#pragma once

#include "machine.h"

BEGIN_MACHINE_NAMESPACE

class machine_t;
class text_console_t;

extern bool				g_real_time_mode;

void
vm_bind_init();

void
vm_bind_cleanup();

s32
vm_inkey(
	machine_t*			vm );

long
vm_mouse_x(
	machine_t*			vm );

long
vm_mouse_y(
	machine_t*			vm );

long
vm_mouse_z(
	machine_t*			vm );

bool
vm_mouse_button(
	machine_t*			vm,
	int					index );

text_console_t*
vm_text_console(
	machine_t*			vm );

bool
vm_console_open();

void
vm_process_os_events();

void
vm_wait_for_quit();

void
vm_flush();

// ===========================================================================

void
vm_start_background_thread();

void
vm_kill_background_thread();

void
vm_broadcast(
	int					message,
	void*				param );

// ===========================================================================

void
vm_enter_real_time_mode();

void
vm_leave_real_time_mode();

inline bool
vm_in_real_time_mode()
{
	return g_real_time_mode;
}

#define VM_RT_FORBID( f )\
	if( vm_in_real_time_mode() )\
		f.fatal_error( ERR_NON_RT_FUNCTION )

END_MACHINE_NAMESPACE

#endif

