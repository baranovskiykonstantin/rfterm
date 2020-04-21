/*
 ============================================================================
 Name        : RFtermScrollBarsObserver.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Custom scrollbars control observer.
 ============================================================================
 */

#ifndef RFTERMSCROLLBARSOBSERVER_H
#define RFTERMSCROLLBARSOBSERVER_H

#include "RFtermScrollBarModel.h"

enum TRFtermScrollBarType
	{
	EVerticalScrollBar,
	EHorizontalScrollBar
	};

class MRFtermScrollBarsObserver
	{
public:
	virtual void HandleScrollEventL(
		TRFtermScrollBarType aScrollBarType,
		const TRFtermScrollBarModel& aModel) = 0;

	virtual void HandleScrollBarVisibilityChangeL(
		TBool aVScrollBarIsVisible,
		TBool aHScrollBarIsVisible) = 0;
	};

#endif /* RFTERMSCROLLBARSOBSERVER_H */
