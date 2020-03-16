/*
 ============================================================================
 Name        : RFtermBtServiceAdvertiser.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Advertises a service in the SDP database.
 ============================================================================
 */

// INCLUDE FILES
#include <bt_sock.h>
#include "RFtermBtServiceAdvertiser.h"
#include "RFterm.pan"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::CRFtermBtServiceAdvertiser()
// Constructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceAdvertiser::CRFtermBtServiceAdvertiser() :
	iRecord(0), 
	iIsConnected(EFalse)
	{
	// no implementation required
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::~CRFtermBtServiceAdvertiser()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceAdvertiser::~CRFtermBtServiceAdvertiser()
	{
	if (IsAdvertising())
		{
		TRAPD(err, StopAdvertisingL());
		if (err != KErrNone)
			{
			User::Panic(KPanicBTServiceAdvertiser, err);
			}
		}

	iSdpDatabase.Close();
	iSdpSession.Close();
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::ConnectL()
// Connect to the SDP database.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceAdvertiser::ConnectL()
	{
	if (!iIsConnected)
		{
		User::LeaveIfError(iSdpSession.Connect());
		User::LeaveIfError(iSdpDatabase.Open(iSdpSession));
		iIsConnected = ETrue;
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::StartAdvertisingL()
// Start the advertising of this service.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceAdvertiser::StartAdvertisingL(TInt aPort)
	{
	if (IsAdvertising())
		{
		// could be advertising on a different port
		StopAdvertisingL(); 
		}
	if (! iIsConnected)
		{
		ConnectL();
		}
	iSdpDatabase.CreateServiceRecordL(ServiceClass(), iRecord);

	// add a Protocol to the record
	CSdpAttrValueDES* vProtocolDescriptor = CSdpAttrValueDES::NewDESL(NULL);
	CleanupStack::PushL(vProtocolDescriptor);

	BuildProtocolDescriptionL(vProtocolDescriptor,aPort);

	iSdpDatabase.UpdateAttributeL(iRecord, KSdpAttrIdProtocolDescriptorList, 
		*vProtocolDescriptor);

	CleanupStack::PopAndDestroy(vProtocolDescriptor);

	// Add a name to the record
	iSdpDatabase.UpdateAttributeL(iRecord, 
		KSdpAttrIdBasePrimaryLanguage + 
		KSdpAttrIdOffsetServiceName, 
		ServiceName());

	// Add a description to the record
	iSdpDatabase.UpdateAttributeL(iRecord, 
		KSdpAttrIdBasePrimaryLanguage + 
		KSdpAttrIdOffsetServiceDescription, 
		ServiceDescription());

	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::UpdateAvailabilityL()
// Update the service availability field of the service record.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceAdvertiser::UpdateAvailabilityL(TBool aIsAvailable)
	{
	TUint state;
	if (aIsAvailable)
		{
		state = 0xFF; // Fully unused
		}
	else
		{
		state = 0x00; // Fully used -> can't connect
		}

	//  Update the availibility attribute field
	iSdpDatabase.UpdateAttributeL(iRecord, 
		KSdpAttrIdServiceAvailability, 
		state);

	//  Mark the record as changed - by increasing its state number (version)
	iSdpDatabase.UpdateAttributeL(iRecord, 
		KSdpAttrIdServiceRecordState, 
		++iRecordState);
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::StopAdvertisingL()
// Stop advertising this service. Remove the record from the sdp database.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceAdvertiser::StopAdvertisingL()
	{
	if (IsAdvertising())
		{
		iSdpDatabase.DeleteRecordL(iRecord);
		iRecord = 0;
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::IsAdvertising()
// Does the SDP database contain a record for this service.
// ----------------------------------------------------------------------------
//
TBool CRFtermBtServiceAdvertiser::IsAdvertising()
	{
	return iRecord != 0;
	}

// End of File
