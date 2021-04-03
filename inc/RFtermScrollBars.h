/*
 ============================================================================
 Name        : RFtermScrollBars.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Custom scrollbars control.
 ============================================================================
 */

#ifndef __RFTERMSCROLLBARS_H__
#define __RFTERMSCROLLBARS_H__

#include <gdi.h>
#include <coecntrl.h>
#include "RFtermConstants.h"
#include "RFtermScrollBarModel.h"
#include "RFtermScrollBarsObserver.h"
#include "RFtermOutputObserver.h"

/**
 * CRFtermScrollBars class.
 * The scrollbars control occupies all client area.
 * The related container control is placed over
 * the scrollbars control at the same position (0, 0).
 * To make vertical/horizontal scrollbar visible
 * the related container control has to shrink its right/bottom edge.
 */
class CRFtermScrollBars : public CCoeControl, public MRFtermOutputObserver
	{

public: // Constructor, destructor

	CRFtermScrollBars(const CCoeControl *aParent);
	~CRFtermScrollBars();
	
public: // New functions

	void SetVModel(const TRFtermScrollBarModel& aModel);
	void SetHModel(const TRFtermScrollBarModel& aModel);
	void SetVisibility(TBool aVertical, TBool aHorizontal);
	inline TBool IsVScrollBarVisible() const;
	inline TBool IsHScrollBarVisible() const;
	void Update(TBool aDrawNow);
	void SetColors(TRgb aBg, TRgb aSb);
	/**
	 * Area that can be occupied by the container control.
	 */
	void GetFreeRect(TRect& aRect) const;
	void DrawVScrollBarNow() const;
	void DrawHScrollBarNow() const;
	void DrawCornerNow() const;

public: // For MRFtermScrollBarsObserver

	void SetObserver(MRFtermScrollBarsObserver* aObserver);

private: // New functions

	void SetModel(TRFtermScrollBarModel& aTarget, const TRFtermScrollBarModel& aModel);
	void DrawVScrollBar(CWindowGc& aGc) const;
	void DrawHScrollBar(CWindowGc& aGc) const;
	void DrawCorner(CWindowGc& aGc) const;
	void Draw(const TRect& aRect) const;
	/**
	 * Change color to look different when a scrollbar is pressed.
	 */
	void HighlightColor(TRgb& aColor) const;
	
private: // From MRFtermOutputObserver

	void HandleViewRectChangedL(const TRect& aContentRect, const TRect& aViewRect);

private: // From CCoeControl

	void SizeChanged();
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private: // Data

	struct TRFtermScrollBarModel iVModel;
	TBool iVIsVisible;
	TRect iVThumbRect;
	TRect iVBackgroundRect;

	struct TRFtermScrollBarModel iHModel;
	TBool iHIsVisible;
	TRect iHThumbRect;
	TRect iHBackgroundRect;

	// Bottom right corner between vertical and horizontal scrollbars.
	TRect iCornerBackgroundRect;

	TPoint iPrevPointerPos;
	TBool iVScrollBarIsActive;
	TBool iHScrollBarIsActive;

	// Non owning pointer to observer
	MRFtermScrollBarsObserver* iObserver;

	TRgb iBgColor;
	TRgb iSbColor;

	};

inline TBool CRFtermScrollBars::IsVScrollBarVisible() const
	{ return iVIsVisible; }

inline TBool CRFtermScrollBars::IsHScrollBarVisible() const
	{ return iHIsVisible; }

#endif /* __RFTERMSCROLLBARS_H__ */

// End of File
