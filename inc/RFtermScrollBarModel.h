/*
 ============================================================================
 Name        : RFtermScrollBarModel.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Data model of custom scrollbars control.
 ============================================================================
 */

#ifndef __RFTERMSCROLLBARMODEL_H__
#define __RFTERMSCROLLBARMODEL_H__

struct TRFtermScrollBarModel
	{

	TRFtermScrollBarModel() : iTotal(1), iVisible(1), iPos(0) {}

	TInt iTotal;
	TInt iVisible;
	TInt iPos;

	};

#endif /* __RFTERMSCROLLBARMODEL_H__ */

// End of File
