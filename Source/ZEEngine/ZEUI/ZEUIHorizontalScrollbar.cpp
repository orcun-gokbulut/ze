//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIHorizontalScrollbar.cpp
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "ZEUIHorizontalScrollbar.h"
#include "ZEUI/ZEUIScrollArea.h"
#include "ZEUI/ZEUIRectangle.h"

void ZEUIHorizontalScrollbar::ScrollbarSliderPress(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarSlider.Frame.Color = ZEVector4(0.0f, 0.5f, 0.7f, 1.0f);
}

void ZEUIHorizontalScrollbar::ScrollbarSliderMove(ZEUIMouseKey Button, const ZEVector2& MoveAmount)
{
	if (ScrollbarSlider.GetPosition().x + MoveAmount.x < ScrollbarLeftArrow.GetRectangle().RightDown.x)
	{
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarTrack.GetPosition().x, ScrollbarSlider.GetPosition().y));	
		ContentArea->SetPosition(ZEVector2(VisibleContentArea->GetPosition().x, ContentArea->GetPosition().y));
		return;
	}

	else if (ScrollbarSlider.GetRectangle().RightDown.x + MoveAmount.x > ScrollbarRightArrow.GetRectangle().LeftUp.x)
	{
		return;
	}

	ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x + MoveAmount.x, ScrollbarSlider.GetPosition().y));	
	ParentScrollArea->MoveContent(ZEVector2( - (MoveAmount.x * ContentArea->GetWidth() / ScrollbarTrack.GetWidth()), 0.0f));
}

void ZEUIHorizontalScrollbar::ScrollbarSliderRelease(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarSlider.Frame.Color = ZEVector4(0.0f, 0.0f, 1.0f, 1.0f);
}

void ZEUIHorizontalScrollbar::ScrollbarLeftArrowPress(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarLeftArrow.Frame.Color = ZEVector4(1.0f, 1.0f, 0.0f, 1.0f);

	if (ScrollbarSlider.GetPosition().x - 2.0f < ScrollbarLeftArrow.GetRectangle().RightDown.x)
	{
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarLeftArrow.GetRectangle().RightDown.x, ScrollbarSlider.GetPosition().y));
		return;
	}

	ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x - 2.0f, ScrollbarSlider.GetPosition().y));
	ParentScrollArea->MoveContent(ZEVector2((2.0f * ContentArea->GetWidth() / ScrollbarTrack.GetWidth()), 0.0f));
}

void ZEUIHorizontalScrollbar::ScrollbarLeftArrowRelease(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarLeftArrow.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
}

void ZEUIHorizontalScrollbar::ScrollbarRightArrowPress(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarRightArrow.Frame.Color = ZEVector4(1.0f, 1.0f, 0.0f, 1.0f);

	if (ScrollbarSlider.GetRectangle().RightDown.x + 2.0f > ScrollbarRightArrow.GetRectangle().LeftUp.x)
	{
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarRightArrow.GetRectangle().LeftUp.x - ScrollbarSlider.GetWidth(), ScrollbarSlider.GetPosition().y));
		return;
	}

	ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x + 2.0f, ScrollbarSlider.GetPosition().y));
	ParentScrollArea->MoveContent(ZEVector2(- (2.0f * ContentArea->GetWidth() / ScrollbarTrack.GetWidth()), 0.0f));
}

void ZEUIHorizontalScrollbar::ScrollbarRightArrowRelease(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarRightArrow.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
}

void ZEUIHorizontalScrollbar::ScrollbarTrackPress(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarTrack.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 0.1f);

	if (MousePosition.x < ScrollbarSlider.GetScreenRectangle().LeftUp.x)
	{
		if (ScrollbarSlider.GetPosition().x - ScrollbarSlider.GetWidth() < ScrollbarLeftArrow.GetRectangle().RightDown.x)
		{
			ScrollbarSlider.SetPosition(ZEVector2(ScrollbarTrack.GetPosition().x, ScrollbarSlider.GetPosition().y));
			ContentArea->SetPosition(ZEVector2(VisibleContentArea->GetPosition().x, ContentArea->GetPosition().y));
			return;
		}

		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x - ScrollbarSlider.GetWidth(), ScrollbarSlider.GetPosition().y));
		ParentScrollArea->MoveContent(ZEVector2((ScrollbarSlider.GetWidth() * ContentArea->GetWidth() / ScrollbarTrack.GetWidth()), 0.0f));
	}

	else if (MousePosition.x > ScrollbarSlider.GetScreenRectangle().RightDown.x)
	{
		if (ScrollbarSlider.GetRectangle().RightDown.x + ScrollbarSlider.GetWidth() > ScrollbarRightArrow.GetRectangle().LeftUp.x)
		{
			ScrollbarSlider.SetPosition(ZEVector2(ScrollbarTrack.GetRectangle().RightDown.x - ScrollbarSlider.GetWidth(), ScrollbarSlider.GetPosition().y));
			ContentArea->SetPosition(ZEVector2(VisibleContentArea->GetRectangle().RightDown.x - ContentArea->GetWidth(), ContentArea->GetPosition().y));
			return;
		}

		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x + ScrollbarSlider.GetWidth(), ScrollbarSlider.GetPosition().y));
		ParentScrollArea->MoveContent(ZEVector2(- (ScrollbarSlider.GetWidth() * ContentArea->GetWidth() / ScrollbarTrack.GetWidth()), 0.0f));
	}
}

void ZEUIHorizontalScrollbar::ScrollbarTrackRelease(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarTrack.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 0.0f);
}
void ZEUIHorizontalScrollbar::SetContentArea(ZEUIControl* ContentArea, ZEUIFrameControl* VisibleContentArea)
{
	this->ContentArea = ContentArea;
	this->VisibleContentArea = VisibleContentArea;

	if (ParentScrollArea->GetHorizontalScrollbarPolicy() == SCROLLBAR_ALWAYS_OFF)
		return;

	if (ParentScrollArea->GetHorizontalScrollbarPolicy() == SCROLLBAR_AS_NEEDED)
	{
		if (ContentArea->GetWidth() <= VisibleContentArea->GetWidth())
		{		
			SetVisiblity(false);
			SetEnabled(false);
			ParentScrollArea->Update();
			return;
		}
		else
		{
			SetVisiblity(true);
			SetEnabled(true);
			ParentScrollArea->Update();
		}
	}

	if (ContentArea->GetWidth() > VisibleContentArea->GetWidth())
		ScrollbarSlider.SetWidth(ScrollbarTrack.GetWidth() * VisibleContentArea->GetWidth() / ContentArea->GetWidth());		

	else
	{
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarTrack.GetPosition().x, ScrollbarSlider.GetPosition().y));
		ScrollbarSlider.SetWidth(ScrollbarTrack.GetWidth());
	}

	if (ContentArea->GetRectangle().LeftUp.x <= VisibleContentArea->GetRectangle().LeftUp.x)
	{
		float ScrollbarSliderXPosition = ScrollbarTrack.GetPosition().x + ScrollbarTrack.GetWidth() * (VisibleContentArea->GetRectangle().LeftUp.x - ContentArea->GetRectangle().LeftUp.x) / ContentArea->GetWidth();
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSliderXPosition, ScrollbarSlider.GetPosition().y));
	}
}

ZEUIControl* ZEUIHorizontalScrollbar::GetViewport()
{
	return ContentArea;
}

void ZEUIHorizontalScrollbar::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);
	
	ScrollbarLeftArrow.SetPosition(ZEVector2(0.0f, 0.0f));
	ScrollbarTrack.SetWidth(Width - ScrollbarLeftArrow.GetWidth() - ScrollbarRightArrow.GetWidth());
	ScrollbarTrack.SetPosition(ZEVector2(ScrollbarLeftArrow.GetPosition().x + ScrollbarLeftArrow.GetWidth(), ScrollbarLeftArrow.GetPosition().y));
	ScrollbarRightArrow.SetPosition(ZEVector2(ScrollbarTrack.GetPosition().x + ScrollbarTrack.GetWidth(), ScrollbarRightArrow.GetPosition().y));	
}

void ZEUIHorizontalScrollbar::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);

	ScrollbarTrack.SetHeight(Height);
	ScrollbarLeftArrow.SetHeight(Height);
	ScrollbarRightArrow.SetHeight(Height);
	ScrollbarSlider.SetHeight(Height);
}

ZEMaterial* ZEUIHorizontalScrollbar::GetMaterial() const
{
	return NULL;
}

void ZEUIHorizontalScrollbar::SetMaterial(ZEMaterial* Material)
{

}

ZEUIHorizontalScrollbar::ZEUIHorizontalScrollbar(ZEUIScrollArea* Parent)
{	
	ParentScrollArea = Parent;

	ScrollbarLeftArrow.SetPosition(ZEVector2(0, 0));
	ScrollbarLeftArrow.SetWidth(15);
	ScrollbarLeftArrow.SetHeight(15);
	ScrollbarLeftArrow.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);

	ScrollbarTrack.SetPosition(ZEVector2(30, 0));
	ScrollbarTrack.SetWidth(140);
	ScrollbarTrack.SetHeight(30);
	ScrollbarTrack.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 0.0f);

	ScrollbarRightArrow.SetPosition(ZEVector2(170, 0));
	ScrollbarRightArrow.SetWidth(15);
	ScrollbarRightArrow.SetHeight(15);
	ScrollbarRightArrow.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);

	ScrollbarSlider.SetPosition(ZEVector2(30, 0));
	ScrollbarSlider.SetWidth(140);
	ScrollbarSlider.SetHeight(30);
	ScrollbarSlider.Frame.Color = ZEVector4(0.0f, 0.0f, 1.0f, 1.0f);

	AddChildControl(&ScrollbarTrack);
	AddChildControl(&ScrollbarLeftArrow);	
	AddChildControl(&ScrollbarRightArrow);
	AddChildControl(&ScrollbarSlider);

	ScrollbarSlider.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIHorizontalScrollbar::ScrollbarSliderPress));
	ScrollbarSlider.SetMouseMovedEvent(BindDelegate(this, &ZEUIHorizontalScrollbar::ScrollbarSliderMove));
	ScrollbarSlider.SetMouseButtonReleasedEvent(BindDelegate(this, &ZEUIHorizontalScrollbar::ScrollbarSliderRelease));

	ScrollbarLeftArrow.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIHorizontalScrollbar::ScrollbarLeftArrowPress));
	ScrollbarLeftArrow.SetMouseButtonReleasedEvent(BindDelegate(this, &ZEUIHorizontalScrollbar::ScrollbarLeftArrowRelease));

	ScrollbarRightArrow.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIHorizontalScrollbar::ScrollbarRightArrowPress));
	ScrollbarRightArrow.SetMouseButtonReleasedEvent(BindDelegate(this, &ZEUIHorizontalScrollbar::ScrollbarRightArrowRelease));

	ScrollbarTrack.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIHorizontalScrollbar::ScrollbarTrackPress));
	ScrollbarTrack.SetMouseButtonReleasedEvent(BindDelegate(this, &ZEUIHorizontalScrollbar::ScrollbarTrackRelease));

	SetWidth(200);
	SetHeight(15);

	ScrollbarSlider.SetFocusable(true);
	ScrollbarRightArrow.SetFocusable(true);
	ScrollbarLeftArrow.SetFocusable(true);
	ScrollbarTrack.SetFocusable(true);
}
