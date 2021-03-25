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
#include <caknmemoryselectiondialogmultidrive.h>
#include <caknfileselectiondialog.h>
#include <caknfilenamepromptdialog.h> 
#include <utf.h>
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

CRFtermOutput::CRFtermOutput()
	: iRFtermFontID(-1)
	, iTabSize(4)
	, iCtrlCharMapping(EMapCRtoCRLF)
	, iCodePage(KCodePageLatin1)
	, iSaveNotifies(ETrue)
	, iLastLineStartPos(0)
	, iLastLineCursorPos(0)
	{
	}

void CRFtermOutput::ConstructL(const CCoeControl *aParent)
	{
	CEikEdwin::ConstructL(
			ENoAutoSelection | EOnlyASCIIChars | EResizable | EReadOnly | EAvkonDisableCursor | ENoWrap,
			0, 0, 0);
	SetContainerWindowL(*aParent);

	// Check if font is present in Font and Bitmap Server.
	// If not then add it manually.
	TInt numTypefaces = CEikonEnv::Static()->ScreenDevice()->NumTypefaces();
	TBool fontIsPresent(EFalse);
	for (TInt i = 0; i < numTypefaces; i++)
		{
		TTypefaceSupport typefaceSupport;
		CEikonEnv::Static()->ScreenDevice()->TypefaceSupport(typefaceSupport, i);
		if (typefaceSupport.iTypeface.iName == KRFtermFontName)
			{
			fontIsPresent = ETrue;
			break;
			}
		}
	if (!fontIsPresent)
		{
		TInt fontLoadingError;
		fontLoadingError = CEikonEnv::Static()->ScreenDevice()->AddFile(
				KCourierNewFontPath, iRFtermFontID);
		if (fontLoadingError != KErrNone)
			{
			Panic(ERFtermCannotLoadFont);
			}
		}

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

	SetAknEditorAllowedInputModes(EAknEditorNullInputMode);

	iOutputCursor.iType = TTextCursor::ETypeRectangle;
	iOutputCursor.iHeight = 1;
	iOutputCursor.iAscent = -1;
	iOutputCursor.iWidth = 10;
	iOutputCursor.iFlags = 0;
	iOutputCursor.iColor = KDefaultFontColor;

	SetFontSizeL(120); // Create with default font size

	iBellNote = CAknGlobalNote::NewL();
	iBellNote->SetTone(EAknNoteDialogWarningTone);
	}

CRFtermOutput::~CRFtermOutput()
	{
	delete iBellNote;
	iBellNote = NULL;

	if (iRFtermFontID >= 0)
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
	return (iText->DocumentLength() == 0);
	}

void CRFtermOutput::ClearL()
	{
	SetCursorPosL(0, EFalse);
	iText->DeleteL(0, iText->DocumentLength());
	iTextView->HandleGlobalChangeL();
	iTextView->FinishBackgroundFormattingL();
	iLastLineStartPos = 0;
	iLastLineCursorPos = iLastLineStartPos;
	NotifyViewRectChangedL();
	UpdateCursorL();
	}

void CRFtermOutput::UpdateCursorL()
	{
	TPoint outputCursorPos;
	iLayout->DocPosToXyPosL(iLastLineCursorPos, outputCursorPos);
	outputCursorPos.iX += iOutputCursor.iWidth / 6; // Tiny position correction is needed
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
	if (iFontSize == aFontSize)
		{
		return;
		}

	iFontSize = aFontSize;
	CCharFormatLayer* pCharFormatLayer = CEikonEnv::NewDefaultCharFormatLayerL();
	CleanupStack::PushL(pCharFormatLayer);
//	TCharFormat charFormat;
	TCharFormat charFormat(KRFtermFontName, 1);
	TCharFormatMask charFormatMask;
//	pCharFormatLayer->Sense(charFormat, charFormatMask);
	charFormat.iFontPresentation.iTextColor = KDefaultFontColor;
	charFormatMask.SetAttrib(EAttColor);
//	charFormat.iFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	charFormatMask.SetAttrib(EAttFontTypeface);
	charFormat.iFontSpec.iHeight = aFontSize;
	charFormatMask.SetAttrib(EAttFontHeight);
	pCharFormatLayer->SetL(charFormat, charFormatMask);
	SetCharFormatLayer(pCharFormatLayer);
	CleanupStack::Pop(pCharFormatLayer);

	iOutputCursor.iWidth = aFontSize / 12;

	DrawDeferred();
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

TBool CRFtermOutput::SaveOutputAsTextL()
	{
	TBool isSaved = EFalse;
	_LIT(KDefaultOutputPath, "C:\\data\\");
	_LIT(KDefaultOutputFileName, "RFtermOutput.txt");

	TDriveNumber drive = EDriveC;
	TPath path(KDefaultOutputPath);
	TFileName fileName(KDefaultOutputFileName);

	CAknMemorySelectionDialogMultiDrive* dlgDrive = CAknMemorySelectionDialogMultiDrive::NewL(ECFDDialogTypeSave, ETrue);
	CleanupStack::PushL(dlgDrive);
	TBool result = dlgDrive->ExecuteL(drive, &path, NULL);
	if (result)
		{
		CAknFileSelectionDialog* dlgFolder = CAknFileSelectionDialog::NewL(ECFDDialogTypeSave);
		CleanupStack::PushL(dlgFolder);
		result = dlgFolder->ExecuteL(path);
		if(result)
			{
			CAknFileNamePromptDialog* dlgFileName = CAknFileNamePromptDialog::NewL();
			result = dlgFileName->RunDlgLD(fileName, path);
			if(result)
				{
				RFs fileSession;
				User::LeaveIfError(fileSession.Connect());
				CleanupClosePushL(fileSession);
				RFile outputFile;
				path.Append(fileName);
				result = outputFile.Replace(fileSession, path, EFileWrite);
				if (result != KErrNone)
					{
					HBufC* errFileOpening = StringLoader::LoadLC(R_ERR_FILE_OPENING);
					AppendMessageL(*errFileOpening);
					CleanupStack::PopAndDestroy(errFileOpening);
					}
				else
					{
					CleanupClosePushL(outputFile);
					TPtrC outputText(iText->Read(0));
					TBuf8 <2> outputChar;
					_LIT8(KLineBreak8, "\r\n");
					for (TInt i = 0; i < outputText.Length(); i++)
						{
						TChar ch = outputText[i];
						outputChar.Zero();
						if ((TUint)ch == CEditableText::EParagraphDelimiter)
							{
							// skip paragraph delimiter at the end
							continue;
							}
						else if ((TUint)ch == CEditableText::ELineBreak)
							{
							outputFile.Write(KLineBreak8);
							}
						else if ((TUint)ch < 0x80)
							{
							// ASCII
							outputChar.Append(ch);
							outputFile.Write(outputChar);
							}
						else if ((TUint)ch == KRFtermOutputNotifyMark && !iSaveNotifies)
							{
							// Notify starts with KRFtermOutputNotifyMark and ends with CEditableText::ELineBreak
							do
								{
								ch = outputText[++i];
								}
							while (i < outputText.Length() && (TUint)ch != CEditableText::ELineBreak);
							}
						else
							{
							TInt pos = iCodePage.Locate(ch);
							if (pos != KErrNotFound)
								{
								// Extended ASCII (code page)
								TChar chNormal(0x80 + pos);
								outputChar.Append(chNormal);
								outputFile.Write(outputChar);
								}
							else
								{
								// Unicode characters are converted to UTF-8
								CnvUtfConverter::ConvertFromUnicodeToUtf8(outputChar, outputText.Mid(i, 1));
								outputFile.Write(outputChar);
								}
							}
						}
					isSaved = ETrue;
					CleanupStack::PopAndDestroy(&outputFile);
					}
				CleanupStack::PopAndDestroy(&fileSession);
				}
			}
		CleanupStack::PopAndDestroy(dlgFolder);
		}
	CleanupStack::PopAndDestroy(dlgDrive);

	return isSaved;
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
		if (pos != KErrNotFound)
			{
			aPos = pos;
			aCtrlChar = specChar;
			if (pos == 0) break;
			subText.Set(subText.Left(aPos));
			}
		}

	if (aPos == KErrNotFound)
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
			switch (iCtrlCharMapping)
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
			switch (iCtrlCharMapping)
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
			TInt tabLength = iTabSize - (indentLength % iTabSize);
			TInt targetPos = iLastLineCursorPos + tabLength;
			if (iText->DocumentLength() < targetPos)
				{
				iLastLineCursorPos = iText->DocumentLength();
				tabLength = targetPos - iText->DocumentLength();
				TChar space(0x20);
				HBufC* tab = HBufC::NewLC(tabLength);
				tab->Des().SetLength(tabLength);
				tab->Des().Fill(space);
				AppendRawTextL(*tab);
				CleanupStack::PopAndDestroy(tab);
				}
			else
				{
				iLastLineCursorPos = targetPos;
				}
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
			iLastLineStartPos = 0;
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
	if (indentLength || iText->DocumentLength() > iLastLineCursorPos)
		{
		AppendNewLineL();
		}
	// AppendRawTextL(aMessage);
	// This function is not acceptable because it filters
	// characters accordingly to the current code page.
	// Messages are localized and contain characters encoded
	// in UTF-8, so put the message to the output directly.
	iText->InsertL(iLastLineCursorPos, KRFtermOutputNotifyLabel);
	iLastLineCursorPos += KRFtermOutputNotifyLabel().Length();
	iText->InsertL(iLastLineCursorPos, aMessage);
	iLastLineCursorPos += aMessage.Length();
	AppendNewLineL();

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
