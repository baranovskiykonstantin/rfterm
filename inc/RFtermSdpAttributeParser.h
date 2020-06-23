/*
 ============================================================================
 Name        : RFtermSdpAttributeParser.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Check an SDP attribute value and read selected parts.
 ============================================================================
 */

#ifndef __RFTERMSDPATTRIBUTEPARSER_H__
#define __RFTERMSDPATTRIBUTEPARSER_H__

#include <btsdp.h>

class MRFtermSdpAttributeNotifier;

/**
* TRFtermSdpAttributeParser
* An instance of TRFtermSdpAttributeParser is used to check an SDP
* attribute value, and read selected parts
*/
class TRFtermSdpAttributeParser : public MSdpAttributeValueVisitor
	{

public: // Enumerations

	/**
	* TNodeCommand
	* The command to be carried out at a node
	* ECheckType. Check the type of the value
	* ECheckValue. Check the type and the value
	* ECheckEnd. Check that a list ends at this point
	* ESkip. Do not check this value - can not match a list end
	* EReadValue. Pass the value onto the observer
	* EFinished. Marks the end of the node list
	*/
	enum TNodeCommand
		{
		ECheckType,
		ECheckValue,
		ECheckEnd,
		ESkip,
		EReadValue,
		EFinished
		};

	/**
	* TRFtermSdpAttributeNode
	* An instance of TRFtermSdpAttributeNode is used to determine how
	* to parse an SDP attribute value data element
	*/
	class TRFtermSdpAttributeNode
		{

	public:

		/**
		* TRFtermSdpAttributeNode
		* Construct a TRFtermSdpAttributeNode
		*/
		TRFtermSdpAttributeNode();

		/**
		* ~TRFtermSdpAttributeNode
		* destructor
		*/
		~TRFtermSdpAttributeNode();

		/**
		* SetCommand
		* set value of iCommand member variable,
		* iCommand is the command for the node
		* @param aCommand the value of the iCommand
		*/
		void SetCommand(TNodeCommand aCommand);

		/**
		* SetType
		* set value of iType member variable
		* iType is the expected type
		* @param aType the value of the iType
		*/
		void SetType(TSdpElementType aType);

		/**
		* SetValue
		* set value of iValue member variable,
		* iValue the expected value for ECheckValue,the value of aKey
		* passed to the observer for EReadValue
		* @param aValue the value of the iValue
		*/
		void SetValue(TInt aValue);

		/**
		* Command
		* get value of iCommand member variable
		* @return the value of the iCommand
		*/
		TNodeCommand Command() const;

		/**
		* Type
		* get value of iType member variable
		* @return the value of the iType
		*/
		TSdpElementType Type() const;

		/**
		* Value
		* get value of iValue member variable
		* @return the value of the iValue
		*/
		TInt Value() const;

	private:

		/**
		* iCommand the command for the node
		*/
		TNodeCommand iCommand;

		/**
		* iType the expected type
		*/
		TSdpElementType iType;

		/**
		* iValue the expected value for ECheckValue,the value of aKey
		* passed to the observer for EReadValue
		*/
		TInt iValue;

	};


	/**
	* TRFtermSdpAttributeParser
	* Construct a TRFtermSdpAttributeParser
	* @param aNodeList the list of expected nodes
	* @param aObserver an observer to read specified node values
	*/
	TRFtermSdpAttributeParser(
		RArray<TRFtermSdpAttributeParser::TRFtermSdpAttributeNode>& aNodeList,
		MRFtermSdpAttributeNotifier& aObserver);

	/**
	* ~TRFtermSdpAttributeParser
	* Destructor
	*/
	virtual ~TRFtermSdpAttributeParser();

	/**
	* HasFinished
	* Check if parsing processed the whole list
	* @return true is the index refers to the EFinished node
	*/
	TBool HasFinished() const;

public: // from MSdpAttributeValueVisitor

	/**
	* VisitAttributeValueL
	* Process a data element
	* @param aValue the data element
	* @param aType the type of the data element
	*/
	void VisitAttributeValueL(CSdpAttrValue& aValue,
		TSdpElementType aType);

	/**
	* StartListL
	* Process the start of a data element list
	* @param aList the data element list
	*/
	void StartListL(CSdpAttrValueList& aList);

	/**
	* EndListL
	* Process the end of a data element list
	*/
	void EndListL();

private: // Functions from base classes

	/**
	* CheckTypeL
	* Check the type of the current node is the same as
	* the specified type.
	* @param aElementType the type of the current data element
	*/
	void CheckTypeL(TSdpElementType aElementType) const;

	/**
	* CheckValueL
	* Check the value of the current node is the same as the specified value
	* @param aValue the value of the current data element.
	*/
	void CheckValueL(CSdpAttrValue& aValue) const;

	/**
	* ReadValueL
	* Pass the data element value to the observer
	* @param aValue the value of the current data element.
	*/
	void ReadValueL(CSdpAttrValue& aValue) const;

	/**
	* AdvanceL
	* Advance to the next node. Leaves with KErrEof
	* if at the finished node.
	*/
	void AdvanceL();

private: // data

	/**
	* iObserver the observer to read values
	*/
	MRFtermSdpAttributeNotifier& iObserver;

	/**
	* iNodeList a list defining the expected
	* structure of the value
	*/
	RArray<TRFtermSdpAttributeParser::TRFtermSdpAttributeNode>& iNodeList;

	/**
	* iCurrentNodeIndex the index of the current node in iNodeList
	*/
	TInt iCurrentNodeIndex;

	/**
	* iCurrentNode the current node
	*/
	TRFtermSdpAttributeNode iCurrentNode;

	};

#endif // __RFTERMSDPATTRIBUTEPARSER_H__

// End of File
