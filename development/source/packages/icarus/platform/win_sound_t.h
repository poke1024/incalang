// ===========================================================================
//	win_sound_t.h			   ©2000-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#if 0

#ifndef WIN_SOUND_H
#define WIN_SOUND_H

//#pragma once

#include "sound_t.h"
#include <dsound.h>

BEGIN_ICARUS_NAMESPACE

class random_stream_t;

class win_sound_t : public sound_t {
public:
						win_sound_t();
						
	virtual				~win_sound_t();

	virtual void		load(
							const char*				path );
		
	virtual void		dispose();

protected:
	friend class		win_sound_track_t;	

	LPDIRECTSOUNDBUFFER	m_buffer;

private:
	void				read_wave(
							random_stream_t&		stream );
};

typedef win_sound_t native_sound_t;

END_ICARUS_NAMESPACE

#endif

#endif
