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
		ENoAutoSelection | EOnlyASCIIChars | EResizable | EReadOnly | EAvkonDisableCursor,
		0,
		0,
		0
	);
	SetContainerWindowL(*aParent);

	CreateScrollBarFrameL();
	ScrollBarFrame()->SetScrollBarVisibilityL(
		CEikScrollBarFrame::EOff,
		CEikScrollBarFrame::EOn
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
		KRFtermFontPath, iRFtermFontID
	);
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
	charFormat.iFontSpec.iHeight = 116;
	charFormatMask.SetAttrib(EAttFontHeight);
	pCharFormatLayer->SetL(charFormat, charFormatMask);
	SetCharFormatLayer(pCharFormatLayer);
	CleanupStack::Pop(pCharFormatLayer);

	SetAknEditorAllowedInputModes(EAknEditorNullInputMode);
	}

CRFtermOutput::CRFtermOutput()
	: iCurrentPrefix(KNullDesC)
	{
	}

CRFtermOutput::~CRFtermOutput()
	{
	CEikonEnv::Static()->ScreenDevice()->RemoveFile(iRFtermFontID); 
	}

TBool CRFtermOutput::IsEmpty()
	{
	if (iText->DocumentLength())
		{
		return EFalse;
		}
	return ETrue;
	}

void CRFtermOutput::Clear()
	{
	SetTextL(&KNullDesC);
	HandleTextChangedL();
	}

void CRFtermOutput::ScrollToEnd()
	{
	TInt isScrolled;
	do
		{
		isScrolled = iTextView->ScrollDisplayL(
				TCursorPosition::EFLineDown);
		}
	while (isScrolled);
	TInt lastPos = iText->DocumentLength();
	SetSelectionL(lastPos, lastPos);
	}

void CRFtermOutput::AppendL(const TDesC& aBuf)
	{
	TInt lastPos = iText->DocumentLength();
	iText->InsertL(lastPos, aBuf);
	iTextView->HandleGlobalChangeL();
	ScrollToEnd();
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
	iCurrentPrefix.Set(aPrefix);
	}
