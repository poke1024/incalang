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
//	VMManager.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "input_stream_t.h"
#include "output_stream_t.h"
#include "pipe_stream_t.h"
#include "aklabeth.h"
#include "code_file_t.h"
#include "VMThread.h"

BEGIN_AKLABETH_NAMESPACE

class VMManager {
public:
						VMManager();

	virtual				~VMManager();

	bool				StartVM(
							const void*			programData,
							long				programLength,
							const CString&		basePath,
							long				stackSize,
							code::file_t*		codeFile,
							HWND				hwnd,
							istring&			error );

	void				KillVM();

	void				ExitVM();

	bool				IsVMRunning();

protected:
	virtual void		OnKillVM();

protected:
	VMThreadData		mVMThreadData;

private:
	HANDLE				m_hVMPipeReadClient;
	HANDLE				m_hVMPipeWriteClient;
	HANDLE				m_hVMPipeReadServer;
	HANDLE				m_hVMPipeWriteServer;
	HANDLE				m_hVMProcess;

	HANDLE				m_hVMThread;

	input_stream_t*		mVMReadStream;
	output_stream_t*	mVMWriteStream;
	pipe_stream_t*		mVMPipeStream;

	u32					mPartitionSize;
};

END_AKLABETH_NAMESPACE
