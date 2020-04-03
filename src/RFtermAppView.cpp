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
#include <aknlistquerydialog.h>
#include <RFterm_0xae7f53fa.rsg>
#include "RFtermAppUi.h"
#include "RFtermAppView.h"
#include "RFtermConstants.h"
#include "RFtermTextQueryDialog.h"
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

	// Set the windows size
	SetRect(aRect);
	
	iRFtermOutput = CRFtermOutput::NewL(this, aRect);

	// It's needed for iRFtermOutput->UpdateCursor (SetTextCursor)
	iRFtermOutput->TextView()->SetDisplayContextL(
			static_cast<CBitmapDevice*> (CEikonEnv::Static()->ScreenDevice()),
			&Window(),
			&iEikonEnv->RootWin(),
			&iEikonEnv->WsSession()
	);

	iRFtermOutput->SetFocus(ETrue);
	
	iMessageHistoryArray = new (ELeave) CDesCArraySeg(4);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
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
	delete iRFtermOutput;
	iRFtermOutput = NULL;
	
	delete iMessageHistoryArray;
	iMessageHistoryArray = NULL;
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::CountComponentControls()
// Returns the specified component control.
// ----------------------------------------------------------------------------
//
TInt CRFtermAppView::CountComponentControls() const
	{
	return 1; // Only have one Component
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::ComponentControl()
// Gets the specified component of a compound control.
// ----------------------------------------------------------------------------
//
CCoeControl* CRFtermAppView::ComponentControl(TInt aIndex) const
	{
	__ASSERT_ALWAYS(aIndex == 0, Panic(ERFtermInvalidControlIndex));
	return iRFtermOutput;    //  Return the component
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::OfferKeyEventL()
// Offer the key event to the list box.
// ----------------------------------------------------------------------------
//
TKeyResponse CRFtermAppView
::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
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
	if (iRFtermOutput) 
		{
		// The next line is important!
		// Without it the custom text cursor gets disappeared on size changing.
		iEikonEnv->RootWin().CancelTextCursor();

		TRect clientRect = iAvkonAppUi->ClientRect(); 
		TRect outputRect(clientRect.Size());
		iRFtermOutput->SetRect(outputRect); 
		iRFtermOutput->UpdateVScrollBarL(ETrue);
		iRFtermOutput->UpdateCursorL();
		}

	DrawNow();
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

	if (TPointerEvent::EButton1Down == aPointerEvent.iType)
		{
		if (iRFtermOutput->SelectionLength())
			{
			// Do not show the send query if a text selection is present.
			iDownPointerPos.SetXY(-KMaxDelta, -KMaxDelta);
			
			}
		else
			{
			iDownPointerPos = aPointerEvent.iPosition;
			}
		}
	else if (TPointerEvent::EButton1Up == aPointerEvent.iType)
		{
		TPoint posDelta = iDownPointerPos - aPointerEvent.iPosition;
		if (Abs(posDelta.iX) < KMaxDelta && Abs(posDelta.iY) < KMaxDelta)
			{
			// Tap output to send
			iAvkonAppUi->HandleCommandL(EMessage);
			}
		}

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
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
	CleanupStack::PushL(dlg);

	dlg->SetMaxLength(KRFtermTextBufLength);

	CleanupStack::Pop(dlg);
	TBool answer(dlg->ExecuteLD(R_DIALOG_TEXT_QUERY));
	
	if (EAknSoftkeyOk == answer)
		{
		// get message
		aText = textData;

		if (textData.Length())
			{
			CRFtermAppUi* appUi = (CRFtermAppUi*)CEikonEnv::Static()->EikAppUi();
			TInt historySize = appUi->iSettings->iMessageHistorySize;

			if (historySize > 0)
				{
				if (historySize > 1)
					{
					TInt posOfCopy;
					if (iMessageHistoryArray->Find(textData, posOfCopy) == 0)
						{
						iMessageHistoryArray->Delete(posOfCopy);
						iMessageHistoryArray->Compress();
						}
					}

				if (iMessageHistoryArray->Count() == historySize)
					{
					iMessageHistoryArray->Delete(0);
					iMessageHistoryArray->Compress();
					}
				iMessageHistoryArray->AppendL(textData);
				}
			}

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
	TInt chosenItem;
	CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog(&chosenItem);
	dlg->PrepareLC(R_DIALOG_HISTORY_QUERY);
	dlg->SetItemTextArray(iMessageHistoryArray);
	dlg->SetOwnershipType(ELbmDoesNotOwnItemArray);
	TInt answer = dlg->RunLD(); 
	
	if (EAknSoftkeyOk == answer)
		{
		aText = iMessageHistoryArray->MdcaPoint(chosenItem);

		// Move the selected item to the end
		iMessageHistoryArray->Delete(chosenItem);
		iMessageHistoryArray->Compress();
		iMessageHistoryArray->AppendL(aText);

		return ETrue;
		}
	return EFalse;
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

// End of File
