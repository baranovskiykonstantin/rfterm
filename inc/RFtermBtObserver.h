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

/**
 * MRFtermBtObserver mixin.
 * All information from BT client is sent
 * to observer using this interface.
 */
class MRFtermBtObserver
	{

public:

	/**
	 * HandleBtDeviceChangeL()
	 * BT device connection/disconnection notify.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aRemoteDevice Contains the name and address of connected device.
	 */
	virtual void HandleBtDeviceChangeL(CBTDevice* aRemoteDevice) = 0;

	/**
	 * HandleBtNotifyL()
	 * Notify/error message from BT client.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aMessage Message from BT client.
	 */
	virtual void HandleBtNotifyL(const TDesC& aMessage) = 0;

	/**
	 * HandleBtDataL()
	 * Received data from remote BT device.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aData Received BT data.
	 */
	virtual void HandleBtDataL(const TDesC& aData) = 0;

	/**
	 * HandleBtFileSendingFinishL()
	 * File has been successfully sent.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 */
	virtual void HandleBtFileSendingFinishL() = 0;

	};

#endif /* RFTERMBTOBSERVER_H */

// End of File
