// ===========================================================================
//	sound_t.cp				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "sound_t.h"
#include "sound_track_t.h"

BEGIN_ICARUS_NAMESPACE

sound_t::sound_t() :

	m_tracks( sizeof( sound_track_t* ) )
{
}

sound_t::~sound_t()
{
	dispose();
}

void
sound_t::dispose()
{
	array_t					tracks( sizeof( void* ) );
	sound_track_t*			track;

	for( int i = 0; i < m_tracks.count(); i++ )
	{
		track = *(sound_track_t**)m_tracks[ i ];
		tracks.append( &track );
	}

	for( int i = 0; i < tracks.count(); i++ )
	{
		track = *(sound_track_t**)tracks[ i ];
		track->quiet();
	}
}

void
sound_t::started(
	sound_track_t*			track )
{
	m_tracks.append( &track );
}

void
sound_t::finished(
	sound_track_t*			track )
{
	for( int i = 0; i < m_tracks.count(); i++ )
	{
		sound_track_t*	item;
		
		item = *(sound_track_t**)m_tracks[ i ];
		if( item == track )
		{
			m_tracks.remove( i );
			break;
		}
	}
}

END_ICARUS_NAMESPACE
