/*
 ============================================================================
 Name        : ChargingStatus.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Charging status notifier.
 ============================================================================
 */

#ifndef __CHARGINGSTATUS_H__
#define __CHARGINGSTATUS_H__

#include <e32base.h>
#include <HWRMPowerStateSDKPSKeys.h>
#include <e32property.h>

/**
 * MChargingStatusObserver mixin.
 * Handles the charging status change.
 */
class MChargingStatusObserver
	{

public:

	/**
	 * HandleChargingStatusChangeL()
	 * Charging status change notify.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aChargingStatus Contains the new status of the charging.
	 */
	virtual void HandleChargingStatusChangeL(EPSHWRMChargingStatus aChargingStatus) = 0;

	};

/**
* CChargingStatus
* Checks the charging status and sends the notification of charging status change
* to MChargingStatusObserver.
*/
class CChargingStatus : public CActive
	{

public: // Constructors and destructor

	static CChargingStatus* NewL(MChargingStatusObserver* aObserver);
	virtual ~CChargingStatus();

private: // Constructors

	CChargingStatus(MChargingStatusObserver* aObserver);
	void ConstructL();

private: // From CActive

	void RunL();
	void DoCancel();

private: // Data

	/**
	 * iProperty
	 * Reflects status of the charging.
	 */
	RProperty iProperty;

	/**
	 * iObserver
	 * Handles the charging status change.
	 * Non-owning pointer.
	 */
	MChargingStatusObserver* iChargingStatusObserver;

	};

#endif /* __CHARGINGSTATUS_H__ */

// End of File
