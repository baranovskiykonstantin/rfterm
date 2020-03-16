/*
 ============================================================================
 Name        : RFtermServiceAdvertiser.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Advertises a service in the SDP database.
 ============================================================================
 */

#ifndef RFTERMSERVICEADVERTISER_H
#define RFTERMSERVICEADVERTISER_H

// INCLUDES
#include <e32base.h>
#include <btsdp.h>
#include "RFtermBtServiceAdvertiser.h"


// CLASS DECLARATIONS
/**
* CRFtermServiceAdvertiser
* Advertises the message service using the SDP database.
*/
class CRFtermServiceAdvertiser : public CRFtermBtServiceAdvertiser
	{
public: // Constructors and destructor

	/**
	* NewL()
	* Create a CRFtermServiceAdvertiser object
	* @return a pointer to the created instance of
	* CRFtermServiceAdvertiser
	*/
	static CRFtermServiceAdvertiser* NewL();

	/**
	* NewLC()
	* Create a CRFtermServiceAdvertiser object, which will
	* draw itself to aRect.
	* @return a pointer to the created instance of
	* CRFtermServiceAdvertiser
	*/
	static CRFtermServiceAdvertiser* NewLC();

	/**
	* ~CRFtermServiceAdvertiser()
	* Destroy the object and release all memory objects.
	* Close any open sockets
	*/
	virtual ~CRFtermServiceAdvertiser();

protected: // from CRFtermBtServiceAdvertiser

	/**
	* BuildProtocolDescriptionL()
	* Builds the protocol description
	* @param aProtocolDescriptor the protocol descriptor
	* @param aPort the service port
	*/
	void BuildProtocolDescriptionL(
		CSdpAttrValueDES* aProtocolDescriptor, TInt aPort);

	/**
	* ServiceClass()
	* @return the service class
	*/
	TInt ServiceClass();

	/**
	* ServiceName()
	* @return the service name
	*/
	const TDesC& ServiceName();

	/**
	* ServiceDescription()
	* @return the service description
	*/
	const TDesC& ServiceDescription();

private: // Constructors

	/**
	* CRFtermServiceAdvertiser()
	* Constructs this object
	*/
	CRFtermServiceAdvertiser();

	/**
	* ConstructL()
	* 2nd phase construction of this object
	*/
	void ConstructL();

private: // data

	/**
	* iServiceName
	* Owned by CRFtermServiceAdvertiser
	*/
	HBufC* iServiceName;

	/**
	* iServiceDescription
	* Owned by CRFtermServiceAdvertiser
	*/
	HBufC* iServiceDescription;
	};

#endif // RFTERMSERVICEADVERTISER_H

// End of File
