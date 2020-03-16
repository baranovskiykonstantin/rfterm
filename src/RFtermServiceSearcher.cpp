/*
 ============================================================================
 Name        : RFtermServiceSearcher.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Searching for a service on a remote machine.
 ============================================================================
 */

// INCLUDE FILES
#include <bt_sock.h>
#include "RFtermServiceSearcher.h"
#include "RFtermBtServiceSearcher.pan"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CRFtermServiceSearcher::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermServiceSearcher* CRFtermServiceSearcher::NewL(CRFtermOutput* aOutput)
	{
	CRFtermServiceSearcher* self = CRFtermServiceSearcher::NewLC(aOutput);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermServiceSearcher::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRFtermServiceSearcher* CRFtermServiceSearcher::NewLC(CRFtermOutput* aOutput)
	{
	CRFtermServiceSearcher* self = new (ELeave) CRFtermServiceSearcher(aOutput);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CRFtermServiceSearcher::CRFtermServiceSearcher()
// Construcor.
// ----------------------------------------------------------------------------
//
CRFtermServiceSearcher::CRFtermServiceSearcher(CRFtermOutput* aOutput)
	: CRFtermBtServiceSearcher(aOutput),
	iServiceClass(KServiceClass),
	iPort(-1)
	{
	}

// ----------------------------------------------------------------------------
// CRFtermServiceSearcher::~CRFtermServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CRFtermServiceSearcher::~CRFtermServiceSearcher()
	{
	iProtocolArray.Close();
	}

// ----------------------------------------------------------------------------
// CRFtermServiceSearcher::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CRFtermServiceSearcher::ConstructL()
	{
	// no implementation required
	}

// ----------------------------------------------------------------------------
// CRFtermServiceSearcher::ServiceClass()
// The service class to search.
// ----------------------------------------------------------------------------
//
const TUUID& CRFtermServiceSearcher::ServiceClass() const
	{
	return iServiceClass;
	}

// ----------------------------------------------------------------------------
// CRFtermServiceSearcher::ProtocolList()
// The list of Protocols required by the service.
// ----------------------------------------------------------------------------
//
 RArray <TRFtermSdpAttributeParser::TRFtermSdpAttributeNode>& CRFtermServiceSearcher
::ProtocolList()
	{
	TRFtermSdpAttributeParser::TRFtermSdpAttributeNode attrib;
	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckType);
	attrib.SetType(ETypeDES);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckType);
	attrib.SetType(ETypeDES);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckValue);
	attrib.SetType(ETypeUUID);
	attrib.SetValue(KL2CAP);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckEnd);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckType);
	attrib.SetType(ETypeDES);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckValue);
	attrib.SetType(ETypeUUID);
	attrib.SetValue(KRFCOMM);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::EReadValue);
	attrib.SetType(ETypeUint);
	attrib.SetValue(KRfcommChannel);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckEnd);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::ECheckEnd);
	iProtocolArray.Append(attrib);

	attrib.SetCommand(TRFtermSdpAttributeParser::EFinished);
	iProtocolArray.Append(attrib);

	return iProtocolArray;
	}

// ----------------------------------------------------------------------------
// CRFtermServiceSearcher::FoundElementL()
// Read the data element.
// ----------------------------------------------------------------------------
//
void CRFtermServiceSearcher::FoundElementL(TInt aKey, CSdpAttrValue& aValue)
	{
	__ASSERT_ALWAYS(aKey == static_cast<TInt>(KRfcommChannel),
		Panic(ERFtermBtServiceSearcherProtocolRead));
	iPort = aValue.Uint();
	}

// ----------------------------------------------------------------------------
// CRFtermServiceSearcher::Port()
// Port connection on the remote machine.
// ----------------------------------------------------------------------------
//
TInt CRFtermServiceSearcher::Port()
	{
	return iPort;
	}

// End of File
