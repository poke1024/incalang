// This file is part of incalang.

// incalang is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.

// incalang is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Foobar; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// ===========================================================================
//	timer_t.cp			 	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include <list>
#include <algorithm>
#include "timer_t.h"

BEGIN_OSWRAP_NAMESPACE

std::list<timer_t*> s_timers;

timer_t::timer_t() :

	m_millis( 1000 ),
	m_timer( 0 )
{
	s_timers.push_back( this );
}

timer_t::~timer_t()
{
	if( m_timer )
		stop();

	s_timers.erase( std::find(
		s_timers.begin(), s_timers.end(), this ) );
}

void
timer_t::set_frequency(
	long				millis )
{
	m_millis = millis;
}

long
timer_t::frequency() const
{
	return m_millis;
}

VOID CALLBACK
MyTimerProc(
  HWND					hwnd,
  UINT					uMsg,
  UINT					idEvent,
  DWORD					dwTime )
{
	for( std::list<timer_t*>::iterator it = s_timers.begin();
		it != s_timers.end(); it++ )
	{
		timer_t*		timer = *it;

		if( timer->m_timer == idEvent )
		{		
			timer->tick();
			break;
		}
	}
}

void
timer_t::restart()
{
	m_timer = 0;
	start();
}

void
timer_t::start()
{
	if( m_timer )
		stop();
		
	m_timer = SetTimer( NULL, (UINT)this,
		m_millis, MyTimerProc );
}
	
void
timer_t::stop()
{
	if( not m_timer )
		return;

	KillTimer( 0, m_timer );
	m_timer = 0;
}	

bool
timer_t::running() const
{
	return m_timer != 0;
}

END_OSWRAP_NAMESPACE