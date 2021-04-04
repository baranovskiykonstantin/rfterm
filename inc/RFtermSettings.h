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
#include <gdi.h>

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
	void SetDefaultValues(TBool aNotify=ETrue);

	// Getters
	const TDesC& MessageAddendum() const;
	TInt MessageHistorySize() const;
	TBool IsEchoEnabled() const;
	TInt FontSize() const;
	TInt TabSize() const;
	TBool FontAntialiasing() const;
	TCtrlCharMapping CtrlCharMapping() const;
	TCodePage CodePage() const;
	TBool DoSaveNotifies() const;
	TRgb BackgroundColor() const;
	TRgb FontColor() const;
	TRgb CursorColor() const;
	TRgb ScrollbarsColor() const;

	// Setters
	void SetMessageAddendum(const TDesC& aAddendum, TBool aNotify=ETrue);
	void SetMessageHistorySize(TInt aSize, TBool aNotify=ETrue);
	void EnableEcho(TBool aState, TBool aNotify=ETrue);
	void SetFontSize(TInt aSize, TBool aNotify=ETrue);
	void SetTabSize(TInt aSize, TBool aNotify=ETrue);
	void SetFontAntialiasing(TBool aState, TBool aNotify=ETrue);
	void SetCtrlCharMapping(TCtrlCharMapping aMapping, TBool aNotify=ETrue);
	void SetCodePage(TCodePage aCodePage, TBool aNotify=ETrue);
	void SetNotifySaving(TBool aSaveNotifies, TBool aNotify=ETrue);
	void SetBackgroundColor(TRgb aColor, TBool aNotify=ETrue);
	void SetFontColor(TRgb aColor, TBool aNotify=ETrue);
	void SetCursorColor(TRgb aColor, TBool aNotify=ETrue);
	void SetScrollbarsColor(TRgb aColor, TBool aNotify=ETrue);

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
	 * Font antialiasing
	 */
	TBool iFontAntialiasing;

	/**
	 * Map one control character with another.
	 */
	TCtrlCharMapping iCtrlCharMapping;

	/**
	 * Output code page
	 */
	TCodePage iCodePage;

	/**
	 * Save notifies or not
	 */
	TBool iSaveNotifies;

	/**
	 * Output background color
	 */
	TRgb iBgColor;

	/**
	 * Output font color
	 */
	TRgb iFontColor;

	/**
	 * Output cursor color
	 */
	TRgb iCursorColor;

	/**
	 * Output scrollbars color
	 */
	TRgb iSbColor;

private: // Observers

	/**
	 * Array of non ownign pointers to settings observers.
	 */
	RPointerArray<MRFtermSettingsObserver> iObservers;

	};

inline const TDesC& CRFtermSettings::MessageAddendum() const { return iMessageAddendum; }
inline TInt CRFtermSettings::MessageHistorySize() const { return iMessageHistorySize; }
inline TBool CRFtermSettings::IsEchoEnabled() const { return iEcho; }
inline TInt CRFtermSettings::FontSize() const { return iFontSize; }
inline TInt CRFtermSettings::TabSize() const { return iTabSize; }
inline TBool CRFtermSettings::FontAntialiasing() const { return iFontAntialiasing; }
inline TCtrlCharMapping CRFtermSettings::CtrlCharMapping() const { return iCtrlCharMapping; }
inline TCodePage CRFtermSettings::CodePage() const { return iCodePage; }
inline TBool CRFtermSettings::DoSaveNotifies() const { return iSaveNotifies; }
inline TRgb CRFtermSettings::BackgroundColor() const { return iBgColor; }
inline TRgb CRFtermSettings::FontColor() const { return iFontColor; }
inline TRgb CRFtermSettings::CursorColor() const { return iCursorColor; }
inline TRgb CRFtermSettings::ScrollbarsColor() const { return iSbColor; }

#endif /* __RFTERMSETTINGS_H__ */

// End of File
