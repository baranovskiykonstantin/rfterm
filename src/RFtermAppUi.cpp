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
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>

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

_LIT(KFileName, "C:\\private\\ae7f53fa\\RFterm.txt");
_LIT(KText, "Hello World!");

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
	
	// Prepare private path
	RFs& fsSession = iEikonEnv->FsSession();
	User::LeaveIfError(fsSession.CreatePrivatePath(EDriveC));
	User::LeaveIfError(fsSession.PrivatePath(iSettingsFileName));
	iSettingsFileName += KSettingsFile;
	iSettings = CRFtermSettings::NewL();
	// Read settings from stream
	InternalizeSettingsL();

	// Create view object
	iAppView = CRFtermAppView::NewL(ClientRect());
	AddToStackL(iAppView);

	iBtClient = CRFtermBt::NewL(iAppView->iRFtermOutput);

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

/*
	// Create a file to write the text to
	TInt err = CCoeEnv::Static()->FsSession().MkDirAll(KFileName);
	if ((KErrNone != err) && (KErrAlreadyExists != err))
		{
		return;
		}

	RFile file;
	err = file.Replace(CCoeEnv::Static()->FsSession(), KFileName, EFileWrite);
	CleanupClosePushL(file);
	if (KErrNone != err)
		{
		CleanupStack::PopAndDestroy(1); // file
		return;
		}

	RFileWriteStream outputFileStream(file);
	CleanupClosePushL(outputFileStream);
	outputFileStream << KText;

	CleanupStack::PopAndDestroy(2); // outputFileStream, file
*/

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
				HandleCommandL(ESend);
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
		if (!iBtClient->IsReadyToSendMessage())
			{
			aMenuPane->SetItemDimmed(ESend, ETrue);
			}
		if (iAppView->iRFtermOutput->IsEmpty())
			{
			aMenuPane->SetItemDimmed(EClear, ETrue);
			}
		if (iBtClient->IsConnected() || iBtClient->IsConnecting())
			{
			aMenuPane->SetItemDimmed(EConnect, ETrue);
			aMenuPane->SetItemDimmed(EStart, ETrue);
			if (iBtClient->Server())
				{
				aMenuPane->SetItemDimmed(EDisconnect, ETrue);
				}
			else
				{
				aMenuPane->SetItemDimmed(EStop, ETrue);
				}
			}
		else
			{
			aMenuPane->SetItemDimmed(EDisconnect, ETrue);
			aMenuPane->SetItemDimmed(EStop, ETrue);
			}
		}

	else if (aResourceId == R_SEND_MENU)
		{
		if (iAppView->iMessageHistoryArray->Count() == 0)
			{
			aMenuPane->SetItemDimmed(EHistory, ETrue);
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

		case EConnect:
			{
			if (!iBtAvailable)
				{
				ShowBTNotAvailableNoteL();
				}
			else
				{
				iBtClient->ConnectL();
				}
/*
			{
			// Load a string from the resource file and display it
			HBufC* textResource = StringLoader::LoadLC(R_MSG_CONNECTED_TEXT);
			CAknInformationNote* informationNote;

			informationNote = new (ELeave) CAknInformationNote;

			// Show the information Note with
			// textResource loaded with StringLoader.
			informationNote->ExecuteLD(*textResource);

			// Pop HBuf from CleanUpStack and Destroy it.
			CleanupStack::PopAndDestroy(textResource);
			}
*/
			break;
			}

		case EDisconnect:
			{
			if (iBtClient->IsConnected())
				{
				iBtClient->DisconnectL(); 
				}
/*
			RFile rFile;

			//Open file where the stream text is
			User::LeaveIfError(
					rFile.Open(CCoeEnv::Static()->FsSession(), KFileName,
							EFileStreamText));//EFileShareReadersOnly));// EFileStreamText));
			CleanupClosePushL(rFile);

			// copy stream from file to RFileStream object
			RFileReadStream inputFileStream(rFile);
			CleanupClosePushL(inputFileStream);

			// HBufC descriptor is created from the RFileStream object.
			HBufC* fileData = HBufC::NewLC(inputFileStream, 32);

			CAknInformationNote* informationNote;

			informationNote = new (ELeave) CAknInformationNote;
			// Show the information Note
			informationNote->ExecuteLD(*fileData);

			// Pop loaded resources from the cleanup stack
			CleanupStack::PopAndDestroy(3); // filedata, inputFileStream, rFile
			}
*/
			break;
			}
			
		case EStart:
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
			
		case EStop:
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

		case ESend:
			{
			break;
			}

		case EMessage:
			{
			if(iBtClient->IsConnected())
				{
				TBuf<KRFtermTextBufLength> text;
				if (iAppView->ShowTextQueryL(KNullDesC, text))
					{
					iBtClient->SendMessageL(text);
					}
				else
					{
					iAppView->iRFtermOutput->SetFocus(ETrue);
					}
				}
			break;
			}

		case EHistory:
			{
			if(iBtClient->IsConnected())
				{
				TBuf<KRFtermTextBufLength> text;
				if (iAppView->ShowHistoryQueryL(text))
					{
					iBtClient->SendMessageL(text);
					}
				else
					{
					iAppView->iRFtermOutput->SetFocus(ETrue);
					}
				}
			break;
			}

		case ECtrlChar:
			{
			if(iBtClient->IsConnected())
				{
				TBuf<KRFtermTextBufLength> ctrlChar;
				if (iAppView->ShowCtrlCharQueryL(ctrlChar))
					{
					iBtClient->SendMessageL(ctrlChar, ETrue);
					}
				else
					{
					iAppView->iRFtermOutput->SetFocus(ETrue);
					}
				}
			break;
			}

		case EClear:
			{
			iAppView->iRFtermOutput->ClearL();
			break;
			}

		case ESettings:
			{
			CRFtermSettingsDialog::RunDlgLD(iSettings);
			HandleSettingsChange();
			break;
			}

		case EHelp:
			{
			CArrayFix < TCoeHelpContext > *buf = CCoeAppUi::AppHelpContextL();
			HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
			break;
			}

		case EAbout:
			{
			CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
			dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
			HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
			dlg->QueryHeading()->SetTextL(*title);
			CleanupStack::PopAndDestroy(); //title
			HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
			dlg->SetMessageTextL(*msg);
			CleanupStack::PopAndDestroy(); //msg
			dlg->RunLD();
			break;
			}

		case ETest:
			{
			iAppView->iRFtermOutput->AppendTextL(_L("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz"));
			iAppView->iRFtermOutput->AppendMessageL(_L("0123456789"));
			iAppView->iRFtermOutput->AppendTextL(_L("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"));
			iAppView->iRFtermOutput->AppendTextL(_L("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz"));
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
	array->AppendL(TCoeHelpContext(KUidRFtermApp, KGeneral_Information));
	CleanupStack::Pop(array);
	return array;
#else
	return NULL;
#endif
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

// -----------------------------------------------------------------------------
// CRFtermAppUi::HandleSettingsChange()
// Update application to new settings.
// -----------------------------------------------------------------------------
//
void CRFtermAppUi::HandleSettingsChange()
	{
	// Message history size
	TInt sizeDiff = iAppView->iMessageHistoryArray->Count() - iSettings->iMessageHistorySize;
	if (sizeDiff > 0)
		{
		iAppView->iMessageHistoryArray->Delete(0, sizeDiff);
		iAppView->iMessageHistoryArray->Compress();
		}

	// Output code page
	if (iAppView->iRFtermOutput)
		{
		iAppView->iRFtermOutput->ChangeCodePage(iSettings->iCodePage);
		}
	}

// End of File
