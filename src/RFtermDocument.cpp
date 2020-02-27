/*
 ============================================================================
 Name		: RFtermDocument.cpp
 Author	  : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : CRFtermDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "RFtermAppUi.h"
#include "RFtermDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRFtermDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRFtermDocument* CRFtermDocument::NewL(CEikApplication& aApp)
	{
	CRFtermDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CRFtermDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRFtermDocument* CRFtermDocument::NewLC(CEikApplication& aApp)
	{
	CRFtermDocument* self = new (ELeave) CRFtermDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CRFtermDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRFtermDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CRFtermDocument::CRFtermDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CRFtermDocument::CRFtermDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CRFtermDocument::~CRFtermDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CRFtermDocument::~CRFtermDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CRFtermDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CRFtermDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) CRFtermAppUi;
	}

// End of File
