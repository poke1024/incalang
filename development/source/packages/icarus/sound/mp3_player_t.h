// ===========================================================================
//	mp3_player_t.h			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class mp3_player_t {
public:
						mp3_player_t();

	virtual				~mp3_player_t();

	virtual void		start(
							const char*				path ) = 0;
							
	virtual void		stop() = 0;

	virtual void		set_volume(
							u16						volume ) = 0;
};

END_ICARUS_NAMESPACE

#if ICARUS_OS_MACOS
#include "mac_mp3_player_t.h"
#elif ICARUS_OS_WIN32
#include "win_mp3_player_t.h"
#endif
