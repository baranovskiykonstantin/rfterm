/*
 ============================================================================
 Name        : RFtermBt.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Bluetooth client.
 ============================================================================
 */

#ifndef __RFTERMBT_H__
#define __RFTERMBT_H__

#include <es_sock.h>
#include <bt_sock.h>
#include <btextnotifiers.h>
#include <btsdp.h>
#include <btdevice.h>
#include "RFtermBtServiceSearcher.h"
#include "RFtermBtServiceAdvertiser.h"
#include "RFtermBtObserver.h"
#include "RFtermConstants.h"
#include "RFtermBatteryStatus.h"

/**
* TRFtermState
* The state of the active object, determines behaviour within
* the RunL method.
* EWaitingToGetDevice waiting for the user to select a device
* EGettingDevice searching for a device
* EGettingService searching for a service
* EGettingConnection connecting to a service on a remote machine
* EConnected connected to a service on a remote machine
* ESendingMessage sending a message to the remote machine
* ESendingFile sending content of a file to the remote machine
* EWaitingForMessage wainting for message from remote machine
* EConnecting connecting to remote machine
* EDisconnecting disconnecting from remote machine
*/

enum TRFtermState
	{
	EWaitingToGetDevice,
	EGettingDevice,
	EGettingService,
	EGettingConnection,
	EConnected,
	EDisconnected,
	ESendingMessage,
	ESendingFile,
	EWaitingForMessage,
	EConnecting,
	EDisconnecting
	};

/**
* CRFtermBt
* Connects and sends messages to a remote machine using bluetooth
*/
class CRFtermBt :
	public CActive,
	public MRFtermBatteryStatusObserver
	{

public: // Constructors and destructor

	/**
	* NewL()
	* Construct a CRFtermClient
	* @param aLog the log to send output to
	* @return a pointer to the created instance of CRFtermClient
	*/
	static CRFtermBt* NewL();

	/**
	* NewLC()
	* Construct a CRFtermClient
	* @param aLog the log to send output to
	* @return a pointer to the created instance of CRFtermClient
	*/
	static CRFtermBt* NewLC();

	/**
	* CRFtermBt()
	* Constructs this object
	* @param aLog the log to send output to
	*/
	CRFtermBt();

	/**
	* ~CRFtermBt()
	* Destroy the object and release all memory objects.
	* Close any open sockets.
	*/
	virtual ~CRFtermBt();

public: // New functions

	/**
	* ConnectL()
	* Connect to an available service on a remote machine
	*/
	void ConnectL();

	/**
	* DisconnectL()
	* Disconnect from remote machine
	*/
	void DisconnectL();

	/**
	* SendMessageL()
	* Sends a message to a service on a remote machine.
	* @param aText text to send
	* @param aDoEcho send aText to observer as received data
	*/
	void SendMessageL(TDesC& aText, const TBool aDoEcho=EFalse);

	/**
	* SendFileL()
	* Sends a file to a service on a remote machine.
	* @param aFileName name of the file to send
	* @param aDoEcho to send content of file to observer as received data
	*/
	void SendFileL(const TDesC& aFileName, const TBool aDoEcho=EFalse);

	/**
	* StartL()
	* Start server and waiting for connection.
	*/
	void StartL();

	/**
	* StopL()
	* Stop server.
	*/
	void StopL();

	/**
	* SetServer()
	* Set servermode
	* @param aServerMode ETrue if servermode is used
	*/
	void SetServer(TBool aServerMode);

	/**
	* Server()
	* Get Servermode
	* @return servermode
	*/
	TBool Server();

	/**
	* SetState()
	* Set state
	* @param aState the state of the engine
	*/
	void SetState(TRFtermState aState);

	/**
	* State()
	* Get State
	* @return state
	*/
	TInt State();

	/**
	* IsConnected()
	* @return ETrue if the client is fully connected to the server.
	*/
	TBool IsConnected();

	/**
	* IsConnecting()
	* @return ETrue if the client is establishing a
	* connection to the server.
	*/
	TBool IsConnecting();

	/**
	* IsReadyToSend()
	* @return ETrue if the client can send a message or a file.
	*/
	TBool IsReadyToSend();

	/**
	 * SetObserver()
	 * Assing an observer to receive log messages.
	 */
	void SetObserver(MRFtermBtObserver* aObserver);

protected: // from CActive

	/**
	* DoCancel()
	* Cancel any outstanding requests
	*/
	void DoCancel();

	/**
	* RunL()
	* Respond to an event
	*/
	void RunL();

private: // Functions from base classes

	/**
	* ConstructL()
	* Performs second phase construction of this object
	*/
	void ConstructL();

private: // From MRFtermBatteryStatusObserver

	/**
	 * HandleBatteryStatusChangeL()
	 * Battery status change notify.
	 */
	void HandleBatteryStatusChangeL(EPSHWRMBatteryStatus aBatteryStatus);

private: // New private functions

	/**
	* ConnectToServerL
	* Connects to the service
	*/
	void ConnectToServerL();

	/**
	* DisonnectFromServerL()
	* Disconnects from the service
	*/
	void DisconnectFromServerL();

	/**
	* RequestData()
	* Request data from the client, this object is notified
	* when data becomes available
	*/
	void RequestData();

	/**
	* SetSecurityWithChannelL
	* Sets the security on the channel port
	* And returns the channel available.
	* @param aAuthentication is authentication used
	* @param aEncryption is encryption used
	* @param aAuthorisation is authorisation used
	* @param aDenied is denied used
	* @return an available channel
	*/
	void SetSecurityWithChannelL(TBool aAuthentication,
		TBool aEncryption,
		TBool aAuthorisation,
		TBool aDenied,
		TInt& aChannel
	);

	/**
	* SetSecurityOnChannelL()
	* Sets the security on the channel port
	* @param aAuthentication is authentication used
	* @param aEncryption is encryption used
	* @param aAuthorisation is authorisation used
	* @param aChannel used channel ID
	*/
	static void SetSecurityOnChannelL(TBool aAuthentication,
		TBool aEncryption,
		TBool aAuthorisation,
		TInt aChannel
	);

	/**
	 * NotifyL()
	 * Send log message to observer.
	 */
	void NotifyL(const TDesC& aMessage);

	/**
	 * NotifyDeviceIsConnectedL()
	 * Send to observer information about connected BT device.
	 */
	void NotifyDeviceIsConnectedL();

	/**
	 * NotifyDeviceIsDisconnectedL()
	 * Send to observer BT device disconnect notify.
	 */
	void NotifyDeviceIsDisconnectedL();

	/**
	 * AllowLowPowerModes()
	 * Enable low power modes to save the battery.
	 */
	void AllowLowPowerModes();

	/**
	 * PreventLowPowerModes()
	 * Disable low power modes to reach max bandwidth.
	 */
	void PreventLowPowerModes();

private: // Data

	/**
	* iState the current state of the client
	*/
	TRFtermState iState;

	/**
	* iServerMode indicates server mode
	*/
	TBool iServerMode;

	/**
	* iServiceSearcher searches for service this
	* client can connect to.
	* Owned by CRFtermBt
	*/
	CRFtermBtServiceSearcher* iServiceSearcher;

	/**
	* iMessage a copy of the message to send
	* Owned by CRFtermBt
	*/
	HBufC8* iMessage;

	/**
	 * iFileSession
	 * file session for file reading
	 */
	RFs iFileSession;

	/**
	 * iFile
	 * file to send
	 */
	RFile iFile;

	/**
	 * iFileIsOpenned
	 * flag to know is iFile opened or not
	 */
	TBool iFileIsOpenned;

	/**
	 * iDoFileEcho
	 * variable for storing aDoEcho setting of SendFile()
	 * while sending the file chunks.
	 */
	TBool iDoFileEcho;

	/**
	* iSocketServer
	* a connection to the socket server
	*/
	RSocketServ iSocketServer;

	/**
	* iSocket
	* a socket to connect with
	*/
	RSocket iSocket;

	/**
	* iServiceClass
	* the service class UUID to search for
	*/
	TUUID iServiceClass;

	/**
	* iBuffer
	* the buffer to read data to
	*/
	TBuf8<KRFtermTextBufLength> iBuffer;

	/**
	* iAdvertiser used to advertise this service
	* Owned by CRFtermServer
	*/
	CRFtermBtServiceAdvertiser* iAdvertiser;

	/**
	* iAcceptedSocket
	* the socket that connections are accepted to
	*/
	RSocket iAcceptedSocket;

	/**
	* iLen
	* length of data read
	*/
	TSockXfrLength iLen;

	/**
	* iActiveSocket
	* the active socket
	*/
	RSocket* iActiveSocket;

	/**
	 * iRemoteDevice
	 * information about remote BT device
	 */
	CBTDevice* iRemoteDevice;

	/**
	* iObserver the handler of log messages
	*/
	MRFtermBtObserver* iObserver;

	/**
	 * iBatteryStatus
	 * the battery status checker
	 */
	CRFtermBatteryStatus* iBatteryStatus;

	/**
	 * iBatteryIsOK
	 * if battery is OK the low power modes of BT are not used
	 * to reach max bandwidth.
	 */
	TBool iBatteryIsOK;

	/**
	 * iBTPhysicalLinkAdapter
	 * enable/disable low power modes.
	 */
	RBTPhysicalLinkAdapter iBTPhysicalLinkAdapter;

	};

#endif /* __RFTERMBT_H__ */

// End of File
