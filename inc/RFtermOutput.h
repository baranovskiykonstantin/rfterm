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

class CRFtermOutput : public CEikEdwin
	{
public:
	static CRFtermOutput* NewL(const CCoeControl *aParent, const TRect& aRect);
	static CRFtermOutput* NewLC(const CCoeControl *aParent, const TRect& aRect);
	virtual ~CRFtermOutput();

	TBool IsEmpty();
	void ClearL();
	void AppendTextL(const TDesC& aText, const TDesC& aPrefix);
	void AppendTextOnNewLineL(const TDesC& aText, const TDesC& aPrefix);
	void UpdateVScrollBarL(TBool aIsSizeChanged=EFalse);
	/* If "aProcessLastLine" is true and
	 * last line of the text is partially showing
	 * at the bottom of the view
	 * the edwin will be scrolled down by one line.
	 */
	void UpdateCursorL(TBool aProcessLastLine=EFalse);

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
	void ScrollToEndL();
	void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);
	void HandlePointerEventL(const TPointerEvent &aPointerEvent);
	
private:
	TInt iRFtermFontID;
	TPtrC iCurrentPrefix;
	TTextCursor iOutputCursor;
	TBool iIsVScrollBarShown;
	TInt iLastLineStartPos;
	TInt iLastLineCursorPos;
	CAknGlobalNote* iBellNote;
	TInt iBellNoteID;
	};

#endif /* RFTERMOUTPUT_H */
