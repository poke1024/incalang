// ===========================================================================
//	win_mp3_player_t.h		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#if 0
//#ifndef WIN_MP3_PLAYER_H
#define WIN_MP3_PLAYER_H

//#pragma once

#include "mp3_player_t.h"
#include <dshow.h>

BEGIN_ICARUS_NAMESPACE

class win_mp3_player_t : public mp3_player_t {
public:
						win_mp3_player_t();

	virtual				~win_mp3_player_t();

	virtual void		start(
							const char*				path );

	virtual void		stop();
	
	virtual void		set_volume(
							u16						volume );	

	void				handle_mp3_events();

private:
	BOOL				init_mp3(
							HWND					hWndMain );

	IBaseFilter*		load_mp3(
							LPCWSTR					wszFilename );

	void				play_mp3(
							IBaseFilter*			pSource );

	void				replay_mp3();

	void				stop_mp3();

	void				exit_mp3();

	void				on_mp3_finish();

	IGraphBuilder*		g_pGraphBuilder;
	IMediaControl*		g_pMediaControl;
	IMediaSeeking*		g_pMediaSeeking;
	IMediaEventEx*		g_pMediaEventEx;
	bool				m_enabled;
	bool				m_supported;
};

typedef win_mp3_player_t native_mp3_player_t;

END_ICARUS_NAMESPACE

#endif

