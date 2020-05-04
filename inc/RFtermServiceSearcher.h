/*
 ============================================================================
 Name        : RFtermServiceSearcher.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Searching for a service on a remote machine.
 ============================================================================
 */

#ifndef RFTERMSERVICESEARCHER_H
#define RFTERMSERVICESEARCHER_H

// INCLUDES
#include <e32base.h>

#include "RFtermBtObserver.h"
#include "RFtermBtServiceSearcher.h"

/**
* CRFtermServiceSearcher
* Searches for a chat service.
*/
class CRFtermServiceSearcher : public CRFtermBtServiceSearcher
	{
public: // Constructors and destructor

	/*
	* NewL()
	* Create a CRFtermServiceSearcher object
	* @return a pointer to the created instance of
	* CRFtermServiceSearcher
	*/
	static CRFtermServiceSearcher* NewL();

	/**
	* NewLC()
	* Create a CRFtermServiceSearcher object
	* @return a pointer to the created instance of
	* CRFtermServiceSearcher
	*/
	static CRFtermServiceSearcher* NewLC();

	/**
	* ~CMessageServiceSearcher()
	* Destroy the object and release all memory objects.
	*/
	virtual ~CRFtermServiceSearcher();

public: // New function

	/**
	* Port()
	* Retrieve the port on which the service is installed
	* @return the port number
	*/
	TInt Port();

protected: // New functions

	/**
	* ServiceClass()
	* @return the service class uid.
	*/
	const TUUID& ServiceClass() const;

	/**
	* FoundElementL()
	* Read the data element
	* @param aKey a key that identifies the element
	* @param aValue the data element
	*/
	virtual void FoundElementL(TInt aKey, CSdpAttrValue& aValue);

	/**
	* ProtocolList()
	* @return the attribute list.
	*/
	RArray<TRFtermSdpAttributeParser::TRFtermSdpAttributeNode>& ProtocolList();

private: // Constructors

	/**
	* CRFtermServiceSearcher()
	* Constructs this object
	*/
	CRFtermServiceSearcher();

	/**
	* ConstructL()
	* Performs second phase construction of this object
	*/
	void ConstructL();

private: // data

	/**
	* iServiceClass
	* the service class UUID to search for
	*/
	TUUID iServiceClass;

	/**
	* iPort
	* the port number that the remote service is installed
	*/
	TInt iPort;

	/**
	* iProtocolArray
	* the attribute list
	*/
	RArray<TRFtermSdpAttributeParser::TRFtermSdpAttributeNode> iProtocolArray;
	};

#endif // RFTERMSERVICESEARCHER_H

// End of File
