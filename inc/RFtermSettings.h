/*
 ============================================================================
 Name        : RFtermSettings.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application settings.
 ============================================================================
 */

#ifndef __RFTERMSETTINGS_H__
#define __RFTERMSETTINGS_H__

#include <e32base.h>
#include <e32cmn.h>
#include <s32strm.h>

/**
 * Enum TCtrlCharMapping.
 * Range of values of the CtrlCharMapping setting.
 */
enum TCtrlCharMapping {
	EMapCRtoLF = 1,
	EMapCRtoCRLF,
	EMapLFtoCR,
	EMapLFtoCRLF,
	EMapNone
};

/**
 * Enum TCodePage.
 * List of supported code pages.
 */
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

class CRFtermSettings;

/**
 * MRFtermSettingsObserver mixin.
 * Interface for objects that wish to receive
 * notifications of settings changes.
 */
class MRFtermSettingsObserver
	{

public:

	/**
	 * HandleSettingsChange()
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aSettings Pointer to observed settings that have been changed.
	 */
	virtual void HandleSettingsChange(const CRFtermSettings* aSettings) = 0;

	};

/**
 * CRFtermSettings class.
 * Contains and provides access to settings of RFterm applications.
 */
class CRFtermSettings : public CBase
	{

public: // Constructors, destructor
	static CRFtermSettings* NewL();
	static CRFtermSettings* NewLC();
	virtual ~CRFtermSettings();

public: // New functions

	// Loads settings from stream.
	void LoadL(RReadStream& aStream);

	// Saves settings to stream.
	void SaveL(RWriteStream& aStream) const;

	// Using default values.
	void SetDefaultValues();

	// Getters
	const TDesC& MessageAddendum() const;
	TInt MessageHistorySize() const;
	TBool IsEchoEnabled() const;
	TInt FontSize() const;
	TInt TabSize() const;
	TCtrlCharMapping CtrlCharMapping() const;
	TCodePage CodePage() const;

	// Setters
	void SetMessageAddendum(const TDesC& aAddendum);
	void SetMessageHistorySize(TInt aSize);
	void EnableEcho(TBool aState);
	void SetFontSize(TInt aSize);
	void SetTabSize(TInt aSize);
	void SetCtrlCharMapping(TCtrlCharMapping aMapping);
	void SetCodePage(TCodePage aCodePage);

	// MRFtermSettingsObserver
	void AddObserver(MRFtermSettingsObserver* aObserver);
	void RemoveObserver(MRFtermSettingsObserver* aObserver);

private: // Constructors

	CRFtermSettings();
	void ConstructL();

private: // For MRFtermSettingsObserver

	// Send messages to observers.
	void Notify();

private: // Data

	/**
	 * Finish a message with iMessageAddendum on sending.
	 */
	TBuf<2> iMessageAddendum;

	/**
	 * Size of the message history.
	 */
	TInt iMessageHistorySize;

	/**
	 * Output sent message
	 */
	TBool iEcho;

	/**
	 * Size of the output font.
	 */
	TInt iFontSize;

	/**
	 * Size of the tabulation step.
	 */
	TInt iTabSize;

	/**
	 * Map one control character with another.
	 */
	TCtrlCharMapping iCtrlCharMapping;

	/**
	 * Output code page
	 */
	TCodePage iCodePage;

private: // Observers

	/**
	 * Array of non ownign pointers to settings observers.
	 */
	RPointerArray<MRFtermSettingsObserver> iObservers;
	
	/**
	 * Flag to pause observer notification.
	 */
	TBool iSkipNotifications;

	};

inline const TDesC& CRFtermSettings::MessageAddendum() const { return iMessageAddendum; }
inline TInt CRFtermSettings::MessageHistorySize() const { return iMessageHistorySize; }
inline TBool CRFtermSettings::IsEchoEnabled() const { return iEcho; }
inline TInt CRFtermSettings::FontSize() const { return iFontSize; }
inline TInt CRFtermSettings::TabSize() const { return iTabSize; }
inline TCtrlCharMapping CRFtermSettings::CtrlCharMapping() const { return iCtrlCharMapping; }
inline TCodePage CRFtermSettings::CodePage() const { return iCodePage; }

#endif /* __RFTERMSETTINGS_H__ */

// End of File
