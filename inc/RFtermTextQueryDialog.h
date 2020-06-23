/*
 ============================================================================
 Name        : RFtermTextQueryDialog.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Query of send data.
 ============================================================================
 */

#ifndef __RFTERMTEXTQUERYDIALOG_H__
#define __RFTERMTEXTQUERYDIALOG_H__

#include <AknQueryDialog.h>

/**
 * CRFtermTextQueryDialog class.
 * This class is inherited from CAknTextQueryDialog to override
 * the UpdateLeftSoftKey method that hides OK button if query is empty.
 * Overrided method allows to accept emty text input.
 */
class CRFtermTextQueryDialog: public CAknTextQueryDialog
	{

public: // Static constructor

	static CRFtermTextQueryDialog* NewL(TDes &aDataText);

protected: // From CAknTextQueryDialog

	virtual void UpdateLeftSoftKeyL();

private: // Constructor, destructor

	CRFtermTextQueryDialog(TDes &aDataText);
	~CRFtermTextQueryDialog();

	};

#endif /* __RFTERMTEXTQUERYDIALOG_H__ */

// End of File
