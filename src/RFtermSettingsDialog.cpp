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

#include "RFtermSettingsDialog.h"
#include "RFterm.hrh"
#include "RFtermConstants.h"

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
		ProcessCommandL(EAknFormCmdEdit);
		CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
		cba->SetCommandSetL(R_AVKON_SOFTKEYS_SAVE_BACK);
		cba->DrawNow();
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
				{
				ProcessCommandL(EAknFormCmdEdit);
				CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
				cba->SetCommandSetL(R_AVKON_SOFTKEYS_SAVE_BACK);
				cba->DrawNow();
				}
			}
		}
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

	index = ((TInt) iSettings->CtrlCharMapping()) - 1;
	CAknPopupFieldText* mappingControl =
		(CAknPopupFieldText*)Control(ESettingMapping);
	mappingControl->SetCurrentValueIndex(index);

	index = ((TInt) iSettings->CodePage()) - 1;
	CAknPopupFieldText* codePageControl =
		(CAknPopupFieldText*)Control(ESettingCodePage);
	codePageControl->SetCurrentValueIndex(index);
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
			iSettings->SetMessageAddendum(KCR);
			break;
			}
		case 1:
			{
			iSettings->SetMessageAddendum(KLF);
			break;
			}
		case 2:
			{
			iSettings->SetMessageAddendum(KCRLF);
			break;
			}
		default:
			{
			iSettings->SetMessageAddendum(KNullDesC);
			}
		}

	CEikNumberEditor* historySizeControl = 
		(CEikNumberEditor*)Control(ESettingHistorySize);
	iSettings->SetMessageHistorySize(historySizeControl->Number());

	CAknPopupFieldText* echoControl =
		(CAknPopupFieldText*)Control(ESettingEcho);
	iSettings->EnableEcho(echoControl->CurrentValueIndex());

	// Output
	CAknSlider* fontSizeControl =
		(CAknSlider*)Control(ESettingFontSize);
	iSettings->SetFontSize(fontSizeControl->Value());

	CAknSlider* tabSizeControl =
		(CAknSlider*)Control(ESettingTabSize);
	iSettings->SetTabSize(tabSizeControl->Value());

	CAknPopupFieldText* mappingControl =
		(CAknPopupFieldText*)Control(ESettingMapping);
	iSettings->SetCtrlCharMapping((TCtrlCharMapping) (mappingControl->CurrentValueIndex() + 1));

	CAknPopupFieldText* codePageControl =
		(CAknPopupFieldText*)Control(ESettingCodePage);
	iSettings->SetCodePage((TCodePage) (codePageControl->CurrentValueIndex() + 1));
	
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
		tabs->SetTabFixedWidthL(KTabWidthWithTwoTabs);
		}
	}

