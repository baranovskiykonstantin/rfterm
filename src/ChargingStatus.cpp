/*
 ============================================================================
 Name        : ChargingStatus.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Charging status notifier.
 ============================================================================
 */

// INCLUDE FILES
#include "ChargingStatus.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CChargingStatus::CChargingStatus()
// Constructor.
// ----------------------------------------------------------------------------
//
CChargingStatus::CChargingStatus(MChargingStatusObserver* aObserver) :
	CActive(CActive::EPriorityStandard),
	iChargingStatusObserver(aObserver)
	{
	}

// ----------------------------------------------------------------------------
// CChargingStatus::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChargingStatus* CChargingStatus::NewL(MChargingStatusObserver* aObserver)
	{
	CChargingStatus* self = new(ELeave) CChargingStatus(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CChargingStatus::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CChargingStatus::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPSUidHWRMPowerState, KHWRMChargingStatus));
	CActiveScheduler::Add(this);
	// initial subscription and process current property value
	RunL();
	}

// ----------------------------------------------------------------------------
// CChargingStatus::~CChargingStatus()
// Destructor.
// ----------------------------------------------------------------------------
//
CChargingStatus::~CChargingStatus()
	{
	Cancel();
	iProperty.Close();
	}

// ----------------------------------------------------------------------------
// CChargingStatus::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CChargingStatus::DoCancel()
	{
	iProperty.Cancel();
	}

// ----------------------------------------------------------------------------
// CChargingStatus::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CChargingStatus::RunL()
	{
	//resubscribe before processing new value to prevent missing updates
	iProperty.Subscribe(iStatus);
	SetActive();

	if (iChargingStatusObserver)
		{
		TInt keyValue;
		if (iProperty.Get(keyValue) == KErrNotFound)
			{
			// property deleted
			iChargingStatusObserver->HandleChargingStatusChangeL(EChargingStatusError);
			}
		else
			{
			iChargingStatusObserver->HandleChargingStatusChangeL((EPSHWRMChargingStatus)keyValue);
			}
		}
	}

// End of File
