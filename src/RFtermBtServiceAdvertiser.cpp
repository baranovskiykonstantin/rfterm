/*
 ============================================================================
 Name        : RFtermBtServiceAdvertiser.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Advertises a service in the SDP database.
 ============================================================================
 */

#include <bt_sock.h>
#include <StringLoader.h>
#include <RFterm_0xae7f53fa.rsg>
#include "RFtermBtServiceAdvertiser.h"
#include "RFterm.pan"
#include "RFtermConstants.h"

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceAdvertiser* CRFtermBtServiceAdvertiser::NewL()
	{
	CRFtermBtServiceAdvertiser* self = CRFtermBtServiceAdvertiser::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermBtServiceAdvertiser* CRFtermBtServiceAdvertiser::NewLC()
	{
	CRFtermBtServiceAdvertiser* self = new (ELeave) CRFtermBtServiceAdvertiser();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

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

	delete iServiceName;
	delete iServiceDescription;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CRFtermBtServiceAdvertiser::ConstructL()
	{
	iServiceName = StringLoader::LoadL(R_RFTERM_SERVICE_NAME);
	iServiceDescription = StringLoader::LoadL(R_RFTERM_SERVICE_DESCRIPTION);
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

// -----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::BuildProtocolDescriptionL()
// Builds the protocol description.
// -----------------------------------------------------------------------------
//
void CRFtermBtServiceAdvertiser
::BuildProtocolDescriptionL(CSdpAttrValueDES* aProtocolDescriptor, TInt aPort)
	{
	TBuf8<1> channel;
	channel.Append((TChar)aPort);

	aProtocolDescriptor
		->StartListL()
			->BuildDESL()
			->StartListL() // Details of lowest level protocol
				->BuildUUIDL(KL2CAP)
			->EndListL()

			->BuildDESL()
			->StartListL()
				->BuildUUIDL(KRFCOMM)
				->BuildUintL(channel)
			->EndListL()
		->EndListL();
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::ServiceName()
// ----------------------------------------------------------------------------
//
const TDesC& CRFtermBtServiceAdvertiser::ServiceName()
	{
	return *iServiceName;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::ServiceDescription()
// ----------------------------------------------------------------------------
//
const TDesC& CRFtermBtServiceAdvertiser::ServiceDescription()
	{
	return *iServiceDescription;
	}

// ----------------------------------------------------------------------------
// CRFtermBtServiceAdvertiser::ServiceClass()
// ----------------------------------------------------------------------------
//
TInt CRFtermBtServiceAdvertiser::ServiceClass()
	{
	return KServiceClass;
	}

// End of File
