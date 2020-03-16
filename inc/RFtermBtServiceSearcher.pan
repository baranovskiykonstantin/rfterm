/*
 ============================================================================
 Name        : RFtermBtServiceSearcher.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains service searcher panic codes.
 ============================================================================
 */

#ifndef __RFTERMBTSERVICE_SEARCHER_PAN__
#define __RFTERMBTSERVICE_SEARCHER_PAN__

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

#endif // __RFTERMBTSERVICE_SEARCHER_PAN__

// End of File
