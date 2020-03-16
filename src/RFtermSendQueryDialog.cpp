/*
 ============================================================================
 Name        : RFtermSendQueryDialog.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Query of send data.
 ============================================================================
 */

#include "RFtermSendQueryDialog.h"

CRFtermSendQueryDialog* CRFtermSendQueryDialog::NewL(TDes &aDataText)
	{
	CRFtermSendQueryDialog* self = new (ELeave) CRFtermSendQueryDialog(aDataText);
	return self;
	}

CRFtermSendQueryDialog::CRFtermSendQueryDialog(TDes &aDataText)
	: CAknTextQueryDialog(aDataText)
	{
	}

CRFtermSendQueryDialog::~CRFtermSendQueryDialog()
	{
	}

void CRFtermSendQueryDialog::UpdateLeftSoftKeyL()
	{
	// Do not hide OK button. Allow empty input.
	}

