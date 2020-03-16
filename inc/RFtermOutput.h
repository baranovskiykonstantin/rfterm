/*
 ============================================================================
 Name        : RFtermOutput.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : RFterm's output control.
 ============================================================================
 */

#ifndef RFTERMOUTPUT_H
#define RFTERMOUTPUT_H

#include <eikedwin.h>

class CRFtermOutput : public CEikEdwin
	{
public:
	static CRFtermOutput* NewL(const CCoeControl *aParent);
	static CRFtermOutput* NewLC(const CCoeControl *aParent);
	virtual ~CRFtermOutput();
	TBool IsEmpty();
	void Clear();
	void AppendTextL(const TDesC& aText, const TDesC& aPrefix);
	void AppendTextOnNewLineL(const TDesC& aText, const TDesC& aPrefix);

private:
	void ConstructL(const CCoeControl *aParent);
	CRFtermOutput();
	void AppendL(const TDesC& aBuf);
	void ScrollToEnd();
	
private:
	TInt iRFtermFontID;
	TPtrC iCurrentPrefix;
	};

#endif /* RFTERMOUTPUT_H */
