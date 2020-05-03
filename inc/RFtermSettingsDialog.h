/*
 ============================================================================
 Name        : RFtermSettingsDialog.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Query of send data.
 ============================================================================
 */

#ifndef RFTERMSETTINGSDIALOG_H
#define RFTERMSETTINGSDIALOG_H

#include <aknform.h>
#include "RFtermSettings.h"

class CRFtermSettingsDialog: public CAknForm
	{
public:
	static TInt RunDlgLD(CRFtermSettings* aSettings);

private:
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

private: // data
	// Not owning pointer to application settings
	CRFtermSettings* iSettings;

	// Pointer position on down event
	TPoint iDownPointerPos;
	};

#endif /* RFTERMSETTINGSDIALOG_H */
