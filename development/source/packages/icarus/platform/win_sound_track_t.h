// ===========================================================================
//	win_sound_track_t.h		   ©2000-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#if 0

#ifndef WIN_SOUND_TRACK_H
#define WIN_SOUND_TRACK_H

//#pragma once

#include "sound_track_t.h"
#include <Windows.h>
#include <dsound.h>

BEGIN_ICARUS_NAMESPACE

class win_sound_track_t : public sound_track_t {
public:
						win_sound_track_t();
						
	virtual				~win_sound_track_t();
	
	virtual void		play(
							sound_t*				sound );
						
	virtual void		quiet();
	
	virtual void		set_volume(
							u16						left,
							u16 					right );
							
	virtual bool		busy() const;

	static void			init(
							HWND				hWnd );

	static void			cleanup();

private:
	LPDIRECTSOUNDBUFFER	m_buffer;
	u64					m_busy;
};

typedef win_sound_track_t native_sound_track_t;

END_ICARUS_NAMESPACE

#endif

#endif

