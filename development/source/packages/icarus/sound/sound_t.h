// ===========================================================================
//	sound_t.h				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef SOUND_H
#define SOUND_H

//#pragma once

#include "icarus.h"
#include "array_t.h"

BEGIN_ICARUS_NAMESPACE

class sound_t {
public:
						sound_t();

	virtual				~sound_t();

	virtual void		load(
							const char*				path ) = 0;
		
	virtual void		dispose();

protected:
	friend class		sound_track_t;	

	virtual void		started(
							sound_track_t*			track );

	virtual void		finished(
							sound_track_t*			track );
							
	array_t				m_tracks;
};

END_ICARUS_NAMESPACE

#if ICARUS_OS_MACOS
#include "mac_sound_t.h"
#elif ICARUS_OS_WIN32
#include "win_sound_t.h"
#endif

#endif

