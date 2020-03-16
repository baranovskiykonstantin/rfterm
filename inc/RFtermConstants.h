/*
 ============================================================================
 Name        : RFtermConstants.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Common constants.
 ============================================================================
 */

#ifndef RFTERMCONSTANTS_H
#define RFTERMCONSTANTS_H

#include <bt_sock.h>
#include "RFterm.hrh"

const TInt KRfcommChannel = 1;
const TInt KServiceClass = 0x1101; // SerialPort

const TUid KUidRFtermApp = {_UID3};

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

_LIT(KPrefixNote, "* ");
_LIT(KPrefixError, "! ");
_LIT(KPrefixIn, "> ");
_LIT(KPrefixOut, "< ");
_LIT(KCR, "\r");
_LIT(KLF, "\n");
_LIT(KCRLF, "\r\n");
_LIT(KParagraphDelimeter, "\x2029"); //CEditableText::EParagraphDelimiter

#endif /* RFTERMCONSTANTS_H */
