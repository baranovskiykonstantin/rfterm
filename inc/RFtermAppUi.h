/*
 ============================================================================
 Name        : RFtermAppUi.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef RFTERMAPPUI_H
#define RFTERMAPPUI_H

// INCLUDES
#include <aknappui.h>

#include "RFtermAppView.h"
#include "RFtermBt.h"
#include "RFtermBtObserver.h"
#include "RFtermSettings.h"

/**
 * CRFtermAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CRFtermAppUi : public CAknAppUi, public MRFtermBtObserver
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CRFtermAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CRFtermAppUi();

	/**
	 * ~CRFtermAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CRFtermAppUi();

private:
	// Functions from base classes

	/**
	* From CEikAppUi, handles key events.
	* @param aKeyEvent Event to handled.
	* @param aType Type of the key event. 
	* @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
	*/
	virtual TKeyResponse HandleKeyEventL(
		const TKeyEvent& aKeyEvent,TEventCode aType );

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange();

	/**
	 *  From CCoeAppUi, HelpContextL.
	 *  Provides help context for the application.
	 *  size is changed.
	 */
	CArrayFix<TCoeHelpContext>* HelpContextL() const;
	
	/**
	*  ShowBTNotAvailableNoteL.
	*  Show note if BT is not available
	*/
	void ShowBTNotAvailableNoteL();

private: // From MEikMenuObserver

	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

private: // From MRFtermBtObserver

	void HandleBtNotifyL(const TDesC& aMessage);
	void HandleBtDataL(const TDesC& aData);

private: // New functions
	
	void InternalizeSettingsL();
	void ExternalizeSettingsL() const;
	
	void HandleSettingsChange();

private:
	// Data

	/**
	 * The application view
	 * Owned by CRFtermAppUi
	 */
	CRFtermAppView* iAppView;

	/** 
	* iClient
	* the BT engine 
	* Owned by CRFtermAppUi
	*/
	CRFtermBt* iBtClient;

	/**
	* iBtAvailable, ETrue if an SDP session can be opened, EFalse otherwise.
	*/
	TBool iBtAvailable;
	
	/**
	 * iSettingsFileName - name of the file with stored settings.
	 */
	TFileName iSettingsFileName;
	
public:
	
	/**
	 * iSettings - application settings.
	 */
	CRFtermSettings* iSettings;
	};

#endif // RFTERMAPPUI_H
// End of File
