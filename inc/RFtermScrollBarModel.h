/*
 ============================================================================
 Name        : RFtermScrollBarModel.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Data model of custom scrollbars control.
 ============================================================================
 */

#ifndef RFTERMSCROLLBARMODEL_H
#define RFTERMSCROLLBARMODEL_H

struct TRFtermScrollBarModel
	{
	TRFtermScrollBarModel() : iTotal(1), iVisible(1), iPos(0) {}
	TInt iTotal;
	TInt iVisible;
	TInt iPos;
	};

#endif /* RFTERMSCROLLBARMODEL_H */
