/*
 ============================================================================
 Name        : RFtermBatteryStatus.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Battery status notifier.
 ============================================================================
 */

#ifndef __RFTERMBATTERYSTATUS_H__
#define __RFTERMBATTERYSTATUS_H__

#include <e32base.h>
#include <HWRMPowerStateSDKPSKeys.h>
#include <e32property.h>

/**
 * MRFtermBatteryStatusObserver mixin.
 * Handles the battery status change.
 */
class MRFtermBatteryStatusObserver
	{

public:

	/**
	 * HandleBatteryStatusChangeL()
	 * Battery status change notify.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aBatteryStatus Contains the new status of the battery.
	 */
	virtual void HandleBatteryStatusChangeL(EPSHWRMBatteryStatus aBatteryStatus) = 0;

	};

/**
* CRFtermBatteryStatus
* Checks the battery status and sends the notify of battery status change
* to MRFtermBatteryStatusObserver.
*/
class CRFtermBatteryStatus : public CActive
	{

public: // Constructors and destructor
	static CRFtermBatteryStatus* NewL(MRFtermBatteryStatusObserver* aObserver);
	virtual ~CRFtermBatteryStatus();

private: // Constructors
	CRFtermBatteryStatus(MRFtermBatteryStatusObserver* aObserver);
	void ConstructL();

private: // From CActive
	void RunL();
	void DoCancel();

private: // Data

	/**
	 * iProperty
	 * Reflects status of the battery.
	 */
	RProperty iProperty;

	/**
	 * iObserver
	 * Handles the battery status change.
	 * Non-owning pointer.
	 */
	MRFtermBatteryStatusObserver* iBatteryStatusObserver;

	};

#endif /* __RFTERMBATTERYSTATUS_H__ */

// End of File
