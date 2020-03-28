/*
 ============================================================================
 Name        : RFtermTextQueryDialog.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Query of send data.
 ============================================================================
 */

#include "RFtermTextQueryDialog.h"

CRFtermTextQueryDialog* CRFtermTextQueryDialog::NewL(TDes &aDataText)
	{
	CRFtermTextQueryDialog* self = new (ELeave) CRFtermTextQueryDialog(aDataText);
	return self;
	}

CRFtermTextQueryDialog::CRFtermTextQueryDialog(TDes &aDataText)
	: CAknTextQueryDialog(aDataText)
	{
	}

CRFtermTextQueryDialog::~CRFtermTextQueryDialog()
	{
	}

void CRFtermTextQueryDialog::UpdateLeftSoftKeyL()
	{
	// Do not hide OK button. Allow empty input.
	}

