/*
 ============================================================================
 Name        : RFtermTextQueryDialog.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Query of send data.
 ============================================================================
 */

#ifndef RFTERMTEXTQUERYDIALOG_H
#define RFTERMTEXTQUERYDIALOG_H

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

#endif /* RFTERMTEXTQUERYDIALOG_H */
