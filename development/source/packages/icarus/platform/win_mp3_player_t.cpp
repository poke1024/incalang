// ===========================================================================
//	win_mp3_player_t.cpp	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#if 0

#include "win_mp3_player_t.h"
#include "file_stream_t.h"

// Force the inclusion of the libraries we need
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "amstrmid.lib")

BEGIN_ICARUS_NAMESPACE

win_mp3_player_t::win_mp3_player_t()
{
	m_enabled = true;
	m_supported = true;

	g_pGraphBuilder = NULL;
	g_pMediaControl = NULL;
	g_pMediaSeeking = NULL;
	g_pMediaEventEx = NULL;

	HWND					hWnd;

	hWnd = GetForegroundWindow();
	if( hWnd == 0 )
		hWnd = GetDesktopWindow();

	if( init_mp3( hWnd ) == FALSE )
		m_supported = false;
}

win_mp3_player_t::~win_mp3_player_t()
{
	exit_mp3();
}

void
win_mp3_player_t::start(
	const char*				path )
{
	if( not m_supported )
		return;

	static wchar_t			wpath[ MAX_PATH ];
	char*					s;
	
	s = strstr( path, "mp3/" );
	if( s == 0 )
		return;

	int						j = 0;
	while( *s != '\0' )
	{
		wpath[ j++ ] = *s;
		s++;
	}
	wpath[ j++ ] = '\0';

	IBaseFilter*			filter;

	filter = load_mp3( wpath );
	if( filter )
		play_mp3( filter );
}

void
win_mp3_player_t::stop()
{
	if( m_supported )
		stop_mp3();
}

void
win_mp3_player_t::set_volume(
	u16						volume )
{	
}

// ===========================================================================

BOOL
win_mp3_player_t::init_mp3( HWND hWndMain )
{
	HRESULT h;

	// Initialise COM, so that we can use CoCreateInstance.
	::CoInitialize(NULL);
	
	// Create an IGraphBuilder object, through which 
	//  we will create a DirectShow graph.
	h = CoCreateInstance(CLSID_FilterGraph, NULL,
                                      CLSCTX_INPROC, IID_IGraphBuilder,
                                      (void **)&g_pGraphBuilder);
	if (FAILED(h))
		return FALSE;

    // Get the IMediaControl Interface
    h = g_pGraphBuilder->QueryInterface(IID_IMediaControl,
                                 (void **)&g_pMediaControl);
	if (FAILED(h))
		return FALSE;

    // Get the IMediaSeeking Interface
    h = g_pGraphBuilder->QueryInterface(IID_IMediaSeeking,
                                 (void **)&g_pMediaSeeking);
	if (FAILED(h))
		return FALSE;

    // Get the IMediaEventEx Interface
    h = g_pGraphBuilder->QueryInterface(IID_IMediaEventEx,
                                 (void **)&g_pMediaEventEx);
	if (FAILED(h))
		return FALSE;
    g_pMediaEventEx->SetNotifyWindow((OAHWND)hWndMain, WM_APP, 0);

	return TRUE;
}

IBaseFilter*
win_mp3_player_t::load_mp3(LPCWSTR wszFilename)
{
	IBaseFilter *pSource;
	HRESULT		h;

    // Add the new source filter to the graph.
    h = g_pGraphBuilder->AddSourceFilter(wszFilename, wszFilename, &pSource);
	if (FAILED(h))
		return NULL;

	return pSource;
}

void
win_mp3_player_t::play_mp3(IBaseFilter *pSource)
{
    IPin			*pPin = NULL;
    IEnumFilters	*pFilterEnum = NULL;
	IBaseFilter		**ppFilters;
    IBaseFilter		*pFilterTemp = NULL;
	int				iFiltCount;
    int				iPos;

    // Get the first output pin of the new source filter. Audio sources 
    // typically have only one output pin, so for most audio cases finding 
    // any output pin is sufficient.
	pSource->FindPin(L"Output", &pPin);  

    // Stop the graph
	g_pMediaControl->Stop();

    // Break all connections on the filters. You can do this by adding 
    // and removing each filter in the graph

    g_pGraphBuilder->EnumFilters(&pFilterEnum);
    
    // Need to know how many filters. If we add/remove filters during the
    // enumeration we'll invalidate the enumerator
	for (iFiltCount = 0; pFilterEnum->Skip(1) == S_OK; iFiltCount++)
		;

    // Allocate space, then pull out all of the 
    ppFilters = new IBaseFilter*[ iFiltCount ];

    pFilterEnum->Reset();

    for (iPos = 0; pFilterEnum->Next(1, &ppFilters[iPos], NULL) == S_OK; iPos++)
		;
    
	pFilterEnum->Release();

    for (iPos = 0; iPos < iFiltCount; iPos++) 
	{
        g_pGraphBuilder->RemoveFilter(ppFilters[iPos]);
		
		// Put the filter back
		g_pGraphBuilder->AddFilter(ppFilters[iPos], NULL);

		ppFilters[iPos]->Release();
    }

	delete[] ppFilters;

    // We have the new output pin. Render it
	g_pGraphBuilder->Render(pPin);

    pPin->Release();

    // Re-seek the graph to the beginning
    LONGLONG llPos = 0;
    g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
								&llPos, AM_SEEKING_NoPositioning);

    // Start the graph
	g_pMediaControl->Run();
}

void
win_mp3_player_t::replay_mp3()
{
	if (g_pMediaControl != NULL)
	{
		// Re-seek the graph to the beginning
		LONGLONG llPos = 0;
		g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
									&llPos, AM_SEEKING_NoPositioning);

		// Start the graph
		g_pMediaControl->Run();
	}
}

void
win_mp3_player_t::stop_mp3()
{
    // Stop the graph
	if (g_pMediaControl != NULL)
		g_pMediaControl->Stop();
}

void
win_mp3_player_t::exit_mp3()
{
	if (g_pMediaEventEx != NULL)
	{
		g_pMediaEventEx->Release();
		g_pMediaEventEx = NULL;
	}
	if (g_pMediaSeeking != NULL)
	{
		g_pMediaSeeking->Release();
		g_pMediaSeeking = NULL;
	}
	if (g_pMediaControl != NULL)
	{
		g_pMediaControl->Release();
		g_pMediaControl = NULL;
	}
	if (g_pGraphBuilder != NULL)
	{
		g_pGraphBuilder->Release();
		g_pGraphBuilder = NULL;
	}
}

void
win_mp3_player_t::handle_mp3_events()
{
    long	evCode, param1, param2;
    HRESULT	h;

	for (;;)
    { 
		h = g_pMediaEventEx->GetEvent(&evCode, &param1, &param2, 0);
		if (FAILED(h))
			return;
        
		g_pMediaEventEx->FreeEventParams(evCode, param1, param2);
		on_mp3_finish();
    } 
}

void
win_mp3_player_t::on_mp3_finish()
{
	if( m_enabled )
		replay_mp3();
}

END_ICARUS_NAMESPACE

#endif

