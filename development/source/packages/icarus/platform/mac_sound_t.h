// ===========================================================================
//	mac_sound_t.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "sound_t.h"

BEGIN_ICARUS_NAMESPACE

class random_stream_t;

class mac_sound_t : public sound_t {
public:
						mac_sound_t();
						
	virtual				~mac_sound_t();

	virtual void		load(
							const char*				path );
		
	virtual void		dispose();

protected:
	friend class		mac_sound_track_t;	

	CmpSoundHeader		m_header;

private:
	void				read_wave(
							random_stream_t&		stream );

	u08*				m_samples;
};

typedef mac_sound_t native_sound_t;

END_ICARUS_NAMESPACE
