/*
 * RFtermOutput.h
 *
 *  Created on: 28 Feb 2020
 */

#ifndef RFTERMOUTPUT_H_
#define RFTERMOUTPUT_H_

#include <eikedwin.h>

class CRFtermOutput : public CEikEdwin
	{
public:
	static CRFtermOutput* NewL(const CCoeControl *aParent);
	static CRFtermOutput* NewLC(const CCoeControl *aParent);
	virtual ~CRFtermOutput();
	TBool IsEmpty();
	void Clear();
	void AppendLineL(const TDesC& aLine);
	void AppendTextL(const TDesC& aText);

private:
	void ConstructL(const CCoeControl *aParent);
	CRFtermOutput();
	void AppendL(const TDesC& aBuf);
	
private:
	TInt iRFtermFontID;
	};

#endif /* RFTERMOUTPUT_H_ */
