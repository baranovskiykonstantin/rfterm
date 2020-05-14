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

/**
 * CRFtermSettingsDialog class.
 * Dialog to edit settings of RFterm application.
 */
class CRFtermSettingsDialog: public CAknForm
	{

public: // Run settings dialog

	static TInt RunDlgLD(CRFtermSettings* aSettings);

private: // Constructor, destructor

	CRFtermSettingsDialog(CRFtermSettings* aSettings);
	~CRFtermSettingsDialog();

public: // From CAknForm

	TBool OkToExitL(TInt aButtonId);
	void HandlePointerEventL(const TPointerEvent &aPointerEvent);
	TBool SaveFormDataL();
	void DoNotSaveFormDataL();
	void PreLayoutDynInitL();

private: // From CAknForm

	void LoadFormDataL();

private: // From CCoeControl

	void ActivateL();

private: // Data

	// Not owning pointer to application settings
	CRFtermSettings* iSettings;

	// Pointer position at down event
	TPoint iDownPointerPos;

	};

#endif /* __RFTERMSETTINGSDIALOG_H__ */

// End of File
