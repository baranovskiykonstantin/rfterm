/*
 ============================================================================
 Name        : RFtermSignalQueryDialog.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Dialog with the list of RS-232 input/output signals.
 ============================================================================
 */

#include <e32std.h> // TCallBack
#include <aknlists.h>
#include <akniconarray.h>
#include <gulicon.h>
#include <akniconutils.h>
#include <stringloader.h>
#include <RFterm_0xae7f53fa.rsg>
#include <RFterm_0xae7f53fa.mbg>
#include "RFtermConstants.h"
#include "RFtermSignalQueryDialog.h"

// Signal labels
_LIT(KSignalLabelRTS, "RTS");
_LIT(KSignalLabelCTS, "CTS");
_LIT(KSignalLabelDSR, "DSR");
_LIT(KSignalLabelDTR, "DTR");
_LIT(KSignalLabelDCD, "DCD");
_LIT(KSignalLabelRI,  "RI");

// List item pattern
_LIT(KListItemPattern, "%d\t%S\t%S");

// -----------------------------------------------------------------------------
// CRFtermSignalQueryDialog::RunDlgLD()
// Shows RS-232 signals dialog and destroys it on exit.
// -----------------------------------------------------------------------------
//
TInt CRFtermSignalQueryDialog::RunDlgLD(CRFtermBt* aBtClient)
	{
	CRFtermSignalQueryDialog* dlg = new (ELeave) CRFtermSignalQueryDialog(aBtClient);
	dlg->PrepareLC(R_DIALOG_SIGNAL_QUERY);
	dlg->ConstructL();
	return dlg->RunLD();
	}

// -----------------------------------------------------------------------------
// CRFtermSignalQueryDialog::CRFtermSignalQueryDialog()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CRFtermSignalQueryDialog::CRFtermSignalQueryDialog(CRFtermBt* aBtClient)
	: CAknListQueryDialog(&iIndex)
	, iBtClient(aBtClient)
	{
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRFtermSignalQueryDialog::ConstructL()
	{
	CArrayPtr<CGulIcon>* iconList = new (ELeave) CAknIconArray(1);
	CleanupStack::PushL(iconList);

	CFbsBitmap* offBitmap = NULL;
	CFbsBitmap* offMask = NULL;
	CFbsBitmap* onBitmap = NULL;
	CFbsBitmap* onMask = NULL;

	AknIconUtils::CreateIconL(offBitmap, offMask, KRFtermIconsFile,
								EMbmRfterm_0xae7f53faSignal_off,
								EMbmRfterm_0xae7f53faSignal_off_mask);
	AknIconUtils::CreateIconL(onBitmap, onMask, KRFtermIconsFile,
								EMbmRfterm_0xae7f53faSignal_on,
								EMbmRfterm_0xae7f53faSignal_on_mask);

	// It takes ownership of the bitmap and mask.
	CGulIcon* offIcon = CGulIcon::NewL(offBitmap, offMask);
	CGulIcon* onIcon = CGulIcon::NewL(onBitmap, onMask);
	iconList->AppendL(offIcon);
	iconList->AppendL(onIcon);

	CAknSingleGraphicHeadingPopupMenuStyleListBox* listBox =
			static_cast<CAknSingleGraphicHeadingPopupMenuStyleListBox*>(ListBox());
	listBox->ItemDrawer()->ColumnData()->SetIconArray(iconList);
	UpdateSignalStatesL();
	
	CleanupStack::Pop(iconList);
	
	iTimer = CPeriodic::NewL(EPriorityLow);
	const TInt tickInterval = 1000000;
	iTimer->Start(tickInterval, tickInterval, TCallBack(TimerCallBack, this));
	}

// ----------------------------------------------------------------------------
// CRFtermSignalQueryDialog::~CRFtermSignalQueryDialog()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermSignalQueryDialog::~CRFtermSignalQueryDialog()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	iTimer = NULL;
	}

// ----------------------------------------------------------------------------
// CRFtermSignalQueryDialog::HandleListBoxEventL()
// Close dialog with "Back" button.
// ----------------------------------------------------------------------------
//
TBool CRFtermSignalQueryDialog::OkToExitL(TInt aButtonId)
	{
	if (aButtonId == EAknSoftkeyBack)
		{
		return ETrue;
		}

	return EFalse;
	}

// ----------------------------------------------------------------------------
// CRFtermSignalQueryDialog::HandleListBoxEventL()
// Toggle output if clicked.
// ----------------------------------------------------------------------------
//
void CRFtermSignalQueryDialog::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
	{
	if (aEventType == EEventItemClicked)
		{
		if (iBtClient->IsConnected())
			{
			// 0 -> RTS, 3 -> DTR; outputs only
			if (aListBox->CurrentItemIndex() == 0)
				{
				iBtClient->ToggleOutputSignals(KRS232SignalRTS);
				}
			else if (aListBox->CurrentItemIndex() == 3)
				{
				iBtClient->ToggleOutputSignals(KRS232SignalDTR);
				}
			}
		UpdateSignalStatesL();
		}
	}

// ----------------------------------------------------------------------------
// CRFtermSignalQueryDialog::UpdateSignalStatesL()
// Update signal states.
// ----------------------------------------------------------------------------
//
void CRFtermSignalQueryDialog::UpdateSignalStatesL()
	{
	CAknSingleGraphicHeadingPopupMenuStyleListBox* listBox =
			static_cast<CAknSingleGraphicHeadingPopupMenuStyleListBox*>(ListBox());
	CDesCArray* listArray = (CDesCArray*) listBox->Model()->ItemTextArray();
	listArray->Reset();

	if (iBtClient->IsConnected())
		{
		HBufC* inputLabel = StringLoader::LoadLC(R_STR_SIGNAL_INPUT);
		HBufC* outputLabel = StringLoader::LoadLC(R_STR_SIGNAL_OUTPUT);

		TBuf<16> listItemBuf;
		TInt signalState(0);

		TUint8 inputSignals = iBtClient->GetInputSignals();
		TUint8 outputSignals = iBtClient->GetOutputSignals();

		// RTS
		//(outputSignals & KRS232SignalRTS) ? signalState = 1 : signalState = 0;
		signalState = (outputSignals & KRS232SignalRTS) != 0;
		listItemBuf.Format(KListItemPattern, signalState, outputLabel, &KSignalLabelRTS);
		listArray->AppendL(listItemBuf);
		// CTS
		listItemBuf.Zero();
		signalState = (inputSignals & KRS232SignalCTS) != 0;
		listItemBuf.Format(KListItemPattern, signalState, inputLabel, &KSignalLabelCTS);
		listArray->AppendL(listItemBuf);
		// DSR
		listItemBuf.Zero();
		signalState = (inputSignals & KRS232SignalDSR) != 0;
		listItemBuf.Format(KListItemPattern, signalState, inputLabel, &KSignalLabelDSR);
		listArray->AppendL(listItemBuf);
		// DTR
		listItemBuf.Zero();
		signalState = (outputSignals & KRS232SignalDTR) != 0;
		listItemBuf.Format(KListItemPattern, signalState, outputLabel, &KSignalLabelDTR);
		listArray->AppendL(listItemBuf);
		// DCD
		listItemBuf.Zero();
		signalState = (inputSignals & KRS232SignalDCD) != 0;
		listItemBuf.Format(KListItemPattern, signalState, inputLabel, &KSignalLabelDCD);
		listArray->AppendL(listItemBuf);
		// RI
		listItemBuf.Zero();
		signalState = (inputSignals & KRS232SignalRI) != 0;
		listItemBuf.Format(KListItemPattern, signalState, inputLabel, &KSignalLabelRI);
		listArray->AppendL(listItemBuf);

		CleanupStack::PopAndDestroy(outputLabel);
		CleanupStack::PopAndDestroy(inputLabel);
		}
	listBox->HandleItemAdditionL();
	}

// ----------------------------------------------------------------------------
// CRFtermSignalQueryDialog::TimerCallBack()
// Update signal states every second.
// ----------------------------------------------------------------------------
//
TInt CRFtermSignalQueryDialog::TimerCallBack(TAny* aObject)
	{
	CRFtermSignalQueryDialog* self = static_cast<CRFtermSignalQueryDialog*> (aObject);
	TRAPD(error, self->UpdateSignalStatesL());
	return error;
	}

// ----------------------------------------------------------------------------
// CRFtermSignalQueryDialog::OfferKeyEventL()
// Offer the key event to the list box.
// ----------------------------------------------------------------------------
//
TKeyResponse CRFtermSignalQueryDialog
::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if (aType == EEventKey)
		{
		// "1" -> RTS
		if (aKeyEvent.iCode == 0x31)
			{
			if (iBtClient->IsConnected())
				{
				iBtClient->ToggleOutputSignals(KRS232SignalRTS);
				UpdateSignalStatesL();
				}
			}
		// "4" -> DTR
		else if (aKeyEvent.iCode == 0x34)
			{
			if (iBtClient->IsConnected())
				{
				iBtClient->ToggleOutputSignals(KRS232SignalDTR);
				UpdateSignalStatesL();
				}
			}
		}
	return CAknListQueryDialog::OfferKeyEventL(aKeyEvent, aType);
	}

// End of File
