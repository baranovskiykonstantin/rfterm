/*
 ============================================================================
 Name        : RFtermAppUi.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : CRFtermAppUi implementation
 ============================================================================
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>
#include <caknmemoryselectiondialogmultidrive.h>
#include <caknfileselectiondialog.h>

#include <RFterm_0xae7f53fa.rsg>

#ifdef _HELP_AVAILABLE_
#include "RFterm_0xae7f53fa.hlp.hrh"
#endif
#include "RFterm.hrh"
#include "RFterm.pan"
#include "RFtermApplication.h"
#include "RFtermAppUi.h"
#include "RFtermAppView.h"
#include "RFtermBt.h"
#include "RFtermConstants.h"
#include "RFtermSettingsDialog.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CRFtermAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRFtermAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);

	// Create view object
	iAppView = CRFtermAppView::NewL(ClientRect());
	AddToStackL(iAppView);

	iBtClient = CRFtermBt::NewL();
	iBtClient->SetObserver(iAppView);

	// check whether BT is available or not

	RSdp sdpSession;

	if (sdpSession.Connect() == KErrNone)
		{
		sdpSession.Close();
		
		iBtAvailable = ETrue;
		}
	else
		{
		iBtAvailable = EFalse;
		}
	
	// Settings
	// Prepare private path
	RFs& fsSession = iEikonEnv->FsSession();
	User::LeaveIfError(fsSession.CreatePrivatePath(EDriveC));
	User::LeaveIfError(fsSession.PrivatePath(iSettingsFileName));
	iSettingsFileName += KSettingsFile;
	iSettings = CRFtermSettings::NewL();
	iSettings->AddObserver(iAppView);
	// Read settings from stream
	InternalizeSettingsL();
	}
// -----------------------------------------------------------------------------
// CRFtermAppUi::CRFtermAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CRFtermAppUi::CRFtermAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CRFtermAppUi::~CRFtermAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CRFtermAppUi::~CRFtermAppUi()
	{
	delete iBtClient;
	iBtClient = NULL;

	if (iAppView)
		{
		RemoveFromStack(iAppView);
		iSettings->RemoveObserver(iAppView);
		delete iAppView;
		iAppView = NULL;
		}

	delete iSettings;
	iSettings = NULL;
	}

// -----------------------------------------------------------------------------
// CRFtermAppUi::InternalizeSettingsL()
// Load settings.
// -----------------------------------------------------------------------------
//
void CRFtermAppUi::InternalizeSettingsL() 
	{
	RFs& fs = iEikonEnv->FsSession();
	RFileReadStream readStream;
	TInt error = readStream.Open(fs, iSettingsFileName, EFileRead);
	TInt internalizationError = KErrNone;
	// If file existed, try to read settings.
	if (error == KErrNone)
		{
		TRAP(internalizationError, iSettings->LoadL(readStream);)
		}
	else
		{
		// Use default values in first time when no 
		// setting file exists
		iSettings->SetDefaultValues();
		}
	readStream.Release();
	// Reading failed, settings file might be corrupted.
	if (internalizationError != KErrNone)
		{
		User::LeaveIfError(fs.Delete(iSettingsFileName));
		iSettings->SetDefaultValues();
		}
	}

// -----------------------------------------------------------------------------
// CRFtermAppUi::ExternalizeSettingsL()
// Save settings.
// -----------------------------------------------------------------------------
//
void CRFtermAppUi::ExternalizeSettingsL() const 

	{
	RFs& fs = iEikonEnv->FsSession();
	RFileWriteStream writeStream;
	TInt error = writeStream.Open(fs, iSettingsFileName, EFileWrite);
	// Setting file did not exist, create one.
	if (error != KErrNone)
		{
		User::LeaveIfError(writeStream.Create(fs, iSettingsFileName, EFileWrite));
		}
	writeStream.PushL();
	iSettings->SaveL(writeStream);
	writeStream.CommitL();
	writeStream.Pop();
	writeStream.Release();
	}

// ----------------------------------------------------
// CRFtermAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode aType)
// takes care of key event handling
// ----------------------------------------------------
//
TKeyResponse CRFtermAppUi::HandleKeyEventL(
	const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType == EEventKeyDown)
		{
		switch (aKeyEvent.iScanCode)
			{
			case EStdKeyEnter:
				HandleCommandL(ERFtermMessage);
				break;
			}
		}
	
	return EKeyWasNotConsumed;
	}

// ------------------------------------------------------------------------------
// CRFtermAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CRFtermAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
	if (aResourceId == R_MENU)
		{
		if (!iBtClient->IsReadyToSend())
			{
			aMenuPane->SetItemDimmed(ERFtermSend, ETrue);
			}
		CRFtermOutput* output = static_cast<CRFtermOutput*>(
				iAppView->ComponentControl(ERFtermAppViewOutput));
		if (output->IsEmpty())
			{
			aMenuPane->SetItemDimmed(ERFtermClear, ETrue);
			aMenuPane->SetItemDimmed(ERFtermSave, ETrue);
			}
		if (iBtClient->IsConnected() || iBtClient->IsConnecting())
			{
			aMenuPane->SetItemDimmed(ERFtermConnect, ETrue);
			aMenuPane->SetItemDimmed(ERFtermStart, ETrue);
			if (iBtClient->Server())
				{
				aMenuPane->SetItemDimmed(ERFtermDisconnect, ETrue);
				}
			else
				{
				aMenuPane->SetItemDimmed(ERFtermStop, ETrue);
				}
			}
		else
			{
			aMenuPane->SetItemDimmed(ERFtermDisconnect, ETrue);
			aMenuPane->SetItemDimmed(ERFtermStop, ETrue);
			}
		}

	else if (aResourceId == R_SEND_MENU)
		{
		if (iAppView->iMessageHistoryArray->Count() == 0)
			{
			aMenuPane->SetItemDimmed(ERFtermHistory, ETrue);
			}
		}
	}

// -----------------------------------------------------------------------------
// CRFtermAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CRFtermAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			{
			if (iBtClient->IsConnected() || iBtClient->IsConnecting())
				{
				if (iBtClient->Server())
					{
					iBtClient->StopL();
					}
				else
					{
					iBtClient->DisconnectL();
					}
				}

			ExternalizeSettingsL();
			Exit();
			break;
			}

		case ERFtermConnect:
			{
			if (!iBtAvailable)
				{
				ShowBTNotAvailableNoteL();
				}
			else
				{
				iBtClient->ConnectL();
				}
			break;
			}

		case ERFtermDisconnect:
			{
			if (iBtClient->IsConnected())
				{
				iBtClient->DisconnectL(); 
				}
			break;
			}

		case ERFtermStart:
			{
			if (!iBtAvailable)
				{
				ShowBTNotAvailableNoteL();
				}
			else
				{
				iBtClient->StartL();
				}
			break;
			}

		case ERFtermStop:
			{
			if (!iBtAvailable)
				{
				ShowBTNotAvailableNoteL();
				}
			else
				{
				iBtClient->StopL();
				}
			break;
			}

		case ERFtermSend:
			{
			break;
			}

		case ERFtermMessage:
			{
			if(iBtClient->IsReadyToSend())
				{
				TBuf<KRFtermTextBufLength> text;
				if (iAppView->ShowTextQueryL(KNullDesC, text))
					{
					HBufC* message = HBufC::NewLC(text.Length() + iSettings->MessageAddendum().Length());
					message->Des().Copy(text);
					message->Des().Append(iSettings->MessageAddendum());

					iBtClient->SendMessageL(*message, iSettings->IsEchoEnabled());

					CleanupStack::PopAndDestroy(message);
					}
				}
			break;
			}

		case ERFtermHistory:
			{
			if(iBtClient->IsReadyToSend())
				{
				TBuf<KRFtermTextBufLength> text;
				if (iAppView->ShowHistoryQueryL(text))
					{
					HBufC* message = HBufC::NewLC(text.Length() + iSettings->MessageAddendum().Length());
					message->Des().Copy(text);
					message->Des().Append(iSettings->MessageAddendum());

					iBtClient->SendMessageL(*message, iSettings->IsEchoEnabled());

					CleanupStack::PopAndDestroy(message);
					}
				}
			break;
			}

		case ERFtermCtrlChar:
			{
			if(iBtClient->IsReadyToSend())
				{
				TBuf<KRFtermTextBufLength> ctrlChar;
				if (iAppView->ShowCtrlCharQueryL(ctrlChar))
					{
					iBtClient->SendMessageL(ctrlChar);
					}
				}
			break;
			}

		case ERFtermFile:
			{
			if(iBtClient->IsReadyToSend())
				{
				TDriveNumber drive = EDriveC;
				TFileName fileName;

				CAknMemorySelectionDialogMultiDrive* dlgDrive = CAknMemorySelectionDialogMultiDrive::NewL(ECFDDialogTypeSelect, ETrue);
				CleanupStack::PushL(dlgDrive);
				TBool result = dlgDrive->ExecuteL(drive, &fileName, NULL);
				if (result)
					{
					CAknFileSelectionDialog* dlgFile = CAknFileSelectionDialog::NewL(ECFDDialogTypeSelect);
					CleanupStack::PushL(dlgFile);
					result = dlgFile->ExecuteL(fileName);
					if(result)
						{
						iBtClient->SendFileL(fileName, iSettings->IsEchoEnabled());
						}
					CleanupStack::PopAndDestroy(dlgFile);
					}
				CleanupStack::PopAndDestroy(dlgDrive);
				}
			break;
			}

		case ERFtermClear:
			{
			CRFtermOutput* output = static_cast<CRFtermOutput*>(
					iAppView->ComponentControl(ERFtermAppViewOutput));
			output->ClearL();
			break;
			}

		case ERFtermSave:
			{
			CRFtermOutput* output = static_cast<CRFtermOutput*>(
					iAppView->ComponentControl(ERFtermAppViewOutput));
			if (output->SaveOutputAsTextL())
				{
				ShowOutputIsSavedNoteL();
				}
			break;
			}

		case ERFtermSettings:
			{
			iAppView->MakeNaviLabelVisible(EFalse);
			CRFtermSettingsDialog::RunDlgLD(iSettings);
			iAppView->MakeNaviLabelVisible(ETrue);
			break;
			}

		case ERFtermHelp:
			{
			CArrayFix < TCoeHelpContext > *buf = CCoeAppUi::AppHelpContextL();
			HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
			break;
			}

		default:
			{
			Panic(ERFtermUi);
			break;
			}
		}
	}
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CRFtermAppUi::HandleStatusPaneSizeChange()
	{
	iAppView->SetRect(ClientRect());
	}

CArrayFix<TCoeHelpContext>* CRFtermAppUi::HelpContextL() const
	{
#ifdef _HELP_AVAILABLE_
	CArrayFixFlat<TCoeHelpContext>* array = new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
	CleanupStack::PushL(array);
	array->AppendL(TCoeHelpContext(KUidRFtermApp, KAbout));
	CleanupStack::Pop(array);
	return array;
#else
	return NULL;
#endif
	}

// -----------------------------------------------------------------------------
// CRFtermAppUi::ShowOutputIsSavedNoteL()
// Show note if output has been saved successfully
// -----------------------------------------------------------------------------
//
void CRFtermAppUi::ShowOutputIsSavedNoteL()
	{
	// Load a string from the resource file and display it
	HBufC* textResource = StringLoader::LoadLC(R_STR_OUTPUT_IS_SAVED);
	CAknConfirmationNote* confirmNote;

	confirmNote = new (ELeave) CAknConfirmationNote;

	// Show the information Note with
	// textResource loaded with StringLoader.
	confirmNote->ExecuteLD(*textResource);

	// Pop HBuf from CleanUpStack and Destroy it.
	CleanupStack::PopAndDestroy(textResource);
	}

// -----------------------------------------------------------------------------
// CRFtermAppUi::ShowBTNotAvailableNoteL()
// Show note if BT is not available 
// -----------------------------------------------------------------------------
//
void CRFtermAppUi::ShowBTNotAvailableNoteL()
	{
	// Load a string from the resource file and display it
	HBufC* textResource = StringLoader::LoadLC(R_ERR_NO_BT);
	CAknErrorNote* errorNote;

	errorNote = new (ELeave) CAknErrorNote;

	// Show the information Note with
	// textResource loaded with StringLoader.
	errorNote->ExecuteLD(*textResource);

	// Pop HBuf from CleanUpStack and Destroy it.
	CleanupStack::PopAndDestroy(textResource);
	}

// End of File
