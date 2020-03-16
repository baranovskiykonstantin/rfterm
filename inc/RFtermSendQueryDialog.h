/*
 ============================================================================
 Name        : RFtermSendQueryDialog.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Query of send data.
 ============================================================================
 */

#ifndef RFTERMSENDQUERYDIALOG_H
#define RFTERMSENDQUERYDIALOG_H

#include <AknQueryDialog.h>

class CRFtermSendQueryDialog: public CAknTextQueryDialog
	{
public:
	static CRFtermSendQueryDialog* NewL(TDes &aDataText);
	
protected:
	virtual void UpdateLeftSoftKeyL();
	
private:
	CRFtermSendQueryDialog(TDes &aDataText);
	~CRFtermSendQueryDialog();
	};

#endif /* RFTERMSENDQUERYDIALOG_H */
