/*
 ============================================================================
 Name        : RFtermBtServiceSearcher.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Searching for a service on a remote machine.
 ============================================================================
 */

#ifndef __RFTERMTBTSERVICESEARCHER_H__
#define __RFTERMTBTSERVICESEARCHER_H__

#include <e32base.h>
#include <bttypes.h>
#include <btextnotifiers.h>
#include "RFtermBtObserver.h"
#include "RFtermSdpAttributeParser.h"
#include "RFtermSdpAttributeNotifier.h"
#include "RFtermConstants.h"

/**
* CRFtermBtServiceSearcher
* Searches for a service on a remote machine
*/

class CRFtermBtServiceSearcher
		: public CBase
		, public MSdpAgentNotifier
		, public MRFtermSdpAttributeNotifier
	{

public: // Constructors and destructor

	/*
	* NewL()
	* Create a CRFtermBtServiceSearcher object
	* @return a pointer to the created instance of
	* CRFtermBtServiceSearcher
	*/
	static CRFtermBtServiceSearcher* NewL();

	/**
	* NewLC()
	* Create a CRFtermBtServiceSearcher object
	* @return a pointer to the created instance of
	* CRFtermBtServiceSearcher
	*/
	static CRFtermBtServiceSearcher* NewLC();

	/**
	* ~CRFtermBtServiceSearcher()
	* Destroy the object and release all memory objects
	*/
	virtual ~CRFtermBtServiceSearcher();

public: // New functions

	/**
	* SelectDeviceByDiscoveryL()
	* Select a device
	* @param aObserverRequestStatus the observer that is to
	* be notified when the device selection is complete.
	*/
	void SelectDeviceByDiscoveryL(
		TRequestStatus& aObserverRequestStatus);

	/**
	* FindServiceL()
	* Find a service on the specified device
	* @param aObserverRequestStatus the observer that is to be notified
	* when the service search is complete
	*/
	void FindServiceL(TRequestStatus& aObserverRequestStatus);

	/**
	* BTDevAddr()
	* @return the bluetooth device address
	*/
	const TBTDevAddr& BTDevAddr();

	/**
	* ResponseParams()
	* @return Information about the device selected by the user
	*/
	const TBTDeviceResponseParams& ResponseParams();

	/**
	* Port()
	* Retrieve the port on which the service is installed
	* @return the port number
	*/
	TInt Port();

	/**
	 * SetObserver()
	 * Assing an observer to receive log messages.
	 */
	void SetObserver(MRFtermBtObserver* aObserver);

protected: // New functions

	/**
	* Finished()
	* The search has finished. Notify the observer
	* that the process is complete.
	*/
	virtual void Finished(TInt aError = KErrNone);

	/**
	* HasFinishedSearching()
	* Is the instance still wanting to search.
	* @return EFalse if the instance wants searching to continue.
	*/
	virtual TBool HasFinishedSearching() const;

	/**
	* HasFoundService()
	* @return ETrue if a service has been found
	*/
	TBool HasFoundService() const;

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
	* CRFtermBtServiceSearcher()
	* Constructs this object
	*/
	CRFtermBtServiceSearcher();

	/**
	* ConstructL()
	* Performs second phase construction of this object
	*/
	void ConstructL();

private: // New functions

	/**
	 * NotifyL()
	 * Send log message to observer.
	 */
	void NotifyL(const TDesC& aMessage);

public: // from MSdpAgentNotifier

	/**
	* NextRecordRequestComplete()
	* Process the result of the next record request
	* @param aError the error code
	* @param aHandle the handle of the service record
	* @param aTotalRecordsCount the total number of matching
	* service records
	*/
	void NextRecordRequestComplete(TInt aError,
		TSdpServRecordHandle aHandle,
		TInt aTotalRecordsCount);

	/**
	* AttributeRequestResult()
	* Process the next attribute requested
	* @param aHandle the handle of the service record
	* @param aAttrID the id of the attribute
	* @param aAttrValue the value of the attribute
	*/
	void AttributeRequestResult(TSdpServRecordHandle aHandle,
		TSdpAttributeID aAttrID,
		CSdpAttrValue* aAttrValue);

	/**
	* AttributeRequestComplete()
	* Process the arrtibute request completion
	* @param aHandle the handle of the service record
	* @param aError the error code
	*/
	void AttributeRequestComplete(TSdpServRecordHandle aHandle,
		TInt aError);

private: // Functions from base classes

	/**
	* NextRecordRequestCompleteL()
	* Process the result of the next record request
	* @param aError the error code
	* @param aHandle the handle of the service record
	* @param aTotalRecordsCount the total number of
	* matching service records
	*/
	void NextRecordRequestCompleteL(TInt aError,
		TSdpServRecordHandle aHandle,
		TInt aTotalRecordsCount);

	/**
	* AttributeRequestResultL()
	* Process the next attribute requested
	* @param aHandle the handle of the service record
	* @param aAttrID the id of the attribute
	* @param aAttrValue the value of the attribute
	*/
	void AttributeRequestResultL(TSdpServRecordHandle aHandle,
		TSdpAttributeID aAttrID,
		CSdpAttrValue* aAttrValue);

	/**
	* AttributeRequestCompleteL()
	* Process the arrtibute request completion
	* @param aHandle the handle of the service record
	* @param aError the error code
	*/
	void AttributeRequestCompleteL(TSdpServRecordHandle,
		TInt aError);

private: // data

	/**
	* iStatusObserver pointer to the request status observer
	* Not owned by CRFtermBtServiceSearcher
	*/
	TRequestStatus* iStatusObserver;

	/**
	* iIsDeviceSelectorConnected is the device
	* selector connected?
	*/
	TBool iIsDeviceSelectorConnected;

	/**
	* iDeviceSelector
	* a handle to the bluetooth device selector notifier
	*/
	RNotifier iDeviceSelector;

	/**
	* iResponse
	* the response of the device selection.
	*/
	TBTDeviceResponseParamsPckg iResponse;

	/**
	* iSelectionFilter
	* Request a device selection
	*/
	TBTDeviceSelectionParamsPckg iSelectionFilter;
	/**
	* iAgent a connetction to the SDP client
	* Owned by CRFtermBtServiceSearcher
	*/
	CSdpAgent* iAgent;

	/**
	* iSdpSearchPattern a search pattern
	* Owned by CRFtermBtServiceSearcher
	*/
	CSdpSearchPattern* iSdpSearchPattern;

	/**
	* iHasFoundService has the service been found ?
	*/
	TBool iHasFoundService;

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

	/**
	* iObserver the handler of log messages
	*/
	MRFtermBtObserver* iObserver;

	};

#endif // __RFTERMTBTSERVICESEARCHER_H__

// End of File
