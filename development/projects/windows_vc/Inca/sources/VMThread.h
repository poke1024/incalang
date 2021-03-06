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
//	VMThread.h	 			   �2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "pipe_stream_t.h"
#include "aklabeth.h"

BEGIN_AKLABETH_NAMESPACE

class variable_t;

struct StackFrame {
	istring				name;
	u32					pos;
	void*				clss;
	bool				hasThis;
	variable_t*			globals;
};

struct ThreadState {
	istring				name;
	int					frameCount;
	StackFrame*			frames;
};

struct VMThreadData {
	HWND				hwnd;
	pipe_stream_t*		stream;

	bool				paused;
	bool				resume;
	istring				error;

	int					threadCount;
	ThreadState*		threads;

	HANDLE				resumeEvent;
};

unsigned __stdcall
VMThreadFunc(
	void*				pArguments );

void
InitThreadStates(
	VMThreadData*		data );

void
CleanupThreadStates(
	VMThreadData*		data );

END_AKLABETH_NAMESPACE
