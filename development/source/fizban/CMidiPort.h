/*
Module : CMidiPort.h
Purpose: Defines the header implementation for the CMidiPort and CDirectMusic classes
Created: CJP / 08-02-2002
History: CJP / 14-07-2002 
	
	1. Fixed bug when m_pPort of CMidiPort is NULL
	
	2. Improved CMidiPort destructor

	3. Check member variables which values are NULL before performing an operation
	
Copyright (c) 2002 by C. Jiménez de Parga  
All rights reserved.
*/

#ifndef CMIDIPORT_H
#define CMIDIPORT_H

// Some required DX headers
#include <dmusicc.h>
#include <dmusici.h>
#include <dmksctrl.h>
#include <process.h>

// Macro definition for COM object releases
#define SAFE_RELEASE(p)      {if(p) {(p)->Release(); (p)=NULL;}}
#define SET_REVERB 0x1
#define SET_CHORUS 0x2

// Infoport structure
typedef struct INFOPORT {
   char szPortDescription[DMUS_MAX_DESCRIPTION*2];
   DWORD dwFlags;
   DWORD dwClass;
   DWORD dwType;
   DWORD dwMaxAudioChannels;
   DWORD dwMaxVoices;
   DWORD dwMaxChannelGroups ;
   DWORD dwEffectFlags;
   GUID guidSynthGUID;
} *LPINFOPORT;



// CMidiPort class definition

class CMidiPort
{
protected:
	IDirectMusicThru8*        m_pThru;								// Protected MIDI port COM interfaces 
	IReferenceClock*		  m_pClock;
	HANDLE					  m_hEvent;
	void					  ( __cdecl *m_start_address)(void *);	// Address of worker thread
public:
	IDirectMusicPerformance8*  m_pPerformance;						// Interfaces accessible by the CDirectMusic class
	IDirectMusicPort8*		   m_pPort;
	IDirectMusicBuffer8*	   m_pBuffer;
public:
    CMidiPort();										// Constructor and the destructor of the class
    ~CMidiPort();
   	inline HANDLE *GetEventHandler() { return &m_hEvent; }	// Public member functions exposed by the class
	HRESULT ActivateEvent();
	HRESULT ReleasePort();
	HRESULT SetThru(CMidiPort *dstInfoPort);
	HRESULT BreakThru();
	HRESULT SetNotification(void(__cdecl *start_address)(void *),LPVOID lpObj);
	HRESULT TerminateNotification();
	HRESULT ReadBuffer();
	HRESULT ResetBuffer();
	HRESULT GetMidiEvent(REFERENCE_TIME *lprt,DWORD *lpdwGroup,DWORD *lpcb,BYTE **lppb);
	HRESULT SetEffect(BOOL bActivate,int nEffect);
	HRESULT GetLatencyClock();
	HRESULT SendMidiMsg(DWORD dwMsg,DWORD dwChannel);
	inline BOOL	IsPortNULL() { return (m_pPort == NULL); }
};
													// End of class definition			

// CMidiMusic class definition

class CDirectMusic
{
protected:
	IDirectMusic*			   m_pMusic;			//Main DirectMusic COM interfaces
	IDirectMusic8*			   m_pMusic8;
	IDirectMusicPerformance8*  m_pPerformance;
public:
	CDirectMusic();									// Constructor and the destructor of the class
	~CDirectMusic();

	HRESULT Initialize(HWND hWnd=NULL);				// Public member functions
	HRESULT PortEnumeration(DWORD dwIndex,LPINFOPORT lpInfoPort);
	HRESULT ActivatePort(LPINFOPORT InfoPort,CMidiPort *pMidiPort);
	
};


#endif												// End of class definition