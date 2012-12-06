//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIVerticalScrollbar.cpp
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

#include "ZEUIVerticalScrollBar.h"
#include "ZEUI/ZEUIScrollArea.h"
#include "ZEUI/ZEUIRectangle.h"

void ZEUIVerticalScrollbar::ScrollbarSliderPress(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarSlider.Frame.Color = ZEVector4(0.0f, 0.5f, 0.7f, 1.0f);
}

void ZEUIVerticalScrollbar::ScrollbarSliderMove(ZEUIMouseKey Button, const ZEVector2& MoveAmount)
{
	if (ScrollbarSlider.GetPosition().y + MoveAmount.y < ScrollbarUpArrow.GetRectangle().RightDown.y)
	{
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarTrack.GetPosition().y));	
		ContentArea->SetPosition(ZEVector2(ContentArea->GetPosition().x, VisibleContentArea->GetPosition().y));
		return;
	}

	else if (ScrollbarSlider.GetRectangle().RightDown.y + MoveAmount.y > ScrollbarDownArrow.GetRectangle().LeftUp.y)
	{
		return;
	}

	ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarSlider.GetPosition().y + MoveAmount.y));	
	ParentScrollArea->MoveContent(ZEVector2(0.0f, - (MoveAmount.y * ContentArea->GetHeight() / ScrollbarTrack.GetHeight())));
}

void ZEUIVerticalScrollbar::ScrollbarSliderRelease(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarSlider.Frame.Color = ZEVector4(0.0f, 0.0f, 1.0f, 1.0f);
}

void ZEUIVerticalScrollbar::ScrollbarUpArrowPress(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarUpArrow.Frame.Color = ZEVector4(1.0f, 1.0f, 0.0f, 1.0f);

	if (ScrollbarSlider.GetPosition().y - 2.0f < ScrollbarUpArrow.GetRectangle().RightDown.y)
	{
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarUpArrow.GetRectangle().RightDown.y));
		return;
	}

	ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarSlider.GetPosition().y - 2.0f));
	ParentScrollArea->MoveContent(ZEVector2(0.0f, (2.0f * ContentArea->GetHeight() / ScrollbarTrack.GetHeight())));
}

void ZEUIVerticalScrollbar::ScrollbarUpArrowRelease(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarUpArrow.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
}

void ZEUIVerticalScrollbar::ScrollbarDownArrowPress(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarDownArrow.Frame.Color = ZEVector4(1.0f, 1.0f, 0.0f, 1.0f);

	if (ScrollbarSlider.GetRectangle().RightDown.y + 2.0f > ScrollbarDownArrow.GetRectangle().LeftUp.y)
	{
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarDownArrow.GetRectangle().LeftUp.y - ScrollbarSlider.GetHeight()));
		return;
	}

	ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarSlider.GetPosition().y + 2.0f));
	ParentScrollArea->MoveContent(ZEVector2(0.0f, - (2.0f * ContentArea->GetHeight() / ScrollbarTrack.GetHeight())));
}

void ZEUIVerticalScrollbar::ScrollbarDownArrowRelease(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarDownArrow.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
}

void ZEUIVerticalScrollbar::ScrollbarTrackPress(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarTrack.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 0.1f);

	if (MousePosition.y < ScrollbarSlider.GetScreenRectangle().LeftUp.y)
	{
		if (ScrollbarSlider.GetPosition().y - ScrollbarSlider.GetHeight() < ScrollbarUpArrow.GetRectangle().RightDown.y)
		{
			ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarTrack.GetPosition().y));
			ContentArea->SetPosition(ZEVector2(ContentArea->GetPosition().x, VisibleContentArea->GetPosition().y));
			return;
		}

		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarSlider.GetPosition().y - ScrollbarSlider.GetHeight()));
		ParentScrollArea->MoveContent(ZEVector2(0.0f, (ScrollbarSlider.GetHeight() * ContentArea->GetHeight() / ScrollbarTrack.GetHeight())));
	}

	else if (MousePosition.y > ScrollbarSlider.GetScreenRectangle().RightDown.y)
	{
		if (ScrollbarSlider.GetRectangle().RightDown.y + ScrollbarSlider.GetHeight() > ScrollbarDownArrow.GetRectangle().LeftUp.y)
		{
			ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarTrack.GetRectangle().RightDown.y - ScrollbarSlider.GetHeight()));
			ContentArea->SetPosition(ZEVector2(ContentArea->GetPosition().x, VisibleContentArea->GetRectangle().RightDown.y - ContentArea->GetHeight()));
			return;
		}

		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarSlider.GetPosition().y + ScrollbarSlider.GetHeight()));
		ParentScrollArea->MoveContent(ZEVector2(0.0f, - (ScrollbarSlider.GetHeight() * ContentArea->GetHeight() / ScrollbarTrack.GetHeight())));
	}
}

void ZEUIVerticalScrollbar::ScrollbarTrackRelease(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	ScrollbarTrack.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 0.0f);
}

void ZEUIVerticalScrollbar::SetContentArea(ZEUIControl* ContentArea, ZEUIFrameControl* VisibleContentArea)
{
	this->ContentArea = ContentArea;
	this->VisibleContentArea = VisibleContentArea;

	if (ParentScrollArea->GetVerticalScrollbarPolicy() == SCROLLBAR_ALWAYS_OFF)
		return;

	if (ParentScrollArea->GetVerticalScrollbarPolicy() == SCROLLBAR_AS_NEEDED)
	{
		if (ContentArea->GetHeight() <= VisibleContentArea->GetHeight())
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

	if (ContentArea->GetHeight() > VisibleContentArea->GetHeight())
		ScrollbarSlider.SetHeight(ScrollbarTrack.GetHeight() * VisibleContentArea->GetHeight() / ContentArea->GetHeight());		

	else
	{
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarTrack.GetPosition().y));
		ScrollbarSlider.SetHeight(ScrollbarTrack.GetHeight());
	}

	if (ContentArea->GetRectangle().LeftUp.y <= VisibleContentArea->GetRectangle().LeftUp.y)
	{
		float ScrollbarSliderYPosition = ScrollbarTrack.GetPosition().y + ScrollbarTrack.GetHeight() * (VisibleContentArea->GetRectangle().LeftUp.y - ContentArea->GetRectangle().LeftUp.y) / ContentArea->GetHeight();
		ScrollbarSlider.SetPosition(ZEVector2(ScrollbarSlider.GetPosition().x, ScrollbarSliderYPosition));
	}
}

ZEUIControl* ZEUIVerticalScrollbar::GetViewport()
{
	return ContentArea;
}

void ZEUIVerticalScrollbar::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);

	ScrollbarUpArrow.SetWidth(Width);
	ScrollbarTrack.SetWidth(Width);
	ScrollbarDownArrow.SetWidth(Width);
	ScrollbarSlider.SetWidth(Width);
}

void ZEUIVerticalScrollbar::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);

	ScrollbarUpArrow.SetPosition(ZEVector2(0.0f, 0.0f));
	ScrollbarTrack.SetHeight(Height - ScrollbarUpArrow.GetHeight() - ScrollbarDownArrow.GetHeight());
	ScrollbarTrack.SetPosition(ZEVector2(ScrollbarUpArrow.GetPosition().x, ScrollbarUpArrow.GetPosition().y + ScrollbarUpArrow.GetHeight()));
	ScrollbarDownArrow.SetPosition(ZEVector2(ScrollbarDownArrow.GetPosition().x, ScrollbarTrack.GetPosition().y + ScrollbarTrack.GetHeight()));
}

ZEMaterial* ZEUIVerticalScrollbar::GetMaterial() const
{
	return NULL;
}

void ZEUIVerticalScrollbar::SetMaterial(ZEMaterial* Material)
{

}

ZEUIVerticalScrollbar::ZEUIVerticalScrollbar(ZEUIScrollArea* Parent)
{
	ParentScrollArea = Parent;

	ScrollbarUpArrow.SetPosition(ZEVector2(0, 0));
	ScrollbarUpArrow.SetWidth(15);
	ScrollbarUpArrow.SetHeight(15);
	ScrollbarUpArrow.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);

	ScrollbarTrack.SetPosition(ZEVector2(0, 30));
	ScrollbarTrack.SetWidth(30);
	ScrollbarTrack.SetHeight(140);
	ScrollbarTrack.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 0.0f);

	ScrollbarDownArrow.SetPosition(ZEVector2(0, 170));
	ScrollbarDownArrow.SetWidth(15);
	ScrollbarDownArrow.SetHeight(15);
	ScrollbarDownArrow.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);

	ScrollbarSlider.SetPosition(ZEVector2(0, 30));
	ScrollbarSlider.SetWidth(30);
	ScrollbarSlider.SetHeight(140);
	ScrollbarSlider.Frame.Color = ZEVector4(0.0f, 0.0f, 1.0f, 1.0f);

	AddChildControl(&ScrollbarTrack);
	AddChildControl(&ScrollbarUpArrow);	
	AddChildControl(&ScrollbarDownArrow);
	AddChildControl(&ScrollbarSlider);

	ScrollbarSlider.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIVerticalScrollbar::ScrollbarSliderPress));
	ScrollbarSlider.SetMouseMovedEvent(BindDelegate(this, &ZEUIVerticalScrollbar::ScrollbarSliderMove));
	ScrollbarSlider.SetMouseButtonReleasedEvent(BindDelegate(this, &ZEUIVerticalScrollbar::ScrollbarSliderRelease));

	ScrollbarUpArrow.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIVerticalScrollbar::ScrollbarUpArrowPress));
	ScrollbarUpArrow.SetMouseButtonReleasedEvent(BindDelegate(this, &ZEUIVerticalScrollbar::ScrollbarUpArrowRelease));

	ScrollbarDownArrow.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIVerticalScrollbar::ScrollbarDownArrowPress));
	ScrollbarDownArrow.SetMouseButtonReleasedEvent(BindDelegate(this, &ZEUIVerticalScrollbar::ScrollbarDownArrowRelease));

	ScrollbarTrack.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIVerticalScrollbar::ScrollbarTrackPress));
	ScrollbarTrack.SetMouseButtonReleasedEvent(BindDelegate(this, &ZEUIVerticalScrollbar::ScrollbarTrackRelease));

	SetWidth(15);
	SetHeight(200);

	ScrollbarSlider.SetFocusable(true);
	ScrollbarDownArrow.SetFocusable(true);
	ScrollbarUpArrow.SetFocusable(true);
	ScrollbarTrack.SetFocusable(true);
}
