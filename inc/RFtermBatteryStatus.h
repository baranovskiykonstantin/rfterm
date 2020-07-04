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

#include <BatteryLevel.h>
#include <ChargingStatus.h>

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
	 */
	virtual void HandleBatteryStatusChangeL() = 0;

	};

/**
* CRFtermBatteryStatus
* Checks the battery status and sends the notification of battery status change
* to MRFtermBatteryStatusObserver.
*/
class CRFtermBatteryStatus :
	public CBase,
	private MBatteryLevelObserver,
	private MChargingStatusObserver
	{

public: // Constructors and destructor

	static CRFtermBatteryStatus* NewL(MRFtermBatteryStatusObserver* aObserver);
	virtual ~CRFtermBatteryStatus();

private: // Constructors

	CRFtermBatteryStatus(MRFtermBatteryStatusObserver* aObserver);
	void ConstructL();

private: // From MBatteryLevelObserver

	void HandleBatteryLevelChangeL(EPSHWRMBatteryLevel aBatteryLevel);

private: // From MChargingStatusObserver

	void HandleChargingStatusChangeL(EPSHWRMChargingStatus aChargingStatus);

public: // New public functions

	/**
	 * IsOK()
	 * Get status of the battery.
	 */
	TBool IsOK();

private: // Data

	/**
	 * iBatteryLevel
	 */
	CBatteryLevel* iBatteryLevel;
	EPSHWRMBatteryLevel iBatteryLevelValue;

	/**
	 * iChargingStatus
	 */
	CChargingStatus* iChargingStatus;
	EPSHWRMChargingStatus iChargingStatusValue;

	/**
	 * iObserver
	 * Handles the battery status change.
	 * Non-owning pointer.
	 */
	MRFtermBatteryStatusObserver* iObserver;

	};

#endif /* __RFTERMBATTERYSTATUS_H__ */

// End of File
