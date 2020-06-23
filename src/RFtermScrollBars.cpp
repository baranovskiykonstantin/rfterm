/*
 ============================================================================
 Name        : RFtermScrollBars.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Custom scrollbars control.
 ============================================================================
 */

#include "RFtermScrollBars.h"

CRFtermScrollBars::CRFtermScrollBars(const CCoeControl *aParent)
	: iVIsVisible(EFalse)
	, iHIsVisible(EFalse)
	, iObserver(NULL)
	{
	SetContainerWindowL(*aParent);
	}

CRFtermScrollBars::~CRFtermScrollBars()
	{
	}

void CRFtermScrollBars::DrawVScrollBar(CWindowGc& aGc) const
	{
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbDarkGray);
	aGc.Clear(iVBackgroundRect);
	if (iVScrollBarIsActive)
		{
		aGc.SetBrushColor(KRgbGray);
		}
	else
		{
		aGc.SetBrushColor(KRgbWhite);
		}
	aGc.DrawRect(iVThumbRect);
	}

void CRFtermScrollBars::DrawHScrollBar(CWindowGc& aGc) const
	{
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbDarkGray);
	aGc.Clear(iHBackgroundRect);
	if (iHScrollBarIsActive)
		{
		aGc.SetBrushColor(KRgbGray);
		}
	else
		{
		aGc.SetBrushColor(KRgbWhite);
		}
	aGc.DrawRect(iHThumbRect);
	}

void CRFtermScrollBars::DrawCorner(CWindowGc& aGc) const
	{
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbBlack);
	aGc.Clear(iCornerBackgroundRect);
	}

void CRFtermScrollBars::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.SetClippingRect(aRect);

	if (aRect.Intersects(iVBackgroundRect))
		{
		DrawVScrollBar(gc);
		}
	if (aRect.Intersects(iHBackgroundRect))
		{
		DrawHScrollBar(gc);
		}
	if (aRect.Intersects(iCornerBackgroundRect))
		{
		DrawCorner(gc);
		}
	}

void CRFtermScrollBars::DrawVScrollBarNow() const
	{
	Window().Invalidate(iVBackgroundRect);
	ActivateGc();
	Window().BeginRedraw(iVBackgroundRect);
	DrawVScrollBar(SystemGc());
	Window().EndRedraw();
	DeactivateGc();
	}

void CRFtermScrollBars::DrawHScrollBarNow() const
	{
	Window().Invalidate(iHBackgroundRect);
	ActivateGc();
	Window().BeginRedraw(iHBackgroundRect);
	DrawHScrollBar(SystemGc());
	Window().EndRedraw();
	DeactivateGc();
	}

void CRFtermScrollBars::DrawCornerNow() const
	{
	Window().Invalidate(iCornerBackgroundRect);
	ActivateGc();
	Window().BeginRedraw(iCornerBackgroundRect);
	DrawCorner(SystemGc());
	Window().EndRedraw();
	DeactivateGc();
	}

void CRFtermScrollBars::SizeChanged()
	{
	Update(EFalse);
	}

void CRFtermScrollBars::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	if (TPointerEvent::EButton1Down == aPointerEvent.iType)
		{
		iVScrollBarIsActive = EFalse;
		iHScrollBarIsActive = EFalse;
		iPrevPointerPos = aPointerEvent.iPosition;
		TRect vSensorRect = iVBackgroundRect;
		vSensorRect.iTl.iX -= KRFtermScrollBarSensorBreadth;
		TRect hSensorRect = iHBackgroundRect;
		vSensorRect.iTl.iY -= KRFtermScrollBarSensorBreadth;
		if (vSensorRect.Contains(aPointerEvent.iPosition))
			{
			iVScrollBarIsActive = ETrue;
			}
		else if (hSensorRect.Contains(aPointerEvent.iPosition))
			{
			iHScrollBarIsActive = ETrue;
			}
		}
	else if (TPointerEvent::EDrag == aPointerEvent.iType)
		{
		if (iVScrollBarIsActive)
			{
			TInt deltaY = aPointerEvent.iPosition.iY - iPrevPointerPos.iY;
			if (deltaY < 0 && -(deltaY) > iVThumbRect.iTl.iY)
				{
				// Limit vertical scrollbar movement on upward direction.
				deltaY = -iVThumbRect.iTl.iY;
				}
			else if (deltaY > (iVBackgroundRect.Height() - iVThumbRect.iTl.iY - iVThumbRect.Height()))
				{
				// Limit vertical scrollbar movement on downward direction.
				deltaY = iVBackgroundRect.Height() - iVThumbRect.iTl.iY - iVThumbRect.Height();
				}
			iVThumbRect.Move(0, deltaY);
			
			DrawVScrollBarNow();

			iVModel.iPos = iVModel.iTotal * iVThumbRect.iTl.iY / iVBackgroundRect.Height();
			if (iObserver)
				{
				iObserver->HandleScrollEventL(EVerticalScrollBar, iVModel);
				}

			iPrevPointerPos = aPointerEvent.iPosition;
			}
		else if (iHScrollBarIsActive)
			{
			TInt deltaX = aPointerEvent.iPosition.iX - iPrevPointerPos.iX;
			if (deltaX < 0 && -(deltaX) > iHThumbRect.iTl.iX)
				{
				// Limit vertical scrollbar movement on leftward direction.
				deltaX = -iHThumbRect.iTl.iX;
				}
			else if (deltaX > (iHBackgroundRect.Width() - iHThumbRect.iTl.iX - iHThumbRect.Width()))
				{
				// Limit vertical scrollbar movement on rightward direction.
				deltaX = iHBackgroundRect.Width() - iHThumbRect.iTl.iX - iHThumbRect.Width();
				}
			iHThumbRect.Move(deltaX, 0);

			DrawHScrollBarNow();

			iHModel.iPos = iHModel.iTotal * iHThumbRect.iTl.iX / iHBackgroundRect.Width();
			if (iObserver)
				{
				iObserver->HandleScrollEventL(EHorizontalScrollBar, iHModel);
				}

			iPrevPointerPos = aPointerEvent.iPosition;
			
			}
		}
	else if (TPointerEvent::EButton1Up == aPointerEvent.iType)
		{
		if (iVScrollBarIsActive)
			{
			iVScrollBarIsActive = EFalse;
			DrawVScrollBarNow();
			}
		if (iHScrollBarIsActive)
			{
			iHScrollBarIsActive = EFalse;
			DrawHScrollBarNow();
			}
		}
	}

void CRFtermScrollBars::SetModel(TRFtermScrollBarModel& aTarget, const TRFtermScrollBarModel& aModel)
	{
	aTarget = aModel;
	if (aTarget.iTotal < 1)
		{
		aTarget.iTotal = 1;
		}
	if (aTarget.iVisible > aTarget.iTotal)
		{
		aTarget.iVisible = aTarget.iTotal;
		}
	if (aTarget.iVisible < 1)
		{
		aTarget.iVisible = 1;
		}
	if (aTarget.iPos > (aTarget.iTotal - aTarget.iVisible))
		{
		aTarget.iPos = aTarget.iTotal - aTarget.iVisible;
		}
	if (aTarget.iPos < 0)
		{
		aTarget.iPos = -(aTarget.iPos);
		aTarget.iTotal += aTarget.iPos;
		}
	}

void CRFtermScrollBars::SetVModel(const TRFtermScrollBarModel& aModel)
	{
	SetModel(iVModel, aModel);
	}

void CRFtermScrollBars::SetHModel(const TRFtermScrollBarModel& aModel)
	{
	SetModel(iHModel, aModel);
	}

void CRFtermScrollBars::SetVisibility(TBool aVertical, TBool aHorizontal)
	{
	if (iVIsVisible != aVertical || iHIsVisible != aHorizontal)
		{
		iVIsVisible = aVertical;
		iHIsVisible = aHorizontal;
		if (iObserver)
			{
			iObserver->HandleScrollBarVisibilityChangeL(
				aVertical, aHorizontal);
			}
		}
	}

void CRFtermScrollBars::Update(TBool aDrawNow)
	{
	TRect windowRect = Rect();
	TRect restRect;
	GetFreeRect(restRect);

	iVBackgroundRect = TRect();
	iVBackgroundRect.Move(windowRect.Width(), 0);
	iVBackgroundRect.SetHeight(windowRect.Height());
	iVThumbRect = iVBackgroundRect;
	if (iVIsVisible)
		{
		iVBackgroundRect.iTl.iX -= KRFtermScrollBarBreadth;
		if (iHIsVisible)
			{
			iVBackgroundRect.iBr.iY -= KRFtermScrollBarBreadth;
			}

		iVThumbRect = iVBackgroundRect;
		TInt thumbSize = iVModel.iVisible * restRect.Height() / iVModel.iTotal;
		TInt thumbPos = iVModel.iPos * restRect.Height() / iVModel.iTotal;
		iVThumbRect.Move(0, thumbPos);
		iVThumbRect.SetHeight(thumbSize);

		if (aDrawNow)
			{
			DrawVScrollBarNow();
			}
		}

	iHBackgroundRect = TRect();
	iHBackgroundRect.Move(0, windowRect.Height());
	iHBackgroundRect.SetWidth(windowRect.Width());
	iHThumbRect = iHBackgroundRect;
	if (iHIsVisible)
		{
		iHBackgroundRect.iTl.iY -= KRFtermScrollBarBreadth;
		if (iVIsVisible)
			{
			iHBackgroundRect.iBr.iX -= KRFtermScrollBarBreadth;
			}

		iHThumbRect = iHBackgroundRect;
		TInt thumbSize = iHModel.iVisible * restRect.Width() / iHModel.iTotal;
		TInt thumbPos = iHModel.iPos * restRect.Width() / iHModel.iTotal;
		iHThumbRect.Move(thumbPos, 0);
		iHThumbRect.SetWidth(thumbSize);

		if (aDrawNow)
			{
			DrawHScrollBarNow();
			}
		}

	iCornerBackgroundRect = TRect();
	iCornerBackgroundRect.Move(windowRect.Width(), windowRect.Height());
	if (iVIsVisible && iHIsVisible)
		{
		// Bottom right corner area.
		iCornerBackgroundRect.iTl.iX -= KRFtermScrollBarBreadth;
		iCornerBackgroundRect.iTl.iY -= KRFtermScrollBarBreadth;

		if (aDrawNow)
			{
			DrawCornerNow();
			}
		}
	}

void CRFtermScrollBars::GetFreeRect(TRect& aRect) const
	{
	TRect windowRect = Rect();
	aRect = windowRect;
	if (iVIsVisible)
		{
		aRect.SetWidth(windowRect.Width() - KRFtermScrollBarBreadth);
		}
	if (iHIsVisible)
		{
		aRect.SetHeight(windowRect.Height() - KRFtermScrollBarBreadth);
		}
	}

void CRFtermScrollBars::SetObserver(MRFtermScrollBarsObserver* aObserver)
	{
	iObserver = aObserver;
	}

void CRFtermScrollBars::HandleViewRectChangedL(const TRect& aContentRect, const TRect& aViewRect)
	{
	TBool vIsVisible = EFalse;
	TBool hIsVisible = EFalse;

	TRect outputRect = aViewRect;

	vIsVisible = aContentRect.Height() > outputRect.Height();
	hIsVisible = aContentRect.Width() > outputRect.Width();

	outputRect = aViewRect;

	TRFtermScrollBarModel model;

	model.iTotal = aContentRect.Height();
	model.iVisible = outputRect.Height();
	model.iPos = outputRect.iTl.iY;
	SetVModel(model);

	model.iTotal = aContentRect.Width();
	model.iVisible = outputRect.Width();
	model.iPos = outputRect.iTl.iX;
	SetHModel(model);
	
	SetVisibility(vIsVisible, hIsVisible);
	
	Update(ETrue);
	}

