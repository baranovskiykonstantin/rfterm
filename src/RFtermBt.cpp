/*
 ============================================================================
 Name        : RFtermBT.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Bluetooth client.
 ============================================================================
 */

// INCLUDE FILES
#include <StringLoader.h>
#include <coemain.h>
#include <RFterm_0xae7f53fa.rsg>
#include "RFtermServiceAdvertiser.h"
#include "RFtermServiceSearcher.h"
#include "RFtermAppUi.h"
#include "RFtermBt.h"
#include "RFtermConstants.h"
#include "RFterm.pan"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CRFtermBt::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermBt* CRFtermBt::NewL()
	{
	CRFtermBt* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermBt* CRFtermBt::NewLC()
	{
	CRFtermBt* self = new (ELeave) CRFtermBt();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::CRFtermBt()
// Constructor.
// ----------------------------------------------------------------------------
//
CRFtermBt::CRFtermBt() :
	CActive(CActive::EPriorityStandard),
	iState(EWaitingToGetDevice),
	iServerMode(EFalse)
	{
	CActiveScheduler::Add(this);
	}

// ----------------------------------------------------------------------------
// CRFtermBt::~CRFtermBt()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermBt::~CRFtermBt()
	{
	// Close() will wait forever for Read to complete
	if (State() == EConnected)
		{
		if (iActiveSocket)
			{
			iActiveSocket->CancelRead();
			}
		}
	Cancel();

	iSocket.Close();
	iAcceptedSocket.Close();
	iSocketServer.Close();

	delete iMessage;
	iMessage = NULL;

	delete iServiceSearcher;
	iServiceSearcher = NULL;
	
	delete iAdvertiser;
	iAdvertiser = NULL;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CRFtermBt::ConstructL()
	{
	iServiceSearcher = CRFtermServiceSearcher::NewL();
	iServiceSearcher->SetObserver(iObserver);
	iAdvertiser = CRFtermServiceAdvertiser::NewL();
	User::LeaveIfError(iSocketServer.Connect());
	}

// ----------------------------------------------------------------------------
// CRFtermBt::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CRFtermBt::DoCancel()
	{
	// no implementation required
	}

// ----------------------------------------------------------------------------
// CRFtermBt::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CRFtermBt::RunL()
	{

	HBufC* textResource = NULL;

	if (iStatus == KErrDisconnected)
		{
		// Disconnected
		HBufC* strDisconnected = StringLoader::LoadLC(R_STR_DISCONNECTED);
		NotifyL(*strDisconnected);
		CleanupStack::PopAndDestroy(strDisconnected);
		StopL();
		return;
		}
		
	else if (iStatus == KErrAbort)
		{
		HBufC* strDisconnected = StringLoader::LoadLC(R_STR_DISCONNECTED);
		NotifyL(*strDisconnected);
		CleanupStack::PopAndDestroy(strDisconnected);
		StopL();
		return;
		}

	else if (iStatus != KErrNone)
		{
		switch (State())
			{
			case EGettingDevice:
				if (iStatus == KErrCancel)
					{
					textResource = StringLoader::LoadLC(R_ERR_NO_DEVICE_SELECTED);
					NotifyL(*textResource);
					CleanupStack::PopAndDestroy(textResource);
					}
				SetState(EWaitingToGetDevice);
				break;
				
			case EGettingService:
			case EGettingConnection:
				textResource = StringLoader::LoadLC(R_ERR_CONNECTION_ERROR);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				SetState(EWaitingToGetDevice);
				break;
				
			case EConnected:
				textResource = StringLoader::LoadLC(R_ERR_LOST_CONNECTION);
				NotifyL(*textResource);
				DisconnectFromServerL();
				CleanupStack::PopAndDestroy(textResource);
				SetState(EDisconnecting);
				break;
				
			case ESendingMessage:
				textResource = StringLoader::LoadLC(R_ERR_MESSAGE_FAILED);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				DisconnectFromServerL();
				SetState(EDisconnecting);
				break;
				
			case EDisconnecting:
				if (iStatus == KErrDisconnected)
					{
					textResource = StringLoader::LoadLC(R_STR_DISCONNECT_COMPLETE);
					NotifyL(*textResource);
					CleanupStack::PopAndDestroy(textResource);

					StopL();
					SetState(EWaitingToGetDevice);
					}
				else
					{
					textResource = StringLoader::LoadLC(R_ERR_FAILED_TO_DISCONNECT);
					NotifyL(*textResource);
					CleanupStack::PopAndDestroy(textResource);

					Panic(ERFtermUnableToDisconnect);
					}
				break;
			
			case EWaitingToGetDevice:
				textResource = StringLoader::LoadLC(R_STR_DISCONNECTED);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				break;
				
			default:
				Panic(ERFtermInvalidLogicState);
				break;
			}
		}

	else
		{
		switch (State())
			{
			case EGettingDevice:
				// found a device now search for a suitable service
				NotifyL(iServiceSearcher->ResponseParams().DeviceName());
				SetState(EGettingService);
				iStatus = KRequestPending;  // this means that the RunL 
											// can not be called until
											// this program does something 
											// to iStatus
				iServiceSearcher->FindServiceL(iStatus);
				SetActive();
				break;
				
			case EConnecting:
				textResource = StringLoader::LoadLC(R_STR_CONNECTED);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				
				// do not accept any more connections
				iAdvertiser->UpdateAvailabilityL(EFalse);
				RequestData();
				SetState(EConnected);
				break;
				
			case EGettingService:
				textResource = StringLoader::LoadLC(R_STR_FOUND_SERVICE);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				SetState(EGettingConnection);
				ConnectToServerL();
				break;
				
			case EGettingConnection:
				textResource = StringLoader::LoadLC(R_STR_CONNECTED);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				SetState(EConnected);
				RequestData();
				break;
				
			case EConnected:
				textResource = HBufC::NewLC(iBuffer.Length());
				textResource->Des().Copy(iBuffer); // Convert buf8 to buf16
				if (iObserver)
					{
					iObserver->HandleBtDataL(*textResource);
					}
				iBuffer.Zero();
				CleanupStack::PopAndDestroy(textResource);
				RequestData();
				break;
				
			case ESendingMessage:
				SetState(EConnected);
				RequestData();
				break;
				
			case EDisconnecting:
				textResource = StringLoader::LoadLC(R_STR_DISCONNECT_COMPLETE);
				NotifyL(*textResource);
				CleanupStack::PopAndDestroy (textResource);
				iSocket.Close();
				SetState(EWaitingToGetDevice);
				break;
			
			case EWaitingToGetDevice:
				
				break;
				
			default:
				Panic(ERFtermInvalidLogicState);
				break;
			};
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::IsServer()
// True if the acting as server.
// ----------------------------------------------------------------------------
//
TBool CRFtermBt::Server()
	{
	return iServerMode;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::SetServer()
// 
// ----------------------------------------------------------------------------
//
void CRFtermBt::SetServer(TBool aServerMode)
	{
	iServerMode = aServerMode;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::SetState()
// 
// ----------------------------------------------------------------------------
//
void CRFtermBt::SetState(TRFtermState aState)
	{
	iState = aState;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::State()
// 
// ----------------------------------------------------------------------------
//
TInt CRFtermBt::State()
	{
	return iState;
	}

// ----------------------------------------------------------------------------
// CRFtermBt::IsReadyToSendMessage()
// True if the client can send a message.
// ----------------------------------------------------------------------------
//
TBool CRFtermBt::IsReadyToSendMessage()
	{
	return (State() == EConnected);
	}

// ----------------------------------------------------------------------------
// CRFtermBt::IsConnected()
// True if the client can send a message.
// ----------------------------------------------------------------------------
//
TBool CRFtermBt::IsConnected()
	{
	return ((State() == EConnected )||(State() == ESendingMessage));
	}

// ----------------------------------------------------------------------------
// CRFtermBt::IsConnecting()
// True if is establishing a connection.
// ----------------------------------------------------------------------------
//
TBool CRFtermBt::IsConnecting()
	{
	return ((State() == EGettingDevice)
		||
		(State() == EGettingService)
		||
		(State() == EGettingConnection)
		|| 
		(State() == EConnecting)
		);
	}

// ----------------------------------------------------------------------------
// CRFtermBt::IsSendingMessage()
// True if the client is connected.
// ----------------------------------------------------------------------------
//
TBool CRFtermBt::IsSendingMessage()
	{
	return (State() == ESendingMessage);
	}

// ----------------------------------------------------------------------------
// CRFtermBt::ConnectL()
// Connect to a service.
// ----------------------------------------------------------------------------
//
void CRFtermBt::ConnectL()
	{
	if (State() == EWaitingToGetDevice && !IsActive())
		{
		SetState(EGettingDevice);
		iServiceSearcher->SelectDeviceByDiscoveryL(iStatus);
		SetActive();
		}
	else
		{
		HBufC* errClientBusy = StringLoader::LoadLC(R_STR_CLIENT_BUSY);
		NotifyL(*errClientBusy);
		CleanupStack::PopAndDestroy(errClientBusy);
		
		User::Leave(KErrInUse);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::DisconnectL()
// Disconnects from the remote machine.
// ----------------------------------------------------------------------------
//
void CRFtermBt::DisconnectL()
	{
	if ((State() == EConnected)||(State() == ESendingMessage))
		{
		DisconnectFromServerL();
		SetState(EDisconnecting);
		}
	else
		{
		HBufC* errNoConn = StringLoader::LoadLC(R_ERR_NO_CONN);
		NotifyL(*errNoConn);
		CleanupStack::PopAndDestroy(errNoConn);
		User::Leave(KErrDisconnected);
		}
	}

// ----------------------------------------------------------------------------
// CRFtermBt::DisconnectFromServerL()
// Disconnects from the service
// ----------------------------------------------------------------------------
//
void CRFtermBt::DisconnectFromServerL()
	{
	// Terminate all operations
	iSocket.CancelAll();
	Cancel();

	HBufC* strReleasingConn = StringLoader::LoadLC(R_STR_RELEASING_CONN);
	NotifyL(*strReleasingConn);
	CleanupStack::PopAndDestroy(strReleasingConn);
	iSocket.Shutdown(RSocket::ENormal, iStatus);
	SetActive();
	}

// ----------------------------------------------------------------------------
// CRFtermBt::ConnectToServerL()
// Connect to the server.
// ----------------------------------------------------------------------------
//
void CRFtermBt::ConnectToServerL()
	{
	HBufC* strConnecting = StringLoader::LoadLC(R_STR_CONNECTING);
	NotifyL(*strConnecting);
	CleanupStack::PopAndDestroy(strConnecting);

	User::LeaveIfError(iSocket.Open(iSocketServer, KStrRFCOMM));

	TBTSockAddr address;
	address.SetBTAddr(iServiceSearcher->BTDevAddr());
	address.SetPort(iServiceSearcher->Port());

	iSocket.Connect(address, iStatus);

	iActiveSocket = &iSocket;

	#ifdef __WINS__
		// Fix to allow emulator client to connect to server
		User::After(1);
	#endif

	SetActive();
	}


// ----------------------------------------------------------------------------
// CRFtermBt::SendMessageL()
// Send a message to a service on a remote machine.
// ----------------------------------------------------------------------------
//
void CRFtermBt::SendMessageL(TDesC& aText, const TBool aDoEcho)
	{
	if (State() != EConnected)
		{
		User::Leave(KErrDisconnected);
		}

	// stop reading socket
	if (iActiveSocket)
		{
		iActiveSocket->CancelRead();
		}

	if (IsActive())
		{
		Cancel();
		}
	SetState(ESendingMessage);

	// convert 16-bit to 8-bit data
	delete iMessage;
	iMessage = HBufC8::NewL(aText.Length());
	iMessage->Des().Copy(aText);

	if (aDoEcho && iObserver)
		{
		iObserver->HandleBtDataL(aText);
		}

	if (iActiveSocket)
		{
		iActiveSocket->Write(*iMessage, iStatus);
		}

	SetActive();
	}

// ----------------------------------------------------------------------------
// CRFtermBt::StartL()
// Starts the server.
// ----------------------------------------------------------------------------
//
void CRFtermBt::StartL()
	{
	if (State() != EWaitingToGetDevice)
		{
		User::Leave(KErrInUse);
		}

	TInt result(0);
	result = iSocket.Open(iSocketServer, KStrRFCOMM);
	if (result != KErrNone)
		{
		iSocketServer.Close();
		User::Leave(result); 
		}


	// 
	// Set the Socket's security with parameters, 
	// Authentication, Encryption, Authorisation and Denied
	// Method also return the channel available to listen to. 
	TInt channel;
	SetSecurityWithChannelL(EFalse, EFalse, ETrue, EFalse, channel);

	iAdvertiser->StartAdvertisingL(channel);
	iAdvertiser->UpdateAvailabilityL(ETrue);
	
	SetServer(ETrue);

	}

// ----------------------------------------------------------------------------
// CRFtermBt::SetSecurityWithChannelL()
// Sets the security on the channel port and returns the available port.
// ----------------------------------------------------------------------------
//
void CRFtermBt::SetSecurityWithChannelL(
	TBool aAuthentication,
	TBool aEncryption,
	TBool aAuthorisation,
	TBool aDenied,
	TInt& aChannel)

	{

	// Local variable to channel to listen to.
	TInt channel;

	User::LeaveIfError(iSocket.GetOpt(
		KRFCOMMGetAvailableServerChannel, 
		KSolBtRFCOMM, 
		channel));

	TBTSockAddr listeningAddress;
	// Set the Port to listen to.
	listeningAddress.SetPort(channel);

	User::LeaveIfError(iSocket.Bind(listeningAddress));
	User::LeaveIfError(iSocket.Listen(KListeningQueSize));

	// close old connection - if any
	iAcceptedSocket.Close();

	// Open abstract socket
	User::LeaveIfError(iAcceptedSocket.Open(iSocketServer));  

	// Set the Active Object's State to Connecting indicated.
	SetState(EConnecting);

	iSocket.Accept(iAcceptedSocket, iStatus);

	iActiveSocket = &iAcceptedSocket;

	// Set the Active Object Active again,
	SetActive();

	// Write Log events
	HBufC* strWaitingConn = StringLoader::LoadLC(R_STR_WAITING_CONN);
	NotifyL(*strWaitingConn);
	CleanupStack::PopAndDestroy(strWaitingConn);

	HBufC* strPortNumber = StringLoader::LoadLC(R_STR_PORT_NUMBER);
	TBuf<10> channelStr;
	channelStr.Num(channel);
	HBufC* portStrWithNumber = HBufC::NewLC(strPortNumber->Length() + channelStr.Length());
	portStrWithNumber->Des().Copy(*strPortNumber);
	portStrWithNumber->Des().Append(channelStr);
	NotifyL(*portStrWithNumber);
	CleanupStack::PopAndDestroy(2); // strPortNumber, portStrWithNumber

	// Set the security according to.
	TBTServiceSecurity serviceSecurity;
	serviceSecurity.SetUid (KUidRFtermApp);
	serviceSecurity.SetAuthentication (aAuthentication);
	serviceSecurity.SetEncryption (aEncryption);
	serviceSecurity.SetAuthorisation (aAuthorisation);
	serviceSecurity.SetDenied(aDenied);

	// Attach the security settings.
	listeningAddress.SetSecurity(serviceSecurity);

	// Return the port to listen to.
	aChannel = channel;

	}

// ----------------------------------------------------------------------------
// CRFtermBt::StopL()
// Stops the server.
// ----------------------------------------------------------------------------
//
void CRFtermBt::StopL()
	{
	if (State() != EDisconnected)
		{
		if (iAdvertiser->IsAdvertising())
			{
			iAdvertiser->StopAdvertisingL();
			}
		// Close() will wait forever for Read to complete
		if (State() == EConnected)
			{
			if (iActiveSocket)
				{
				iActiveSocket->CancelRead();
				}
			}
		iAcceptedSocket.Close();
		iSocket.Close();
		}
		SetState(EWaitingToGetDevice);
	}

// ----------------------------------------------------------------------------
// CRFtermBt::RequestData()
// Request data from the client.
// ----------------------------------------------------------------------------
//
void CRFtermBt::RequestData()
	{
	if (iActiveSocket)
		{
		iActiveSocket->RecvOneOrMore(iBuffer, 0, iStatus, iLen);
		}
	SetActive();
	}

void CRFtermBt::SetObserver(MRFtermBtObserver* aObserver)
	{
	iObserver = aObserver;
	}

void CRFtermBt::NotifyL(const TDesC& aMessage)
	{
	if (iObserver)
		{
		iObserver->HandleBtNotifyL(aMessage);
		}
	}

// End of File
