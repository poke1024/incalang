
/*
Module : CMidiPort.cpp
Purpose: Defines the code implementation for the CMidiPort and CDirectMusic classes
Created: CJP / 08-02-2002
History: CJP / 14-07-2002 
	
	1. Fixed bug when m_pPort of CMidiPort is NULL
	
	2. Improved CMidiPort destructor

	3. Check member variables which values are NULL before performing an operation
	
Copyright (c) 2002 by C. Jiménez de Parga  
All rights reserved.
*/

#include <windows.h>
#include "CMidiPort.h"

// The constructor of the class, member variables initialization

CDirectMusic::CDirectMusic()
{
    m_pMusic			= NULL; 
	m_pMusic8			= NULL; 
	m_pPerformance		= NULL;
	// Initialize COM
	CoInitialize(NULL);	
}


// Intialization of CDirectMusic object.
// Creates instances of the most important interfaces

HRESULT CDirectMusic::Initialize(HWND hWnd)
{
	HRESULT hr;
	
	
    // Create the performance object

	if (FAILED(hr = CoCreateInstance(CLSID_DirectMusicPerformance,NULL,
        CLSCTX_INPROC,IID_IDirectMusicPerformance8,
        (void**)&m_pPerformance))) return hr;
	
	if (FAILED(hr = m_pPerformance->Init(&m_pMusic,NULL,hWnd))) return hr;
	
	// Invoke the new interface pointer for IDirectMusic8 object
	
	if (FAILED(hr = m_pMusic->QueryInterface(IID_IDirectMusic8,(LPVOID*)&m_pMusic8))) return hr;
	
	// Release the old interface, not needed any more
	
	SAFE_RELEASE(m_pMusic);
	
	return S_OK;
}
      

// Port enumeration function
// Enumerate all the ports active in the system

HRESULT CDirectMusic::PortEnumeration(DWORD dwIndex,LPINFOPORT lpInfoPort)
{
	HRESULT hr;
	DMUS_PORTCAPS portinf;
	
	// Set to 0 the DMUS_PORTCAPS structure  
	
	ZeroMemory(&portinf,sizeof(portinf));
	portinf.dwSize = sizeof(DMUS_PORTCAPS);
     
	// Call the DirectMusic8 member function to enumerate systems ports
	
	if (FAILED(hr = m_pMusic8->EnumPort(dwIndex,&portinf))) return hr;
											 

	// Converts port description to char string 
	
	WideCharToMultiByte(CP_ACP,0,portinf.wszDescription,-1,
                        lpInfoPort->szPortDescription,
                        sizeof(lpInfoPort->szPortDescription)/
						sizeof(lpInfoPort->szPortDescription[0]),0,0);


	// Copy the GUID of DMUS_PORTCAP structure to CMidiMusic port structure
   
	CopyMemory(&(lpInfoPort->guidSynthGUID),&portinf.guidPort,sizeof(GUID));
   
	lpInfoPort->dwClass					= portinf.dwClass;
	lpInfoPort->dwEffectFlags			= portinf.dwEffectFlags;
	lpInfoPort->dwFlags	 				= portinf.dwFlags;
	lpInfoPort->dwMaxAudioChannels	    = portinf.dwMaxAudioChannels;
	lpInfoPort->dwMaxChannelGroups		= portinf.dwMaxChannelGroups;
	lpInfoPort->dwMaxVoices				= portinf.dwMaxVoices;
	lpInfoPort->dwType					= portinf.dwType;
	return hr;
}
	

// Function to select and activate a MIDI port given a CMidiPort object

HRESULT CDirectMusic::ActivatePort(LPINFOPORT InfoPort,CMidiPort* pMidiPort)
{
	HRESULT hr;
	DMUS_PORTPARAMS8 dmus;	
	
	// Sets to 0 the port capabilities structure
	
	ZeroMemory(&dmus,sizeof(DMUS_PORTPARAMS));
	
	// Sets the params for this port
	
	dmus.dwSize = sizeof(DMUS_PORTPARAMS);
	dmus.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
	dmus.dwChannelGroups = 1;
	
	
	// The midi port is created here 
	
	if (FAILED(hr = m_pMusic8->CreatePort(InfoPort->guidSynthGUID,&dmus, 
        &pMidiPort->m_pPort,NULL))) return hr;
	
	// We have to activate it
	
	if (FAILED(hr = pMidiPort->m_pPort->Activate(TRUE))) return hr;
	
	// Add the port to the performance
	
	if (FAILED(hr = m_pPerformance->AddPort(pMidiPort->m_pPort))) return hr;
		
	// Assigns a block of 16 performance channels to the performance 
	
	if (FAILED(hr = m_pPerformance->AssignPChannelBlock(0,pMidiPort->m_pPort,1))) return hr;
	
	pMidiPort->m_pPerformance = m_pPerformance;	// Update the IDirectMusicPerformance8 reference

	DMUS_BUFFERDESC BufferDesc;					// Create the DirectMusic buffer to store MIDI messages
	ZeroMemory(&BufferDesc,sizeof(DMUS_BUFFERDESC));
	BufferDesc.dwSize = sizeof(DMUS_BUFFERDESC);
	BufferDesc.guidBufferFormat = GUID_NULL;
	BufferDesc.cbBuffer = 32;					// at least 32 bytes to store messages
	if (FAILED(hr = m_pMusic8->CreateMusicBuffer(&BufferDesc,&pMidiPort->m_pBuffer,NULL))) return hr;
   	
	return S_OK;
}


// The class destructor
// Release all the interfaces
// Closedown the performance
// Uninitialize COM

CDirectMusic::~CDirectMusic()
{
	
	if (m_pPerformance)		
		m_pPerformance->CloseDown();
	
	SAFE_RELEASE(m_pMusic);
	SAFE_RELEASE(m_pMusic8);	
	CoUninitialize();
}	

// Class constructor, member variables initialization

CMidiPort::CMidiPort()
{
	m_pThru			= NULL;
	m_hEvent		= NULL;
	m_pPerformance	= NULL;
	m_pPort			= NULL;
	m_pBuffer		= NULL;
	m_pClock		= NULL;
}	

// This function removes the selected port from the performance
// and releases all their asociated objects for a new port selection

HRESULT CMidiPort::ReleasePort()
{
	if (m_pPerformance)		
	{
		if (m_pPort)
		{
			m_pPort->Activate(FALSE);
			m_pPerformance->RemovePort(m_pPort);
		}
		SAFE_RELEASE(m_pPort);
		SAFE_RELEASE(m_pBuffer);
		SAFE_RELEASE(m_pClock);
		SAFE_RELEASE(m_pThru);
	}
	return S_OK;
}


// Create the Event object
 
HRESULT CMidiPort::ActivateEvent()
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hEvent) return S_OK;	
	else return S_FALSE;
}


// Activates the MIDI thru from an Input MIDI port to a given Output MIDI port

HRESULT CMidiPort::SetThru(CMidiPort *pdstMidiPort)
{
	HRESULT hr = S_FALSE;
	
	SAFE_RELEASE(m_pThru); // Release the old IDirectMusic thru object
	
	if (m_pPort)
	{

		if (FAILED(m_pPort->QueryInterface(IID_IDirectMusicThru8,(void**)&m_pThru)))
			return S_FALSE;
       
		for (DWORD dwChannel = 0;dwChannel < 16;dwChannel++)
		{
			hr = m_pThru->ThruChannel(1,dwChannel,
				1,dwChannel,pdstMidiPort->m_pPort);
			
			if (FAILED(hr))
            break;    

		}
	}
	
	return hr;
}    

// Desactivates the MIDI thru 

HRESULT CMidiPort::BreakThru()
{
	HRESULT hr = S_FALSE;
	
	if (m_pThru)
	{
		for (DWORD dwChannel = 0;dwChannel < 16;dwChannel++)
		{
			hr = m_pThru->ThruChannel(1,dwChannel,
					1,dwChannel,NULL);
			
			if (FAILED(hr))
            break;    

		}
	}
	
	return hr;
}    

// Activates the notification handle and starts the worker thread   	

HRESULT CMidiPort::SetNotification(void( __cdecl *start_address)(void *),LPVOID lpObj)
{
	HRESULT hr = S_FALSE;
#if 0
	if (m_pPort)
	{
		hr = m_pPort->SetReadNotificationHandle(m_hEvent);
		_beginthread(start_address,0,lpObj);
		m_start_address = start_address;
	}
#endif
	return hr;
}


// Ends the notification and terminate the thread

HRESULT CMidiPort::TerminateNotification()
{
	HRESULT hr = S_FALSE;
	if (m_pPort)
	{
		hr = m_pPort->SetReadNotificationHandle(NULL);
		TerminateThread(m_start_address,0);
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	return hr;
}


// Function to fill a buffer with incoming MIDI data from the port

HRESULT CMidiPort::ReadBuffer()
{
	if (m_pPort)
		return m_pPort->Read(m_pBuffer);
	else return S_FALSE;
}


// Sets the read pointer to the start of the data in the buffer

HRESULT CMidiPort::ResetBuffer()
{	
	if (m_pBuffer)
		return m_pBuffer->ResetReadPtr();
	else return S_FALSE;
}

// Returns information about the next message in the buffer and advances the read pointer

HRESULT CMidiPort::GetMidiEvent(REFERENCE_TIME *lprt,DWORD *lpdwGroup,DWORD *lpcb,BYTE **lppb)
{
	if (m_pBuffer)
		return m_pBuffer->GetNextEvent(lprt,lpdwGroup,lpcb,lppb);
	else return S_FALSE;
}

// Returns the latency clock

HRESULT CMidiPort::GetLatencyClock()
{
	if (m_pPort)
		return m_pPort->GetLatencyClock(&m_pClock);
	else return S_FALSE;
}


// Function to send a MIDI message to the selected output port

HRESULT CMidiPort::SendMidiMsg(DWORD dwMsg,DWORD dwChannel)
{
	HRESULT hr = S_FALSE;
	REFERENCE_TIME rt;
	if ((m_pPort) && (m_pBuffer) && (m_pClock))
	{
		if (FAILED(hr = m_pClock->GetTime(&rt))) return hr;
		if (FAILED(hr = m_pBuffer->PackStructured(rt,dwChannel,dwMsg))) return hr; 
		if (FAILED(hr = m_pPort->PlayBuffer(m_pBuffer))) return hr;
		if (FAILED(hr = m_pBuffer->Flush())) return hr;
		hr=S_OK;
	}

	return hr;
	
}


// Function to activate an effect in the port

HRESULT CMidiPort::SetEffect(BOOL bActivate,int nEffect)
{
	IKsControl   *pControl;
    KSPROPERTY   ksp;
    DWORD        dwEffects=0;
    ULONG        cb;
	HRESULT      hr=S_FALSE;

    // Get the IKsControl interface
	if (m_pPort)
	{
		hr = m_pPort->QueryInterface(IID_IKsControl,(void**)&pControl);

		if (SUCCEEDED(hr))
		{
			ZeroMemory(&ksp, sizeof(ksp));

        
			if (!bActivate)
				dwEffects = 0;
			else
			{
				// Select an audio effect
				switch(nEffect)
				{
					case 0x1:
						dwEffects = DMUS_EFFECT_REVERB;
						break;
					case 0x2:
						dwEffects = DMUS_EFFECT_CHORUS;
						break;
					case 0x3:
						dwEffects = DMUS_EFFECT_CHORUS | DMUS_EFFECT_REVERB;
						break;
				}		
			}
		
		ksp.Set = GUID_DMUS_PROP_Effects;
		ksp.Id = 0;
		ksp.Flags = KSPROPERTY_TYPE_SET;
	
                            
		pControl->KsProperty(&ksp,
			                 sizeof(ksp),
                             (LPVOID)&dwEffects,
                             sizeof(dwEffects),
                             &cb);

		SAFE_RELEASE(pControl);
	
		}
	}
	return hr;

}

// Class destructor
// Release all the interfaces
// Call TerminateNotification in case the thread is active 

CMidiPort::~CMidiPort()
{
	
	if (m_hEvent) TerminateNotification();
	BreakThru();
	ReleasePort();
}
