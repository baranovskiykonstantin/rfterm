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

	// Using default values
	void SetDefaultValues();

private:
	CRFtermSettings();
	void ConstructL();

public:
	// Finish a message with iMessageAddendum on sending.
	TBuf<2> iMessageAddendum;
	
	// Size of the message history.
	TInt iMessageHistorySize;

	// Map one control character with another.
	TCtrlCharMapping iCtrlCharMapping;
	
	// Size of the tabulation step.
	TInt iTabSize;
	};

#endif /* RFTERMSETTINGS_H */
