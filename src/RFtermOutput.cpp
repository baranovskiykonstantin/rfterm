/*
 * RFtermOutput.cpp
 *
 *  Created on: 28 Feb 2020
 */

#include <eikenv.h>
#include <txtfrmat.h>
#include <gdi.h>
#include "RFterm.pan"
#include "RFtermOutput.h"

_LIT(KRFtermFontPath, "\\private\\ae7f53fa\\cour.ttf");
_LIT(KRFtermFontName, "Courier New");

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
		ENoAutoSelection | ENoWrap | EOnlyASCIIChars | EResizable | EReadOnly | EAvkonDisableCursor,
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
	charFormat.iFontSpec.iHeight = 100;
	charFormatMask.SetAttrib(EAttFontHeight);
	pCharFormatLayer->SetL(charFormat, charFormatMask);
	SetCharFormatLayer(pCharFormatLayer);
	CleanupStack::Pop(pCharFormatLayer);

	SetAknEditorAllowedInputModes(EAknEditorNullInputMode);
	}

CRFtermOutput::CRFtermOutput()
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
	TInt textLength = iText->DocumentLength();
	iText->DeleteL(0, textLength);
	iTextView->HandleGlobalChangeL();
	}

void CRFtermOutput::AppendL(const TDesC& aBuf)
	{
	TInt lastPos = iText->DocumentLength();
	iText->InsertL(lastPos, aBuf);
	iTextView->HandleGlobalChangeL();
	}

void CRFtermOutput::AppendLineL(const TDesC& aLine)
	{
	AppendL(aLine);
	AppendL(_L("\x2029")); //CEditableText::EParagraphDelimiter
	}

void CRFtermOutput::AppendTextL(const TDesC& aText)
	{
	_LIT(KLineForward, "\n");
	HBufC* tempTextLine = HBufC::NewLC(aText.Length());
	tempTextLine->Des().Copy(aText);
	TInt lfPos = tempTextLine->Find(KLineForward);
	if (KErrNotFound != lfPos)
		{
		do
			{
			AppendLineL(tempTextLine->Left(lfPos));
			tempTextLine->Des().Copy(
				tempTextLine->Mid(lfPos + KLineForward().Length())
			);
			lfPos = tempTextLine->Find(KLineForward);
			}
		while (KErrNotFound != lfPos);
		}
	if (tempTextLine->Length())
		{
		AppendL(*tempTextLine);
		}
	CleanupStack::PopAndDestroy(tempTextLine);
	}
