/*
 ============================================================================
 Name        : ColorControl.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Custom control for the color setup.
 ============================================================================
 */

#ifndef __COLORCONTROL_H__
#define __COLORCONTROL_H__

#include <coecobs.h>
#include <akncontrol.h>
#include <akndialog.h>
#include <aknnavide.h>
#include <gdi.h>
#include <eikmfne.h>

// FORWARD DECLARATION
class CAknsBasicBackgroundControlContext;


/**
 * CColorPreview class.
 * Small control for previewing the initial
 * and current colors.
 */
class CColorPreview : public CAknControl
	{

public: // Constructor
	void ConstructL(const CCoeControl* aParent,
			const TRgb* aInitialColor,
			const TRgb* aColor);

public: // From CAknControl
	TSize MinimumSize();

private: // From CAknControl
	void Draw(const TRect& aRect) const;

private: // Data
	/**
	 * Non owning pointers to colors to preview.
	 */
	const TRgb* iInitialColor;
	const TRgb* iColor;

	};


/**
 * CColorControl class.
 * Control for setting up RGB color with sliders.
 */
class CColorControl : public CAknControl, public MCoeControlObserver
	{

public: // Constructor, destructor
	~CColorControl();
	void ConstructL(const CCoeControl* aParent, TRgb aColor);

public: // From CAknControl
	TSize MinimumSize();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

public: // From MCoeControlObserver
	void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType);

public: // New functions
	TRgb Color();

private: // New functions
	void UpdateNaviLabelL();

protected: // From CAknControl
	void SizeChanged();

private: // From CAknControl
	void Draw(const TRect& aRect) const;

private: // Data
	/**
	 * First member shomehow resets to zero, why?!!!
	 */
	TInt iDumy;

	CAknsBasicBackgroundControlContext* iBackground;
	TInt iActiveSlider;

	TRgb iInitialColor;
	TRgb iColor;

	CFont* iFont;

	CAknNavigationDecorator* iNaviDecorator;

	};


/**
 * CColorDialog class.
 * Dialog for setting up RGB color.
 */
class CColorDialog : public CAknDialog
	{

	public: // Constructor
		CColorDialog(TRgb* aColor);

	public: // From CAknDialog
		TBool OkToExitL(TInt aButtonId);

	private: // From CAknDialog
		SEikControlInfo CreateCustomControlL(TInt aControlType);
		void PreLayoutDynInitL();

	private: // Data
		// Non owning pointer to color setup control.
		CColorControl* iColorControl;
		// Non owning pointer to color to setup.
		TRgb* iColor;

	};


class CColorEditor;


/**
 * CColorEditorPreviewTimer class.
 * Timer for deferred redrawing of CColorEditorPreview.
 */
class MColorEditorPreviewTimerCallback { public: virtual void HandleTimedOut() = 0; };
class CColorEditorPreviewTimer : public CTimer
	{

public:
	static CColorEditorPreviewTimer* NewL(MColorEditorPreviewTimerCallback& aCallback);

private:
	CColorEditorPreviewTimer(MColorEditorPreviewTimerCallback& aCallback);
	void ConstructL();
	void RunL();

private:
	MColorEditorPreviewTimerCallback& iCallback;

	};


/**
 * CColorEditorPreview class.
 * Small control for previewing the color.
 * This control is drawn over CColorEditor.
 */
class CColorEditorPreview : public CAknControl, public MColorEditorPreviewTimerCallback
	{

public: // Constructor and destructor
	~CColorEditorPreview();
	void ConstructL(const CColorEditor* aParent);

public: // New functions
	void UpdateDeferred();

public: // From MColorEditorPreviewTimerCallback
	void HandleTimedOut();

private: // From CAknControl
	void Draw(const TRect& aRect) const;

private: // Data
	const CColorEditor* iParent;
	CColorEditorPreviewTimer* iTimer;

	};


/**
 * CColorEditor class.
 * Color editor for forms.
 */
class CColorEditor : public CEikMfne, public MCoeControlObserver
	{

public: // From CCoeControl
	void ConstructFromResourceL(TResourceReader& aResourceReader);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);

public: // From MCoeControlObserver
	void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType);

public: // New functions
	TRgb Color() const;
	void SetColor(TRgb aColor);
	void SetColorWithDialog();

protected: // From CCoeControl
	void SizeChanged();

private: // Data
	CColorEditorPreview* iPreview;

	};

#endif /* __COLORCONTROL_H__ */

// End of File
