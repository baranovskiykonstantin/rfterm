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
	iObservers.ResetAndDestroy();
	}

CRFtermSettings::CRFtermSettings()
	{
	iSkipNotifications = ETrue;
	SetDefaultValues();
	iSkipNotifications = EFalse;
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
	iSaveNotifies = ETrue;
	iBgColor = KRgbBlack;
	iFontColor = KDefaultFontColor;
	iCursorColor = KDefaultFontColor;
	iSbColor = KDefaultFontColor;
	Notify();
	}

void CRFtermSettings::SetMessageAddendum(const TDesC& aAddendum)
	{
	if (aAddendum != KCR &&
			aAddendum != KLF &&
			aAddendum != KCRLF &&
			aAddendum != KNullDesC)
		{
		iMessageAddendum = KCRLF;
		}
	else
		{
		iMessageAddendum = aAddendum;
		}
	Notify();
	}

void CRFtermSettings::SetMessageHistorySize(TInt aSize)
	{
	if (aSize < 0 || aSize > 15)
		{
		iMessageHistorySize = 8;
		}
	else
		{
		iMessageHistorySize = aSize;
		}
	Notify();
	}

void CRFtermSettings::EnableEcho(TBool aState)
	{
	iEcho = (TBool)aState;
	Notify();
	}

void CRFtermSettings::SetFontSize(TInt aSize)
	{
	if (aSize < 80 || aSize > 160)
		{
		iFontSize = 120;
		}
	else
		{
		iFontSize = aSize;
		}
	Notify();
	}

void CRFtermSettings::SetTabSize(TInt aSize)
	{
	if (aSize < 1 || aSize > 8)
		{
		iTabSize = 4;
		}
	else
		{
		iTabSize = aSize;
		}
	Notify();
	}

void CRFtermSettings::SetCtrlCharMapping(TCtrlCharMapping aMapping)
	{
	if (aMapping < EMapCRtoLF || aMapping > EMapNone)
		{
		iCtrlCharMapping = EMapCRtoCRLF;
		}
	else
		{
		iCtrlCharMapping = aMapping;
		}
	Notify();
	}

void CRFtermSettings::SetCodePage(TCodePage aCodePage)
	{
	if (aCodePage < ECodePageLatin1 || aCodePage > ECodePageKOI8)
		{
		iCodePage = ECodePageLatin1;
		}
	else
		{
		iCodePage = aCodePage;
		}
	Notify();
	}

void CRFtermSettings::SetNotifySaving(TBool aSaveNotifies)
	{
	iSaveNotifies = (TBool)aSaveNotifies;
	Notify();
	}

void CRFtermSettings::SetBackgroundColor(TRgb aColor)
	{
	iBgColor = aColor;
	Notify();
	}

void CRFtermSettings::SetFontColor(TRgb aColor)
	{
	iFontColor = aColor;
	Notify();
	}

void CRFtermSettings::SetCursorColor(TRgb aColor)
	{
	iCursorColor = aColor;
	Notify();
	}

void CRFtermSettings::SetScrollbarsColor(TRgb aColor)
	{
	iSbColor = aColor;
	Notify();
	}

void CRFtermSettings::ConstructL()
	{
	}

// Reading setting data from stream void
void CRFtermSettings::LoadL(RReadStream& aStream)
	{
	iSkipNotifications = ETrue;
	aStream >> iMessageAddendum;
	SetMessageAddendum(iMessageAddendum);
	SetMessageHistorySize(aStream.ReadInt32L());
	EnableEcho(aStream.ReadInt8L());
	SetFontSize(aStream.ReadInt32L());
	SetTabSize(aStream.ReadInt32L());
	SetCtrlCharMapping((TCtrlCharMapping) aStream.ReadInt32L());
	SetCodePage((TCodePage) aStream.ReadInt32L());
	SetNotifySaving(aStream.ReadInt8L());
	SetBackgroundColor(aStream.ReadInt32L());
	SetFontColor(TRgb(aStream.ReadInt32L()));
	SetCursorColor(TRgb(aStream.ReadInt32L()));
	SetScrollbarsColor(TRgb(aStream.ReadInt32L()));
	iSkipNotifications = EFalse;
	Notify();
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
	aStream.WriteInt8L(iSaveNotifies);
	aStream.WriteInt32L(iBgColor.Value());
	aStream.WriteInt32L(iFontColor.Value());
	aStream.WriteInt32L(iCursorColor.Value());
	aStream.WriteInt32L(iSbColor.Value());
	}

void CRFtermSettings::AddObserver(MRFtermSettingsObserver* aObserver)
	{
	iObservers.Append(aObserver);
	}

void CRFtermSettings::RemoveObserver(MRFtermSettingsObserver* aObserver)
	{
	TInt id = iObservers.Find(aObserver);
	if (id != KErrNotFound)
		{
		iObservers.Remove(id);
		}
	}

void CRFtermSettings::Notify()
	{
	if (iSkipNotifications)
		{
		return;
		}

	for (TInt i = iObservers.Count(); i > 0; i--)
		{
		MRFtermSettingsObserver* observer = iObservers[i - 1];
		observer->HandleSettingsChange(this);
		}
	}

