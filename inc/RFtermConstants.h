/*
 ============================================================================
 Name        : RFtermConstants.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Common constants.
 ============================================================================
 */

#ifndef __RFTERMCONSTANTS_H__
#define __RFTERMCONSTANTS_H__

#include <bt_sock.h>
#include "RFterm.hrh"

const TInt KRfcommChannel = 1;
const TInt KServiceClass = 0x1101; // SerialPort

const TUid KUidRFtermApp = {_UID3};

_LIT(KSettingsFile, "settings.ini");

_LIT(KStrRFCOMM, "RFCOMM");

const TInt KListeningQueSize = 1;
const TInt KRFtermTextBufLength = 128;
const TInt KDefaultPortNumber = 5005;
const TInt KMaxServerNameLength = 256;
const TInt KMaxMarkLen = 2;
const TInt KMessageHeaderLen = 8;

const TInt KRFtermTimeOut = 20000000; // 20 seconds time-out

_LIT(KRFtermFontPath, "c:\\system\\fonts\\RFterm_0xae7f53fa_font.ttf");
_LIT(KRFtermFontName, "Courier New");

_LIT(KRFtermOutputMessageMark, " RFterm: ");

_LIT(KCtrlChars, "\r\n\t\b\a\v\f\0\x007f");
_LIT(KCR, "\r"); // CARRIAGE RETURN ^M
_LIT(KLF, "\n"); // LINE FEED ^J
_LIT(KCRLF, "\r\n"); // CR+LF
_LIT(KTB, "\t"); // CHARACTER TABULATION ^I
_LIT(KBS, "\b"); // BACKSPACE ^H
_LIT(KBL, "\a"); // BELL ^G
_LIT(KLT, "\v"); // LINE TABULATION ^K
_LIT(KFF, "\f"); // FORM FEED ^L
_LIT(KNL, "\0"); // NULL ^@
_LIT(KDL, "\x007f"); // Delete

const TInt KRFtermScrollBarBreadth = 5;
const TInt KRFtermScrollBarSensorBreadth = 30;

#endif /* __RFTERMCONSTANTS_H__ */

// End of File
