/*
 ============================================================================
 Name        : RFtermScrollBarsObserver.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Custom scrollbars control observer.
 ============================================================================
 */

#ifndef __RFTERMSCROLLBARSOBSERVER_H__
#define __RFTERMSCROLLBARSOBSERVER_H__

#include "RFtermScrollBarModel.h"

/**
 * Enum TRFtermScrollBarType.
 * IDs to identify the scrollbar type.
 */
enum TRFtermScrollBarType
	{
	EVerticalScrollBar,
	EHorizontalScrollBar
	};

/**
 * MRFtermScrollBarsObserver mixin.
 * Provides interface to handle events from scrollbars.
 */
class MRFtermScrollBarsObserver
	{

public:

	/**
	 * HandleScrollEventL()
	 * Scrollbar model change notification.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aScrollBarType Specify which of the scrollbars is affected.
	 * @param aModel Reference to the changed scrollbar model.
	 */
	virtual void HandleScrollEventL(
		TRFtermScrollBarType aScrollBarType,
		const TRFtermScrollBarModel& aModel) = 0;

	/**
	 * HandleScrollBarVisibilityChangeL()
	 * Scrollbar visibility change notification.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aVScrollBarIsVisible Visibility of vertical scrollbar.
	 * @param aHScrollBarIsVisible Visibility of horizontal scrollbar.
	 */
	virtual void HandleScrollBarVisibilityChangeL(
		TBool aVScrollBarIsVisible,
		TBool aHScrollBarIsVisible) = 0;

	};

#endif /* __RFTERMSCROLLBARSOBSERVER_H__ */

// End of File
