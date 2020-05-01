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
	iEcho = ETrue;
	iFontSize = 120;
	iTabSize = 4;
	iCtrlCharMapping = EMapCRtoCRLF;
	iCodePage = ECodePageLatin1;
	}

void CRFtermSettings::Normalize()
	{
	if (iMessageAddendum != KCR &&
			iMessageAddendum != KLF &&
			iMessageAddendum != KCRLF &&
			iMessageAddendum != KNullDesC)
		{
		iMessageAddendum = KCRLF;
		}
	if (iMessageHistorySize < 0 || iMessageHistorySize > 15)
		{
		iMessageHistorySize = 8;
		}
	iEcho = (TBool)iEcho;
	if (iFontSize < 80 || iFontSize > 160)
		{
		iFontSize = 120;
		}
	if (iTabSize < 1 || iTabSize > 8)
		{
		iTabSize = 4;
		}
	if (iCtrlCharMapping < EMapCRtoLF || iCtrlCharMapping > EMapNone)
		{
		iCtrlCharMapping = EMapCRtoCRLF;
		}
	if (iCodePage < ECodePageLatin1 || iCodePage > ECodePageKOI8)
		{
		iCodePage = ECodePageLatin1;
		}
	}

void CRFtermSettings::ConstructL()
	{
	}

// Reading setting data from stream void
void CRFtermSettings::LoadL(RReadStream& aStream)
	{
	aStream >> iMessageAddendum;
	iMessageHistorySize = aStream.ReadInt32L();
	iEcho = aStream.ReadInt8L();
	iFontSize = aStream.ReadInt32L();
	iTabSize = aStream.ReadInt32L();
	iCtrlCharMapping = (TCtrlCharMapping) aStream.ReadInt32L();
	iCodePage = (TCodePage) aStream.ReadInt32L();
	}

// Storing setting data into stream void
void CRFtermSettings::SaveL(RWriteStream& aStream) const
	{
	aStream << iMessageAddendum;
	aStream.WriteInt32L(iMessageHistorySize);
	aStream.WriteInt8L(iEcho);
	aStream.WriteInt32L(iFontSize);
	aStream.WriteInt32L(iTabSize);
	aStream.WriteInt32L((TInt)iCtrlCharMapping);
	aStream.WriteInt32L((TInt)iCodePage);
	}

