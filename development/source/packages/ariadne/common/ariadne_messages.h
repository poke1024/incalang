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

/*
 *  ariadne_messages.h
 *
 *  Created by Bernhard Liebl on Sat Apr 13 2002.
 *  Copyright (c) 2001 Bernhard Liebl.
 *
 */
 
enum {
	msg_MachineStopped					= 1,
	msg_ProgramTerminated				= 2,
	msg_NoMainFound						= 3,
	
	msg_QuitMachine						= -1,
	msg_GetStackFrameDepth				= -2,
	msg_GetStackFrameInfo				= -3,
	msg_GetFrameVariables				= -4,
	msg_ReadFrameVariable				= -5,
	msg_Continue						= -6,
	msg_FindTypeClass					= -7,
	msg_ExpandClass						= -8,
	msg_ExpandGlobals					= -9,
	msg_StepOver						= -10,
	msg_StepInto						= -11,
	msg_Stop							= -12,

	msg_ReadByte						= 1000,
	msg_ReadWord,
	msg_ReadQuad,
	msg_ReadOcta,
	msg_ReadFloat,
	msg_ReadDouble,
	msg_ReadPointer,
	msg_ReadBlock
};
