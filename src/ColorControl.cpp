/*
 ============================================================================
 Name        : ColorControl.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Custom control for the color setup.
 ============================================================================
 */

#include <aknutils.h>
#include <aknsdrawutils.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <eikenv.h>
#include <e32std.h> // Max()
#include <barsread.h>
#include <aknslider.h>
#include <eikspane.h>
#include <aknnavilabel.h>
#include <eikmfne.h>
#include <RFterm_0xae7f53fa.rsg>
#include "ColorControl.h"
#include "ColorControl.hrh"

_LIT(KModule, "CColorDialog");

const TInt KPreviewTileWidth = 25;
const TInt KPreviewTileHeight = 25;
const TInt KControlMargins = 10;
_LIT(KDelimiterR, "R ");
_LIT(KDelimiterG, "G ");
_LIT(KDelimiterB, "B");
_LIT(KColorPlaceKeeper, "      ");
const TInt KFieldR = 0;
const TInt KFieldG = 2;
const TInt KFieldB = 4;
const TInt KMinColor = 0;
const TInt KMaxColor = 255;

enum
	{
	EColorControlPreview = 1,
	EColorControlSliderR,
	EColorControlSliderG,
	EColorControlSliderB
	};

/**
 * CColorPreview
 */

void CColorPreview::ConstructL(const CCoeControl* aParent, const TRgb* aInitialColor, const TRgb* aColor)
	{
	iInitialColor = aInitialColor;
	iColor = aColor;
	SetContainerWindowL(*aParent);
	ActivateL();
	}

TSize CColorPreview::MinimumSize()
	{
	TSize ms;
	ms.iWidth = KPreviewTileWidth + KControlMargins * 2;
	ms.iHeight = KPreviewTileHeight * 2 + KControlMargins * 2;
	return ms;
	}

void CColorPreview::Draw(const TRect& /*aRect*/) const
	{
	TRect rect = Rect();
	CWindowGc& gc = SystemGc();

	// Initial color preview
	TRect previewRect(rect);
	previewRect.Shrink(KControlMargins, KControlMargins);
	previewRect.SetHeight(previewRect.Height() / 2);
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(*iInitialColor);
	gc.DrawRect(previewRect);

	// Current color preview
	previewRect.Move(0, previewRect.Height());
	gc.SetBrushColor(*iColor);
	gc.DrawRect(previewRect);
	}

/**
 * CColorControl
 */

CColorControl::~CColorControl()
	{
	delete iBackground;
	iBackground = NULL;
	delete iNaviDecorator;
	iNaviDecorator = NULL;
	}

void CColorControl::ConstructL(const CCoeControl* aParent, TRgb aColor)
	{
	if(aParent)
		SetContainerWindowL(*aParent);
	else
		CreateWindowL();

	iInitialColor = aColor;
	iColor = aColor;
	iFont = (CFont*)AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);

	// support themes
	iBackground = CAknsBasicBackgroundControlContext::NewL(
			KAknsIIDQsnBgAreaMain, Rect(), EFalse);

	TResourceReader reader;

	// array for components
	InitComponentArrayL();

	// first component - preview
	CColorPreview* colorPreview = new (ELeave) CColorPreview;

	// second component - slider for Red
	CAknSlider* sliderR = new (ELeave) CAknSlider;
	Components().AppendLC(sliderR, EColorControlSliderR);
	iEikonEnv->CreateResourceReaderLC(reader, R_COLOR_SLIDER_R);
	sliderR->ConstructFromResourceL(this, iColor.Red(), reader);
	sliderR->HandleResourceChange(KEikMessageCaptionedControlEditableStateChange);
	sliderR->SetObserver(this);
	CleanupStack::PopAndDestroy(); // reader
	CleanupStack::Pop(sliderR);

	// third component - slider for Green
	CAknSlider* sliderG = new (ELeave) CAknSlider;
	Components().AppendLC(sliderG, EColorControlSliderG);
	iEikonEnv->CreateResourceReaderLC(reader, R_COLOR_SLIDER_G);
	sliderG->ConstructFromResourceL(this, iColor.Green(), reader);
	sliderG->HandleResourceChange(KEikMessageCaptionedControlEditableStateChange);
	sliderG->SetObserver(this);
	CleanupStack::PopAndDestroy(); // reader
	CleanupStack::Pop(sliderG);

	// fourth component - slider for Blue
	CAknSlider* sliderB = new (ELeave) CAknSlider;
	Components().AppendLC(sliderB, EColorControlSliderB);
	iEikonEnv->CreateResourceReaderLC(reader, R_COLOR_SLIDER_B);
	sliderB->ConstructFromResourceL(this, iColor.Blue(), reader);
	sliderB->HandleResourceChange(KEikMessageCaptionedControlEditableStateChange);
	sliderB->SetObserver(this);
	CleanupStack::PopAndDestroy(); // reader
	CleanupStack::Pop(sliderB);

	// draw preview over sliders
	Components().AppendLC(colorPreview, EColorControlPreview);
	colorPreview->ConstructL(this, &iInitialColor, &iColor);
	CleanupStack::Pop(colorPreview);

	// slider group is in focus
	Components().ControlById<CAknSlider>(EColorControlSliderR)->SetFocus(ETrue, ENoDrawNow);
	iActiveSlider = EColorControlSliderR;
	Components().ControlById<CColorPreview>(EColorControlPreview)->SetNonFocusing();

	TUid naviPaneUid = TUid::Uid(EEikStatusPaneUidNavi);
	CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
	CEikStatusPaneBase::TPaneCapabilities naviPaneCap = statusPane->PaneCapabilities(naviPaneUid);
	if (naviPaneCap.IsPresent() && naviPaneCap.IsAppOwned())
		{
		CAknNavigationControlContainer* naviPane =
				static_cast<CAknNavigationControlContainer*> (statusPane->ControlL(naviPaneUid));
		iNaviDecorator = naviPane->CreateNavigationLabelL(KNullDesC);
		naviPane->PushL(*iNaviDecorator);
		UpdateNaviLabelL();
		}

	ActivateL();
	}

void CColorControl::Draw(const TRect& /*aRect*/) const
	{
	// draw theme background
	TRect rect = Rect();
	CWindowGc& gc = SystemGc();
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	MAknsControlContext* cc = AknsDrawUtils::ControlContext(this);
	AknsDrawUtils::Background(skin, cc, this, gc, rect);
	}

void CColorControl::SizeChanged()
	{
	TRect rect = Rect();
	if(iBackground)
		iBackground->SetRect(rect);

	if (rect.Height() > rect.Width())
		{
		// portrait
		TRect previewRect;
		previewRect.SetSize(TSize(rect.Width(), rect.Height() / 4));
		Components().ControlById<CColorPreview>(EColorControlPreview)->SetRect(previewRect);
		TRect sliderRect = TRect(previewRect);
		// Default slider has an empty field at the left side.
		// Make a slider wider than a parent window and move empty part outside the window.
		// The size of the empty part was found manually.
		TInt emptyPart = (TInt)((TReal)iFont->HeightInPixels() * 5.6);
		sliderRect.SetWidth(sliderRect.Width() + emptyPart);
		sliderRect.Move(-emptyPart, sliderRect.Height());
		sliderRect.SetWidth(sliderRect.Width() - KControlMargins * 2);
		sliderRect.Move(KControlMargins, 0);
		Components().ControlById<CAknSlider>(EColorControlSliderR)->SetRect(sliderRect);
		sliderRect.Move(0, sliderRect.Height());
		Components().ControlById<CAknSlider>(EColorControlSliderG)->SetRect(sliderRect);
		sliderRect.Move(0, sliderRect.Height());
		Components().ControlById<CAknSlider>(EColorControlSliderB)->SetRect(sliderRect);
		}
	else
		{
		// landscape
		TRect previewRect;
		previewRect.SetSize(TSize(rect.Width() / 4, rect.Height()));
		Components().ControlById<CColorPreview>(EColorControlPreview)->SetRect(previewRect);
		TRect sliderRect = TRect(TSize(rect.Width(), rect.Height() / 3));
		TInt emptyPart = (TInt)((TReal)iFont->HeightInPixels() * 5.6);
		if (previewRect.Width() > emptyPart)
			{
			sliderRect.SetWidth(sliderRect.Width() - (previewRect.Width() - emptyPart));
			sliderRect.Move(previewRect.Width() - emptyPart, 0);
			}
		sliderRect.SetWidth(sliderRect.Width() - KControlMargins * 2);
		sliderRect.Move(KControlMargins, 0);
		Components().ControlById<CAknSlider>(EColorControlSliderR)->SetRect(sliderRect);
		sliderRect.Move(0, sliderRect.Height());
		Components().ControlById<CAknSlider>(EColorControlSliderG)->SetRect(sliderRect);
		sliderRect.Move(0, sliderRect.Height());
		Components().ControlById<CAknSlider>(EColorControlSliderB)->SetRect(sliderRect);
		}
	}

TTypeUid::Ptr CColorControl::MopSupplyObject(TTypeUid aId)
	{
	// support themes
	return iBackground != NULL ?
			MAknsControlContext::SupplyMopObject(aId, iBackground):
			CAknControl::MopSupplyObject(aId);
	}

TSize CColorControl::MinimumSize()
	{
	// this control covers whole main pane
	TSize mainPaneSize;
	AknLayoutUtils :: LayoutMetricsSize(
		AknLayoutUtils::EMainPane, mainPaneSize);
	return mainPaneSize;
	}

TKeyResponse CColorControl::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	TKeyResponse response;
	if(aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow)
		{
		// focus changing
		Components().ControlById<CAknSlider>(iActiveSlider)->SetFocus(EFalse, ENoDrawNow);
		switch (iActiveSlider)
			{
			case EColorControlSliderR:
				if (aKeyEvent.iCode == EKeyDownArrow)
					iActiveSlider = EColorControlSliderG;
				break;
			case EColorControlSliderG:
				if (aKeyEvent.iCode == EKeyDownArrow)
					iActiveSlider = EColorControlSliderB;
				else
					iActiveSlider = EColorControlSliderR;
				break;
			case EColorControlSliderB:
				if (aKeyEvent.iCode == EKeyUpArrow)
					iActiveSlider = EColorControlSliderG;
				break;
			default:
				iActiveSlider = EColorControlSliderG;
				break;
			}
		Components().ControlById<CAknSlider>(iActiveSlider)->SetFocus(ETrue, EDrawNow);
		response = EKeyWasConsumed;
		}
	else
		{
		// current slider proesses the event
		response = Components().ControlById<CAknSlider>(iActiveSlider)->OfferKeyEventL(aKeyEvent, aType);
		}

	return response;
	}

void CColorControl::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent aEventType)
	{
	// slider value has been changed
	if (aEventType == MCoeControlObserver::EEventStateChanged)
		{
		iColor.SetRed(Components().ControlById<CAknSlider>(EColorControlSliderR)->Value());
		iColor.SetGreen(Components().ControlById<CAknSlider>(EColorControlSliderG)->Value());
		iColor.SetBlue(Components().ControlById<CAknSlider>(EColorControlSliderB)->Value());
		DrawNow();
		UpdateNaviLabelL();
		}
	}

void CColorControl::UpdateNaviLabelL()
	{
	if (iNaviDecorator)
		{
		_LIT(KColorFormat, "#%02X%02X%02X/#%02X%02X%02X");
		TBuf<15> colorHex;
		colorHex.Format(
				KColorFormat,
				iInitialColor.Red(), iInitialColor.Green(), iInitialColor.Blue(),
				iColor.Red(), iColor.Green(), iColor.Blue());
		CAknNaviLabel* naviLabel = (CAknNaviLabel*)iNaviDecorator->DecoratedControl();
		naviLabel->SetTextL(colorHex);
		iNaviDecorator->DrawDeferred();
		}
	}

TRgb CColorControl::Color()
	{
	return iColor;
	}

/**
 * CColorDialog
 */

CColorDialog::CColorDialog(TRgb* aColor)
	:iColor(aColor)
	{
	}

SEikControlInfo CColorDialog::CreateCustomControlL(TInt aControlType)
	{
	SEikControlInfo controlInfo;
	controlInfo.iControl = NULL;
	controlInfo.iTrailerTextId = 0;
	controlInfo.iFlags = 0;

	if (aControlType == EColorControl)
		{
		controlInfo.iControl = new (ELeave) CColorControl;
		}

	return controlInfo;
	}

void CColorDialog::PreLayoutDynInitL()
	{
	CAknDialog::PreLayoutDynInitL();
	const TInt KColorControlId = 1;
	CCoeControl* ctrl = CreateLineByTypeL(KNullDesC, KColorControlId, EColorControl, NULL);
	iColorControl = static_cast<CColorControl*>(ctrl);
	iColorControl->ConstructL(this, *iColor);
	}

TBool CColorDialog::OkToExitL(TInt aButtonId)
	{
	if (aButtonId == EAknSoftkeyOk)
		*iColor = iColorControl->Color();
	return ETrue;
	}

/**
 * CColorEditorPreviewTimer
 */

CColorEditorPreviewTimer* CColorEditorPreviewTimer::NewL(MColorEditorPreviewTimerCallback& aCallback)
	{
	CColorEditorPreviewTimer* self = new (ELeave) CColorEditorPreviewTimer(aCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CColorEditorPreviewTimer::CColorEditorPreviewTimer(MColorEditorPreviewTimerCallback& aCallback)
	: CTimer(CActive::EPriorityStandard)
	,iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

void CColorEditorPreviewTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

void CColorEditorPreviewTimer::RunL()
	{
	iCallback.HandleTimedOut();
	}

/**
 * CColorEditorPreview
 */

CColorEditorPreview::~CColorEditorPreview()
	{
	iTimer->Cancel();
	delete iTimer;
	iTimer = NULL;
	}

void CColorEditorPreview::ConstructL(const CColorEditor* aParent)
	{
	iParent = aParent;
	// Create its own window.
	// This control will be drawn ower MFNE.
	CreateWindowL();
	SetContainerWindowL(*aParent);
	iTimer = CColorEditorPreviewTimer::NewL(*this);
	ActivateL();
	}

void CColorEditorPreview::Draw(const TRect& aRect) const
	{
	if (iParent)
		{
		CWindowGc& gc = SystemGc();
		gc.SetPenStyle(CGraphicsContext::ENullPen);
		gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		gc.SetBrushColor(iParent->Color());
		gc.DrawRect(aRect);
		}
	}

void CColorEditorPreview::HandleTimedOut()
	{
	DrawNow();
	}

void CColorEditorPreview::UpdateDeferred()
	{
	const TTimeIntervalMicroSeconds32 KPreviewDelay(1000);
	iTimer->Cancel();
	iTimer->After(KPreviewDelay);
	}


void CColorEditor::ConstructFromResourceL(TResourceReader& /*aResourceReader*/)
	{
	HBufC* delimiterText;
	CreateFieldArrayL(7); // 3 fields  + 3 delimiters + 1 place keeper
	// Separator cannot be the first field!
	// (MFNE panics on pen input starting)
	// R
	AddField(CEikMfneNumber::NewL(*Font(), KMinColor, KMaxColor, KMaxColor, 0));
	delimiterText = HBufC::NewLC(2);
	delimiterText->Des().Copy(KDelimiterR);
	AddField(CEikMfneSeparator::NewL(delimiterText));
	CleanupStack::Pop(); // delimiterText
	// G
	AddField(CEikMfneNumber::NewL(*Font(), KMinColor, KMaxColor, KMaxColor, 0));
	delimiterText = HBufC::NewLC(2);
	delimiterText->Des().Copy(KDelimiterG);
	AddField(CEikMfneSeparator::NewL(delimiterText));
	CleanupStack::Pop(); // delimiterText
	// B
	AddField(CEikMfneNumber::NewL(*Font(), KMinColor, KMaxColor, KMaxColor, 0));
	delimiterText = HBufC::NewLC(2);
	delimiterText->Des().Copy(KDelimiterB);
	AddField(CEikMfneSeparator::NewL(delimiterText));
	CleanupStack::Pop(); // delimiterText

	// Place keeper is a separator that is used
	// to give a place for color preview drawing.
	delimiterText = HBufC::NewLC(6);
	delimiterText->Des().Copy(KColorPlaceKeeper);
	AddField(CEikMfneSeparator::NewL(delimiterText));
	CleanupStack::Pop(); // delimiterText

	// array for components
	InitComponentArrayL();

	iPreview = new (ELeave) CColorEditorPreview;
	Components().AppendLC(iPreview);
	iPreview->ConstructL(this);
	CleanupStack::Pop(iPreview);

	}

TKeyResponse CColorEditor::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	TKeyResponse res = CEikMfne::OfferKeyEventL(aKeyEvent, aType);
	if (aType == EEventKey && aKeyEvent.iScanCode == EStdKeyDevice3)
		{
		SetColorWithDialog();
		res = EKeyWasConsumed;
		}
	iPreview->UpdateDeferred();
	return res;
	}

void CColorEditor::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	if (aPointerEvent.iType == TPointerEvent::EButton1Up &&
			!Rect().Contains(aPointerEvent.iPosition))
		SetColorWithDialog();
	else
		CEikMfne::HandlePointerEventL(aPointerEvent);
	iPreview->UpdateDeferred();
	}

TRgb CColorEditor::Color() const
	{
	TRgb color;
	color.SetRed((STATIC_CAST(CEikMfneNumber*, Field(KFieldR)))->Value());
	color.SetGreen((STATIC_CAST(CEikMfneNumber*, Field(KFieldG)))->Value());
	color.SetBlue((STATIC_CAST(CEikMfneNumber*, Field(KFieldB)))->Value());
	return color;
	}

void CColorEditor::SetColor(TRgb aColor)
	{
	if (aColor != Color())
		{
		(STATIC_CAST(CEikMfneNumber*, Field(KFieldR)))->SetValue(aColor.Red(), *Font());
		(STATIC_CAST(CEikMfneNumber*, Field(KFieldG)))->SetValue(aColor.Green(), *Font());
		(STATIC_CAST(CEikMfneNumber*, Field(KFieldB)))->SetValue(aColor.Blue(), *Font());
		}
	DrawNow();
	iPreview->UpdateDeferred();
	}

void CColorEditor::SetColorWithDialog()
	{
	TRgb c = Color();
	CColorDialog* dialog = new (ELeave) CColorDialog(&c);
	dialog->ExecuteLD(R_COLOR_DIALOG);
	SetColor(c);
	}

void CColorEditor::SizeChanged()
	{
	CEikMfne::SizeChanged();
	if (iPreview)
		{
		TRect previewRect = Rect();
		// Color preview is a square
		previewRect.SetWidth(previewRect.Height());
		// that is placed behind the MFNE
		previewRect.Move(MfneSize().iWidth - previewRect.Width(), 0);
		iPreview->SetRect(previewRect);
		}
	}

void CColorEditor::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent aEventType)
	{
	if (aEventType == MCoeControlObserver::EEventStateChanged)
		{
		// This handler is called before fields drawing so we have to
		// draw the preview control after some delay when fields will have been drawn.
		// It is a silly trick but it is simpler than reimplement a whole Mfne
		// (the Draw method is not virtual and cannot be overridden).
		iPreview->UpdateDeferred();
		}
	}

