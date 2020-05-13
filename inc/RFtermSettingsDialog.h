/*
 ============================================================================
 Name        : RFtermSettingsDialog.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Query of send data.
 ============================================================================
 */

#ifndef __RFTERMSETTINGSDIALOG_H__
#define __RFTERMSETTINGSDIALOG_H__

#include <aknform.h>
#include "RFtermSettings.h"

class CRFtermSettingsDialog: public CAknForm
	{

public: // Run settings dialog

	static TInt RunDlgLD(CRFtermSettings* aSettings);

private: // Constructor, destructor

	CRFtermSettingsDialog(CRFtermSettings* aSettings);
	~CRFtermSettingsDialog();

public: // from CAknForm

	TBool OkToExitL(TInt aButtonId);
	void HandlePointerEventL(const TPointerEvent &aPointerEvent);
	TBool SaveFormDataL();
	void DoNotSaveFormDataL();
	void PreLayoutDynInitL();

private: // from CAknForm

	void LoadFormDataL();

private: // from CCoeControl

	void ActivateL();

private: // data

	// Not owning pointer to application settings
	CRFtermSettings* iSettings;

	// Pointer position on down event
	TPoint iDownPointerPos;

	};

#endif /* __RFTERMSETTINGSDIALOG_H__ */

// End of File
