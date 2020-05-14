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

/**
 * MRFtermOutputObserver mixin.
 * This class is designed to notify the scrollbars
 * control about content and view changes.
 */
class MRFtermOutputObserver
	{

public:

	/**
	 * HandleViewRectChangedL()
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aContentRect Contains the size of the total content.
	 * @param aViewRect Contains the sizs and position of the visible content.
	 */
	virtual void HandleViewRectChangedL(const TRect& aContentRect, const TRect& aViewRect) = 0;

	};

#endif /* __RFTERMOUTPUTOBSERVER_H__ */

// End of File
