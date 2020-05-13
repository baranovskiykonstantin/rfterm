/*
 ============================================================================
 Name        : RFtermOutputObserver.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : RFterm's output control observer.
 ============================================================================
 */

#ifndef __RFTERMOUTPUTOBSERVER_H__
#define __RFTERMOUTPUTOBSERVER_H__

class MRFtermOutputObserver
	{

public:

	virtual void HandleViewRectChangedL(const TRect& aContentRect, const TRect& aViewRect) = 0;

	};

#endif /* __RFTERMOUTPUTOBSERVER_H__ */

// End of File
