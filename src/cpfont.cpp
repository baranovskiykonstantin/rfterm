/*
 ============================================================================
 Name        : cpfont.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Copy font file to c:\resource\fonts\.
 
 A font must be placed in "resource\fonts\" directory to be properly
 processed by Font and Bitmap Server.
 But once font file is copied there it cannot be removed because
 this file is locked by FbServ.
 Locked font file does not allow to remove the application
 that installed this font.
 Manual font adding from different place brings sporadic device reboots:
 https://bugreports.qt.io/browse/QTBUG-6611

 The solution from Nokia Wiki does not work.
 The next funcion is not supported by s60v5:
 CFbsTypefaceStore::RemoveFontFileLocksL(_L("c:\\resource\\customFont.ttf"));

 Possible solution:
 1) install font file to "\resource\apps\" with other app resources
    (from there font file can be removed successfully);
 2) copy font file to "\resource\fonts\" with this tiny app at installation time
    and leave it there forever.
 This solution is dirty but Symbian API does not leave a choice.
 ============================================================================
 */

// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include "RFtermConstants.h"

_LIT(KTxtEPOC32EX, "CPFONT");
_LIT(KFontDir, "c:\\resource\\fonts\\");

LOCAL_D RFs fsSession;

LOCAL_C void MainL()
	{
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	TInt dirError = fsSession.MkDir(KFontDir);
	if (dirError != KErrNone && dirError != KErrAlreadyExists)
		User::LeaveIfError(dirError);
	CFileMan* fileMan = CFileMan::NewL(fsSession);
	CleanupStack::PushL(fileMan);
	fileMan->Copy(KRFtermFontPath, KCourierNewFontPath, 0); // do not overwrite
	CleanupStack::PopAndDestroy(2); // fileMan, fsSession
	}

GLDEF_C TInt E32Main() // main function called by E32
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, MainL());
	__ASSERT_ALWAYS(!error, User::Panic(KTxtEPOC32EX, error));
	delete cleanup;
	__UHEAP_MARKEND;
	return 0;
	}
