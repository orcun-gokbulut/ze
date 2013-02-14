//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEIUCheckBoxControl.cpp
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

#include "ZEIUCheckBoxControl.h"
#include "ZEUIManager.h"
#include "ZEUILabel.h"

void ZEUICheckBoxControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if(State == ZE_UI_CBS_UNCHECKED)
		SetState(ZE_UI_CBS_CHECKED);
	else
		SetState(ZE_UI_CBS_UNCHECKED);

	ZEUIControl::MouseButtonPressed(Button, MousePosition);
}

void ZEUICheckBoxControl::Draw(ZEUIRenderer* Renderer)
{
	ZEUIRectangle Output;
	Output.ZOrder = GetZOrder() + 1;

	if(!ZEUIRectangle::Clip(Output, Box, GetRectangle()))
		Renderer->AddRectangle(Output);

	ZEUIControl::Draw(Renderer);
}

void ZEUICheckBoxControl::SetState(ZEUICheckBoxState State)
{
	this->State = State;
	
	if(State == ZE_UI_CBS_CHECKED)
		Box.Color = ZEUIManager::GetDefaultForegroundColor();
	else
		Box.Color = ZEUIManager::GetDefaultBackgroundColor();
}

ZEUICheckBoxState ZEUICheckBoxControl::GetState() const
{
	return State;
}

void ZEUICheckBoxControl::SetText(ZEString Text)
{
	Label->SetText(Text);
}

ZEString ZEUICheckBoxControl::GetText()
{
	return Label->GetText();
}

void ZEUICheckBoxControl::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);

	if(Height < Label->GetHeight())
		Label->SetHeight(Height);
// 	else
// 		Label->SetHeight(Label->GetFontResource()->)
//		Font size height adjustment fix needed

 	ZEVector2 LabelPos = ZEVector2((Box.Positions.RightDown - Box.Positions.LeftUp).x, (GetHeight() - Label->GetHeight()) / 2);
 	Label->SetPosition(LabelPos);
	Box.Positions.RightDown.y = Box.Positions.LeftUp.y + GetHeight();
}

void ZEUICheckBoxControl::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);
	Label->SetPosition(ZEVector2((Box.Positions.RightDown - Box.Positions.LeftUp).x , Label->GetPosition().y));
	float LabelWidth = ZEMath::Floor(GetWidth() - (Box.Positions.RightDown - Box.Positions.LeftUp).x);
	Label->SetWidth(ZEMath::Max(0.0f, LabelWidth));
}

void ZEUICheckBoxControl::SetPosition(const ZEVector2& Position)
{
	ZEVector2 Displacement = Position - GetPosition();
	Box.Positions.LeftUp += Displacement;
	Box.Positions.RightDown += Displacement;
	ZEUIControl::SetPosition(Position);
}

ZEUICheckBoxControl::ZEUICheckBoxControl()
{
	State = ZE_UI_CBS_UNCHECKED;
	Label = new ZEUILabel();
	AddChildControl(Label);
	Label->SetTextMargins(ZEVector4(0, 2, 0, 0));
	Box.Positions.LeftUp = GetPosition();
	Box.Positions.RightDown = GetPosition() + ZEVector2::One * 22;
	Label->SetText("Check Box");
	Label->SetHeight(16);
	BoxMaterial = ZEMaterialUserInterface::CreateInstance();
	Box.Material = BoxMaterial;
	SetPosition(ZEVector2::Zero);
	SetWidth(120);
	SetHeight(22);
	SetState(ZE_UI_CBS_UNCHECKED);
	SetMoveable(false);
	Label->SetMoveable(false);
	Label->SetBackgroundColor(ZEVector4::Zero);
}

ZEUICheckBoxControl::~ZEUICheckBoxControl()
{

}
