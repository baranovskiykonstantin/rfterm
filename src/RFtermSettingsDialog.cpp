/*
 ============================================================================
 Name        : RFtermSettingsDialog.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Query of send data.
 ============================================================================
 */
#include <avkon.rsg>
#include <eikmenup.h>
#include <eikbtgpc.h>
#include <eikmfne.h>
#include <aknslider.h>
#include <aknpopupfieldtext.h> 
#include <eikspane.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <RFterm_0xae7f53fa.rsg>

#include "ColorControl.h"
#include "ColorControl.hrh"
#include "RFterm.hrh"
#include "RFtermConstants.h"
#include "RFtermSettingsDialog.h"

TInt CRFtermSettingsDialog::RunDlgLD(CRFtermSettings* aSettings)
	{
	CRFtermSettingsDialog* dlg = new (ELeave) CRFtermSettingsDialog(aSettings);
	CleanupStack::PushL(dlg);
	dlg->ConstructL(0); // Default menu items
	CleanupStack::Pop(dlg);
	return dlg->ExecuteLD(R_SETTINGS_DIALOG);
	}

CRFtermSettingsDialog::CRFtermSettingsDialog(CRFtermSettings* aSettings)
	: CAknForm()
	, iSettings(aSettings)
	{
	}

CRFtermSettingsDialog::~CRFtermSettingsDialog()
	{
	}

TBool CRFtermSettingsDialog::OkToExitL(TInt aButtonId)
	{
	TBool result = CAknForm::OkToExitL(aButtonId);

	if (aButtonId == EAknSoftkeySave)
		{
		ProcessCommandL(EAknFormCmdSave);
		}
	else if (aButtonId == EAknSoftkeyEdit)
		{
		StartEditing();
		}
	else if (aButtonId == EAknSoftkeyDone)
		{
		result = ETrue;
		}

	if (!IsEditable())
		{
		CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
		cba->SetCommandSetL(R_AVKON_SOFTKEYS_EDIT_DONE);
		cba->DrawNow();
		}

	return result;
	}

void CRFtermSettingsDialog::HandlePointerEventL(const TPointerEvent &aPointerEvent)
	{
	CAknForm::HandlePointerEventL(aPointerEvent);

	TInt KMaxDelta = 15;

	if (!IsEditable())
		{
		if (aPointerEvent.iType == TPointerEvent::EButton1Down)
			{
			iDownPointerPos = aPointerEvent.iPosition;
			}
		else if (aPointerEvent.iType == TPointerEvent::EButton1Up)
			{
			TPoint posDelta = iDownPointerPos - aPointerEvent.iPosition;
			if (Abs(posDelta.iX) < KMaxDelta && Abs(posDelta.iY) < KMaxDelta)
				StartEditing();
			}
		}
	}

TKeyResponse CRFtermSettingsDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if (aType == EEventKey && aKeyEvent.iScanCode == EStdKeyDevice3 && !IsEditable())
		{
		StartEditing();
		return EKeyWasConsumed;
		}
	return CAknForm::OfferKeyEventL(aKeyEvent, aType);
	}

void CRFtermSettingsDialog::PreLayoutDynInitL()
	{
	CAknForm::PreLayoutDynInitL();
	LoadFormDataL();
	}

void CRFtermSettingsDialog::LoadFormDataL()
	{
	TInt index;

	// Input
	if (iSettings->MessageAddendum().Compare(KCR) == 0)
		{
		index = 0;
		}
	else if (iSettings->MessageAddendum().Compare(KLF) == 0)
		{
		index = 1;
		}
	else if (iSettings->MessageAddendum().Compare(KCRLF) == 0)
		{
		index = 2;
		}
	else
		{
		index = 3;
		}
	CAknPopupFieldText* appendixControl =
		(CAknPopupFieldText*)Control(ESettingAppendix);
	appendixControl->SetCurrentValueIndex(index);

	CEikNumberEditor* historySizeControl = 
		(CEikNumberEditor*)Control(ESettingHistorySize);
	historySizeControl->SetNumber(iSettings->MessageHistorySize());

	index = ((TInt) iSettings->IsEchoEnabled());
	CAknPopupFieldText* echoControl =
		(CAknPopupFieldText*)Control(ESettingEcho);
	echoControl->SetCurrentValueIndex(index);

	// Output
	CAknSlider* fontSizeControl =
		(CAknSlider*)Control(ESettingFontSize);
	fontSizeControl->SetValueL(iSettings->FontSize());

	CAknSlider* tabSizeControl =
		(CAknSlider*)Control(ESettingTabSize);
	tabSizeControl->SetValueL(iSettings->TabSize());

	index = ((TInt) iSettings->FontAntialiasing());
	CAknPopupFieldText* fontAntialiasingControl =
		(CAknPopupFieldText*)Control(ESettingFontAntialiasing);
	fontAntialiasingControl->SetCurrentValueIndex(index);

	index = ((TInt) iSettings->DoWrapText());
	CAknPopupFieldText* wrapTextControl =
		(CAknPopupFieldText*)Control(ESettingWrapText);
	wrapTextControl->SetCurrentValueIndex(index);

	index = ((TInt) iSettings->CtrlCharMapping()) - 1;
	CAknPopupFieldText* mappingControl =
		(CAknPopupFieldText*)Control(ESettingMapping);
	mappingControl->SetCurrentValueIndex(index);

	index = ((TInt) iSettings->CodePage()) - 1;
	CAknPopupFieldText* codePageControl =
		(CAknPopupFieldText*)Control(ESettingCodePage);
	codePageControl->SetCurrentValueIndex(index);

	index = ((TInt) iSettings->DoSaveNotifies());
	CAknPopupFieldText* saveNotifiesControl =
		(CAknPopupFieldText*)Control(ESettingSaveNotifies);
	saveNotifiesControl->SetCurrentValueIndex(index);

	// Colors
	TRgb color = iSettings->BackgroundColor();
	CColorEditor* bgColorEditorControl =
		(CColorEditor*)Control(ESettingColorBackground);
	bgColorEditorControl->SetColor(color);

	color = iSettings->FontColor();
	CColorEditor* fontColorEditorControl =
		(CColorEditor*)Control(ESettingColorFont);
	fontColorEditorControl->SetColor(color);

	color = iSettings->CursorColor();
	CColorEditor* cursorColorEditorControl =
		(CColorEditor*)Control(ESettingColorCursor);
	cursorColorEditorControl->SetColor(color);

	color = iSettings->ScrollbarsColor();
	CColorEditor* sbColorEditorControl =
		(CColorEditor*)Control(ESettingColorScrollbars);
	sbColorEditorControl->SetColor(color);
	}

TBool CRFtermSettingsDialog::SaveFormDataL()
	{
	//Input
	CAknPopupFieldText* appendixControl =
		(CAknPopupFieldText*)Control(ESettingAppendix);
	switch (appendixControl->CurrentValueIndex())
		{
		case 0:
			{
			iSettings->SetMessageAddendum(KCR, EFalse);
			break;
			}
		case 1:
			{
			iSettings->SetMessageAddendum(KLF, EFalse);
			break;
			}
		case 2:
			{
			iSettings->SetMessageAddendum(KCRLF, EFalse);
			break;
			}
		default:
			{
			iSettings->SetMessageAddendum(KNullDesC, EFalse);
			}
		}

	CEikNumberEditor* historySizeControl = 
		(CEikNumberEditor*)Control(ESettingHistorySize);
	iSettings->SetMessageHistorySize(historySizeControl->Number(), EFalse);

	CAknPopupFieldText* echoControl =
		(CAknPopupFieldText*)Control(ESettingEcho);
	iSettings->EnableEcho(echoControl->CurrentValueIndex(), EFalse);

	// Output
	CAknSlider* fontSizeControl =
		(CAknSlider*)Control(ESettingFontSize);
	iSettings->SetFontSize(fontSizeControl->Value(), EFalse);

	CAknSlider* tabSizeControl =
		(CAknSlider*)Control(ESettingTabSize);
	iSettings->SetTabSize(tabSizeControl->Value(), EFalse);

	CAknPopupFieldText* fontAntialiasingControl =
		(CAknPopupFieldText*)Control(ESettingFontAntialiasing);
	iSettings->SetFontAntialiasing(fontAntialiasingControl->CurrentValueIndex(), EFalse);

	CAknPopupFieldText* wrapTextControl =
		(CAknPopupFieldText*)Control(ESettingWrapText);
	iSettings->SetTextWrapping(wrapTextControl->CurrentValueIndex(), EFalse);

	CAknPopupFieldText* mappingControl =
		(CAknPopupFieldText*)Control(ESettingMapping);
	iSettings->SetCtrlCharMapping((TCtrlCharMapping) (mappingControl->CurrentValueIndex() + 1), EFalse);

	CAknPopupFieldText* codePageControl =
		(CAknPopupFieldText*)Control(ESettingCodePage);
	iSettings->SetCodePage((TCodePage) (codePageControl->CurrentValueIndex() + 1), EFalse);

	CAknPopupFieldText* saveNotifiesControl =
		(CAknPopupFieldText*)Control(ESettingSaveNotifies);
	iSettings->SetNotifySaving(saveNotifiesControl->CurrentValueIndex(), EFalse);

	// Colors
	CColorEditor* bgColorEditorControl =
		(CColorEditor*)Control(ESettingColorBackground);
	iSettings->SetBackgroundColor(bgColorEditorControl->Color(), EFalse);

	CColorEditor* fontColorEditorControl =
		(CColorEditor*)Control(ESettingColorFont);
	iSettings->SetFontColor(fontColorEditorControl->Color(), EFalse);

	CColorEditor* cursorColorEditorControl =
		(CColorEditor*)Control(ESettingColorCursor);
	iSettings->SetCursorColor(cursorColorEditorControl->Color(), EFalse);

	CColorEditor* scrollbarsColorEditorControl =
		(CColorEditor*)Control(ESettingColorScrollbars);
	iSettings->SetScrollbarsColor(scrollbarsColorEditorControl->Color());

	// Notify settings change at the last item only!

	return ETrue;
	}

void CRFtermSettingsDialog::DoNotSaveFormDataL()
	{
	LoadFormDataL();
	}

void CRFtermSettingsDialog::ActivateL()
	{
	CAknForm::ActivateL();

	TUid naviPaneUid = TUid::Uid(EEikStatusPaneUidNavi);
	CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
	CEikStatusPaneBase::TPaneCapabilities naviPaneCap = statusPane->PaneCapabilities(naviPaneUid);
	if (naviPaneCap.IsPresent() && naviPaneCap.IsAppOwned())
		{
		CAknNavigationControlContainer* naviPane =
				static_cast<CAknNavigationControlContainer*> (statusPane->ControlL(naviPaneUid));
		CAknNavigationDecorator* naviDecorator = naviPane->Top();
		CAknTabGroup* tabs = (CAknTabGroup*)naviDecorator->DecoratedControl();
		tabs->SetTabFixedWidthL(KTabWidthWithThreeTabs);
		}
	}

void CRFtermSettingsDialog::StartEditing()
	{
	ProcessCommandL(EAknFormCmdEdit);
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	cba->SetCommandSetL(R_AVKON_SOFTKEYS_SAVE_BACK);
	cba->DrawNow();
	}

SEikControlInfo CRFtermSettingsDialog::CreateCustomControlL(TInt aControlType)
	{
	SEikControlInfo controlInfo;
	controlInfo.iControl = NULL;
	controlInfo.iTrailerTextId = 0;
	controlInfo.iFlags = 0;

	if (aControlType == EColorEditor)
		{
		controlInfo.iControl = new (ELeave) CColorEditor;
		}

	return controlInfo;
	}

MEikDialogPageObserver::TFormControlTypes
CRFtermSettingsDialog::ConvertCustomControlTypeToBaseControlType(TInt aControlType) const
	{
	if (aControlType == EColorEditor)
		return MEikDialogPageObserver::EMfneDerived;
	return CAknForm::ConvertCustomControlTypeToBaseControlType(aControlType);
	};

