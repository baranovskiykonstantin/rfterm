/*
 ============================================================================
 Name        : RFtermAttributeparser.pan
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This file contains SDP attr panic codes.
 ============================================================================
 */

#ifndef RFTERMSDPATTRIBUTEPARSER_PAN
#define RFTERMSDPATTRIBUTEPARSER_PAN

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

#endif // RFTERMSDPATTRIBUTEPARSER_PAN

// End of File
