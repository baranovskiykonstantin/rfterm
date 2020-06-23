/*
 ============================================================================
 Name        : RFtermAttributeparser.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains SDP attr panic codes.
 ============================================================================
 */

#ifndef __RFTERMSDPATTRIBUTEPARSER_PAN__
#define __RFTERMSDPATTRIBUTEPARSER_PAN__

#include <e32std.h>

/** Panic Category */
_LIT(KPanicSAP, "SAP");

/** RFterm application panic codes */
enum TRFtermSdpAttributeParserPanics
	{
	ERFtermSdpAttributeParserInvalidCommand = 1,
	ERFtermSdpAttributeParserNoValue,
	ERFtermSdpAttributeParserValueIsList,
	ERFtermSdpAttributeParserValueTypeUnsupported
	};

inline void Panic(TRFtermSdpAttributeParserPanics aReason)
	{
	User::Panic(KPanicSAP, aReason);
	}

#endif // __RFTERMSDPATTRIBUTEPARSER_PAN__

// End of File
