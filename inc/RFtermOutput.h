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

class CRFtermOutput : public CEikEdwin
	{
public:
	static CRFtermOutput* NewL(const CCoeControl *aParent, const TRect& aRect);
	static CRFtermOutput* NewLC(const CCoeControl *aParent, const TRect& aRect);
	virtual ~CRFtermOutput();

	void ChangeCodePage(TCodePage aCodePage);
	TBool IsEmpty();
	void ClearL();
	void AppendTextL(const TDesC& aText, const TDesC& aPrefix);
	void AppendTextOnNewLineL(const TDesC& aText, const TDesC& aPrefix);
	void ScrollToEndL();
	void UpdateScrollBarsL();
	void UpdateCursorL();

	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

private:
	void ConstructL(const CCoeControl *aParent, const TRect& aRect);
	CRFtermOutput();
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
	void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);
	void HandlePointerEventL(const TPointerEvent &aPointerEvent);
	
private:
	TInt iRFtermFontID;
	TPtrC iCodePage;
	TPtrC iCurrentPrefix;
	TTextCursor iOutputCursor;

	CEikScrollBarFrame* iScrollBars;
	TAknDoubleSpanScrollBarModel iVScrollBarModel;
	TAknDoubleSpanScrollBarModel iHScrollBarModel;
	TInt iVScrollBarPos;
	TInt iHScrollBarPos;
	TBool iVScrollBarIsShown;
	TBool iHScrollBarIsShown;

	TInt iLastLineStartPos;
	TInt iLastLineCursorPos;

	CAknGlobalNote* iBellNote;
	TInt iBellNoteID;
	};

#endif /* RFTERMOUTPUT_H */
