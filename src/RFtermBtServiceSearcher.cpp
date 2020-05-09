/*
 ============================================================================
 Name        : RFtermBtServiceSearcher.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Searching for a service on a remote machine.
 ============================================================================
 */

// INCLUDE FILES
#include <StringLoader.h>
#include <RFterm_0xae7f53fa.rsg>
#include "RFtermBtServiceSearcher.h"
#include "RFtermBtServiceSearcher.pan"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::CRFtermBtServiceSearcher()
// Constructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceSearcher::CRFtermBtServiceSearcher()
	: iIsDeviceSelectorConnected(EFalse)
	{
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::~CRFtermBtServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceSearcher::~CRFtermBtServiceSearcher()
	{
	if (iIsDeviceSelectorConnected)
		{
		iDeviceSelector.CancelNotifier(KDeviceSelectionNotifierUid);
		iDeviceSelector.Close();
		}
	delete iSdpSearchPattern;
	iSdpSearchPattern = NULL;

	delete iAgent;
	iAgent = NULL;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::SelectDeviceByDiscoveryL()
// Select a device.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::SelectDeviceByDiscoveryL(
	TRequestStatus& aObserverRequestStatus)
	{
	if (iIsDeviceSelectorConnected)
		{
		iDeviceSelector.CancelNotifier(KDeviceSelectionNotifierUid);
		}
	else
		{
		User::LeaveIfError(iDeviceSelector.Connect());
		iIsDeviceSelectorConnected = ETrue;
		}

	iSelectionFilter().SetUUID(ServiceClass());

	iDeviceSelector.StartNotifierAndGetResponse(
		aObserverRequestStatus,
		KDeviceSelectionNotifierUid,
		iSelectionFilter,
		iResponse);
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::FindServiceL()
// Find a service on the specified device.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::FindServiceL(TRequestStatus& aObserverRequestStatus)
	{
	if (!iResponse().IsValidBDAddr())
		{
		User::Leave(KErrNotFound);
		}
	iHasFoundService = EFalse;

	// delete any existing agent and search pattern
	delete iSdpSearchPattern;
	iSdpSearchPattern = NULL;

	delete iAgent;
	iAgent = NULL;

	iAgent = CSdpAgent::NewL(*this, BTDevAddr());

	iSdpSearchPattern = CSdpSearchPattern::NewL();

	iSdpSearchPattern->AddL(ServiceClass());
	// return code is the position in the list that the UUID is inserted at
	// and is intentionally ignored

	iAgent->SetRecordFilterL(*iSdpSearchPattern);

	iStatusObserver = &aObserverRequestStatus;

	iAgent->NextRecordRequestL();
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::NextRecordRequestComplete()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::NextRecordRequestComplete(
	TInt aError,
	TSdpServRecordHandle aHandle,
	TInt aTotalRecordsCount)
	{
	TRAPD(error,
		NextRecordRequestCompleteL(aError, aHandle, aTotalRecordsCount));

	if (error != KErrNone)
		{
		Panic(ERFtermBtServiceSearcherNextRecordRequestComplete);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::NextRecordRequestCompleteL()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::NextRecordRequestCompleteL(
	TInt aError,
	TSdpServRecordHandle aHandle,
	TInt aTotalRecordsCount)
	{
	if (aError == KErrEof)
		{
		Finished();
		return;
		}

	if (aError != KErrNone)
		{
		TBuf<6> errorStr;
		errorStr.Num(aError);
		HBufC* errNRRC = StringLoader::LoadLC(R_ERR_NRRC_ERROR);
		HBufC* errFull = HBufC::NewLC(errNRRC->Length() + errorStr.Length());
		NotifyL(*errFull);
		CleanupStack::PopAndDestroy(2); // errNRRC, errFull
		Finished(aError);
		return;
		}

	if (aTotalRecordsCount == 0)
		{
		HBufC* errNRRCNoRecords = StringLoader::LoadLC(R_ERR_NRRC_NO_RECORDS);
		NotifyL(*errNRRCNoRecords);
		CleanupStack::PopAndDestroy(errNRRCNoRecords);
		Finished(KErrNotFound);
		return;
		}

	//  Request its attributes
	iAgent->AttributeRequestL(aHandle, KSdpAttrIdProtocolDescriptorList);
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::AttributeRequestResult()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::AttributeRequestResult(
	TSdpServRecordHandle aHandle,
	TSdpAttributeID aAttrID,
	CSdpAttrValue* aAttrValue)
	{
	TRAPD(error, AttributeRequestResultL(aHandle, aAttrID, aAttrValue));
	if (error != KErrNone)
		{
		Panic(ERFtermBtServiceSearcherAttributeRequestResult);
		}
	// Delete obsolete local atribute pointer.
	delete aAttrValue;
	aAttrValue = NULL;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::AttributeRequestResultL()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::AttributeRequestResultL(
	TSdpServRecordHandle /*aHandle*/,
	TSdpAttributeID aAttrID,
	CSdpAttrValue* aAttrValue)
	{
	__ASSERT_ALWAYS(aAttrID == KSdpAttrIdProtocolDescriptorList,
					User::Leave(KErrNotFound));
	TRFtermSdpAttributeParser parser(ProtocolList(), *this);

	// Validate the attribute value, and extract the RFCOMM channel
	aAttrValue->AcceptVisitorL(parser);

	if (parser.HasFinished())
		{
		// Found a suitable record so change state
		iHasFoundService = ETrue;
		}
	}

// -----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::AttributeRequestComplete()
// Process the attribute request completion.
// -----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::AttributeRequestComplete(TSdpServRecordHandle aHandle,
	TInt aError)
	{
	TRAPD(error, AttributeRequestCompleteL(aHandle, aError));
	if (error != KErrNone)
		{
		Panic(ERFtermBtServiceSearcherAttributeRequestComplete);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::AttributeRequestCompleteL()
// Process the attribute request completion.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::AttributeRequestCompleteL(
	TSdpServRecordHandle /*aHandle*/,
	TInt aError)
	{
	if (aError != KErrNone)
		{
		HBufC* errCantGetAttribute = StringLoader::LoadLC(R_ERR_CANT_GET_ATTRIBUTE);
		TBuf<6> errorStr;
		errorStr.Num(aError);
		HBufC* errFull = HBufC::NewLC(errCantGetAttribute->Length() + errorStr.Length());
		NotifyL(*errFull);
		CleanupStack::PopAndDestroy(2); // errCantGetAttribute, errFull
		}
	else if (!HasFinishedSearching())
		{
		// have not found a suitable record so request another
		iAgent->NextRecordRequestL();
		}
	else
		{
		HBufC* errAttrReqCom = StringLoader::LoadLC(R_ERR_ATTR_REQ_COM);
		NotifyL(*errAttrReqCom);
		CleanupStack::PopAndDestroy(errAttrReqCom);
		Finished();
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::Finished()
// The search has finished and notify the observer
// that the process is complete.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::Finished(TInt aError /* default = KErrNone */)
	{
	if (aError == KErrNone && !HasFoundService())
		{
		aError = KErrNotFound;
		}
	User::RequestComplete(iStatusObserver, aError);
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::HasFinishedSearching()
// Is the instance still wanting to search.
// ----------------------------------------------------------------------------
//
TBool CRFtermBtServiceSearcher::HasFinishedSearching() const
	{
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::BTDevAddr()
// Returns the bluetooth device address.
// ----------------------------------------------------------------------------
//
const TBTDevAddr& CRFtermBtServiceSearcher::BTDevAddr()
	{
	return iResponse().BDAddr();
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::ResponseParams()
// Returns information about the device selected by the user.
// ----------------------------------------------------------------------------
//
const TBTDeviceResponseParams& CRFtermBtServiceSearcher::ResponseParams()
	{
	return iResponse();
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::HasFoundService()
// True if a service has been found.
// ----------------------------------------------------------------------------
//
TBool CRFtermBtServiceSearcher::HasFoundService() const
	{
	return iHasFoundService;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::SetObserver()
// Connect an observer for notifications.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::SetObserver(MRFtermBtObserver* aObserver)
	{
	iObserver = aObserver;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::NotifyL()
// Send to observer a log message.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::NotifyL(const TDesC& aMessage)
	{
	if (iObserver)
		{
		iObserver->HandleBtNotifyL(aMessage);
		}
	}

// End of File
