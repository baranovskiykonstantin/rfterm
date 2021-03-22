/*
 ============================================================================
 Name        : RFtermBtServiceSearcher.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Searching for a service on a remote machine.
 ============================================================================
 */

#include <bt_sock.h>
#include <StringLoader.h>
#include <RFterm_0xae7f53fa.rsg>
#include "RFtermBtServiceSearcher.h"
#include "RFtermBtServiceSearcher.pan"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceSearcher* CRFtermBtServiceSearcher::NewL()
	{
	CRFtermBtServiceSearcher* self = CRFtermBtServiceSearcher::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceSearcher* CRFtermBtServiceSearcher::NewLC()
	{
	CRFtermBtServiceSearcher* self = new (ELeave) CRFtermBtServiceSearcher();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::CRFtermBtServiceSearcher()
// Constructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceSearcher::CRFtermBtServiceSearcher()
	: iIsDeviceSelectorConnected(EFalse),
	iServiceClass(KServiceClass),
	iPort(-1)
	{
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::~CRFtermBtServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceSearcher::~CRFtermBtServiceSearcher()
	{
	iProtocolArray.Close();

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
// CRFtermBtServiceSearcher::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::ConstructL()
	{
	// no implementation required
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
	// Canceled by BT client. Quit.
	if (*iStatusObserver != KRequestPending)
		return;

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
		errFull->Des().Copy(*errNRRC);
		errFull->Des().Append(errorStr);
		NotifyL(*errFull);
		CleanupStack::PopAndDestroy(2); // errNRRC, errFull
		Finished(aError);
		return;
		}

	if (aTotalRecordsCount == 0)
		{
		NotifyL(R_ERR_NRRC_NO_RECORDS);
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
	// Canceled by BT client. Quit.
	if (*iStatusObserver != KRequestPending)
		return;

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
	// Canceled by BT client. Quit.
	if (*iStatusObserver != KRequestPending)
		return;

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
		errFull->Des().Copy(*errCantGetAttribute);
		errFull->Des().Append(errorStr);
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
		NotifyL(R_ERR_ATTR_REQ_COM);
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
	// if != KRequestPending -> canceled by BT client
	if (*iStatusObserver == KRequestPending)
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
// CRFtermBtServiceSearcher::ServiceClass()
// The service class to search.
// ----------------------------------------------------------------------------
//
const TUUID& CRFtermBtServiceSearcher::ServiceClass() const
	{
	return iServiceClass;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::ProtocolList()
// The list of Protocols required by the service.
// ----------------------------------------------------------------------------
//
 RArray <TRFtermSdpAttributeParser::TRFtermSdpAttributeNode>& CRFtermBtServiceSearcher
::ProtocolList()
	{
	TRFtermSdpAttributeParser::TRFtermSdpAttributeNode attrib;
	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckType);
	attrib.SetType(ETypeDES);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckType);
	attrib.SetType(ETypeDES);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckValue);
	attrib.SetType(ETypeUUID);
	attrib.SetValue(KL2CAP);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckEnd);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckType);
	attrib.SetType(ETypeDES);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckValue);
	attrib.SetType(ETypeUUID);
	attrib.SetValue(KRFCOMM);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::EReadValue);
	attrib.SetType(ETypeUint);
	attrib.SetValue(KRfcommChannel);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckEnd);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckEnd);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::EFinished);
	iProtocolArray.Append(attrib);

	return iProtocolArray;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::FoundElementL()
// Read the data element.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceSearcher::FoundElementL(TInt aKey, CSdpAttrValue& aValue)
	{
	__ASSERT_ALWAYS(aKey == static_cast<TInt>(KRfcommChannel),
		Panic(ERFtermBtServiceSearcherProtocolRead));
	iPort = aValue.Uint();
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceSearcher::Port()
// Port connection on the remote machine.
// ----------------------------------------------------------------------------
//
TInt CRFtermBtServiceSearcher::Port()
	{
	return iPort;
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
void CRFtermBtServiceSearcher::NotifyL(TInt aResId)
	{
	HBufC* message = StringLoader::LoadLC(aResId);
	NotifyL(*message);
	CleanupStack::PopAndDestroy(message);
	}

// End of File
