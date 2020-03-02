/*
 ============================================================================
 Name		: RFtermAppUi.cpp
 Author	  : Konstantin Baranovskiy
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

#include <rfterm_0xae7f53fa.rsg>

#ifdef _HELP_AVAILABLE_
#include "RFterm_0xae7f53fa.hlp.hrh"
#endif
#include "RFterm.hrh"
#include "RFterm.pan"
#include "RFtermApplication.h"
#include "RFtermAppUi.h"
#include "RFtermAppView.h"

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

	// Create view object
	iAppView = CRFtermAppView::NewL(ClientRect());
	AddToStackL(iAppView);

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
	if (iAppView)
		{
		RemoveFromStack(iAppView);
		delete iAppView;
		iAppView = NULL;
		}

	}

// ------------------------------------------------------------------------------
// CChatAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
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
		if (iAppView->iRFtermOutput->IsEmpty())
			{
			aMenuPane->SetItemDimmed(EClear, ETrue);
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
			Exit();
			break;

		case EConnect:
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
			break;
		case EDisconnect:
			{
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
			break;
		case ESend:
			{
			iAppView->iRFtermOutput->AppendLineL(_L("ESend"));
			break;
			}
		case EClear:
			{
			iAppView->iRFtermOutput->Clear();
			break;
			}
		case ESettings:
			{
			iAppView->iRFtermOutput->AppendLineL(_L("ESettings"));
			break;
			}
		case EHelp:
			{
			CArrayFix < TCoeHelpContext > *buf = CCoeAppUi::AppHelpContextL();
			HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
			}
			break;
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
			}
			break;
		default:
			Panic(ERFtermUi);
			break;
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

// End of File
