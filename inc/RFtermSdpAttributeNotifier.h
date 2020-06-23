/*
 ============================================================================
 Name        : RFtermSdpAttributeNotifier.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Read selected SDP Attributes.
 ============================================================================
 */

#ifndef __RFTERMSDPATTRIBUTENOTIFIER_H__
#define __RFTERMSDPATTRIBUTENOTIFIER_H__

class CSdpAttrValue;

/**
* MRFtermSdpAttributeNotifier
* An instance of MRFtermSdpAttributeNotifier is used to read selected SDP Attribute
* elements from an attribute value.
*/
class MRFtermSdpAttributeNotifier
	{

public: // New functions

	/**
	* FoundElementL
	* Read the data element
	* @param aKey a key that identifies the element
	* @param aValue the data element
	*/
	virtual void FoundElementL(TInt aKey, CSdpAttrValue& aValue) = 0;

	};

#endif // __RFTERMSDPATTRIBUTENOTIFIER_H__

// End of File
