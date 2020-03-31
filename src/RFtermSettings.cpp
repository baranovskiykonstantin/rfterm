/*
 ============================================================================
 Name        : RFtermSettings.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application settings.
 ============================================================================
 */

#include "RFtermSettings.h"
#include "RFtermConstants.h"

CRFtermSettings *CRFtermSettings::NewL()
	{
	CRFtermSettings *self = CRFtermSettings::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CRFtermSettings *CRFtermSettings::NewLC()
	{
	CRFtermSettings *self = new (ELeave) CRFtermSettings();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRFtermSettings::~CRFtermSettings()
	{
	}

CRFtermSettings::CRFtermSettings()
	{
	}

void CRFtermSettings::SetDefaultValues()
	{
	iMessageAddendum = KCRLF;
	iMessageHistorySize = 8;
	iCtrlCharMapping = EMapCRtoCRLF;
	iTabSize = 4;
	}

void CRFtermSettings::ConstructL()
	{
	}

// Reading setting data from stream void
void CRFtermSettings::LoadL(RReadStream& aStream)
	{
	aStream >> iMessageAddendum;
	iMessageHistorySize = aStream.ReadInt32L();
	iCtrlCharMapping = (TCtrlCharMapping) aStream.ReadInt32L();
	iTabSize = aStream.ReadInt32L();
	}

// Storing setting data into stream void
void CRFtermSettings::SaveL(RWriteStream& aStream) const
	{
	aStream << iMessageAddendum;
	aStream.WriteInt32L(iMessageHistorySize);
	aStream.WriteInt32L((TInt)iCtrlCharMapping);
	aStream.WriteInt32L(iTabSize);
	}

