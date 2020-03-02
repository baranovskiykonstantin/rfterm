/*
 ============================================================================
 Name		: RFterm.pan
 Author	  : Konstantin Baranovskiy
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
	// add further panics here
	};

inline void Panic(TRFtermPanics aReason)
	{
	_LIT(applicationName, "RFterm");
	User::Panic(applicationName, aReason);
	}

#endif // __RFTERM_PAN__
