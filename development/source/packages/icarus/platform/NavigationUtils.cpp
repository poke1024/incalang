#include		<Navigation.h>
#include		<AEDataModel.h>

#include		"NavigationUtils.h"

//ROUTINES:

#if !TARGET_CARBON
static void AEGetDescData(const AEDesc* theAEDesc, void* dataPtr, Size maximumSize)
{
	BlockMove(*theAEDesc->dataHandle, dataPtr, maximumSize);
}
#endif

static pascal void NavEventProc(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, NavCallBackUserData callBackUD)
{
	switch(callBackSelector) {
		
		case kNavCBEvent:
		if((((NavigationParamsPtr) callBackUD)->eventCallback != NULL) && (((NavigationParamsPtr) callBackUD)->eventCallback != kFakeEventCallBack))
		((NavigationParamsPtr) callBackUD)->eventCallback(callBackParms->eventData.eventDataParms.event);
		break;
		
	}
}

static pascal Boolean NavFilterProc(AEDesc* theItem, void* info, NavCallBackUserData callBackUD, NavFilterModes filterMode)
{
	filterMode;

	FSSpec					finalFSSpec;	
	NavFileOrFolderInfo*	objectInfo = (NavFileOrFolderInfo*) info;
	
	//Make sure we have a FSSpec
	if(theItem->descriptorType != typeFSS)
	return true;
	
	//Check for folder
	if((objectInfo != NULL) && (objectInfo->isFolder))
	return true;
	
	//Check for file
	AEGetDescData(theItem, &finalFSSpec, sizeof(FSSpec));
	if(((NavigationParamsPtr) callBackUD)->filterCallback != NULL)
	return ((NavigationParamsPtr) callBackUD)->filterCallback(&finalFSSpec);
	else
	return false;
		
	return true;
}

static Handle NewOpenHandle(OSType applicationSignature, short numTypes, OSType typeList[])
{
	Handle hdl = NULL;
	
	if(numTypes > 0) {
		hdl = NewHandle(sizeof(NavTypeList) + numTypes * sizeof(OSType));
		if (hdl != NULL) {
			NavTypeListHandle open		= (NavTypeListHandle)hdl;
			
			(*open)->componentSignature = applicationSignature;
			(*open)->osTypeCount		= numTypes;
			BlockMoveData(typeList, (*open)->osType, numTypes * sizeof(OSType));
		}
	}
	
	return hdl;
}

OSErr NavigationGetFile(NavigationParamsPtr params, Str255 message)
{
	NavReplyRecord		theReply;
	NavDialogOptions	dialogOptions;
	OSErr				theErr = noErr;
	NavTypeListHandle	openList = NULL;
	long				count = 0;
	NavEventUPP			eventUPP = NULL;
	NavObjectFilterUPP	filterUPP = NULL;
	CGrafPtr			savePort = NULL;
	GDHandle			saveDevice = NULL;
	
	GetGWorld(&savePort, &saveDevice);
	
	theErr = NavGetDefaultDialogOptions(&dialogOptions);
	if(theErr)
	return theErr;
	dialogOptions.dialogOptionFlags -= kNavDontAddTranslateItems;
	dialogOptions.dialogOptionFlags += kNavNoTypePopup;
	if(!params->displayPreview)
	dialogOptions.dialogOptionFlags -= kNavAllowPreviews;
	//dialogOptions.preferenceKey = kSavePrefKey;
	BlockMove(params->applicationName, dialogOptions.clientName, sizeof(Str63));
	
	if(message != nil)
	BlockMove(message, dialogOptions.message, sizeof(Str255));
	else
	dialogOptions.message[0] = 0;
	
	if(params->eventCallback != NULL)
	eventUPP = NewNavEventUPP(NavEventProc);
	
	if(params->filterCallback != NULL)
	filterUPP = NewNavObjectFilterUPP(NavFilterProc);
	else {
		openList = (NavTypeListHandle) NewOpenHandle(params->applicationCreator, params->numTypes, params->typeList);
		if(openList)
		HLock((Handle) openList);
	}
	
	theErr = NavGetFile(NULL, &theReply, &dialogOptions, eventUPP, NULL, filterUPP, (NavTypeListHandle) openList, (NavCallBackUserData) params);
	if(theErr)
	goto Bail;
	if(theReply.validRecord) {
		FSSpec	finalFSSpec;	
		AEDesc 	resultDesc;
		FInfo	fileInfo;

		if((theErr = AEGetNthDesc(&(theReply.selection), 1, typeFSS, NULL, &resultDesc)) == noErr) {
			AEGetDescData(&resultDesc, &finalFSSpec, sizeof(FSSpec));
			
			FSpGetFInfo(&finalFSSpec, &fileInfo);
			params->outFileType = fileInfo.fdType;
			BlockMove(&finalFSSpec, &params->outFile, sizeof(FSSpec));
			
			AEDisposeDesc(&resultDesc);
		}
	}
	
	NavDisposeReply(&theReply);
Bail:
	if(openList)
	DisposeHandle((Handle) openList);
	if(eventUPP)
	DisposeNavEventUPP(eventUPP);
	if(filterUPP)
	DisposeNavObjectFilterUPP(filterUPP);
	
	SetGWorld(savePort, saveDevice);
	
	return theErr;
}

OSErr NavigationPutFile(NavigationParamsPtr params, Str255 message, Str255 defaultName)
{
	OSErr				theErr = noErr;
	NavReplyRecord		theReply;
	NavDialogOptions	dialogOptions;
	NavEventUPP			eventUPP = NULL;
	CGrafPtr			savePort = NULL;
	GDHandle			saveDevice = NULL;
	
	GetGWorld(&savePort, &saveDevice);
	
	theErr = NavGetDefaultDialogOptions(&dialogOptions);
	if(theErr)
	return theErr;
	dialogOptions.dialogOptionFlags -= kNavDontAddTranslateItems;
	dialogOptions.dialogOptionFlags += kNavNoTypePopup;
	//dialogOptions.preferenceKey = kSavePrefKey;
	BlockMove(params->applicationName, dialogOptions.clientName, sizeof(Str63));
	BlockMove(defaultName, dialogOptions.savedFileName, sizeof(Str255));
	
	if(message != nil)
	BlockMove(message, dialogOptions.message, sizeof(Str255));
	else
	dialogOptions.message[0] = 0;
	
	if(params->eventCallback != NULL)
	eventUPP = NewNavEventUPP(NavEventProc);
	
	theErr = NavPutFile(NULL, &theReply, &dialogOptions, eventUPP, params->inFileType, params->applicationCreator, (NavCallBackUserData) params);
	if(theErr)
	goto Bail;
	if(theReply.validRecord) {
		FSSpec	finalFSSpec;	
		AEDesc 	resultDesc;
		
		if((theErr = AEGetNthDesc(&(theReply.selection), 1, typeFSS, NULL, &resultDesc)) == noErr) {
			AEGetDescData(&resultDesc, &finalFSSpec, sizeof(FSSpec));
			
			params->replacing = theReply.replacing;
			BlockMove(&finalFSSpec, &params->outFile, sizeof(FSSpec));
			
			AEDisposeDesc(&resultDesc);
		}
	}
	
	NavDisposeReply(&theReply);
Bail:

	SetGWorld(savePort, saveDevice);
	
	return theErr;
}
