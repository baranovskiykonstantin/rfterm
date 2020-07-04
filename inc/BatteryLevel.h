/*
 ============================================================================
 Name        : BatteryLevel.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Battery level notifier.
 ============================================================================
 */

#ifndef __BATTERYLEVEL_H__
#define __BATTERYLEVEL_H__

#include <e32base.h>
#include <HWRMPowerStateSDKPSKeys.h>
#include <e32property.h>

/**
 * MBatteryLevelObserver mixin.
 * Handles the battery level change.
 */
class MBatteryLevelObserver
	{

public:

	/**
	 * HandleBatteryLevelChangeL()
	 * Battery level change notify.
	 * 
	 * Derived classes provide the implementation.
	 * 
	 * @param aBatteryLevel Contains the new level of the battery.
	 */
	virtual void HandleBatteryLevelChangeL(EPSHWRMBatteryLevel aBatteryLevel) = 0;

	};

/**
* CBatteryLevel
* Checks the battery level and sends the notification of battery level change
* to MBatteryLevelObserver.
*/
class CBatteryLevel : public CActive
	{

public: // Constructors and destructor

	static CBatteryLevel* NewL(MBatteryLevelObserver* aObserver);
	virtual ~CBatteryLevel();

private: // Constructors

	CBatteryLevel(MBatteryLevelObserver* aObserver);
	void ConstructL();

private: // From CActive

	void RunL();
	void DoCancel();

private: // Data

	/**
	 * iProperty
	 * Reflects level of the battery.
	 */
	RProperty iProperty;

	/**
	 * iBatteryLevelObserver
	 * Handles the battery level change.
	 * Non-owning pointer.
	 */
	MBatteryLevelObserver* iBatteryLevelObserver;

	};

#endif /* __BATTERYLEVEL_H__ */

// End of File
