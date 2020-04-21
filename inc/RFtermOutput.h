/*
 ============================================================================
 Name        : RFtermOutput.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : RFterm's output control.
 ============================================================================
 */

#ifndef RFTERMOUTPUT_H
#define RFTERMOUTPUT_H

#include <eikedwin.h>
#include <aknglobalnote.h> 
#include "RFtermSettings.h"
#include "RFtermOutputObserver.h"
#include "RFtermScrollBarsObserver.h"

class CRFtermOutput
	: public CEikEdwin
	, public MRFtermScrollBarsObserver
	, public MEikEdwinSizeObserver
	{
public:
	// Constructor, destructor
	static CRFtermOutput* NewL(const CCoeControl *aParent, const TRect& aRect);
	static CRFtermOutput* NewLC(const CCoeControl *aParent, const TRect& aRect);
	virtual ~CRFtermOutput();

	// New functions
	void ChangeCodePage(TCodePage aCodePage);
	TBool IsEmpty();
	void ClearL();
	void AppendTextL(const TDesC& aText, const TDesC& aPrefix);
	void AppendTextOnNewLineL(const TDesC& aText, const TDesC& aPrefix);
	void ScrollToEndL();
	void UpdateCursorL();

	// MEikEdwinSizeObserver
	TBool HandleEdwinSizeEventL(CEikEdwin *aEdwin, TEdwinSizeEvent aEventType, TSize aDesirableEdwinSize);

	// CEikEdwin
	void FocusChanged(TDrawNow aDrawNow);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

	// MRFtermOutputObserver
	void SetObserver(MRFtermOutputObserver* aObserver);
	void NotifyViewRectChangedL();

private:
	// Constructor
	void ConstructL(const CCoeControl *aParent, const TRect& aRect);
	CRFtermOutput();

	// CEikEdwin
	void HandlePointerEventL(const TPointerEvent &aPointerEvent);

	// New functions
	void AppendL(const TDesC& aBuf);
	void AppendCRL();
	void AppendLFL(const TDesC& aPrefix);
	/*
	 * Find control character (one of KCtrlChars).
	 * aText - the text for searching;
	 * aCtrlChar - the special character that has been found;
	 * aPos - the index of aCtrlChar in aText;
	 * Returns ETrue if found and EFalse otherwise.
	 */
	TBool TextHasCtrlChar(const TDesC& aText, TDes& aCtrlChar, TInt& aPos);
	TBool IsViewPosChangedL();

	// MRFtermScrollBarsObserver
	void HandleScrollEventL(
		TRFtermScrollBarType aScrollBarType,
		const TRFtermScrollBarModel& aModel);
	void HandleScrollBarVisibilityChangeL(
		TBool aVScrollBarIsVisible,
		TBool aHScrollBarIsVisible);

private:
	TInt iRFtermFontID;
	TPtrC iCodePage;
	TPtrC iCurrentPrefix;
	TTextCursor iOutputCursor;

	/**
	 * iViewPos
	 * Position of the view rect relative
	 * to the content rect.
	 */
	TPoint iViewPos;

	TInt iLastLineStartPos;
	TInt iLastLineCursorPos;

	CAknGlobalNote* iBellNote;
	TInt iBellNoteID;
	
	MRFtermOutputObserver* iObserver;
	};

#endif /* RFTERMOUTPUT_H */
