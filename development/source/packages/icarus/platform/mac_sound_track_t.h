// ===========================================================================
//	mac_sound_track_t.cp	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "sound_track_t.h"

BEGIN_ICARUS_NAMESPACE

class mac_sound_track_t : public sound_track_t {
public:
						mac_sound_track_t();
						
	virtual				~mac_sound_track_t();
	
	virtual void		play(
							sound_t*				sound );
						
	virtual void		quiet();
	
	virtual void		set_volume(
							u16						left,
							u16 					right );
							
	virtual bool		busy() const;

private:
	static pascal void	callback(
							SndChannelPtr			chan,
							SndCommand*				cmd );

	struct callback_t {
		callback_t*			next;
		mac_sound_track_t*	track;
		mac_sound_t*		sound;
	};
	
	SndChannelPtr		m_channel;
	callback_t*			m_callback;
	bool				m_busy;
};

typedef mac_sound_track_t native_sound_track_t;

END_ICARUS_NAMESPACE
