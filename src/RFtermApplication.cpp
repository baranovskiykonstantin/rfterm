/*
 ============================================================================
 Name        : RFtermApplication.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "RFterm.hrh"
#include "RFtermDocument.h"
#include "RFtermApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRFtermApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CRFtermApplication::CreateDocumentL()
	{
	// Create an RFterm document, and return a pointer to it
	return CRFtermDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CRFtermApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CRFtermApplication::AppDllUid() const
	{
	// Return the UID for the RFterm application
	return KUidRFtermApp;
	}

// End of File
