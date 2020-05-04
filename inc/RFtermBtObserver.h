/*
 ============================================================================
 Name        : RFtermBtObserver.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : RFterm's bluetooth client observer.
 ============================================================================
 */

#ifndef RFTERMBTOBSERVER_H
#define RFTERMBTOBSERVER_H

class MRFtermBtObserver
	{
public:
	virtual void HandleBtNotifyL(const TDesC& aMessate) = 0;
	virtual void HandleBtDataL(const TDesC& aData) = 0;
	};

#endif /* RFTERMBTOBSERVER_H */
