/*
 ============================================================================
 Name        : RFtermBtServiceAdvertiser.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Advertises a service in the SDP database.
 ============================================================================
 */

#ifndef __RFTERMBTSERVICEADVERTISER_H__
#define __RFTERMBTSERVICEADVERTISER_H__

#include <e32base.h>
#include <btsdp.h>
#include <StringLoader.h>

/**
* CRFtermBtServiceAdvertiser
* Advertises a service in the SDP database.
*/
class CRFtermBtServiceAdvertiser : public CBase
	{

public: // Constructors and destructor

	/**
	* NewL()
	* Create a CRFtermBtServiceAdvertiser object
	* @return a pointer to the created instance of
	* CRFtermBtServiceAdvertiser
	*/
	static CRFtermBtServiceAdvertiser* NewL();

	/**
	* NewLC()
	* Create a CRFtermBtServiceAdvertiser object, which will
	* draw itself to aRect.
	* @return a pointer to the created instance of
	* CRFtermBtServiceAdvertiser
	*/
	static CRFtermBtServiceAdvertiser* NewLC();

	/**
	* ~CRFtermBtServiceAdvertiser
	* Destroy the object, close all open handles and remove
	* the advertised service.
	*/
	virtual ~CRFtermBtServiceAdvertiser();

public: // New functions

	/**
	* StartAdvertisingL
	* Start the advertising of this service
	* @param aPort the port being used by the service
	*/
	void StartAdvertisingL(TInt aPort);

	/**
	* StopAdvertisingL
	* Stop advertising this service. Remove the record
	* from the sdp database
	*/
	void StopAdvertisingL();

	/**
	* IsAdvertising
	* Does the SDP database contain a record for this service
	* @return ETrue whether the service is being advertised
	*/
	TBool IsAdvertising();

	/**
	* UpdateAvailabilityL
	* Update the service availability field of the service record
	* @param aIsAvailable ETrue is the service is not busy.
	*/
	void UpdateAvailabilityL(TBool aIsAvailable);

protected: // New functions

	/**
	* ConnectL
	* Connect to the SDP database
	* CRFtermBtServiceAdvertiser()
	* Constructs this object^M
	*/
	void ConnectL();

	/**
	* BuildProtocolDescriptionL
	* Builds the protocol description
	* @param aProtocolDescriptor the protocol descriptor
	* @param aPort the service port
	*/
	void BuildProtocolDescriptionL(
		CSdpAttrValueDES* aProtocolDescriptor, TInt aPort);

	/**
	* ServiceClass
	* @return the service class
	*/
	TInt ServiceClass();

	/**
	* ServiceName
	* @return the service name
	*/
	const TDesC& ServiceName();

	/**
	* ServiceDescription
	* @return the service description
	*/
	const TDesC& ServiceDescription();

private: // Constructors

	/**
	* CRFtermBtServiceAdvertiser()
	* Constructs this object
	*/
	CRFtermBtServiceAdvertiser();

	/**
	* ConstructL()
	* 2nd phase construction of this object
	*/
	void ConstructL();

private: // data

	/**
	* iSdpSession
	* a handle to the SDP session
	*/
	RSdp iSdpSession;

	/**
	* iSdpDatabase
	* a connection to the SDP database
	*/
	RSdpDatabase iSdpDatabase;

	/**
	* iRecord
	* the record handle of the service record for this server
	*/
	TSdpServRecordHandle iRecord;

	/**
	* iRecordState
	* the current record state - change number
	*/
	TInt iRecordState;

	/**
	* iIsConnected
	* has a connection been made to the SDP Database
	*/
	TBool iIsConnected;

	/**
	* iServiceName
	* Owned by CRFtermBtServiceAdvertiser
	*/
	HBufC* iServiceName;

	/**
	* iServiceDescription
	* Owned by CRFtermBtServiceAdvertiser
	*/
	HBufC* iServiceDescription;

	};

#endif // __RFTERMBTSERVICEADVERTISER_H__

// End of File
