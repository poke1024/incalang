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
//	lib_sound.cp			   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "lib_protos.h"
#include "machine_t.h"
#include "fiber_t.h"
#include "vmutils.h"

#include <windows.h>
#include "CMidiPort.h"
#include "vmbind.h"

BEGIN_MACHINE_NAMESPACE

// ---------------------------------------------------------------------------

static bool					sMidiInit = false;
CDirectMusic				gMusic8;
CMidiPort					gOutPort;

static void enumerate_ports()
{
	INFOPORT				info;
	DWORD					dwCount = 0;
	BOOL					bOutSelected = FALSE;
	int						nOutCount = 0;

	while( gMusic8.PortEnumeration( dwCount, &info ) == S_OK )
	{
		if( info.dwClass != DMUS_PC_INPUTCLASS ) 
		{
			if( !( info.dwFlags & DMUS_PC_SOFTWARESYNTH ) )
			{
				//InfOutPort[ nOutCount ] = info;
				//m_OutPortList.AddString( info.szPortDescription );
				
				// FIXME
				//if( strstr( info.szPortDescription, "EDIROL" ) &&
				//	strstr( info.szPortDescription, "MIDI 1" ) )
				
				if( !bOutSelected )
				{
					gMusic8.ActivatePort( &info, &gOutPort );
					//m_OutPortList.SetCurSel(nOutCount);
					bOutSelected = TRUE;
				}
				nOutCount++;
			}
		}
		dwCount++;  
	}	
}

static void
init_midi()
{
	if( not sMidiInit )
	{
		if( FAILED( gMusic8.Initialize() ) )
			icarus_fatal( "could not intialize DirectMusic" );

		enumerate_ports();

		// IMPORTANT: GetLatencyClock() has to be called AFTER
		// the port has been activated in enumerate_ports() !
		gOutPort.GetLatencyClock();

		sMidiInit = true;
	}
}

STDLIB_FUNCTION( midi_all_notes_off )
	init_midi();
	gOutPort.SendMidiMsg( 0x123b0, 1 );	
END_STDLIB_FUNCTION

STDLIB_FUNCTION( midi_select_instrument )
	init_midi();

	DWORD					dwMsg;

	dwMsg = f.pop_int();
	dwMsg = dwMsg << 8 | 0xc0;
	gOutPort.SendMidiMsg( dwMsg, 1 );
END_STDLIB_FUNCTION

STDLIB_FUNCTION( midi_send_message )
	init_midi();

	u08						note = f.pop_byte();
	u08						velocity = f.pop_byte();
	bool					on = f.pop_bool();

	// code a keyboard event in the MIDI 1.0 message format
	DWORD					dwMsg;

	dwMsg = velocity;
	dwMsg  = ( dwMsg << 8 ) | note;
	dwMsg = ( dwMsg << 8 ) | ( on ? 0x90 : 0x80 );

	gOutPort.SendMidiMsg( dwMsg, 1 );
END_STDLIB_FUNCTION

// ---------------------------------------------------------------------------

void
init_sound(
	machine_t&			m )
{
	class_t*			rootclss = m.get_root_class();

	register_native_function(
		m, rootclss, "selectMidiInstrument;i", midi_select_instrument );

	register_native_function(
		m, rootclss, "sendMidiMessage;B;B;a", midi_send_message );

	register_native_function(
		m, rootclss, "allMidiNotesOff", midi_all_notes_off );
}

END_MACHINE_NAMESPACE
