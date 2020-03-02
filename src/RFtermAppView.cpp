/*
 ============================================================================
 Name		: RFtermAppView.cpp
 Author	  : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include <aknappui.h>
#include "RFtermAppView.h"
#include "RFterm.pan"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRFtermAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRFtermAppView* CRFtermAppView::NewL(const TRect& aRect)
	{
	CRFtermAppView* self = CRFtermAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRFtermAppView* CRFtermAppView::NewLC(const TRect& aRect)
	{
	CRFtermAppView* self = new (ELeave) CRFtermAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();
	
	iRFtermOutput = CRFtermOutput::NewL(this);

	// Set the windows size
	SetRect(aRect);

	iRFtermOutput->SetFocus(ETrue);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::CRFtermAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CRFtermAppView::CRFtermAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::~CRFtermAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CRFtermAppView::~CRFtermAppView()
	{
	delete iRFtermOutput;
	iRFtermOutput = NULL;
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::CountComponentControls()
// Returns the specified component control.
// ----------------------------------------------------------------------------
//
TInt CRFtermAppView::CountComponentControls() const
	{
	return 1; // Only have one Component
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::ComponentControl()
// Gets the specified component of a compound control.
// ----------------------------------------------------------------------------
//
CCoeControl* CRFtermAppView::ComponentControl(TInt aIndex) const
	{
	__ASSERT_ALWAYS(aIndex == 0, Panic(ERFtermInvalidControlIndex));
	return iRFtermOutput;    //  Return the component
	}

// ----------------------------------------------------------------------------
// CRFtermAppView::OfferKeyEventL()
// Offer the key event to the list box.
// ----------------------------------------------------------------------------
//
TKeyResponse CRFtermAppView
::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	return iRFtermOutput->OfferKeyEventL(aKeyEvent, aType);
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(Rect());

	// Clears the screen
	gc.SetBrushColor(KRgbBlack);
	gc.Clear(drawRect);

	}

// -----------------------------------------------------------------------------
// CRFtermAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::SizeChanged()
	{
	if (iRFtermOutput) 
		{ 
		TRect clientRect = iAvkonAppUi->ClientRect(); 
		TRect outputRect(clientRect.Size());
		TInt scrollbarWidth = iRFtermOutput->ScrollBarFrame()->
				VerticalScrollBar()->ScrollBarBreadth();
		outputRect.SetWidth(outputRect.Width() - scrollbarWidth);
		iRFtermOutput-> SetRect(outputRect); 
		} 
	DrawNow();
	}

// -----------------------------------------------------------------------------
// CRFtermAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CRFtermAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

// End of File
