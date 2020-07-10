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
	iBatteryLevelValue(EBatteryLevelUnknown),
	iChargingStatusValue(EChargingStatusNotConnected),
	iObserver(aObserver)
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
	iBatteryLevel = CBatteryLevel::NewL(this);
	iChargingStatus = CChargingStatus::NewL(this);
	}

// ----------------------------------------------------------------------------
// CRFtermBatteryStatus::~CRFtermBatteryStatus()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermBatteryStatus::~CRFtermBatteryStatus()
	{
	delete iChargingStatus;
	iChargingStatus = NULL;
	
	delete iBatteryLevel;
	iBatteryLevel = NULL;
	}

// ----------------------------------------------------------------------------
// CRFtermBatteryStatus::IsOK()
// Get status of the battery.
// ----------------------------------------------------------------------------
//
TBool CRFtermBatteryStatus::IsOK()
	{
	if (iBatteryLevelValue > EBatteryLevelLevel2 || iChargingStatusValue >= EChargingStatusCharging)
		{
		return ETrue;
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CRFtermBatteryStatus::HandleBatteryLevelChangeL()
// Battery level change notify.
// ----------------------------------------------------------------------------
//
void CRFtermBatteryStatus::HandleBatteryLevelChangeL(EPSHWRMBatteryLevel aBatteryLevel)
	{
	TBool prevStatus = IsOK();
	iBatteryLevelValue = aBatteryLevel;
	TBool newStatus = IsOK();
	if (iObserver && prevStatus != newStatus)
		{
		iObserver->HandleBatteryStatusChangeL();
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBatteryStatus::HandleChargingStatusChangeL()
// Charging status change notify.
// ----------------------------------------------------------------------------
//
void CRFtermBatteryStatus::HandleChargingStatusChangeL(EPSHWRMChargingStatus aChargingStatus)
	{
	TBool prevStatus = IsOK();
	iChargingStatusValue = aChargingStatus;
	TBool newStatus = IsOK();
	if (iObserver && prevStatus != newStatus)
		{
		iObserver->HandleBatteryStatusChangeL();
		}
	}

// End of File
