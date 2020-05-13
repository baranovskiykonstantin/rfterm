/*
 ============================================================================
 Name        : RFtermAppView.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include <aknappui.h>
#include <stringloader.h>
#include <aknquerydialog.h>
#include <aknlistquerydialog.h>
#include <eikspane.h>
#include <aknnavilabel.h>
#include <RFterm_0xae7f53fa.rsg>
#include "RFtermAppUi.h"
#include "RFtermAppView.h"
#include "RFtermConstants.h"
#include "RFtermTextQueryDialog.h"
#include "RFtermCodePages.h"
#include "RFterm.pan"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRFtermAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRFtermAppView* CRFtermAppView::NewL(const TRect& aRect)
	{
	CRFtermAppView* self = CRFtermAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRFtermAppView* CRFtermAppView::NewLC(const TRect& aRect)
	{
	CRFtermAppView* self = new (ELeave) CRFtermAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	iRFtermScrollBars = new (ELeave) CRFtermScrollBars(this);
	iRFtermScrollBars->SetPosition(TPoint(0, 0));
	iRFtermScrollBars->SetFocusing(EFalse);

	iRFtermOutput = CRFtermOutput::NewL(this);
	iRFtermOutput->SetPosition(TPoint(0, 0));
	iRFtermOutput->SetFocus(ETrue);

	iRFtermScrollBars->SetObserver(iRFtermOutput);
	iRFtermOutput->SetObserver(iRFtermScrollBars);

	// It's needed for iRFtermOutput->UpdateCursor (SetTextCursor)
//	iRFtermOutput->TextView()->SetDisplayContextL(
//			static_cast<CBitmapDevice*> (CEikonEnv::Static()->ScreenDevice()),
//			&Window(),
//			&iEikonEnv->RootWin(),
//			&iEikonEnv->WsSession()
//	);

	iMessageHistoryArray = new (ELeave) CDesCArraySeg(4);

	TUid naviPaneUid = TUid::Uid(EEikStatusPaneUidNavi);
	CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
	CEikStatusPaneBase::TPaneCapabilities naviPaneCap = statusPane->PaneCapabilities(naviPaneUid);
	if (naviPaneCap.IsPresent() && naviPaneCap.IsAppOwned())
		{
		CAknNavigationControlContainer* naviPane =
				static_cast<CAknNavigationControlContainer*> (statusPane->ControlL(naviPaneUid));
		HBufC* naviLabel = StringLoader::LoadLC(R_STR_NAVI_DISCONNECTED);
		iNaviDecorator = naviPane->CreateNavigationLabelL(*naviLabel);
		naviPane->PushL(*iNaviDecorator);
		CleanupStack::PopAndDestroy(naviLabel);
		}

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();

	iRFtermOutput->ClearL();
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::CRFtermAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CRFtermAppView::CRFtermAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::~CRFtermAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CRFtermAppView::~CRFtermAppView()
	{
	delete iMessageHistoryArray;
	iMessageHistoryArray = NULL;

	if (iRFtermScrollBars)
		{
		iRFtermScrollBars->SetObserver(NULL);
		}

	if (iRFtermOutput)
		{
		iRFtermOutput->SetObserver(NULL);
		}

	delete iRFtermOutput;
	iRFtermOutput = NULL;
	
	delete iRFtermScrollBars;
	iRFtermScrollBars = NULL;

	delete iNaviDecorator;
	iNaviDecorator = NULL;
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::CountComponentControls()
// Returns the specified component control.
// ----------------------------------------------------------------------------
//
TInt CRFtermAppView::CountComponentControls() const
	{
	return 2; // iRFtermScrollBars, iRFtermOutput
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::ComponentControl()
// Gets the specified component of a compound control.
// ----------------------------------------------------------------------------
//
CCoeControl* CRFtermAppView::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
		case ERFtermAppViewScrollBars:
			{
			return iRFtermScrollBars;
			}
		case ERFtermAppViewOutput:
			{
			return iRFtermOutput;
			}
		default:
			{
			Panic(ERFtermInvalidControlIndex);
			}
		}
	return NULL; // This never be reached.
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::OfferKeyEventL()
// Offer the key event to the list box.
// ----------------------------------------------------------------------------
//
TKeyResponse CRFtermAppView
::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if (aType == EEventKey && aKeyEvent.iScanCode == EStdKeyDevice3)
		{
		// Press directional rocker to send
		iAvkonAppUi->HandleCommandL(EMessage);
		}
	return iRFtermOutput->OfferKeyEventL(aKeyEvent, aType);
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::Draw(const TRect& aRect) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(aRect);

	// Clears the screen
	gc.SetBrushColor(KRgbBlack);
	gc.Clear(drawRect);
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::SizeChanged()
	{
	// The next line is important!
	// Without it the custom text cursor gets disappeared on size changing.
	iEikonEnv->RootWin().CancelTextCursor();

	TRect controlRect = Rect();
	if (iRFtermScrollBars)
		{
		iRFtermScrollBars->SetRect(controlRect);
		iRFtermScrollBars->GetFreeRect(controlRect);
		}
	if (iRFtermOutput)
		{
		iRFtermOutput->UpdateRect(controlRect); 
		}

	DrawNow();
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::FocusChanged()
// Called by framework when the focus of the view is changed.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::FocusChanged(TDrawNow aDrawNow)
	{
	if (iRFtermOutput)
		{
		iRFtermOutput->SetFocus(IsFocused(), aDrawNow);
		}
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	TInt KMaxDelta = 15;

	TPointerEvent pointerEvent = aPointerEvent;

	// Scrollbars are too thin to be tapped on the touch screen,
	// so part of the output control is used to.
	TRect vScrollBarArea = Rect();
	vScrollBarArea.iTl.iX = vScrollBarArea.iBr.iX - KRFtermScrollBarSensorBreadth;
	TRect hScrollBarArea = Rect();
	hScrollBarArea.iTl.iY = hScrollBarArea.iBr.iY - KRFtermScrollBarSensorBreadth;
	if (iRFtermScrollBars->IsVScrollBarVisible() &&
			vScrollBarArea.Contains(aPointerEvent.iPosition))
		{
		// Shift pointer event position to the middle of the vert scrollbar
		pointerEvent.iPosition.iX = vScrollBarArea.iBr.iX - (KRFtermScrollBarBreadth / 2);
		}
	else if (iRFtermScrollBars->IsHScrollBarVisible() &&
			hScrollBarArea.Contains(aPointerEvent.iPosition))
		{
		// Shift pointer event position to the middle of the horiz scrollbar
		pointerEvent.iPosition.iY = hScrollBarArea.iBr.iY - (KRFtermScrollBarBreadth / 2);
		}

	if (TPointerEvent::EButton1Down == pointerEvent.iType &&
			iRFtermOutput->Rect().Contains(pointerEvent.iPosition))
		{
		if (iRFtermOutput->SelectionLength())
			{
			// Do not show the send query if a text selection is present.
			iDownPointerPos.SetXY(-KMaxDelta, -KMaxDelta);
			
			}
		else
			{
			iDownPointerPos = pointerEvent.iPosition;
			}
		}
	else if (TPointerEvent::EButton1Up == pointerEvent.iType)
		{
		TPoint posDelta = iDownPointerPos - pointerEvent.iPosition;
		if (Abs(posDelta.iX) < KMaxDelta && Abs(posDelta.iY) < KMaxDelta)
			{
			// Tap output to send
			iAvkonAppUi->HandleCommandL(EMessage);
			return;
			}
		}

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(pointerEvent);
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::ShowTextQueryL()
// Display of Text Query.
// ----------------------------------------------------------------------------
//
TBool CRFtermAppView::ShowTextQueryL(const TDesC& aInitialText, TDes& aText)
	{
	TBuf<KRFtermTextBufLength> textData(aInitialText);
	
	CRFtermTextQueryDialog* dlg = CRFtermTextQueryDialog::NewL(textData);

	dlg->SetMaxLength(KRFtermTextBufLength);

	TBool answer(dlg->ExecuteLD(R_DIALOG_TEXT_QUERY));
	
	if (EAknSoftkeyOk == answer)
		{
		HBufC* normalText = HBufC::NewLC(textData.Length());
		const TInt KBadSymNum = 11;
		TBuf<KBadSymNum> badSymbols;

		// Encode the message accordingly to the current code page.
		// Unsupported chars are ignored!
		TPtrC codePage;
		iRFtermOutput->GetCurrentCodePage(codePage);
		for (TInt i = 0; i < textData.Length(); i++)
			{
			TChar ch = textData[i];
			if ((TUint)ch < 0x80)
				{
				// ASCII
				normalText->Des().Append(ch);
				}
			else
				{
				TInt pos = codePage.Locate(ch);
				if (pos != KErrNotFound)
					{
					// Extended ASCII (code page)
					TChar chNormal(0x80 + pos);
					normalText->Des().Append(chNormal);
					}
				else
					{
					if (badSymbols.Length() < (KBadSymNum - 1))
						{
						badSymbols.Append(ch);
						}
					else if (badSymbols.Length() == (KBadSymNum - 1))
						{
						badSymbols.Append(TChar(0x2026)); // Ellipsis
						}
					}
				}
			}

		if (badSymbols.Length())
			{
			HBufC* confirmResource = StringLoader::LoadLC(R_STR_IGNORESYM_CONFIRMATION);
			HBufC* confirmPrompt = HBufC::NewLC(confirmResource->Length() + badSymbols.Length());
			confirmPrompt->Des().Format(*confirmResource, &badSymbols);

			CAknQueryDialog* confirmDlg = new (ELeave) CAknQueryDialog();
			TBool doIgnore = confirmDlg->ExecuteLD(R_DIALOG_IGNORESYM_CONFIRMATION_QUERY, *confirmPrompt);

			CleanupStack::PopAndDestroy(2); // confirmResource, confirmPrompt

			if (!doIgnore)
				{
				// Cancel sending message
				CleanupStack::PopAndDestroy(normalText);
				return EFalse;
				}
			}

		// get message
		aText = *normalText;

		// add message to history
		if (normalText->Length() && iMessageHistorySize > 0)
			{
			if (iMessageHistorySize > 1)
				{
				TInt posOfCopy;
				if (iMessageHistoryArray->Find(*normalText, posOfCopy) == 0)
					{
					iMessageHistoryArray->Delete(posOfCopy);
					iMessageHistoryArray->Compress();
					}
				}

			if (iMessageHistoryArray->Count() == iMessageHistorySize)
				{
				iMessageHistoryArray->Delete(0);
				iMessageHistoryArray->Compress();
				}
			iMessageHistoryArray->AppendL(*normalText);
			}

		CleanupStack::PopAndDestroy(normalText);

		return ETrue;
		}

	return EFalse;
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::ShowHistoryQueryL()
// Choose a message form history
// ----------------------------------------------------------------------------
//
TBool CRFtermAppView::ShowHistoryQueryL(TDes& aText)
	{
	TBool result(EFalse);
	TInt chosenItem;
	CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog(&chosenItem);
	dlg->PrepareLC(R_DIALOG_HISTORY_QUERY);
	dlg->SetItemTextArray(iMessageHistoryArray);
	dlg->SetOwnershipType(ELbmDoesNotOwnItemArray);
	TInt answer = dlg->RunLD(); 

	if (EAknSoftkeyOk == answer)
		{
		TBuf<KRFtermTextBufLength> messageFromHistory;
		messageFromHistory = iMessageHistoryArray->MdcaPoint(chosenItem);
		result = ShowTextQueryL(messageFromHistory, aText);
		}

	return result;
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::ShowCtrlCharQueryL()
// Choose a control character to send
// ----------------------------------------------------------------------------
//
TBool CRFtermAppView::ShowCtrlCharQueryL(TDes& aText)
	{
	TInt chosenItem;
	CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog(&chosenItem);
	dlg->PrepareLC(R_DIALOG_CTRLCHAR_QUERY);
	TInt answer = dlg->RunLD(); 
	
	if (EAknSoftkeyOk == answer)
		{
		switch (chosenItem)
			{
			case 0:
				{
				aText = KCR;
				break;
				}
			case 1:
				{
				aText = KLF;
				break;
				}
			case 2:
				{
				aText = KTB;
				break;
				}
			case 3:
				{
				aText = KBS;
				break;
				}
			case 4:
				{
				aText = KBL;
				break;
				}
			case 5:
				{
				aText = KFF;
				break;
				}
			case 6:
				{
				aText = KNL;
				break;
				}
			case 7:
				{
				aText = KCRLF;
				break;
				}
			case 8:
				{
				// Custom control character
				TInt ctrlCharInt;
				if (ShowIntQueryL(ctrlCharInt))
					{
					TChar ctrlChar(ctrlCharInt);
					TBuf<1> ctrlCharText;
					ctrlCharText.Append(ctrlChar);
					aText = ctrlCharText;
					}
				else
					{
					return EFalse;
					}
				break;
				}
			default:
				{
				Panic(ERFtermInputBadCtrlChar);
				}
			}

		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::ShowIntQueryL()
// Display of Integer Query.
// ----------------------------------------------------------------------------
//
TBool CRFtermAppView::ShowIntQueryL(TInt& aInt)
	{
	TInt intValue(0);
	CAknNumberQueryDialog* dlg = CAknNumberQueryDialog::NewL(intValue);
	TInt answer = dlg->ExecuteLD(R_DIALOG_INT_QUERY);

	if (EAknSoftkeyOk == answer)
		{
		aInt = intValue;
		return ETrue;
		}

	return EFalse;
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::HandleSettingsChange()
// Update used settings to new values.
// ----------------------------------------------------------------------------
//
void CRFtermAppView::HandleSettingsChange(const CRFtermSettings* aSettings)
	{
	iMessageHistorySize = aSettings->MessageHistorySize();
	// Message history size
	TInt sizeDiff = iMessageHistoryArray->Count() - iMessageHistorySize;
	if (sizeDiff > 0)
		{
		iMessageHistoryArray->Delete(0, sizeDiff);
		iMessageHistoryArray->Compress();
		}

	// Output code page
	if (iRFtermOutput)
		{
		iRFtermOutput->ChangeCodePage(aSettings->CodePage());
		iRFtermOutput->SetFontSizeL(aSettings->FontSize());
		iRFtermOutput->SetCtrlCharMapping(aSettings->CtrlCharMapping());
		iRFtermOutput->SetTabSize(aSettings->TabSize());
		}
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::HandleBtDeviceChangeL()
// Update the information about connected BT device on navigation pane.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::HandleBtDeviceChangeL(CBTDevice* aRemoteDevice)
	{
	_LIT(KPrepend, " (");
	_LIT(KByteSeparator, ":");
	_LIT(KAppend, ")");

	if (aRemoteDevice)
		{
		if (aRemoteDevice->DeviceName().Length())
			{
			TBuf<20> addr;
			aRemoteDevice->BDAddr().GetReadable(addr, KPrepend, KByteSeparator, KAppend);
			addr.UpperCase();

			HBufC* label = HBufC::NewLC(aRemoteDevice->DeviceName().Length() + addr.Length());
			label->Des().Copy(aRemoteDevice->DeviceName());
			label->Des().Append(addr);
			SetNaviLabelL(*label);
			CleanupStack::PopAndDestroy(label);
			}
		else
			{
			TBuf<20> addr;
			aRemoteDevice->BDAddr().GetReadable(addr, KNullDesC, KByteSeparator, KNullDesC);
			addr.UpperCase();
			SetNaviLabelL(addr);
			}
		}
	else
		{
		HBufC* label = StringLoader::LoadLC(R_STR_NAVI_DISCONNECTED);
		SetNaviLabelL(*label);
		CleanupStack::PopAndDestroy(label);
		}
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::HandleBtNotifyL()
// Show log message from BT client.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::HandleBtNotifyL(const TDesC& aMessage)
	{
	if (iRFtermOutput)
		{
		iRFtermOutput->AppendMessageL(aMessage);
		}
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::HandleBtDataL()
// Handle received data from BT client.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::HandleBtDataL(const TDesC& aData)
	{
	if (iRFtermOutput)
		{
		iRFtermOutput->AppendTextL(aData);
		}
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::SetNaviLabelL()
// Set label text on navigation pane of status bar.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::SetNaviLabelL(const TDesC& aText)
	{
	if (iNaviDecorator)
		{
		CAknNaviLabel* naviLabel = (CAknNaviLabel*)iNaviDecorator->DecoratedControl();
		naviLabel->SetTextL(aText);
		iNaviDecorator->DrawDeferred();
		}
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::MakeNaviLabelVisible()
// Show or hide the label on navigation pane.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::MakeNaviLabelVisible(TBool aVisible)
	{
	if (iNaviDecorator)
		{
		CAknNaviLabel* naviLabel = (CAknNaviLabel*)iNaviDecorator->DecoratedControl();
		naviLabel->MakeVisible(aVisible);
		}
	}

// End of File
