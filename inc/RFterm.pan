/*
 ============================================================================
 Name        : RFterm.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __RFTERM_PAN__
#define __RFTERM_PAN__

/** RFterm application panic codes */
enum TRFtermPanics
	{
	ERFtermUi = 1,
	ERFtermInvalidControlIndex,
	ERFtermCannotLoadFont,
	ERFtermInvalidLogicState,
	ERFtermUnableToDisconnect,
	ERFtermOutputIllegalPosition,
	// add further panics here
	};

/** Panic Category */
_LIT(KPanicRFterm, "RFterm");
_LIT(KPanicBTServiceAdvertiser, "BTServiceAdvertiser");

inline void Panic(TRFtermPanics aReason)
	{
	User::Panic(KPanicRFterm, aReason);
	}

#endif // __RFTERM_PAN__
