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
//	oswrapdefs.h		 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

enum mouse_button_t {
	mouse_button_left,
	mouse_button_right,
	mouse_button_middle
};

enum {
	key_modifier_command							= 1,
	key_modifier_shift								= 2,
	key_modifier_option								= 4
};

enum {
	key_enter										= '\r',
	key_backspace									= '\b',
	key_tabulator									= '\t',
	key_escape										= '\33',
	key_arrow_left									= '\34',
	key_arrow_right									= '\35',
	key_arrow_up									= '\36',
	key_arrow_down									= '\37',
	key_delete										= 0x7f,
	key_page_up										= 0x0b,
	key_page_down									= 0x0c,
	key_home_top									= 0x01,
	key_home_bottom									= 0x04
};

struct command_status_t {
	bool					enabled;
	const char*				text;
	char					key;
	int						modifiers;
};

typedef long				command_t;

const command_t				cmd_Cut = -1000;
const command_t				cmd_Copy = -1001;
const command_t				cmd_Paste = -1002;
const command_t				cmd_Clear = -1003;
const command_t				cmd_SelectAll = -1004;
const command_t				cmd_Undo = -1100;
const command_t				cmd_Redo = -1101;

