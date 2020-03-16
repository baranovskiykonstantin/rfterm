/*
 ============================================================================
 Name        : RFtermApplication.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __RFTERMAPPLICATION_H__
#define __RFTERMAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "RFtermConstants.h"

// CLASS DECLARATION

/**
 * CRFtermApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CRFtermApplication is the application part of the
 * AVKON application framework for the RFterm example application.
 */
class CRFtermApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidRFtermApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CRFtermDocument document object. The returned
	 * pointer in not owned by the CRFtermApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __RFTERMAPPLICATION_H__
// End of File
