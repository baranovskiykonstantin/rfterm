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

class CRFtermTextQueryDialog: public CAknTextQueryDialog
	{

public:

	static CRFtermTextQueryDialog* NewL(TDes &aDataText);

protected:

	virtual void UpdateLeftSoftKeyL();

private:

	CRFtermTextQueryDialog(TDes &aDataText);
	~CRFtermTextQueryDialog();

	};

#endif /* __RFTERMTEXTQUERYDIALOG_H__ */

// End of File
