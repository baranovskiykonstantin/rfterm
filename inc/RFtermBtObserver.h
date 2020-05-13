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

#include <btdevice.h>

class MRFtermBtObserver
	{

public:

	virtual void HandleBtDeviceChangeL(CBTDevice* aRemoteDevice) = 0;
	virtual void HandleBtNotifyL(const TDesC& aMessage) = 0;
	virtual void HandleBtDataL(const TDesC& aData) = 0;

	};

#endif /* RFTERMBTOBSERVER_H */

// End of File
