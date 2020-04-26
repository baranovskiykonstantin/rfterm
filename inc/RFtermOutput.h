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

class CRFtermOutput : public CEikEdwin , public MRFtermScrollBarsObserver
	{
public:
	// Constructor, destructor
	static CRFtermOutput* NewL(const CCoeControl *aParent);
	static CRFtermOutput* NewLC(const CCoeControl *aParent);
	virtual ~CRFtermOutput();

	// New functions
	void ChangeCodePage(TCodePage aCodePage);
	TBool IsEmpty() const;
	void ClearL();
	void AppendTextL(const TDesC& aText);
	void AppendMessageL(const TDesC& aMessage);
	// There is no ability to override SizeChanged() of CEikEdwin here,
	// so custom method is used to properly change the rect of the control.
	void UpdateRect(const TRect& aNewRect);

	// CEikEdwin
	void FocusChanged(TDrawNow aDrawNow);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

	// MRFtermOutputObserver
	void SetObserver(MRFtermOutputObserver* aObserver);
	void NotifyViewRectChangedL();

private:
	// Constructor
	void ConstructL(const CCoeControl *aParent);
	CRFtermOutput();

	// CEikEdwin
	void HandlePointerEventL(const TPointerEvent &aPointerEvent);

	// New functions
	void AppendNewLineL();
	void AppendRawTextL(const TDesC& aBuf);
	void AppendCRL();
	void AppendLFL();
	void AppendIndentL(TInt aIndentLength);
	void ScrollToCursorPosL(TBool aSkipAdditionalScroll=EFalse);
	void UpdateCursorL();
	/*
	 * Find control character (one of KCtrlChars).
	 * aText - the text for searching;
	 * aCtrlChar - the special character that has been found;
	 * aPos - the index of aCtrlChar in aText;
	 * Returns ETrue if found and EFalse otherwise.
	 */
	TBool TextHasCtrlChar(const TDesC& aText, TDes& aCtrlChar, TInt& aPos);

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
	TTextCursor iOutputCursor;

	TInt iLastLineStartPos;
	TInt iLastLineCursorPos;

	CAknGlobalNote* iBellNote;
	TInt iBellNoteID;
	
	MRFtermOutputObserver* iObserver;
	TRect iContentRect;
	/**
	 * iOutputRect
	 * Position of the view rect is
	 * relative to the content rect pos.
	 */
	TRect iOutputRect;
	};

#endif /* RFTERMOUTPUT_H */
