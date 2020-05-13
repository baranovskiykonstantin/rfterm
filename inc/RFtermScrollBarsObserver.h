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

#endif /* __RFTERMSCROLLBARSOBSERVER_H__ */

// End of File
