#ifndef __NAVIGATION_UTILS__
#define __NAVIGATION_UTILS__

#ifdef __cplusplus
extern "C" {
#endif

//Callbacks
typedef void (*NavHandleEvent_Proc)(EventRecord* event);
typedef Boolean (*NavFilter_Proc)(FSSpec* file);

//Constants

#define		kNavMaxTypes			8

#define		kFakeEventCallBack		((NavHandleEvent_Proc) (-1))

//Structures

typedef struct {
	//Common
	Str63					applicationName;
	OSType					applicationCreator;
	NavHandleEvent_Proc		eventCallback;
	FSSpec					outFile;
	
	//Get file only
	Boolean					displayPreview;
	short					numTypes;
	OSType					typeList[kNavMaxTypes];
	NavFilter_Proc			filterCallback;
	OSType					outFileType;
	
	//Put file only
	OSType					inFileType;
	Boolean					replacing;
} NavigationParams;
typedef NavigationParams* NavigationParamsPtr;

//Prototypes

OSErr NavigationGetFile(NavigationParamsPtr params, Str255 message);
OSErr NavigationPutFile(NavigationParamsPtr params, Str255 message, Str255 defaultName);

#ifdef __cplusplus
}
#endif

#endif //__NAVIGATION_UTILS__
