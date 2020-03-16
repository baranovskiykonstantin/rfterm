/*
 ============================================================================
 Name        : RFtermDocument.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __RFTERMDOCUMENT_h__
#define __RFTERMDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CRFtermAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CRFtermDocument application class.
 * An instance of class CRFtermDocument is the Document part of the
 * AVKON application framework for the RFterm example application.
 */
class CRFtermDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CRFtermDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CRFtermDocument.
	 */
	static CRFtermDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CRFtermDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CRFtermDocument.
	 */
	static CRFtermDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CRFtermDocument
	 * Virtual Destructor.
	 */
	virtual ~CRFtermDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CRFtermAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CRFtermDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CRFtermDocument(CEikApplication& aApp);

	};

#endif // __RFTERMDOCUMENT_h__
// End of File
