// ===========================================================================
//	mac_mp3_player_t.h		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class mac_mp3_player_t : public mp3_player_t {
public:
						mac_mp3_player_t();

	virtual				~mac_mp3_player_t();

	virtual void		start(
							const char*				path );

	virtual void		stop();
	
	virtual void		set_volume(
							u16						volume );
	
	
	void				callback(
							SndCommand*				command );

	void				deferred();
	
private:
	void				audio_clean_buffer(
							u16*					buffer );

	void				cleanup();

	Handle				m_handle;
	unsigned long		m_size;
	
	SndChannelPtr		m_channel;
	SoundConverter		m_sc;
	Ptr					m_decompression_atom;
	unsigned long		m_input_frames;
	unsigned long		m_input_bytes;
	unsigned long		m_output_bytes;
	unsigned long		m_total_input_bytes;
	Ptr					m_audio_buffer1;
	Ptr					m_audio_buffer2;
	Ptr					m_current_buffer;
	ExtSoundHeader		m_audio_buffer_header;
	DeferredTask		m_dtask;
	bool				m_audio_running;
	
	unsigned long		m_loop_point;
	bool				m_deferred_task_fired;
};

typedef mac_mp3_player_t native_mp3_player_t;

END_ICARUS_NAMESPACE
