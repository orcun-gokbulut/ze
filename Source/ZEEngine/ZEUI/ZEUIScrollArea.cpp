//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIScrollArea.cpp
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

#include "ZEUIScrollArea.h"

void ZEUIScrollArea::SetHorizontalScrollbarPolicy(ScrollbarPolicy Policy)
{
	HorizontalScrollbarPolicy = Policy;

	if (Policy == SCROLLBAR_ALWAYS_ON)
	{
		HorizontalScrollbar->SetVisiblity(true);
		HorizontalScrollbar->SetEnabled(true);
	}

	else if (Policy == SCROLLBAR_ALWAYS_OFF)
	{
		HorizontalScrollbar->SetVisiblity(false);
		HorizontalScrollbar->SetEnabled(false);
	}

	else if (Policy == SCROLLBAR_AS_NEEDED)
	{
		HorizontalScrollbar->SetVisiblity(false);
		HorizontalScrollbar->SetEnabled(false);
	}
	SetHeight(GetHeight());
}

ScrollbarPolicy ZEUIScrollArea::GetHorizontalScrollbarPolicy()
{
	return HorizontalScrollbarPolicy;
}

void ZEUIScrollArea::SetVerticalScrollbarPolicy(ScrollbarPolicy Policy)
{
	VerticalScrollbarPolicy = Policy;

	if (Policy == SCROLLBAR_ALWAYS_ON)
	{
		VerticalScrollbar->SetVisiblity(true);
		VerticalScrollbar->SetEnabled(true);
	}

	else if (Policy == SCROLLBAR_ALWAYS_OFF)
	{
		VerticalScrollbar->SetVisiblity(false);
		VerticalScrollbar->SetEnabled(false);
	}

	else if (Policy == SCROLLBAR_AS_NEEDED)
	{
		VerticalScrollbar->SetVisiblity(false);
		VerticalScrollbar->SetEnabled(false);
	}

	SetWidth(GetWidth());
}

ScrollbarPolicy ZEUIScrollArea::GetVerticalScrollbarPolicy()
{
	return VerticalScrollbarPolicy;
}

ZEUIVerticalScrollbar* ZEUIScrollArea::GetVerticalScrollbar()
{
	return VerticalScrollbar;
}

ZEUIHorizontalScrollbar* ZEUIScrollArea::GetHorizontalScrollbar()
{
	return HorizontalScrollbar;
}

void ZEUIScrollArea::AddChildControl(ZEUIControl* Control)
{
	ContentArea.AddChildControl(Control);

	float XPosition = ContentArea.GetPosition().x;
	float YPosition = ContentArea.GetPosition().y;
	float Width		= ContentArea.GetWidth();
	float Height	= ContentArea.GetHeight();

	for(int I = 0; I < ContentArea.GetChildControls().GetCount(); I++)
	{
		if (XPosition > ContentArea.GetChildControls()[I]->GetPosition().x)
		{
			XPosition = ContentArea.GetChildControls()[I]->GetPosition().x;
		}
		if (YPosition > ContentArea.GetChildControls()[I]->GetPosition().y)
		{
			YPosition = ContentArea.GetChildControls()[I]->GetPosition().y;
		}
		if (XPosition + Width < ContentArea.GetChildControls()[I]->GetPosition().x + ContentArea.GetChildControls()[I]->GetWidth())
		{
			Width = ContentArea.GetChildControls()[I]->GetPosition().x + ContentArea.GetChildControls()[I]->GetWidth() - XPosition;
		}
		if (Height + YPosition < ContentArea.GetChildControls()[I]->GetPosition().y + ContentArea.GetChildControls()[I]->GetHeight())
		{
			Height = ContentArea.GetChildControls()[I]->GetPosition().y + ContentArea.GetChildControls()[I]->GetHeight() - YPosition;
		}
	}

	ContentArea.SetPosition(ZEVector2(XPosition, YPosition));
	ContentArea.SetWidth(Width);
	ContentArea.SetHeight(Height);

	HorizontalScrollbar->SetContentArea(&ContentArea, &VisibleContentArea);
	VerticalScrollbar->SetContentArea(&ContentArea, &VisibleContentArea);
}

ZEUIFrameControl* ZEUIScrollArea::GetVisibleContentArea()
{
	return &VisibleContentArea;
}

ZEUIFrameControl* ZEUIScrollArea::GetContentArea()
{
	return &ContentArea;
}

void ZEUIScrollArea::SetSize(const ZEVector2& Size)
{
	SetWidth(Size.x);
	SetHeight(Size.y);
}

void ZEUIScrollArea::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);

	if (VerticalScrollbar->GetVisiblity())
	{
		VerticalScrollbar->SetPosition(ZEVector2(Width - VerticalScrollbar->GetWidth(), VisibleContentArea.GetPosition().y));
		HorizontalScrollbar->SetWidth(Width - VerticalScrollbar->GetWidth());
		VisibleContentArea.SetWidth(GetWidth() - VerticalScrollbar->GetWidth());
	}
	else
	{
		HorizontalScrollbar->SetWidth(Width);
		VisibleContentArea.SetWidth(GetWidth());
	}
}

void ZEUIScrollArea::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);

	if (HorizontalScrollbar->GetVisiblity())
	{
		HorizontalScrollbar->SetPosition(ZEVector2(VisibleContentArea.GetPosition().x, Height - HorizontalScrollbar->GetHeight()));
		VerticalScrollbar->SetHeight(Height - HorizontalScrollbar->GetHeight());
		VisibleContentArea.SetHeight(GetHeight() - HorizontalScrollbar->GetHeight());
	}
	else
	{
		VerticalScrollbar->SetHeight(Height);
		VisibleContentArea.SetHeight(GetHeight());
	}
}

ZEMaterial* ZEUIScrollArea::GetMaterial() const
{
	return NULL;
}

void ZEUIScrollArea::SetMaterial(ZEMaterial* Material)
{

}

void ZEUIScrollArea::MoveContent(const ZEVector2& MoveAmount)
{
	ContentArea.SetPosition(ContentArea.GetPosition() + MoveAmount);

// 	for(int I = 0; I < ContentArea.GetChildControls().GetCount(); I++)
// 		ContentArea.GetChildControls()[I]->SetPosition(ContentArea.GetChildControls()[I]->GetPosition() + MoveAmount);
}

void ZEUIScrollArea::Update()
{
	if (VerticalScrollbar->GetVisiblity())
	{
		VerticalScrollbar->SetPosition(ZEVector2(GetPosition().x + GetWidth() - VerticalScrollbar->GetWidth(), GetPosition().y));
		HorizontalScrollbar->SetWidth(GetWidth() - VerticalScrollbar->GetWidth());
		VisibleContentArea.SetWidth(GetWidth() - VerticalScrollbar->GetWidth());
	}
	else
	{
		HorizontalScrollbar->SetWidth(GetWidth());
		VisibleContentArea.SetWidth(GetWidth());
	}

	if (HorizontalScrollbar->GetVisiblity())
	{
		HorizontalScrollbar->SetPosition(ZEVector2(GetPosition().x, GetRectangle().RightDown.y - HorizontalScrollbar->GetHeight()));
		VerticalScrollbar->SetHeight(GetHeight() - HorizontalScrollbar->GetHeight());
		VisibleContentArea.SetHeight(GetHeight() - HorizontalScrollbar->GetHeight());
	}
	else
	{
		VerticalScrollbar->SetHeight(GetHeight());
		VisibleContentArea.SetHeight(GetHeight());
	}
}

void ZEUIScrollArea::UpdateScrollbars()
{
// 	float XPosition = ContentArea.GetPosition().x;
// 	float YPosition = ContentArea.GetPosition().y;
// 	float Width		= ContentArea.GetWidth();
// 	float Height	= ContentArea.GetHeight();
	float XPosition = 0;
	float YPosition = 0;
	float Width		= 0;
	float Height	= 0;

	for(int I = 0; I < ContentArea.GetChildControls().GetCount(); I++)
	{
		if (XPosition > ContentArea.GetChildControls()[I]->GetPosition().x)
		{
			XPosition = ContentArea.GetChildControls()[I]->GetPosition().x;
		}
		if (YPosition > ContentArea.GetChildControls()[I]->GetPosition().y)
		{
			YPosition = ContentArea.GetChildControls()[I]->GetPosition().y;
		}
		if (XPosition + Width < ContentArea.GetChildControls()[I]->GetPosition().x + ContentArea.GetChildControls()[I]->GetWidth())
		{
			Width = ContentArea.GetChildControls()[I]->GetPosition().x + ContentArea.GetChildControls()[I]->GetWidth() - XPosition;
		}
		if (Height + YPosition < ContentArea.GetChildControls()[I]->GetPosition().y + ContentArea.GetChildControls()[I]->GetHeight())
		{
			Height = ContentArea.GetChildControls()[I]->GetPosition().y + ContentArea.GetChildControls()[I]->GetHeight() - YPosition;
		}
	}

	ContentArea.SetPosition(ZEVector2(XPosition, YPosition));
	ContentArea.SetWidth(Width);
	ContentArea.SetHeight(Height);

	VerticalScrollbar->SetContentArea(&ContentArea, &VisibleContentArea);
	HorizontalScrollbar->SetContentArea(&ContentArea, &VisibleContentArea);
}

ZEUIScrollArea::ZEUIScrollArea()
{
	VerticalScrollbar = new ZEUIVerticalScrollbar(this);
	HorizontalScrollbar = new ZEUIHorizontalScrollbar(this);

	SetHorizontalScrollbarPolicy(SCROLLBAR_ALWAYS_ON);
	SetVerticalScrollbarPolicy(SCROLLBAR_ALWAYS_ON);

	VisibleContentArea.SetPosition(ZEVector2(0,0));
	VisibleContentArea.Frame.Color = ZEVector4(0.0f, 0.0f, 0.0f, 0.0f);

/*	ContentArea = NULL;*/
	ContentArea.SetPosition(ZEVector2(0.0f, 0.0f));
	ContentArea.SetSize(ZEVector2(1,1));
	ContentArea.SetFocusable(false);
	ContentArea.Frame.Color = ZEVector4(0.0f, 0.0f, 0.0f, 0.7f);

	SetHeight(200);
	SetWidth(200);

	ZEUIControl::AddChildControl(VerticalScrollbar);
	ZEUIControl::AddChildControl(HorizontalScrollbar);
	ZEUIControl::AddChildControl(&VisibleContentArea);

	VisibleContentArea.AddChildControl(&ContentArea);
}
