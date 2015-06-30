//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIBorderedFrameControl.cpp
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

#include "ZEUIBorderedFrameControl.h"

ZEUIBorderedFrameControl::ZEUIBorderedFrameControl()
{
	ZEUIControl::AddChildControl(&MainArea);
	ZEUIControl::AddChildControl(&TopEdgeArea);
	ZEUIControl::AddChildControl(&LeftEdgeArea);
	ZEUIControl::AddChildControl(&RightEdgeArea);
	ZEUIControl::AddChildControl(&BottomEdgeArea);
	ZEUIControl::AddChildControl(&LeftUpCornerArea);
	ZEUIControl::AddChildControl(&LeftDownCornerArea);
	ZEUIControl::AddChildControl(&RightUpCornerArea);
	ZEUIControl::AddChildControl(&RightDownCornerArea);

	MainArea.Frame.Color = ZEVector4(0.0f, 0.0f, 1.0f, 1.0f);

	LeftUpCornerArea.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 1.0f);
	LeftDownCornerArea.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 1.0f);
	RightUpCornerArea.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 1.0f);
	RightDownCornerArea.Frame.Color = ZEVector4(1.0f, 0.0f, 0.0f, 1.0f);

	TopEdgeArea.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
	LeftEdgeArea.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
	RightEdgeArea.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
	BottomEdgeArea.Frame.Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);

	MainArea.SetWidth(376);
	MainArea.SetHeight(376);

	LeftUpCornerArea.SetWidth(24);
	LeftUpCornerArea.SetHeight(24);

	LeftDownCornerArea.SetWidth(24);
	LeftDownCornerArea.SetHeight(24);

	RightUpCornerArea.SetWidth(24);
	RightUpCornerArea.SetHeight(24);

	RightDownCornerArea.SetWidth(24);
	RightDownCornerArea.SetHeight(24);

	TopEdgeArea.SetHeight(24);
	TopEdgeArea.SetWidth(376);

	BottomEdgeArea.SetHeight(24);
	BottomEdgeArea.SetWidth(376);

	LeftEdgeArea.SetHeight(376);
	LeftEdgeArea.SetWidth(24);

	RightEdgeArea.SetHeight(376);
	RightEdgeArea.SetWidth(24);

	ZEUIControl::SetWidth(424);
	ZEUIControl::SetHeight(424);

// 	LeftUpCornerArea.SetPosition(GetPosition());
// 	LeftDownCornerArea.SetPosition(ZEVector2(GetRectangle().LeftUp.x, GetRectangle().RightDown.y - LeftDownCornerArea.GetHeight()));
// 	RightUpCornerArea.SetPosition(ZEVector2(GetRectangle().RightDown.x - LeftUpCornerArea.GetWidth(), GetRectangle().LeftUp.y));
// 	RightDownCornerArea.SetPosition(ZEVector2(GetRectangle().RightDown.x - RightDownCornerArea.GetWidth(), GetRectangle().RightDown.y - RightDownCornerArea.GetHeight()));
// 	
// 	TopEdgeArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x + LeftUpCornerArea.GetWidth(), LeftUpCornerArea.GetPosition().y));
// 	BottomEdgeArea.SetPosition(ZEVector2(LeftDownCornerArea.GetPosition().x + LeftDownCornerArea.GetWidth(), LeftDownCornerArea.GetPosition().y));
// 	LeftEdgeArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x, LeftUpCornerArea.GetPosition().y + LeftUpCornerArea.GetHeight()));
// 	RightEdgeArea.SetPosition(ZEVector2(RightUpCornerArea.GetPosition().x, RightUpCornerArea.GetPosition().y + RightUpCornerArea.GetHeight()));
// 
// 	MainArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x + LeftUpCornerArea.GetWidth(), LeftUpCornerArea.GetPosition().y + LeftUpCornerArea.GetHeight()));

	LeftUpCornerArea.SetPosition(ZEVector2::Zero);
	LeftDownCornerArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x, LeftUpCornerArea.GetPosition().y + LeftUpCornerArea.GetHeight() + LeftEdgeArea.GetHeight()));
	RightUpCornerArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x + LeftUpCornerArea.GetWidth() + TopEdgeArea.GetWidth(), LeftUpCornerArea.GetPosition().y));
	RightDownCornerArea.SetPosition(ZEVector2(RightUpCornerArea.GetPosition().x, LeftDownCornerArea.GetPosition().y));

	TopEdgeArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x + LeftUpCornerArea.GetWidth(), LeftUpCornerArea.GetPosition().y ));
	BottomEdgeArea.SetPosition(ZEVector2(LeftDownCornerArea.GetPosition().x + LeftDownCornerArea.GetWidth(), LeftDownCornerArea.GetPosition().y));
	LeftEdgeArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x, LeftUpCornerArea.GetPosition().y + LeftUpCornerArea.GetHeight()));
	RightEdgeArea.SetPosition(ZEVector2(RightUpCornerArea.GetPosition().x, RightUpCornerArea.GetPosition().y + RightUpCornerArea.GetHeight()));

	MainArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x + LeftUpCornerArea.GetWidth(), LeftUpCornerArea.GetPosition().y + LeftUpCornerArea.GetHeight()));
}

void ZEUIBorderedFrameControl::SetHeight(float Height)
{
	float HeightDifferance = Height - GetHeight();

	ZEUIControl::SetHeight(Height);

	LeftDownCornerArea.SetPosition(ZEVector2(LeftDownCornerArea.GetPosition().x, LeftDownCornerArea.GetPosition().y + HeightDifferance));
	RightDownCornerArea.SetPosition(ZEVector2(RightDownCornerArea.GetPosition().x, RightDownCornerArea.GetPosition().y + HeightDifferance));
	BottomEdgeArea.SetPosition(ZEVector2(BottomEdgeArea.GetPosition().x, BottomEdgeArea.GetPosition().y + HeightDifferance));

	LeftEdgeArea.SetHeight(LeftEdgeArea.GetHeight() + HeightDifferance);
	RightEdgeArea.SetHeight(RightEdgeArea.GetHeight() + HeightDifferance);

	MainArea.SetHeight(MainArea.GetHeight() + HeightDifferance);
}

void ZEUIBorderedFrameControl::SetWidth(float Width)
{
	float WidthDifference = Width - GetWidth();

	ZEUIControl::SetWidth(Width);

	RightUpCornerArea.SetPosition(ZEVector2(RightUpCornerArea.GetPosition().x + WidthDifference, RightUpCornerArea.GetPosition().y));
	RightDownCornerArea.SetPosition(ZEVector2(RightDownCornerArea.GetPosition().x + WidthDifference, RightDownCornerArea.GetPosition().y));

	TopEdgeArea.SetWidth(TopEdgeArea.GetWidth() + WidthDifference);
	BottomEdgeArea.SetWidth(BottomEdgeArea.GetWidth() + WidthDifference);

	RightEdgeArea.SetPosition(ZEVector2(RightEdgeArea.GetPosition().x + WidthDifference, RightEdgeArea.GetPosition().y));

 	MainArea.SetWidth(MainArea.GetWidth() + WidthDifference);
}

void ZEUIBorderedFrameControl::SetMaterial(ZERNMaterial* Material)
{

}

ZERNMaterial*	ZEUIBorderedFrameControl::GetMaterial() const
{
	return NULL;
}

void ZEUIBorderedFrameControl::AddChildControl(ZEUIControl* Control)
{
	MainArea.AddChildControl(Control);
}

void ZEUIBorderedFrameControl::SetElementVisibilities(ZEInt32 Elements, bool Visibility)
{
	if ((Elements & ZEUI_BFC_EL_ALL) == ZEUI_BFC_EL_ALL)
	{
		MainArea.SetVisiblity(Visibility);
		LeftUpCornerArea.SetVisiblity(Visibility);
		LeftDownCornerArea.SetVisiblity(Visibility);
		RightUpCornerArea.SetVisiblity(Visibility);
		RightDownCornerArea.SetVisiblity(Visibility);
		TopEdgeArea.SetVisiblity(Visibility);
		BottomEdgeArea.SetVisiblity(Visibility);
		LeftEdgeArea.SetVisiblity(Visibility);
		RightEdgeArea.SetVisiblity(Visibility);
		return;
	}

	if ((Elements & ZEUI_BFC_EL_MAINAREA) == ZEUI_BFC_EL_MAINAREA)
		MainArea.SetVisiblity(Visibility);

	if ((Elements & ZEUI_BFC_EL_LEFTUPCORNER) == ZEUI_BFC_EL_LEFTUPCORNER)
		LeftUpCornerArea.SetVisiblity(Visibility);

	if ((Elements & ZEUI_BFC_EL_LEFTDOWNCORNER) == ZEUI_BFC_EL_LEFTDOWNCORNER)
		LeftDownCornerArea.SetVisiblity(Visibility);

	if ((Elements & ZEUI_BFC_EL_RIGHTUPCORNER) == ZEUI_BFC_EL_RIGHTUPCORNER)
		RightUpCornerArea.SetVisiblity(Visibility);

	if ((Elements & ZEUI_BFC_EL_RIGHTDOWNCORNER) == ZEUI_BFC_EL_RIGHTDOWNCORNER)
		RightDownCornerArea.SetVisiblity(Visibility);

	if ((Elements & ZEUI_BFC_EL_TOPEDGE) == ZEUI_BFC_EL_TOPEDGE)
		TopEdgeArea.SetVisiblity(Visibility);

	if ((Elements & ZEUI_BFC_EL_BOTTOMEDGE) == ZEUI_BFC_EL_BOTTOMEDGE)
		BottomEdgeArea.SetVisiblity(Visibility);

	if ((Elements & ZEUI_BFC_EL_LEFTEDGE) == ZEUI_BFC_EL_LEFTEDGE)
		LeftEdgeArea.SetVisiblity(Visibility);

	if ((Elements & ZEUI_BFC_EL_RIGHTEDGE) == ZEUI_BFC_EL_RIGHTEDGE)
		RightEdgeArea.SetVisiblity(Visibility);
}

ZEInt32 ZEUIBorderedFrameControl::GetVisibleElements()
{
	ZEInt32 VisibleElements = 0;

	if (MainArea.GetVisiblity())
		VisibleElements = VisibleElements | ZEUI_BFC_EL_MAINAREA;

	if (LeftUpCornerArea.GetVisiblity())
		VisibleElements = VisibleElements | ZEUI_BFC_EL_LEFTUPCORNER;

	if (LeftDownCornerArea.GetVisiblity())
		VisibleElements = VisibleElements | ZEUI_BFC_EL_LEFTDOWNCORNER;

	if (RightUpCornerArea.GetVisiblity())
		VisibleElements = VisibleElements | ZEUI_BFC_EL_RIGHTUPCORNER;

	if (RightDownCornerArea.GetVisiblity())
		VisibleElements = VisibleElements | ZEUI_BFC_EL_RIGHTDOWNCORNER;

	if (TopEdgeArea.GetVisiblity())
		VisibleElements = VisibleElements | ZEUI_BFC_EL_TOPEDGE;

	if (BottomEdgeArea.GetVisiblity())
		VisibleElements = VisibleElements | ZEUI_BFC_EL_BOTTOMEDGE;

	if (LeftEdgeArea.GetVisiblity())
		VisibleElements = VisibleElements | ZEUI_BFC_EL_LEFTEDGE;

	if (RightEdgeArea.GetVisiblity())
		VisibleElements = VisibleElements | ZEUI_BFC_EL_RIGHTEDGE;

	return VisibleElements;
}

void ZEUIBorderedFrameControl::SetBorderThickness(float Thickness)
{
	LeftUpCornerArea.SetHeight(Thickness);
	LeftUpCornerArea.SetWidth(Thickness);
	LeftDownCornerArea.SetHeight(Thickness);
	LeftDownCornerArea.SetWidth(Thickness);
	RightUpCornerArea.SetWidth(Thickness);
	RightUpCornerArea.SetHeight(Thickness);
	RightDownCornerArea.SetHeight(Thickness);
	RightDownCornerArea.SetWidth(Thickness);
	TopEdgeArea.SetHeight(Thickness);
	BottomEdgeArea.SetHeight(Thickness);
	LeftEdgeArea.SetWidth(Thickness);
	RightEdgeArea.SetWidth(Thickness);

	MainArea.SetWidth(GetWidth() - Thickness * 2);
	MainArea.SetHeight(GetHeight() - Thickness * 2);
	MainArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x + LeftUpCornerArea.GetWidth(), LeftUpCornerArea.GetPosition().y + LeftUpCornerArea.GetHeight()));

	LeftUpCornerArea.SetPosition(ZEVector2::Zero);
	LeftDownCornerArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x, MainArea.GetPosition().y + MainArea.GetHeight()));
	RightUpCornerArea.SetPosition(ZEVector2(MainArea.GetPosition().x + MainArea.GetWidth(), LeftUpCornerArea.GetPosition().y));
	RightDownCornerArea.SetPosition(ZEVector2(MainArea.GetPosition().x + MainArea.GetWidth(), MainArea.GetPosition().y + MainArea.GetHeight()));

	TopEdgeArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x + LeftUpCornerArea.GetWidth(), LeftUpCornerArea.GetPosition().y ));
	BottomEdgeArea.SetPosition(ZEVector2(LeftDownCornerArea.GetPosition().x + LeftDownCornerArea.GetWidth(), LeftDownCornerArea.GetPosition().y));
	LeftEdgeArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x, LeftUpCornerArea.GetPosition().y + LeftUpCornerArea.GetHeight()));
	RightEdgeArea.SetPosition(ZEVector2(RightUpCornerArea.GetPosition().x, RightUpCornerArea.GetPosition().y + RightUpCornerArea.GetHeight()));

	TopEdgeArea.SetWidth(MainArea.GetWidth());
	BottomEdgeArea.SetWidth(MainArea.GetWidth());
	LeftEdgeArea.SetHeight(MainArea.GetHeight());
	RightEdgeArea.SetHeight(MainArea.GetHeight());


// 	LeftUpCornerArea.SetPosition(GetPosition());
// 	LeftDownCornerArea.SetPosition(ZEVector2(GetRectangle().LeftUp.x, GetRectangle().RightDown.y - LeftDownCornerArea.GetHeight()));
// 	RightUpCornerArea.SetPosition(ZEVector2(GetRectangle().RightDown.x - LeftUpCornerArea.GetWidth(), GetRectangle().LeftUp.y));
// 	RightDownCornerArea.SetPosition(ZEVector2(GetRectangle().RightDown.x - RightDownCornerArea.GetWidth(), GetRectangle().RightDown.y - RightDownCornerArea.GetHeight()));
// 
// 	TopEdgeArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x + LeftUpCornerArea.GetWidth(), LeftUpCornerArea.GetPosition().y));
// 	BottomEdgeArea.SetPosition(ZEVector2(LeftDownCornerArea.GetPosition().x + LeftDownCornerArea.GetWidth(), LeftDownCornerArea.GetPosition().y));
// 	LeftEdgeArea.SetPosition(ZEVector2(LeftUpCornerArea.GetPosition().x, LeftUpCornerArea.GetPosition().y + LeftUpCornerArea.GetHeight()));
// 	RightEdgeArea.SetPosition(ZEVector2(RightUpCornerArea.GetPosition().x, RightUpCornerArea.GetPosition().y + RightUpCornerArea.GetHeight()));
// 
// 	TopEdgeArea.SetWidth(RightUpCornerArea.GetPosition().x - LeftUpCornerArea.GetPosition().x - Thickness);
// 	BottomEdgeArea.SetWidth(RightDownCornerArea.GetPosition().x - LeftDownCornerArea.GetPosition().x - Thickness);
// 	LeftEdgeArea.SetHeight(LeftDownCornerArea.GetPosition().y - LeftUpCornerArea.GetPosition().y - Thickness);
// 	RightEdgeArea.SetHeight(RightDownCornerArea.GetPosition().y - RightUpCornerArea.GetPosition().y - Thickness);


}
