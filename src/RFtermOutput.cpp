/*
 ============================================================================
 Name        : RFtermOutput.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : RFterm's output control.
 ============================================================================
 */

#include <eikenv.h>
#include <txtfrmat.h>
#include <gdi.h>
#include <fbs.h>
#include <stringloader.h>
#include <aknglobalnote.h> 
#include <avkon.hrh>
#include <RFterm_0xae7f53fa.rsg>
#include "RFterm.pan"
#include "RFtermOutput.h"
#include "RFtermConstants.h"

CRFtermOutput* CRFtermOutput::NewL(const CCoeControl *aParent, const TRect& aRect)
	{
	CRFtermOutput* self = CRFtermOutput::NewLC(aParent, aRect);
	CleanupStack::Pop(self);
	return self;
	}

CRFtermOutput* CRFtermOutput::NewLC(const CCoeControl *aParent, const TRect& aRect)
	{
	CRFtermOutput* self = new (ELeave) CRFtermOutput;
	CleanupStack::PushL(self);
	self->ConstructL(aParent, aRect);
	return self;
	}

void CRFtermOutput::ConstructL(const CCoeControl *aParent, const TRect& aRect)
	{
	CEikEdwin::ConstructL(
		ENoAutoSelection | EOnlyASCIIChars | EResizable | EReadOnly | EAvkonDisableCursor,
		0,
		0,
		0
	);
	SetContainerWindowL(*aParent);
	// A rect placed at (0,0)
	TRect rect(aRect.Size());
	SetRect(rect);

	CreateScrollBarFrameL();
	ScrollBarFrame()->SetScrollBarVisibilityL(
		CEikScrollBarFrame::EOff,
		CEikScrollBarFrame::EAuto
	);

	SetBackgroundColorL(KRgbBlack);
	iTextView->SetBackgroundColor(KRgbBlack);

	CParaFormatLayer* pParaFormatLayer = CEikonEnv::NewDefaultParaFormatLayerL();
	CleanupStack::PushL(pParaFormatLayer);
	CParaFormat paraFormat;
	TParaFormatMask paraFormatMask;
	pParaFormatLayer->SenseL(&paraFormat, paraFormatMask);
	paraFormat.iFillColor = KRgbBlack;
	paraFormatMask.SetAttrib(EAttFillColor);
	pParaFormatLayer->SetL(&paraFormat, paraFormatMask);
	SetParaFormatLayer(pParaFormatLayer);
	CleanupStack::Pop(pParaFormatLayer);

	TInt fontLoadingError; 
	fontLoadingError = CEikonEnv::Static()->ScreenDevice()->AddFile(
		KRFtermFontPath, iRFtermFontID);
	if (KErrNone != fontLoadingError)
		{
		Panic(ERFtermCannotLoadFont);
		}
	CCharFormatLayer* pCharFormatLayer = CEikonEnv::NewDefaultCharFormatLayerL();
	CleanupStack::PushL(pCharFormatLayer);
//	TCharFormat charFormat;
	TCharFormat charFormat(KRFtermFontName, 1);
	TCharFormatMask charFormatMask;
//	pCharFormatLayer->Sense(charFormat, charFormatMask);
	charFormat.iFontPresentation.iTextColor = KRgbWhite;
	charFormatMask.SetAttrib(EAttColor);
//	charFormat.iFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	charFormatMask.SetAttrib(EAttFontTypeface);
	charFormat.iFontSpec.iHeight = 120;
	charFormatMask.SetAttrib(EAttFontHeight);
	pCharFormatLayer->SetL(charFormat, charFormatMask);
	SetCharFormatLayer(pCharFormatLayer);
	CleanupStack::Pop(pCharFormatLayer);

	SetAknEditorAllowedInputModes(EAknEditorNullInputMode);

	iOutputCursor.iType = TTextCursor::ETypeRectangle;
	iOutputCursor.iHeight = 1;
	iOutputCursor.iAscent = -1;
	iOutputCursor.iWidth = 10;
	iOutputCursor.iFlags = 0;
	iOutputCursor.iColor = KRgbWhite;

	ClearL();
	}

CRFtermOutput::CRFtermOutput()
	: iCurrentPrefix(KPrefixIn)
	, iIsVScrollBarShown(EFalse)
	, iLastLineStartPos(0)
	, iLastLineCursorPos(0)
	{
	}

CRFtermOutput::~CRFtermOutput()
	{
	CEikonEnv::Static()->ScreenDevice()->RemoveFile(iRFtermFontID); 
	}

TBool CRFtermOutput::IsEmpty()
	{
	if (iText->DocumentLength() > iCurrentPrefix.Length())
		{
		return EFalse;
		}
	return ETrue;
	}

void CRFtermOutput::ClearL()
	{
	SetTextL(&iCurrentPrefix);
	iLastLineStartPos = iText->DocumentLength();
	iLastLineCursorPos = iLastLineStartPos;
	HandleTextChangedL();
	UpdateVScrollBarL();
	UpdateCursorL();
	}

void CRFtermOutput::UpdateVScrollBarL(TBool aIsSizeChanged)
	{
	TRect viewRect = iTextView->ViewRect();
	TSize textSize;
	iLayout->GetMinimumSizeL(viewRect.Width(), textSize);
	if (textSize.iHeight > viewRect.Height())
		{
		if (!iIsVScrollBarShown or (iIsVScrollBarShown and aIsSizeChanged))
			{
			TRect outputRect = Rect();
			TInt scrollbarWidth = ScrollBarFrame()->VerticalScrollBar()->ScrollBarBreadth();
			outputRect.SetWidth(outputRect.Width() - scrollbarWidth);
			SetRect(outputRect);
			iIsVScrollBarShown = ETrue;
			UpdateScrollBarsL();
			}
		}
	else if (textSize.iHeight <= viewRect.Height() and iIsVScrollBarShown)
		{
		TRect outputRect = Rect();
		TInt scrollbarWidth = ScrollBarFrame()->VerticalScrollBar()->ScrollBarBreadth();
		outputRect.SetWidth(outputRect.Width() + scrollbarWidth);
		SetRect(outputRect);
		iIsVScrollBarShown = EFalse;
		}
	}

void CRFtermOutput::UpdateCursorL(TBool aProcessLastLine)
	{
	TPoint outputCursorPos;
	iLayout->DocPosToXyPosL(iLastLineCursorPos, outputCursorPos);
	
	if (aProcessLastLine)
		{
		TRect firstLineRect;
		iLayout->GetLineRect(0, firstLineRect);
		TInt helfLineHeight = firstLineRect.Height() / 2;
		
		TRect outputRect = Rect();
		
		TInt bottomCursorOffset = outputRect.iBr.iY - outputCursorPos.iY;
		
		// Avoid partial visibility of the last text line at the bottom of the output.
		if (bottomCursorOffset > -helfLineHeight && bottomCursorOffset < helfLineHeight)
			{
			iTextView->ScrollDisplayL(
					TCursorPosition::EFLineDown,
					CTextLayout::EFAllowScrollingBlankSpace);
			iLayout->DocPosToXyPosL(iLastLineCursorPos, outputCursorPos);
			}
		}
	
	iEikonEnv->RootWin().SetTextCursor(
			Window(),
			outputCursorPos,
			iOutputCursor);
	}

void CRFtermOutput::ScrollToEndL()
	{
	TInt isScrolled;
	do
		{
		isScrolled = iTextView->ScrollDisplayL(
				TCursorPosition::EFLineDown);
		}
	while (isScrolled);
	iLastLineCursorPos = iText->DocumentLength();
	SetCursorPosL(iLastLineCursorPos, EFalse);
//	SetSelectionL(iLastLineCursorPos, lastPos);
	UpdateVScrollBarL();
	UpdateCursorL();
	}

void CRFtermOutput::AppendL(const TDesC& aBuf)
	{
	TInt lastPos = iText->DocumentLength();
	if (lastPos > iLastLineCursorPos)
		{
		// Replace the text after cursor
		TInt tailLength = lastPos - iLastLineCursorPos;
		iText->DeleteL(iLastLineCursorPos, Min(tailLength, aBuf.Length()));
		}
	iText->InsertL(iLastLineCursorPos, aBuf);
	iTextView->HandleGlobalChangeL();
	if (aBuf == KParagraphDelimeter)
		{
		ScrollToEndL();
		}
	else
		{
		iLastLineCursorPos += aBuf.Length();
		UpdateCursorL();
		}
	}

TBool CRFtermOutput::TextHasCtrlChar(const TDesC& aText, TDes& aCtrlChar, TInt& aPos)
	{
	aPos = KErrNotFound;
	TPtrC subText(aText);
	TInt pos;

	for (TInt i = 0; i < KCtrlChars().Length(); i++)
		{
		TBufC<1> specChar;
		specChar = KCtrlChars().Mid(i, 1);
		pos = subText.Find(specChar);
		if (KErrNotFound != pos)
			{
			aPos = pos;
			aCtrlChar = specChar;
			if (pos == 0) break;
			subText.Set(subText.Left(aPos));
			}
		}

	return (KErrNotFound == aPos) ? EFalse : ETrue;
	}

void CRFtermOutput::AppendTextL(const TDesC& aText, const TDesC& aPrefix)
	{
	if (iCurrentPrefix != aPrefix)
		{
		iLastLineCursorPos = iText->DocumentLength();
		AppendL(KParagraphDelimeter);
		AppendL(aPrefix);
		iLastLineStartPos = iText->DocumentLength();
		iLastLineCursorPos = iLastLineStartPos;
		}
	
	HBufC* tempText = HBufC::NewLC(aText.Length());
	tempText->Des().Copy(aText);
	
	TBuf<1> specChar;
	TInt specCharPos;
	
	while (TextHasCtrlChar(*tempText, specChar, specCharPos))
		{
		if (specCharPos > 0)
			{
			AppendL(tempText->Left(specCharPos));
			}
		
		if (specChar.Compare(KCR) == 0)
			{
			iLastLineCursorPos = iLastLineStartPos;
			UpdateCursorL();
			}
		else if (specChar.Compare(KLF) == 0 || specChar.Compare(KLT) == 0)
			{
			TInt indentLength = iLastLineCursorPos - iLastLineStartPos;

			// Go to next line
			iLastLineCursorPos = iText->DocumentLength();
			AppendL(KParagraphDelimeter);
			AppendL(aPrefix);
			iLastLineStartPos = iText->DocumentLength();
			iLastLineCursorPos = iLastLineStartPos;

			if (indentLength)
				{
				HBufC* indent = HBufC::NewLC(indentLength);
				TChar space(0x20);
				indent->Des().SetLength(indentLength);
				indent->Des().Fill(space);
				AppendL(*indent);
				CleanupStack::PopAndDestroy(indent);
				}
			}
		else if (specChar.Compare(KTB) == 0)
			{
			TInt indentLength = iLastLineCursorPos - iLastLineStartPos;
			TInt tabLength = KTabWidth - (indentLength % KTabWidth);
			TChar space(0x20);
			HBufC* tab = HBufC::NewLC(tabLength);
			tab->Des().SetLength(tabLength);
			tab->Des().Fill(space);
			AppendL(*tab);
			CleanupStack::PopAndDestroy(tab);
			}
		else if (specChar.Compare(KBS) == 0)
			{
			if (iLastLineCursorPos > iLastLineStartPos)
				{
				iLastLineCursorPos -= 1;
				UpdateCursorL();
				}
			}
		else if (specChar.Compare(KBL) == 0)
			{
			HBufC* bellNotify = StringLoader::LoadLC (R_STR_BELL);
			TRequestStatus status;
			CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
			globalNote->SetTone(EAknNoteDialogWarningTone);
			globalNote->ShowNoteL(status, EAknGlobalInformationNote, *bellNotify);
			User::WaitForRequest(status);
			CleanupStack::PopAndDestroy(2); // bellNotify, globalNote
			}
		else if (specChar.Compare(KFF) == 0)
			{
			ClearL();
			}
		else if (specChar.Compare(KNL) == 0)
			{
			// Do nothing
			}
		else
			{
			Panic(ERFtermOutputBadCtrlChar);
			}

		tempText->Des().Copy(
			tempText->Mid(specCharPos + specChar.Length()));
		}

	if (tempText->Length())
		{
		AppendL(*tempText);
		}

	CleanupStack::PopAndDestroy(tempText);
	iCurrentPrefix.Set(aPrefix);
	}

void CRFtermOutput::AppendTextOnNewLineL(const TDesC& aText, const TDesC& aPrefix)
	{
	iCurrentPrefix.Set(KNullDesC);
	AppendTextL(aText, aPrefix);
	}

void CRFtermOutput::HandleScrollEventL(CEikScrollBar * aScrollBar, TEikScrollEvent aEventType)
	{
	CEikEdwin::HandleScrollEventL(aScrollBar, aEventType);
	if (aEventType == EEikScrollThumbReleaseVert)
		{
		UpdateCursorL(ETrue);
		
		}
	else
		{
		UpdateCursorL(EFalse);
		}
	}

void CRFtermOutput::HandlePointerEventL(const TPointerEvent &aPointerEvent)
	{
	CEikEdwin::HandlePointerEventL(aPointerEvent);
	if (Selection().Length())
		{
		UpdateCursorL();
		}
	}

TKeyResponse CRFtermOutput::OfferKeyEventL(
	const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	TKeyResponse response = CEikEdwin::OfferKeyEventL(aKeyEvent, aType);
	if (aType == EEventKey)
		{
		switch (aKeyEvent.iScanCode)
			{
			case EStdKeyUpArrow:
			case EStdKeyDownArrow:
				UpdateCursorL();
				break;
			}
		}
	return response;
	}
