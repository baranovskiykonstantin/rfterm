/*
 ============================================================================
 Name        : RFtermAppView.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Declares view class for application.
 ============================================================================
 */

#ifndef __RFTERMAPPVIEW_h__
#define __RFTERMAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>
//#include <AknQueryDialog.h>

#include "RFtermOutput.h"

// CLASS DECLARATION
class CRFtermAppView : public CCoeControl
	{
public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CRFtermAppView object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CRFtermAppView.
	 */
	static CRFtermAppView* NewL(const TRect& aRect);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Create a CRFtermAppView object, which will draw itself
	 * to aRect.
	 * @param aRect Rectangle this view will be drawn to.
	 * @return A pointer to the created instance of CRFtermAppView.
	 */
	static CRFtermAppView* NewLC(const TRect& aRect);

	/**
	 * ~CRFtermAppView
	 * Virtual Destructor.
	 */
	virtual ~CRFtermAppView();

public:
	// Functions from base classes

	/**
	 * From CCoeControl, Draw
	 * Draw this CRFtermAppView to the screen.
	 * @param aRect the rectangle of this view that needs updating
	 */
	void Draw(const TRect& aRect) const;

	/**
	 * From CoeControl, SizeChanged.
	 * Called by framework when the view size is changed.
	 */
	virtual void SizeChanged();

	/**
	 * From CoeControl, HandlePointerEventL.
	 * Called by framework when a pointer touch event occurs.
	 * Note: although this method is compatible with earlier SDKs, 
	 * it will not be called in SDKs without Touch support.
	 * @param aPointerEvent the information about this event
	 */
	virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

	
public: // from CCoeControl

	/**
	* OfferKeyEventL()
	* Offer the key event to the list box
	* @param aKeyEvent the key that caused the event
	* @param aType the type of event that occured
	* @return indicates whether or not the key event
	* was used by this control
	*/  
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, 
									   TEventCode aType);

	/**
	* CountComponentControls()
	* Return the number of component controls
	* @return the number of component controls
	*/
	TInt CountComponentControls() const;

	/**
	* ComponentControl()
	* Return a component control
	* @param aIndex the index of the control to return
	* @return the specified component control
	*/
	CCoeControl* ComponentControl(TInt aIndex) const;
	
public: // New functions
	/**
	* ShowDataQueryL()
	* Show data query.
	* @param aQueryResourceId the id of the query
	* @param aTextResourceId for inial value
	* @param aPromptResoureId for prompt
	* @param aMaxLength sets up the length of the character
	* @param aText text of the query
	*/
	TBool ShowDataQueryL(
		const TInt aQueryResourceId,
		const TInt aTextResourceId,
		const TInt aPromptResoureId,
		const TInt aMaxLength,
		TDes& aText);

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CRFtermAppView object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect);

	/**
	 * CRFtermAppView.
	 * C++ default constructor.
	 */
	CRFtermAppView();
	
private:
	
	/**
	 * iPrevPointerPos
	 */
	TPoint iPrevPointerPos;
	
public:
	
	/**
	* iRFtermOutput
	*/
	CRFtermOutput* iRFtermOutput;

	/**
	* iDisplayDialog
	* boolean for Display Dialog
	*/
	TBool iDisplayDialog;

	};

#endif // __RFTERMAPPVIEW_h__
// End of File
