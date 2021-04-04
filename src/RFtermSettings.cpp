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
	SetDefaultValues(EFalse);
	}

void CRFtermSettings::SetDefaultValues(TBool aNotify)
	{
	iMessageAddendum = KCRLF;
	iMessageHistorySize = 8;
	iEcho = ETrue;
	iFontSize = 120;
	iTabSize = 4;
	iFontAntialiasing = EFalse;
	iCtrlCharMapping = EMapCRtoCRLF;
	iCodePage = ECodePageLatin1;
	iSaveNotifies = ETrue;
	iBgColor = KRgbBlack;
	iFontColor = KDefaultFontColor;
	iCursorColor = KDefaultFontColor;
	iSbColor = KDefaultFontColor;
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetMessageAddendum(const TDesC& aAddendum, TBool aNotify)
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
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetMessageHistorySize(TInt aSize, TBool aNotify)
	{
	if (aSize < 0 || aSize > 15)
		{
		iMessageHistorySize = 8;
		}
	else
		{
		iMessageHistorySize = aSize;
		}
	if (aNotify)
		Notify();
	}

void CRFtermSettings::EnableEcho(TBool aState, TBool aNotify)
	{
	iEcho = aState ? 1 : 0;
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetFontSize(TInt aSize, TBool aNotify)
	{
	if (aSize < 80 || aSize > 160)
		{
		iFontSize = 120;
		}
	else
		{
		iFontSize = aSize;
		}
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetTabSize(TInt aSize, TBool aNotify)
	{
	if (aSize < 1 || aSize > 8)
		{
		iTabSize = 4;
		}
	else
		{
		iTabSize = aSize;
		}
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetFontAntialiasing(TBool aState, TBool aNotify)
	{
	iFontAntialiasing = aState ? 1 : 0;
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetCtrlCharMapping(TCtrlCharMapping aMapping, TBool aNotify)
	{
	if (aMapping < EMapCRtoLF || aMapping > EMapNone)
		{
		iCtrlCharMapping = EMapCRtoCRLF;
		}
	else
		{
		iCtrlCharMapping = aMapping;
		}
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetCodePage(TCodePage aCodePage, TBool aNotify)
	{
	if (aCodePage < ECodePageLatin1 || aCodePage > ECodePageKOI8)
		{
		iCodePage = ECodePageLatin1;
		}
	else
		{
		iCodePage = aCodePage;
		}
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetNotifySaving(TBool aSaveNotifies, TBool aNotify)
	{
	iSaveNotifies = aSaveNotifies ? 1 : 0;
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetBackgroundColor(TRgb aColor, TBool aNotify)
	{
	iBgColor = aColor;
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetFontColor(TRgb aColor, TBool aNotify)
	{
	iFontColor = aColor;
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetCursorColor(TRgb aColor, TBool aNotify)
	{
	iCursorColor = aColor;
	if (aNotify)
		Notify();
	}

void CRFtermSettings::SetScrollbarsColor(TRgb aColor, TBool aNotify)
	{
	iSbColor = aColor;
	if (aNotify)
		Notify();
	}

void CRFtermSettings::ConstructL()
	{
	}

// Reading setting data from stream void
void CRFtermSettings::LoadL(RReadStream& aStream)
	{
	aStream >> iMessageAddendum;
	SetMessageAddendum(iMessageAddendum, EFalse);
	SetMessageHistorySize(aStream.ReadInt32L(), EFalse);
	EnableEcho(aStream.ReadInt8L(), EFalse);
	SetFontSize(aStream.ReadInt32L(), EFalse);
	SetTabSize(aStream.ReadInt32L(), EFalse);
	SetFontAntialiasing(aStream.ReadInt8L(), EFalse);
	SetCtrlCharMapping((TCtrlCharMapping) aStream.ReadInt32L(), EFalse);
	SetCodePage((TCodePage) aStream.ReadInt32L(), EFalse);
	SetNotifySaving(aStream.ReadInt8L(), EFalse);
	SetBackgroundColor(aStream.ReadInt32L(), EFalse);
	SetFontColor(TRgb(aStream.ReadInt32L()), EFalse);
	SetCursorColor(TRgb(aStream.ReadInt32L()), EFalse);
	SetScrollbarsColor(TRgb(aStream.ReadInt32L()), EFalse);
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
	aStream.WriteInt8L(iFontAntialiasing);
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
	for (TInt i = iObservers.Count(); i > 0; i--)
		{
		MRFtermSettingsObserver* observer = iObservers[i - 1];
		observer->HandleSettingsChange(this);
		}
	}

