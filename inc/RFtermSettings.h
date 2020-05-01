/*
 ============================================================================
 Name        : RFtermSettings.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application settings.
 ============================================================================
 */

#ifndef RFTERMSETTINGS_H
#define RFTERMSETTINGS_H

#include <e32base.h>
#include <s32strm.h>

enum TCtrlCharMapping {
	EMapCRtoLF = 1,
	EMapCRtoCRLF,
	EMapLFtoCR,
	EMapLFtoCRLF,
	EMapNone
};

enum TCodePage {
	ECodePageLatin1 = 1,
	ECodePageLatin2,
	ECodePageLatin3,
	ECodePageLatin4,
	ECodePageLatinCyrilic,
	ECodePage1251,
	ECodePage1252,
	ECodePageKOI8,
};

class CRFtermSettings : public CBase
	{
public:
	static CRFtermSettings* NewL();
	static CRFtermSettings* NewLC();
	virtual ~CRFtermSettings();

	// Loads settings from stream.
	void LoadL(RReadStream& aStream);

	// Saves settings to stream.
	void SaveL(RWriteStream& aStream) const;

	// Using default values.
	void SetDefaultValues();

	// Move values into correct ranges.
	void Normalize();

private:
	CRFtermSettings();
	void ConstructL();

public:
	// Finish a message with iMessageAddendum on sending.
	TBuf<2> iMessageAddendum;
	
	// Size of the message history.
	TInt iMessageHistorySize;
	
	// Output sent message
	TBool iEcho;
	
	// Size of the output font.
	TInt iFontSize;
	
	// Size of the tabulation step.
	TInt iTabSize;

	// Map one control character with another.
	TCtrlCharMapping iCtrlCharMapping;
	
	// Output code page
	TCodePage iCodePage;
	};

#endif /* RFTERMSETTINGS_H */
