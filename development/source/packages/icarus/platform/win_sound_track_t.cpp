// ===========================================================================
//	win_sound_track_t.cp	   ©2000-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#if 0

#include "win_sound_track_t.h"
#include "win_sound_t.h"
#include "os_core.h"

BEGIN_ICARUS_NAMESPACE

LPDIRECTSOUND			lpds = NULL;

void
win_sound_track_t::init(
	HWND				hWnd )
{
	if( lpds )
		return;

	if( DirectSoundCreate( 0, &lpds, NULL ) != DS_OK )
		throw_icarus_error( "dsound error" );
	if( lpds->SetCooperativeLevel( hWnd, DSSCL_NORMAL  ) != DS_OK )
		throw_icarus_error( "dsound error" );
}

void
win_sound_track_t::cleanup()
{
	if( !lpds )
		return;

	lpds->Release();
	lpds = NULL;
}

// ===========================================================================

win_sound_track_t::win_sound_track_t() :

	m_buffer( 0 )
{
}

win_sound_track_t::~win_sound_track_t()
{
	if( m_buffer )
	{
		m_buffer->Release();
		m_buffer = 0;
	}
}

void
win_sound_track_t::quiet()
{
	if( m_buffer )
		m_buffer->Stop();
}

void
win_sound_track_t::play(
	sound_t*			sound )
{
	if( m_buffer )
	{
		m_buffer->Release();
		m_buffer = 0;
	}

	win_sound_t*		win_sound = (win_sound_t*)sound;
	LPDIRECTSOUNDBUFFER	buffer;

	buffer = win_sound->m_buffer;
	lpds->DuplicateSoundBuffer( buffer, &m_buffer );

	if( m_buffer == 0 )
		return;

	m_buffer->Restore();
	m_buffer->SetCurrentPosition( 0 );
	m_buffer->Play( 0, 0, 0 );
}

void
win_sound_track_t::set_volume(
	u16					left,
	u16					right )
{
	if( m_buffer == 0 )
		return;

	s32					vol;
	const long			mindb = -2000;

	vol = max( left, right );
	vol *= 0 - mindb;
	vol /= 0xffff;

	m_buffer->SetVolume( mindb + vol );	

	long				pan;

	if( left < right )
		pan = ( DSBPAN_RIGHT * (u32)( right - left ) ) / 0xffff;
	else
		pan = ( DSBPAN_LEFT * (u32)( left - right ) ) / 0xffff;

	m_buffer->SetPan( pan );
}

bool
win_sound_track_t::busy() const
{
	if( m_buffer == 0 )
		return false;

	unsigned long		res;

	m_buffer->GetStatus( &res );
	if( res == DSBSTATUS_PLAYING )
		return true;

	return false;
}

END_ICARUS_NAMESPACE

#endif

