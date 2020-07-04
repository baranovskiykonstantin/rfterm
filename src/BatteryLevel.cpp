/*
 ============================================================================
 Name        : BatteryLevel.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Battery level notifier.
 ============================================================================
 */

// INCLUDE FILES
#include "BatteryLevel.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CBatteryLevel::CBatteryLevel()
// Constructor.
// ----------------------------------------------------------------------------
//
CBatteryLevel::CBatteryLevel(MBatteryLevelObserver* aObserver) :
	CActive(CActive::EPriorityStandard),
	iBatteryLevelObserver(aObserver)
	{
	}

// ----------------------------------------------------------------------------
// CBatteryLevel::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBatteryLevel* CBatteryLevel::NewL(MBatteryLevelObserver* aObserver)
	{
	CBatteryLevel* self = new(ELeave) CBatteryLevel(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CBatteryLevel::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CBatteryLevel::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPSUidHWRMPowerState, KHWRMBatteryLevel));
	CActiveScheduler::Add(this);
	// initial subscription and process current property value
	RunL();
	}

// ----------------------------------------------------------------------------
// CBatteryLevel::~CBatteryLevel()
// Destructor.
// ----------------------------------------------------------------------------
//
CBatteryLevel::~CBatteryLevel()
	{
	Cancel();
	iProperty.Close();
	}

// ----------------------------------------------------------------------------
// CBatteryLevel::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CBatteryLevel::DoCancel()
	{
	iProperty.Cancel();
	}

// ----------------------------------------------------------------------------
// CBatteryLevel::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CBatteryLevel::RunL()
	{
	//resubscribe before processing new value to prevent missing updates
	iProperty.Subscribe(iStatus);
	SetActive();

	if (iBatteryLevelObserver)
		{
		TInt keyValue;
		if (iProperty.Get(keyValue) == KErrNotFound)
			{
			// property deleted
			iBatteryLevelObserver->HandleBatteryLevelChangeL(EBatteryLevelUnknown);
			}
		else
			{
			iBatteryLevelObserver->HandleBatteryLevelChangeL((EPSHWRMBatteryLevel)keyValue);
			}
		}
	}

// End of File
