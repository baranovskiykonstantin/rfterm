/*
 ============================================================================
 Name        : RFtermScrollBars.h
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Custom scrollbars control.
 ============================================================================
 */

#ifndef RFTERMSCROLLBARS_H
#define RFTERMSCROLLBARS_H

#include <coecntrl.h>
#include "RFtermConstants.h"
#include "RFtermScrollBarModel.h"
#include "RFtermScrollBarsObserver.h"
#include "RFtermOutputObserver.h"

class CRFtermScrollBars : public CCoeControl, public MRFtermOutputObserver
	{
public:
	// Constructor, destructor
	CRFtermScrollBars(const TRect& aRect);
	~CRFtermScrollBars();
	
	// New functions
	void SetVModel(const TRFtermScrollBarModel& aModel);
	void SetHModel(const TRFtermScrollBarModel& aModel);
	void SetVisibility(TBool aVertical, TBool aHorizontal);
	inline TBool IsVScrollBarVisible() const;
	inline TBool IsHScrollBarVisible() const;
	void Update(TBool aDrawNow);
	void GetFreeRect(TRect& aRect) const;
	void DrawVScrollBarNow() const;
	void DrawHScrollBarNow() const;
	void DrawCornerNow() const;

	// MRFtermScrollBarsObserver
	void SetObserver(MRFtermScrollBarsObserver* aObserver);

private:
	// New functions
	void SetModel(TRFtermScrollBarModel& aTarget, const TRFtermScrollBarModel& aModel);
	void DrawVScrollBar(CWindowGc& aGc) const;
	void DrawHScrollBar(CWindowGc& aGc) const;
	void DrawCorner(CWindowGc& aGc) const;
	void Draw(const TRect& aRect) const;
	void HandleViewRectChangedL(const TRect& aContentRect, const TRect& aViewRect);

	// CCoeControl
	void SizeChanged();
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private:
	struct TRFtermScrollBarModel iVModel;
	TBool iVIsVisible;
	TRect iVThumbRect;
	TRect iVBackgroundRect;

	struct TRFtermScrollBarModel iHModel;
	TBool iHIsVisible;
	TRect iHThumbRect;
	TRect iHBackgroundRect;

	TRect iCornerBackgroundRect;

	TPoint iPrevPointerPos;
	TBool iPointerOnVScrollBar;
	TBool iPointerOnHScrollBar;

	MRFtermScrollBarsObserver* iObserver;
	};

inline TBool CRFtermScrollBars::IsVScrollBarVisible() const
	{ return iVIsVisible; }

inline TBool CRFtermScrollBars::IsHScrollBarVisible() const
	{ return iHIsVisible; }

#endif /* RFTERMSCROLLBARS_H */
