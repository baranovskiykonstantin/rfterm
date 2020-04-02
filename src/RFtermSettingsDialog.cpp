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
	if (iSettings->iMessageAddendum.Compare(KCR) == 0)
		{
		index = 0;
		}
	else if (iSettings->iMessageAddendum.Compare(KLF) == 0)
		{
		index = 1;
		}
	else if (iSettings->iMessageAddendum.Compare(KCRLF) == 0)
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
	historySizeControl->SetNumber(iSettings->iMessageHistorySize);

	index = ((TInt) iSettings->iEcho);
	CAknPopupFieldText* echoControl =
		(CAknPopupFieldText*)Control(ESettingEcho);
	echoControl->SetCurrentValueIndex(index);

	// Output
	index = ((TInt) iSettings->iCtrlCharMapping) - 1;
	CAknPopupFieldText* mappingControl =
		(CAknPopupFieldText*)Control(ESettingMapping);
	mappingControl->SetCurrentValueIndex(index);

	CAknSlider* tabSizeControl =
		(CAknSlider*)Control(ESettingTabSize);
	tabSizeControl->SetValueL(iSettings->iTabSize);

	index = ((TInt) iSettings->iCodePage) - 1;
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
			iSettings->iMessageAddendum = KCR;
			break;
			}
		case 1:
			{
			iSettings->iMessageAddendum = KLF;
			break;
			}
		case 2:
			{
			iSettings->iMessageAddendum = KCRLF;
			break;
			}
		default:
			{
			iSettings->iMessageAddendum = KNullDesC;
			}
		}

	CEikNumberEditor* historySizeControl = 
		(CEikNumberEditor*)Control(ESettingHistorySize);
	iSettings->iMessageHistorySize = historySizeControl->Number();

	CAknPopupFieldText* echoControl =
		(CAknPopupFieldText*)Control(ESettingEcho);
	iSettings->iEcho = (TBool) (echoControl->CurrentValueIndex());

	// Output
	CAknPopupFieldText* mappingControl =
		(CAknPopupFieldText*)Control(ESettingMapping);
	iSettings->iCtrlCharMapping = (TCtrlCharMapping) (mappingControl->CurrentValueIndex() + 1);

	CAknSlider* tabSizeControl =
		(CAknSlider*)Control(ESettingTabSize);
	iSettings->iTabSize = tabSizeControl->Value();

	CAknPopupFieldText* codePageControl =
		(CAknPopupFieldText*)Control(ESettingCodePage);
	iSettings->iCodePage = (TCodePage) (codePageControl->CurrentValueIndex() + 1);
	
	return ETrue;
	}

void CRFtermSettingsDialog::DoNotSaveFormDataL()
	{
	LoadFormDataL();
	}

