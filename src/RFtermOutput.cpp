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
#include <RFterm_0xae7f53fa.rsg>
#include "RFterm.pan"
#include "RFtermAppUi.h"
#include "RFtermOutput.h"
#include "RFtermConstants.h"
#include "RFtermCodePages.h"

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
		ENoAutoSelection | EOnlyASCIIChars | EResizable | EReadOnly | EAvkonDisableCursor | ENoWrap,
		0,
		0,
		0
	);
	SetContainerWindowL(*aParent);
	// A rect is placed at (0,0)
	TRect rect(aRect.Size());
	SetRect(rect);

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

	iScrollBars = new (ELeave) CEikScrollBarFrame(this, this, ETrue);
	iScrollBars->CreateDoubleSpanScrollBarsL(ETrue, EFalse, ETrue, ETrue);
	iScrollBars->DrawBackground(EFalse, EFalse);
	iScrollBars->SetTypeOfVScrollBar(CEikScrollBarFrame::EDoubleSpan);
	iScrollBars->SetTypeOfHScrollBar(CEikScrollBarFrame::EDoubleSpan);
	iScrollBars->SetScrollBarVisibilityL(
		CEikScrollBarFrame::EOn,
		CEikScrollBarFrame::EOn);

	ClearL();

	iBellNote = CAknGlobalNote::NewL();
	iBellNote->SetTone(EAknNoteDialogWarningTone);
	}

CRFtermOutput::CRFtermOutput()
	: iCodePage(KCodePageLatin1)
	, iCurrentPrefix(KPrefixIn)
	, iVScrollBarPos(0)
	, iHScrollBarPos(0)
	, iVScrollBarIsShown(EFalse)
	, iHScrollBarIsShown(EFalse)
	, iLastLineStartPos(0)
	, iLastLineCursorPos(0)
	{
	}

CRFtermOutput::~CRFtermOutput()
	{
	delete iBellNote;
	iBellNote = NULL;

	delete iScrollBars;
	iScrollBars = NULL;

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
	UpdateScrollBarsL();
	UpdateCursorL();
	}

void CRFtermOutput::UpdateCursorL()
	{
	TPoint outputCursorPos;
	iLayout->DocPosToXyPosL(iLastLineCursorPos, outputCursorPos);
	outputCursorPos.iX -= iHScrollBarPos;
	
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

	UpdateScrollBarsL();
	UpdateCursorL();
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

void CRFtermOutput::AppendL(const TDesC& aBuf)
	{
	if (aBuf == KParagraphDelimeter)
		{
		iText->InsertL(iLastLineCursorPos, aBuf);
		iLastLineCursorPos = iText->DocumentLength();
		iTextView->HandleGlobalChangeL();
		ScrollToEndL();
		}
	else
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
					// Unsupported chars are maked with space
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
		iTextView->HandleGlobalChangeL();
		iLastLineCursorPos += normalText->Length();
		CleanupStack::PopAndDestroy(normalText);
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

	if (KErrNotFound == aPos)
		{
		return EFalse;
		}
	return ETrue;
	}

void CRFtermOutput::AppendCRL()
	{
	iLastLineCursorPos = iLastLineStartPos;
	UpdateCursorL();
	
	}

void CRFtermOutput::AppendLFL(const TDesC& aPrefix)
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

void CRFtermOutput::AppendTextL(const TDesC& aText, const TDesC& aPrefix)
	{
	if (iCurrentPrefix != aPrefix)
		{
		if (iLastLineCursorPos > iLastLineStartPos)
			{
			// Append new line
			iLastLineCursorPos = iText->DocumentLength();
			AppendL(KParagraphDelimeter);
			}
		else
			{
			// Line is empty. Change prefix only.
			iText->ToParagraphStart(iLastLineCursorPos);
			TInt prefixLength = iText->DocumentLength() - iLastLineCursorPos;
			iText->DeleteL(iLastLineCursorPos, prefixLength);
			}
		AppendL(aPrefix);
		iLastLineStartPos = iText->DocumentLength();
		iLastLineCursorPos = iLastLineStartPos;
		}
	
	CRFtermAppUi* appUi = (CRFtermAppUi*)CEikonEnv::Static()->EikAppUi();

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
			switch (appUi->iSettings->iCtrlCharMapping)
				{
				case EMapCRtoLF:
					{
					AppendLFL(aPrefix);
					break;
					}
				case EMapCRtoCRLF:
					{
					AppendCRL();
					AppendLFL(aPrefix);
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
					AppendLFL(aPrefix);
					break;
					}
				default:
					{
					AppendLFL(aPrefix);
					}
				}
			}
		else if (specChar.Compare(KLT) == 0)
			{
			AppendLFL(aPrefix);
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
				AppendL(*tab);
				CleanupStack::PopAndDestroy(tab);
				}
			iLastLineCursorPos = targetPos;
			UpdateCursorL();
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
			iBellNote->CancelNoteL(iBellNoteID);
			HBufC* bellNotify = StringLoader::LoadLC(R_STR_BELL);
			iBellNoteID = iBellNote->ShowNoteL(EAknGlobalInformationNote, *bellNotify);
			CleanupStack::PopAndDestroy(bellNotify);
			}
		else if (specChar.Compare(KFF) == 0)
			{
			ClearL();
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
				iTextView->HandleGlobalChangeL();
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
		AppendL(*tempText);
		}

	CleanupStack::PopAndDestroy(tempText);
	iCurrentPrefix.Set(aPrefix);
	}

void CRFtermOutput::AppendTextOnNewLineL(const TDesC& aText, const TDesC& aPrefix)
	{
	if ((iLastLineCursorPos > iLastLineStartPos) && (iCurrentPrefix == aPrefix))
		{
		iCurrentPrefix.Set(KNullDesC);
		}
	AppendTextL(aText, aPrefix);
	}

void CRFtermOutput::HandleScrollEventL(CEikScrollBar * aScrollBar, TEikScrollEvent aEventType)
	{
	TInt thumbPos = aScrollBar->ThumbPosition();

	if (aEventType == EEikScrollThumbDragVert)
		{
		TRect lineRect;
		iLayout->GetLineRect(0, lineRect);
		TInt vDelta = (thumbPos - iVScrollBarPos) / lineRect.Height();
		iVScrollBarPos += vDelta * lineRect.Height();
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
	else if (aEventType == EEikScrollThumbDragHoriz)
		{
		TInt hDelta = thumbPos - iHScrollBarPos;
		iHScrollBarPos += hDelta;
		TCursorPosition::TMovementType direction = TCursorPosition::EFRight;
		if (hDelta < 0)
			{
			hDelta = -hDelta;
			direction = TCursorPosition::EFLeft;
			}
		iTextView->SetHorizontalScrollJump(hDelta);
		iTextView->ScrollDisplayL(direction, CTextLayout::EFAllowScrollingBlankSpace);
		UpdateCursorL();
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

void CRFtermOutput::UpdateScrollBarsL()
	{
	if (!iScrollBars)
		{
		return;
		}

	TSize windowSize = Parent()->Rect().Size();
	TSize contentSize;
	iLayout->GetMinimumSizeL(KMaxTInt, contentSize);

	TRect outputRect(windowSize);
	TInt vBreadth(0);
	TInt hBreadth(0);

	CEikScrollBar* vScrollBar = iScrollBars->GetScrollBarHandle(CEikScrollBar::EVertical);
	CEikScrollBar* hScrollBar = iScrollBars->GetScrollBarHandle(CEikScrollBar::EHorizontal);

	// There are 2 scrollbars in the output, so scrollbars visibility
	// must be checked 2 times.
	// After enabling one of scrollbar another one must be checked again
	// because the size of output changes.
	for (TInt i = 2; i > 0; i--)
		{
		iVScrollBarIsShown = contentSize.iHeight > outputRect.Height();
		iHScrollBarIsShown = contentSize.iWidth > outputRect.Width();

		if (iVScrollBarIsShown)
			{
			vBreadth = vScrollBar->ScrollBarBreadth();
			}
		outputRect.SetWidth(windowSize.iWidth - vBreadth);

		if (iHScrollBarIsShown)
			{
			hBreadth = hScrollBar->ScrollBarBreadth();
			}
		outputRect.SetHeight(windowSize.iHeight - hBreadth);
		}

	if (outputRect != Rect())
		{
		SetRect(outputRect);
		}

	// Avoid partial visibility of the last text line at the bottom of the output.
	TRect lineRect;
	iLayout->GetLineRect(0, lineRect);
	TInt helfLineHeight = lineRect.Height() / 2;
	TPoint outputCursorPos;
	iLayout->DocPosToXyPosL(iLastLineCursorPos, outputCursorPos);
	TInt bottomCursorOffset = outputRect.iBr.iY - outputCursorPos.iY;
	if (bottomCursorOffset > -helfLineHeight && bottomCursorOffset < helfLineHeight)
		{
		iTextView->ScrollDisplayL(
				TCursorPosition::EFLineDown,
				CTextLayout::EFAllowScrollingBlankSpace);
		}

	iVScrollBarPos = contentSize.iHeight + lineRect.Height() - outputRect.Height();
	iVScrollBarModel.SetScrollSpan(contentSize.iHeight + lineRect.Height());
	iVScrollBarModel.SetWindowSize(outputRect.Height());
	iVScrollBarModel.SetFocusPosition(iVScrollBarPos);
	iHScrollBarPos = 0;
	iHScrollBarModel.SetScrollSpan(contentSize.iWidth + iOutputCursor.iWidth);
	iHScrollBarModel.SetWindowSize(outputRect.Width());
	iHScrollBarModel.SetFocusPosition(iHScrollBarPos);
	TEikScrollBarFrameLayout sbLayout;
	sbLayout.iTilingMode = TEikScrollBarFrameLayout::EClientRectConstant;
	TRect sbRect(windowSize);
	iScrollBars->TileL(&iHScrollBarModel, &iVScrollBarModel, outputRect, sbRect, sbLayout);

	if (iVScrollBarIsShown || iHScrollBarIsShown)
		{
		iScrollBars->DrawScrollBarsNow();
		}
	}
