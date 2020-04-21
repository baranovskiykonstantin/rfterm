/*
 ============================================================================
 Name        : RFtermOutputObserver.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : RFterm's output control observer.
 ============================================================================
 */

#ifndef RFTERMOUTPUTOBSERVER_H
#define RFTERMOUTPUTOBSERVER_H

class MRFtermOutputObserver
	{
public:
	virtual void HandleViewRectChangedL(const TRect& aContentRect, const TRect& aViewRect) = 0;
	};

#endif /* RFTERMOUTPUTOBSERVER_H */
