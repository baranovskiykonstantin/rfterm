/*
 ============================================================================
 Name        : RFtermBatteryStatus.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Battery status notifier.
 ============================================================================
 */

// INCLUDE FILES
#include "RFtermBatteryStatus.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CRFtermBatteryStatus::CRFtermBatteryStatus()
// Constructor.
// ----------------------------------------------------------------------------
//
CRFtermBatteryStatus::CRFtermBatteryStatus(MRFtermBatteryStatusObserver* aObserver) :
	CActive(CActive::EPriorityStandard),
	iBatteryStatusObserver(aObserver)
	{
	}

// ----------------------------------------------------------------------------
// CRFtermBatteryStatus::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermBatteryStatus* CRFtermBatteryStatus::NewL(MRFtermBatteryStatusObserver* aObserver)
	{
	CRFtermBatteryStatus* self = new(ELeave) CRFtermBatteryStatus(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermBatteryStatus::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CRFtermBatteryStatus::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPSUidHWRMPowerState, KHWRMBatteryStatus));
	CActiveScheduler::Add(this);
	// initial subscription and process current property value
	RunL();
	}

// ----------------------------------------------------------------------------
// CRFtermBatteryStatus::~CRFtermBatteryStatus()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermBatteryStatus::~CRFtermBatteryStatus()
	{
	Cancel();
	iProperty.Close();
	}

// ----------------------------------------------------------------------------
// CRFtermBatteryStatus::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CRFtermBatteryStatus::DoCancel()
	{
	iProperty.Cancel();
	}

// ----------------------------------------------------------------------------
// CRFtermBt::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CRFtermBatteryStatus::RunL()
	{
	//resubscribe before processing new value to prevent missing updates
	iProperty.Subscribe(iStatus);
	SetActive();

	if (iBatteryStatusObserver)
		{
		TInt keyValue;
		if (iProperty.Get(keyValue) == KErrNotFound )
			{
			// property deleted
			iBatteryStatusObserver->HandleBatteryStatusChangeL(EBatteryStatusUnknown);
			}
		else
			{
			iBatteryStatusObserver->HandleBatteryStatusChangeL((EPSHWRMBatteryStatus)keyValue);
			}
		}
	}

// End of File
