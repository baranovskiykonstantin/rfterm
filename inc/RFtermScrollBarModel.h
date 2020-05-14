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

/**
 * TRFtermScrollBarModel class.
 * Holds data of scrollbar.
 */
struct TRFtermScrollBarModel
	{

	TRFtermScrollBarModel() : iTotal(1), iVisible(1), iPos(0) {}

	/**
	 * Height/width of the total content.
	 */
	TInt iTotal;

	/**
	 * Height/width of the visible content.
	 */
	TInt iVisible;

	/**
	 * Offset of the top left corner of the visible content
	 * relative to the top left corner of total content.
	 */
	TInt iPos;

	};

#endif /* __RFTERMSCROLLBARMODEL_H__ */

// End of File
