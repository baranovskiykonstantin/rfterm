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
#include <avkon.hrh>
#include <coemain.h>
#include <eiksbfrm.h>
#include <gulutil.h>
#include <RFterm_0xae7f53fa.rsg>
#include "RFterm.pan"
#include "RFtermAppUi.h"
#include "RFtermOutput.h"
#include "RFtermConstants.h"
#include "RFtermCodePages.h"

CRFtermOutput* CRFtermOutput::NewL(const CCoeControl *aParent)
	{
	CRFtermOutput* self = CRFtermOutput::NewLC(aParent);
	CleanupStack::Pop(self);
	return self;
	}

CRFtermOutput* CRFtermOutput::NewLC(const CCoeControl *aParent)
	{
	CRFtermOutput* self = new (ELeave) CRFtermOutput;
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	return self;
	}

void CRFtermOutput::ConstructL(const CCoeControl *aParent)
	{
	CEikEdwin::ConstructL(
		ENoAutoSelection | EOnlyASCIIChars | EResizable | EReadOnly | EAvkonDisableCursor | ENoWrap,
		0,
		0,
		0
	);
	SetContainerWindowL(*aParent);

	SetBackgroundColorL(KRgbBlack);
	iTextView->SetBackgroundColor(KRgbBlack);

	SetBorder(TGulBorder::ENone);
	TMargins8 viewMargins;
	viewMargins.SetAllValuesTo(0);
	SetBorderViewMargins(viewMargins);

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
	CRFtermAppUi* appUi = (CRFtermAppUi*)iCoeEnv->AppUi();
	SetFontSizeL(appUi->iSettings->iFontSize);

	SetAknEditorAllowedInputModes(EAknEditorNullInputMode);

	iOutputCursor.iType = TTextCursor::ETypeRectangle;
	iOutputCursor.iHeight = 1;
	iOutputCursor.iAscent = -1;
	iOutputCursor.iWidth = 10;
	iOutputCursor.iFlags = 0;
	iOutputCursor.iColor = KRgbWhite;

	iBellNote = CAknGlobalNote::NewL();
	iBellNote->SetTone(EAknNoteDialogWarningTone);
	}

CRFtermOutput::CRFtermOutput()
	: iCodePage(KCodePageLatin1)
	, iLastLineStartPos(0)
	, iLastLineCursorPos(0)
	{
	}

CRFtermOutput::~CRFtermOutput()
	{
	delete iBellNote;
	iBellNote = NULL;

	CEikonEnv::Static()->ScreenDevice()->RemoveFile(iRFtermFontID); 
	}

void CRFtermOutput::UpdateRect(const TRect& aNewRect)
	{
	// The error does not matter.
	// TRAP is used to hide leaving.
	TInt error;
	TRAP(error, SetCursorPosL(0, EFalse));

	SetRect(aNewRect);

	TRAP(error, ScrollToCursorPosL(ETrue));
	}

TBool CRFtermOutput::IsEmpty() const
	{
	if (iText->DocumentLength())
		{
		return EFalse;
		}
	return ETrue;
	}

void CRFtermOutput::ClearL()
	{
	SetCursorPosL(0, EFalse);
	iText->DeleteL(0, iText->DocumentLength());
	iTextView->HandleGlobalChangeL();
	iTextView->FinishBackgroundFormattingL();
	iLastLineStartPos = iText->DocumentLength();
	iLastLineCursorPos = iLastLineStartPos;
	NotifyViewRectChangedL();
	UpdateCursorL();
	}

void CRFtermOutput::UpdateCursorL()
	{
	TPoint outputCursorPos;
	iLayout->DocPosToXyPosL(iLastLineCursorPos, outputCursorPos);
	outputCursorPos.iX += iOutputCursor.iWidth / 2;
	outputCursorPos.iX -= iOutputRect.iTl.iX;
	
	if (iTextView->ViewRect().Contains(outputCursorPos))
		{
		iEikonEnv->RootWin().SetTextCursor(
			Window(),
			outputCursorPos,
			iOutputCursor);
		}
	else
		{
		iEikonEnv->RootWin().CancelTextCursor();
		}
	}

void CRFtermOutput::ScrollToCursorPosL(TBool aSkipAdditionalScroll)
	{
	// Scroll to the cursor pos
	SetCursorPosL(iLastLineCursorPos, EFalse);

	if (!aSkipAdditionalScroll)
		{
		// Avoid partial visibility of the last text line at the bottom of the output.
		TRect firstLineRect;
		iLayout->GetLineRect(0, firstLineRect);
		TInt helfLineHeight = firstLineRect.Height() / 2;
		TPoint outputCursorPos;
		iLayout->DocPosToXyPosL(iLastLineCursorPos, outputCursorPos);
		TRect outputRect = Rect();
		TInt bottomCursorOffset = Abs(outputCursorPos.iY - outputRect.iBr.iY);
		if (bottomCursorOffset < helfLineHeight)
			{
			iTextView->ScrollDisplayL(
					TCursorPosition::EFLineDown,
					CTextLayout::EFAllowScrollingBlankSpace);
			}
		}

	NotifyViewRectChangedL();
	UpdateCursorL();
	}

void CRFtermOutput::SetFontSizeL(TInt aFontSize)
	{
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
	charFormat.iFontSpec.iHeight = aFontSize;
	charFormatMask.SetAttrib(EAttFontHeight);
	pCharFormatLayer->SetL(charFormat, charFormatMask);
	SetCharFormatLayer(pCharFormatLayer);
	CleanupStack::Pop(pCharFormatLayer);

	iOutputCursor.iWidth = aFontSize / 12;

	ScrollToCursorPosL(ETrue);
	}

void CRFtermOutput::ChangeCodePage(TCodePage aCodePage)
	{
	switch (aCodePage)
		{
		case ECodePageLatin1:
			{
			iCodePage.Set(KCodePageLatin1);
			break;
			}
		case ECodePageLatin2:
			{
			iCodePage.Set(KCodePageLatin2);
			break;
			}
		case ECodePageLatin3:
			{
			iCodePage.Set(KCodePageLatin3);
			break;
			}
		case ECodePageLatin4:
			{
			iCodePage.Set(KCodePageLatin4);
			break;
			}
		case ECodePageLatinCyrilic:
			{
			iCodePage.Set(KCodePageLatinCyrillic);
			break;
			}
		case ECodePage1251:
			{
			iCodePage.Set(KCodePage1251);
			break;
			}
		case ECodePage1252:
			{
			iCodePage.Set(KCodePage1252);
			break;
			}
		case ECodePageKOI8:
			{
			iCodePage.Set(KCodePageKOI8);
			break;
			}
		default:
			{
			Panic(ERFtermOutputBadCodePage);
			}
		}
	}

void CRFtermOutput::GetCurrentCodePage(TPtrC& aCodePage)
	{
	aCodePage.Set(iCodePage);
	}

void CRFtermOutput::AppendNewLineL()
	{
	iLastLineCursorPos = iText->DocumentLength();
	iText->InsertL(iLastLineCursorPos, CPlainText::ELineBreak);
	iLastLineStartPos = iText->DocumentLength();
	iLastLineCursorPos = iLastLineStartPos;
	}

void CRFtermOutput::AppendRawTextL(const TDesC& aBuf)
	{
	HBufC* normalText = HBufC::NewLC(aBuf.Length());

	// Remove unsupported control characters
	TChar ch;
	for (TInt i = 0; i < aBuf.Length(); i++)
		{
		ch = aBuf[i];
		// if inside codepage and not control char
		if ((TUint)ch < KCodePageSize and (TUint)ch > 0x1f)
			{
			if ((TUint)ch > 0x7f)
				{
				// Extended ASCII (code page)
				TUint chOffset = (TUint)ch - 0x80;
				TChar chFromCodePage = iCodePage[chOffset];
				// Unsupported chars are maked with a space
				if ((TUint)chFromCodePage != 0x20)
					{
					normalText->Des().Append(chFromCodePage);
					}
				}
			else
				{
				// ASCII
				normalText->Des().Append(ch);
				}
			}
		}

	// Insert text
	TInt lastPos = iText->DocumentLength();
	if (lastPos > iLastLineCursorPos)
		{
		// Replace the text after cursor
		TInt tailLength = lastPos - iLastLineCursorPos;
		iText->DeleteL(iLastLineCursorPos, Min(tailLength, normalText->Length()));
		}

	iText->InsertL(iLastLineCursorPos, *normalText);
	iLastLineCursorPos += normalText->Length();
	CleanupStack::PopAndDestroy(normalText);
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

	if (KErrNotFound == aPos)
		{
		return EFalse;
		}
	return ETrue;
	}

void CRFtermOutput::AppendCRL()
	{
	iLastLineCursorPos = iLastLineStartPos;
	}

void CRFtermOutput::AppendIndentL(TInt aIndentLength)
	{
	if (aIndentLength > 0)
		{
		HBufC* indent = HBufC::NewLC(aIndentLength);
		TChar space(0x20);
		indent->Des().SetLength(aIndentLength);
		indent->Des().Fill(space);
		AppendRawTextL(*indent);
		CleanupStack::PopAndDestroy(indent);
		}
	}

void CRFtermOutput::AppendLFL()
	{
	TInt indentLength = iLastLineCursorPos - iLastLineStartPos;
	AppendNewLineL();
	AppendIndentL(indentLength);
	}

void CRFtermOutput::AppendTextL(const TDesC& aText)
	{
	CRFtermAppUi* appUi = (CRFtermAppUi*)iCoeEnv->AppUi();

	HBufC* tempText = HBufC::NewLC(aText.Length());
	tempText->Des().Copy(aText);
	
	TBuf<1> specChar;
	TInt specCharPos;
	
	while (TextHasCtrlChar(*tempText, specChar, specCharPos))
		{
		if (specCharPos > 0)
			{
			AppendRawTextL(tempText->Left(specCharPos));
			}
		
		if (specChar.Compare(KCR) == 0)
			{
			switch (appUi->iSettings->iCtrlCharMapping)
				{
				case EMapCRtoLF:
					{
					AppendLFL();
					break;
					}
				case EMapCRtoCRLF:
					{
					AppendCRL();
					AppendLFL();
					break;
					}
				default:
					{
					AppendCRL();
					}
				}
			}
		else if (specChar.Compare(KLF) == 0)
			{
			switch (appUi->iSettings->iCtrlCharMapping)
				{
				case EMapLFtoCR:
					{
					AppendCRL();
					break;
					}
				case EMapLFtoCRLF:
					{
					AppendCRL();
					AppendLFL();
					break;
					}
				default:
					{
					AppendLFL();
					}
				}
			}
		else if (specChar.Compare(KLT) == 0)
			{
			AppendLFL();
			}
		else if (specChar.Compare(KTB) == 0)
			{
			TInt indentLength = iLastLineCursorPos - iLastLineStartPos;
			TInt tabLength = appUi->iSettings->iTabSize - (indentLength % appUi->iSettings->iTabSize);
			TInt targetPos = iLastLineCursorPos + tabLength;
			if (iText->DocumentLength() < targetPos)
				{
				tabLength = targetPos - iText->DocumentLength();
				TChar space(0x20);
				HBufC* tab = HBufC::NewLC(tabLength);
				tab->Des().SetLength(tabLength);
				tab->Des().Fill(space);
				AppendRawTextL(*tab);
				CleanupStack::PopAndDestroy(tab);
				}
			iLastLineCursorPos = targetPos;
			}
		else if (specChar.Compare(KBS) == 0)
			{
			if (iLastLineCursorPos > iLastLineStartPos)
				{
				iLastLineCursorPos -= 1;
				}
			}
		else if (specChar.Compare(KBL) == 0)
			{
			iBellNote->CancelNoteL(iBellNoteID);
			HBufC* bellNotify = StringLoader::LoadLC(R_STR_BELL);
			iBellNoteID = iBellNote->ShowNoteL(EAknGlobalInformationNote, *bellNotify);
			CleanupStack::PopAndDestroy(bellNotify);
			}
		else if (specChar.Compare(KFF) == 0)
			{
			iText->DeleteL(0, iText->DocumentLength());
			iLastLineStartPos = iText->DocumentLength();
			iLastLineCursorPos = iLastLineStartPos;
			}
		else if (specChar.Compare(KNL) == 0)
			{
			// Do nothing
			}
		else if (specChar.Compare(KDL) == 0)
			{
			if (iLastLineCursorPos < iText->DocumentLength())
				{
				iText->DeleteL(iLastLineCursorPos, 1);
				}
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
		AppendRawTextL(*tempText);
		}

	CleanupStack::PopAndDestroy(tempText);

	iTextView->HandleGlobalChangeL();
	iTextView->FinishBackgroundFormattingL();

	ScrollToCursorPosL();
	}

void CRFtermOutput::AppendMessageL(const TDesC& aMessage)
	{
	TInt indentLength = iLastLineCursorPos - iLastLineStartPos;
	if (indentLength)
		{
		AppendNewLineL();
		}
	AppendRawTextL(KRFtermOutputMessageMark);
	AppendRawTextL(aMessage);
	AppendNewLineL();
	AppendIndentL(indentLength);

	iTextView->HandleGlobalChangeL();
	iTextView->FinishBackgroundFormattingL();

	ScrollToCursorPosL();
	}

void CRFtermOutput
:: HandleScrollEventL(TRFtermScrollBarType aScrollBarType, const TRFtermScrollBarModel& aModel)
	{
	if (aScrollBarType == EVerticalScrollBar)
		{
		TRect lineRect;
		iLayout->GetLineRect(0, lineRect);
		TInt vDelta = (aModel.iPos - iOutputRect.iTl.iY) / lineRect.Height();
		iOutputRect.iTl.iY += vDelta * lineRect.Height();
		TCursorPosition::TMovementType direction = TCursorPosition::EFLineDown;
		if (vDelta < 0)
			{
			vDelta = -vDelta;
			direction = TCursorPosition::EFLineUp;
			}
		for (; vDelta > 0; vDelta--)
			{
			// Do not use ScrollDisplayLinesL(...).
			// That skips lines on fast scrolling.
			iTextView->ScrollDisplayL(direction, CTextLayout::EFAllowScrollingBlankSpace);
			}
		UpdateCursorL();
		}
	else if (aScrollBarType == EHorizontalScrollBar)
		{
		TInt hDelta = aModel.iPos - iOutputRect.iTl.iX;
		iOutputRect.iTl.iX += hDelta;
		TCursorPosition::TMovementType direction = TCursorPosition::EFRight;
		if (hDelta < 0)
			{
			hDelta = -hDelta;
			direction = TCursorPosition::EFLeft;
			}
		TInt hsj = iTextView->HorizontalScrollJump();
		iTextView->SetHorizontalScrollJump(hDelta);
		iTextView->ScrollDisplayL(direction, CTextLayout::EFAllowScrollingBlankSpace);
		// HorizontalScrollJump must be restored to default value
		// otherwise Panic KERN-EXEC 3 happens on text selecting or resizing.
		iTextView->SetHorizontalScrollJump(hsj);
		UpdateCursorL();
		}
	}

void CRFtermOutput
:: HandleScrollBarVisibilityChangeL(TBool aVScrollBarIsVisible, TBool aHScrollBarIsVisible)
	{
	TSize outputSize = Parent()->Rect().Size();
	if (aVScrollBarIsVisible)
		{
		outputSize.iWidth -= KRFtermScrollBarBreadth;
		}
	if (aHScrollBarIsVisible)
		{
		outputSize.iHeight -= KRFtermScrollBarBreadth;
		}
	SetSize(outputSize);
	NotifyViewRectChangedL();
	}

void CRFtermOutput::HandlePointerEventL(const TPointerEvent &aPointerEvent)
	{
	CEikEdwin::HandlePointerEventL(aPointerEvent);
	if (aPointerEvent.iType == TPointerEvent::EButton1Up)
		{
		UpdateCursorL();
		}
	else if (aPointerEvent.iType == TPointerEvent::EDrag)
		{
		NotifyViewRectChangedL();
		}
	}

void CRFtermOutput::FocusChanged(TDrawNow aDrawNow)
	{
	CEikEdwin::FocusChanged(aDrawNow);
	if (IsFocused())
		{
		// The error does not matter.
		// TRAP is used to hide leaving.
		TInt error;
		TRAP(error, UpdateCursorL());
		}
	else
		{
		iEikonEnv->RootWin().CancelTextCursor();
		}
	}

TKeyResponse CRFtermOutput
:: OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	TKeyResponse response = EKeyWasNotConsumed;
	if (aType == EEventKey)
		{
		switch (aKeyEvent.iScanCode)
			{
			case EStdKeyUpArrow:
				{
				iTextView->ScrollDisplayL(
					TCursorPosition::EFLineUp,
					CTextLayout::EFDisallowScrollingBlankSpace);
				NotifyViewRectChangedL();
				UpdateCursorL();
				response = EKeyWasConsumed;
				break;
				}
			case EStdKeyDownArrow:
				{
				// Allow scrolling only when bottom adge of viewRect
				// is inside of contentRect
				if (iOutputRect.iBr.iY < iContentRect.Height())
					{
					iTextView->ScrollDisplayL(
						TCursorPosition::EFLineDown,
						CTextLayout::EFAllowScrollingBlankSpace);
					NotifyViewRectChangedL();
					UpdateCursorL();
					}
				response = EKeyWasConsumed;
				break;
				}
			case EStdKeyLeftArrow:
				{
				iTextView->ScrollDisplayL(
					TCursorPosition::EFLeft,
					CTextLayout::EFDisallowScrollingBlankSpace);
				NotifyViewRectChangedL();
				UpdateCursorL();
				response = EKeyWasConsumed;
				break;
				}
			case EStdKeyRightArrow:
				{
				// Allow scrolling only when right adge of viewRect
				// is inside of contentRect
				if (iOutputRect.iBr.iX < iContentRect.Width())
					{
					iTextView->ScrollDisplayL(
						TCursorPosition::EFRight,
						CTextLayout::EFAllowScrollingBlankSpace);
					NotifyViewRectChangedL();
					UpdateCursorL();
					}
				response = EKeyWasConsumed;
				break;
				}
			}
		}
	if (response == EKeyWasNotConsumed)
		{
		response = CEikEdwin::OfferKeyEventL(aKeyEvent, aType);
		}
	return response;
	}

void CRFtermOutput::SetObserver(MRFtermOutputObserver* aObserver)
	{
	iObserver = aObserver;
	}

void CRFtermOutput::NotifyViewRectChangedL()
	{
	TSize contentSize;
	iLayout->GetMinimumSizeL(KMaxTInt, contentSize);
	TRect contentRect(contentSize);

	TRect outputRect = iTextView->ViewRect();
	// Sometimes viewRect has the position (iTl)
	// different from (0, 0).
	TPoint viewOffset = outputRect.iTl;
	outputRect.Move(-viewOffset);
	// Output has only one paragraph.
	// The top left position of this paragraph is
	// relative to top left position of the text view.
	TRect firstParaRect = iTextView->ParagraphRectL(0);
	// Line rect is needed to calculate line height.
	TRect firstLineRect;
	iLayout->GetLineRect(firstParaRect.iTl.iY, firstLineRect);
	firstParaRect.Move(-viewOffset);

	// Add some room for cursor
	contentRect.iBr.iX += iOutputCursor.iWidth * 2;
	// To avoid partial showing of the last line add extra height.
	__ASSERT_ALWAYS(firstLineRect.Height() > 0, Panic(ERFtermOutputBadContent));
	outputRect.iBr.iY -= outputRect.Height() % firstLineRect.Height();

	TPoint viewPos = -(firstParaRect.iTl);
	outputRect.Move(viewPos);

	if (iObserver && (contentRect != iContentRect || outputRect != iOutputRect))
		{
		iContentRect = contentRect;
		iOutputRect = outputRect;
		iObserver->HandleViewRectChangedL(iContentRect, iOutputRect);
		}
	}
