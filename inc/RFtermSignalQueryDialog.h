/*
 ============================================================================
 Name        : RFtermSignalQueryDialog.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Dialog with the list of RS-232 input/output signals.
 ============================================================================
 */

#ifndef __RFTERMSIGNALQUERYDIALOG_H__
#define __RFTERMSIGNALQUERYDIALOG_H__

#include <e32base.h> // CPeriodic
#include <AknListQueryDialog.h>
#include "RFtermBt.h"

/**
 * CRFtermSignalQueryDialog class.
 * This class shows the dialog with the list of RS-232 input/output signals.
 */
class CRFtermSignalQueryDialog : public CAknListQueryDialog
	{

public: // New functions

	/**
	 * RunDlgLD()
	 * Run RS-232 signal dialog
	 * @param aBtClient Pointer to Bluetooth client.
	 */
	static TInt RunDlgLD(CRFtermBt* aBtClient);

public: // from CCoeControl

	/**
	* OfferKeyEventL()
	* Offer the key event to the list box
	* @param aKeyEvent the key that caused the event
	* @param aType the type of event that occured
	* @return indicates whether or not the key event
	* was used by this control
	*/  
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

protected: // From CAknListQueryDialog

	TBool OkToExitL(TInt aButtonId);

protected: // From MEikListBoxObserver

	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

private: // Constructors, destructor

	CRFtermSignalQueryDialog(CRFtermBt* aBtClient);
	void ConstructL();
	~CRFtermSignalQueryDialog();

private: // New functions

	static TInt TimerCallBack(TAny* aObject);
	void UpdateSignalStatesL();

private: // Data

	/**
	 * iBtClient
	 * Non owining pointer to CRFtermBt.
	 */
	CRFtermBt* iBtClient;

	/**
	 * iTimer
	 * Timer for updating signal states.
	 */
	CPeriodic* iTimer;
	/**
	 * iIndex
	 * Dummy index of selected item.
	 */
	TInt iIndex;

	};

#endif /* __RFTERMSIGNALQUERYDIALOG_H__ */

// End of File
