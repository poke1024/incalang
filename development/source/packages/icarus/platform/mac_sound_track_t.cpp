// ===========================================================================
//	mac_sound_track_t.cp	   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "mac_sound_track_t.h"

BEGIN_ICARUS_NAMESPACE

inline void
check_for_mac_sound_error(
	OSErr				err )
{
#if ICARUS_DEBUG
	if( err != noErr )
		throw_icarus_error( "error playing mac sound" );
#endif
}

static SndCallBackUPP	s_callback_upp = 0;

pascal void
mac_sound_track_t::callback(
	SndChannelPtr		/*chan*/,
	SndCommand*			cmd )
{
	callback_t*			callback;

	callback = ( (callback_t*)cmd->param2 );

	mac_sound_t*		sound;
	mac_sound_track_t*	track;

	sound = callback->sound;
	track = callback->track;
	
	callback->next = track->m_callback;
	track->m_callback = callback;

	track->m_busy = false;

	// note that this function is called at interrupt
	// time now, so don't do anything freaky there.
	sound->finished( track );
}

mac_sound_track_t::mac_sound_track_t() :

	m_channel( 0 ),
	m_callback( 0 ),
	m_busy( false )
{
	if( !s_callback_upp )
		s_callback_upp = NewSndCallBackProc( callback );

	OSErr				err;
	
	err = SndNewChannel(& m_channel, sampledSynth, 0, s_callback_upp );
	if( err != noErr || !m_channel )
		throw_icarus_error( "could not open mac sound channel" );
}

mac_sound_track_t::~mac_sound_track_t()
{
	quiet();

	while( m_busy )
		;

	SndDisposeChannel( m_channel, false );
	
	while( m_callback )
	{
		callback_t*		next = m_callback->next;

		delete m_callback;
		m_callback = next;
	}
}

void
mac_sound_track_t::quiet()
{
	SndCommand			command;
	OSErr				err;

	command.cmd = quietCmd;
	command.param1 = 0;
	command.param2 = 0;
	
	err = SndDoImmediate( m_channel, &command );
	check_for_mac_sound_error( err );
}

void
mac_sound_track_t::play(
	sound_t*			sound )
{
	if( m_busy )
		return;

	mac_sound_t*		msound;
	
	msound = dynamic_cast<mac_sound_t*>( sound );
	if( msound != sound )
		throw_icarus_error( "invalid sound_t instance" );

	callback_t*			callback;
	
	callback = m_callback;
	if( callback )
		m_callback = callback->next;
	else
		callback = new callback_t;
	
	callback->track = this;
	callback->sound = msound;

	m_busy = true;

	// submit commands
	
	SndCommand			command;
	OSErr				err;

	command.cmd = bufferCmd;
	command.param1 = 0;
	command.param2 = (long)&msound->m_header;
		
	err = SndDoCommand( m_channel, &command, false );
	check_for_mac_sound_error( err );

	command.cmd = callBackCmd;
	command.param1 = 0;
	command.param2 = (long)callback;
		
	err = SndDoCommand( m_channel, &command, false );
	check_for_mac_sound_error( err );
}

void
mac_sound_track_t::set_volume(
	u16					left,
	u16					right )
{
	long				macleft;
	long				macright;

	macleft = ( left * (long)kFullVolume ) / 0xffff;
	macright = ( right * (long)kFullVolume ) / 0xffff;
	
	SndCommand			command;
	OSErr				err;
	
	command.cmd = volumeCmd;
	command.param1 = 0;
	command.param2 = ( macright << 16 ) + macleft;

	err = SndDoImmediate( m_channel, &command );
	check_for_mac_sound_error( err );
}

bool
mac_sound_track_t::busy() const
{
	return m_busy;
}

END_ICARUS_NAMESPACE
