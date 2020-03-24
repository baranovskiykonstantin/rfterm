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
	TInt lastPos = iText->DocumentLength();
	TPoint outputCursorPos;
	iLayout->DocPosToXyPosL(lastPos, outputCursorPos);
	
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
			iLayout->DocPosToXyPosL(lastPos, outputCursorPos);
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
	TInt lastPos = iText->DocumentLength();
//	SetCursorPosL(lastPos, EFalse);
	SetSelectionL(lastPos, lastPos);
	UpdateVScrollBarL();
	UpdateCursorL(ETrue);
	}

void CRFtermOutput::AppendL(const TDesC& aBuf)
	{
	TInt lastPos = iText->DocumentLength();
	iText->InsertL(lastPos, aBuf);
	iTextView->HandleGlobalChangeL();
	ScrollToEndL();
	}

void CRFtermOutput::AppendTextL(const TDesC& aText, const TDesC& aPrefix)
	{
	if (iCurrentPrefix != aPrefix)
		{
		AppendL(KParagraphDelimeter);
		AppendL(aPrefix);
		}
	
	HBufC* tempText = HBufC::NewLC(aText.Length());
	tempText->Des().Copy(aText);
	
	// Remove carriare return (CR).
	TInt crPos = tempText->Find(KCR);
	if (KErrNotFound != crPos)
		{
		do
			{
			tempText->Des().Replace(crPos, KCR().Length(), KNullDesC);
			crPos = tempText->Find(KCR);
			}
		while (KErrNotFound != crPos);
		}
	
	// Replace line forward (LF) with paragraph delimeter.
	TInt lfPos = tempText->Find(KLF);
	if (KErrNotFound != lfPos)
		{
		if (lfPos > 0)
			{
			AppendL(tempText->Left(lfPos));
			}
		else
			{
			AppendL(KParagraphDelimeter);
			AppendL(aPrefix);
			}
		tempText->Des().Copy(
			tempText->Mid(lfPos + KLF().Length()));

		lfPos = tempText->Find(KLF);
		while (KErrNotFound != lfPos)
			{
			AppendL(KParagraphDelimeter);
			AppendL(aPrefix);
			AppendL(tempText->Left(lfPos));
			tempText->Des().Copy(
				tempText->Mid(lfPos + KLF().Length()));
			lfPos = tempText->Find(KLF);
			}
		if (tempText->Length())
			{
			AppendL(KParagraphDelimeter);
			AppendL(aPrefix);
			AppendL(*tempText);
			}
		}
	else
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
