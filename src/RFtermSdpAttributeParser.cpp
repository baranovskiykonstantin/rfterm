/*
 ============================================================================
 Name        : RFtermSdpAttributeParser.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Check an SDP attribute value and read selected parts.
 ============================================================================
 */

// INCLUDE FILES
#include <bt_sock.h>

#include "RFtermSdpAttributeParser.h"
#include "RFtermSdpAttributeParser.pan"
#include "RFtermSdpAttributeNotifier.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::TRFtermSdpAttributeParser()
// Construct a TRFtermSdpAttributeParser.
// ----------------------------------------------------------------------------
//
TRFtermSdpAttributeParser::TRFtermSdpAttributeParser(
	RArray<TRFtermSdpAttributeParser::TRFtermSdpAttributeNode>& aNodeList,
	MRFtermSdpAttributeNotifier& aObserver) :
	iObserver(aObserver),
	iNodeList(aNodeList),
	iCurrentNodeIndex(0)
	{
	iCurrentNode = iNodeList[iCurrentNodeIndex];
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::~TRFtermSdpAttributeParser()
// Destructor.
// ----------------------------------------------------------------------------
//
TRFtermSdpAttributeParser::~TRFtermSdpAttributeParser()
	{
	// No implementation needed	
	}
	
// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::HasFinished()
// Check if parsing processed the whole list.
// ----------------------------------------------------------------------------
//
TBool TRFtermSdpAttributeParser::HasFinished() const
	{
	return (iCurrentNode.Command() == EFinished);
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::VisitAttributeValueL()
// Process a data element.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::VisitAttributeValueL(CSdpAttrValue& aValue,
	TSdpElementType aType)
	{
	switch (iCurrentNode.Command())
		{
		case ECheckType:
			CheckTypeL(aType);
			break;

		case ECheckValue:
			CheckTypeL(aType);
			CheckValueL(aValue);
			break;

		case ECheckEnd:
			User::Leave(KErrTooBig); //list element contains too many items
			break;

		case ESkip:
			break;  // no checking required

		case EReadValue:
			CheckTypeL(aType);
			ReadValueL(aValue);
			break;

		case EFinished:
			User::Leave(KErrEof); // element is after
			return; // value should have ended

		default:
			Panic(ERFtermSdpAttributeParserInvalidCommand);
		}

	AdvanceL();
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::StartListL()
// Process the start of a data element list.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::StartListL(CSdpAttrValueList& /*aList*/)
	{
	// no checks done here
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::EndListL()
// Process the end of a data element list.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::EndListL()
	{
	// check we are at the end of a list
	if (iCurrentNode.Command() != ECheckEnd)
		{
		User::Leave(KErrGeneral);
		}

	AdvanceL();
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::CheckTypeL()
// Check the type of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::CheckTypeL(TSdpElementType aElementType) const
	{
	if (iCurrentNode.Type() != aElementType)
		{
		User::Leave(KErrGeneral);
		}
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::CheckValueL()
// Check the value of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::CheckValueL(CSdpAttrValue& aValue) const
	{
	switch (aValue.Type())
		{
		case ETypeNil:
			Panic(ERFtermSdpAttributeParserNoValue);
			break;

		case ETypeUint:
			if (aValue.Uint() != (TUint) iCurrentNode.Value())
				{
				User::Leave(KErrArgument);
				}
			break;

		case ETypeInt:
			if (aValue.Int() != iCurrentNode.Value())
				{
				User::Leave(KErrArgument);
				}
			break;

		case ETypeBoolean:
			if (aValue.Bool() != iCurrentNode.Value())
				{
				User::Leave(KErrArgument);
				}
			break;

		case ETypeUUID:
			if (aValue.UUID() != TUUID(iCurrentNode.Value()))
				{
				User::Leave(KErrArgument);
				}
			break;

		case ETypeDES:
		case ETypeDEA:
			Panic(ERFtermSdpAttributeParserValueIsList);
			break;

		default:
			Panic(ERFtermSdpAttributeParserValueTypeUnsupported);
			break;
		}
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::ReadValueL()
// Pass the data element value to the observer.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::ReadValueL(CSdpAttrValue& aValue) const
	{
	iObserver.FoundElementL(iCurrentNode.Value(), aValue);
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::AdvanceL()
// Advance to the next node.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::AdvanceL()
	{
	// check not at end
	if (iCurrentNode.Command() == EFinished)
		{
		User::Leave(KErrEof);
		}

	// move to the next item
	iCurrentNode = iNodeList[++iCurrentNodeIndex];
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::TRFtermSdpAttributeNode()
// constructor.
// ----------------------------------------------------------------------------
//
TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::TRFtermSdpAttributeNode()
	{
	// no implementation needed
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::~TRFtermSdpAttributeNode()
// destructor.
// ----------------------------------------------------------------------------
//
TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::~TRFtermSdpAttributeNode()
	{
	// no implementation needed
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::SetCommand(TNodeCommand aCommand)
// set iCommand member variable.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::SetCommand(TNodeCommand aCommand)
	{
	iCommand = aCommand;
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::SetType(TRFtermSdpElementType aType)
// set iType member variable.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::SetType(TSdpElementType aType)
	{
	iType = aType;
	}

// ----------------------------------------------------------------------------
// void TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::SetValue(TInt aValue)
// set iValue member variable.
// ----------------------------------------------------------------------------
//
void TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::SetValue(TInt aValue)
	{
	iValue = aValue;
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::Command()
// get iCommand member variable value.
// ----------------------------------------------------------------------------
//
TRFtermSdpAttributeParser::TNodeCommand TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::Command() const
	{
	return iCommand;
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::Type()
// get iType member variable value.
// ----------------------------------------------------------------------------
//
TSdpElementType TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::Type() const
	{
	return iType;
	}

// ----------------------------------------------------------------------------
// TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::Value()
// get iValue member variable value.
// ----------------------------------------------------------------------------
//
TInt TRFtermSdpAttributeParser::TRFtermSdpAttributeNode::Value() const
	{
	return iValue;
	}

// End of File
