/*
 ============================================================================
 Name        : RFtermBtServiceSearcher.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains service searcher panic codes.
 ============================================================================
 */

#ifndef RFTERMBTSERVICESEARCHER_PAN
#define RFTERMBTSERVICESEARCHER_PAN

#include <e32std.h>

/** RFtermBtServiceSearcher application panic codes */
enum TRFtermBtServiceSearcherPanics 
	{
	ERFtermBtServiceSearcherNextRecordRequestComplete = 1,
	ERFtermBtServiceSearcherAttributeRequestResult,
	ERFtermBtServiceSearcherAttributeRequestComplete,
	ERFtermBtServiceSearcherProtocolRead
	};


/** Panic Category */
_LIT(KPanicServiceSearcher, "ServiceSearcher");


inline void Panic(TRFtermBtServiceSearcherPanics aReason)
	{
	User::Panic(KPanicServiceSearcher, aReason);
	}

#endif // RFTERMBTSERVICESEARCHER_PAN

// End of File
