/*
 ============================================================================
 Name		: RFtermAppUi.h
 Author	  : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __RFTERMAPPUI_h__
#define __RFTERMAPPUI_h__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CRFtermAppView;

// CLASS DECLARATION
/**
 * CRFtermAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CRFtermAppUi : public CAknAppUi
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
	
private:  // From MEikMenuObserver
        
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

private:
	// Data

	/**
	 * The application view
	 * Owned by CRFtermAppUi
	 */
	CRFtermAppView* iAppView;

	};

#endif // __RFTERMAPPUI_h__
// End of File
