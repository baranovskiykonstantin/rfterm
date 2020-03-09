/*
 ============================================================================
 Name        : RFterm_unlockfont.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : This program will release the file lock on the font file.
 ============================================================================
 */

#include <e32base.h>
#include <fbs.h>

#include "RFtermConstants.h"

TInt E32Main()
	{
	TRAPD(error, CFbsTypefaceStore::RemoveFontFileLocksL(KRFtermFontPath));
	return error;
	}
