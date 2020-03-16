/*
 ============================================================================
 Name        : RFtermServiceAdvertiser.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Advertises a service in the SDP database.
 ============================================================================
 */

// INCLUDE FILES
#include <bt_sock.h>
#include <StringLoader.h>
#include <RFterm_0xae7f53fa.rsg>

#include "RFtermServiceAdvertiser.h"
#include "RFtermConstants.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CRFtermServiceAdvertiser::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermServiceAdvertiser* CRFtermServiceAdvertiser::NewL()
	{
	CRFtermServiceAdvertiser* self = CRFtermServiceAdvertiser::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermServiceAdvertiser::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermServiceAdvertiser* CRFtermServiceAdvertiser::NewLC()
	{
	CRFtermServiceAdvertiser* self = new (ELeave) CRFtermServiceAdvertiser();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermServiceAdvertiser::CRFtermServiceAdvertiser()
// Constructor.
// ----------------------------------------------------------------------------
//
CRFtermServiceAdvertiser::CRFtermServiceAdvertiser()
	{
	// No implementation required
	}

// ----------------------------------------------------------------------------
// CRFtermServiceAdvertiser::~CRFtermServiceAdvertiser()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermServiceAdvertiser::~CRFtermServiceAdvertiser()
	{
	delete iServiceName;
	delete iServiceDescription;
	}

// ----------------------------------------------------------------------------
// CRFtermServiceAdvertiser::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CRFtermServiceAdvertiser::ConstructL()
	{
	iServiceName = StringLoader::LoadL (R_RFTERM_SERVICE_NAME);
	iServiceDescription = StringLoader::LoadL (R_RFTERM_SERVICE_DESCRIPTION);
	}

// -----------------------------------------------------------------------------
// CMessageServiceAdvertiser::BuildProtocolDescriptionL()
// Builds the protocol description.
// -----------------------------------------------------------------------------
//
void CRFtermServiceAdvertiser
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
// CRFtermServiceAdvertiser::ServiceName()
// ----------------------------------------------------------------------------
//
const TDesC& CRFtermServiceAdvertiser::ServiceName()
	{
	return *iServiceName;
	}

// ----------------------------------------------------------------------------
// CRFtermServiceAdvertiser::ServiceDescription()
// ----------------------------------------------------------------------------
//
const TDesC& CRFtermServiceAdvertiser::ServiceDescription()
	{
	return *iServiceDescription;
	}

// ----------------------------------------------------------------------------
// CRFtermServiceAdvertiser::ServiceClass()
// ----------------------------------------------------------------------------
//
TInt CRFtermServiceAdvertiser::ServiceClass()
	{
	return KServiceClass;
	}

// End of File
