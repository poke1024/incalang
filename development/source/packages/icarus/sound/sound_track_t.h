// ===========================================================================
//	sound_track_t.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef SOUND_TRACK_H
#define SOUND_TRACK_H

//#pragma once

#include "sound_t.h"

BEGIN_ICARUS_NAMESPACE

class sound_t;

class sound_track_t {
public:
						sound_track_t();
						
	virtual				~sound_track_t();
	
	virtual void		play(
							sound_t*			sound ) = 0;
						
	virtual void		quiet() = 0;
	
	virtual void		set_volume(
							u16					left,
							u16 				right ) = 0;
							
	virtual bool		busy() const = 0;
};

END_ICARUS_NAMESPACE

#if ICARUS_OS_MACOS
#include "mac_sound_track_t.h"
#elif ICARUS_OS_WIN32
#include "win_sound_track_t.h"
#endif

#endif

