//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIHorizontalSliderControl.cpp
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

#include "ZEUIHorizontalSliderControl.h"

void ZEUIHorizontalSliderControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if(Button == ZE_UI_MOUSE_BUTTON_LEFT && ZERectangle::IntersectionTest(SliderButton.Positions, MousePosition) == true)
		IsButtonPressed = true;

	ZEUIControl::MouseButtonPressed(Button, MousePosition);
}

void ZEUIHorizontalSliderControl::MouseButtonReleased(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if(Button == ZE_UI_MOUSE_BUTTON_LEFT)
		IsButtonPressed = false;

	ZEUIControl::MouseButtonReleased(Button, MousePosition);
}

void ZEUIHorizontalSliderControl::MouseMoveEvent(ZEUIMouseKey Button, const ZEVector2& MoveAmount)
{
	if(IsButtonPressed && (ZEInt32)MoveAmount.x != 0)
	{
		float MinMaxDifference = GetMaximumValue() - GetMinimumValue();
		float ValuePerPixel = GetWidth() / MinMaxDifference;
		float ValueChangeAmount = MoveAmount.x / ValuePerPixel;
		SetValue(Value + ValueChangeAmount);
	}

	ZEUIControl::MouseMoveEvent(Button, MoveAmount);
}

void ZEUIHorizontalSliderControl::Draw(ZEUIRenderer* Renderer)
{
	ZEUIRectangle ButtonOutput, LineOutput;
	SliderLine.ZOrder = GetZOrder() + 1;
	SliderButton.ZOrder = GetZOrder() + 2;

	ZERectangle TempRect = GetVisibleRectangle();
	TempRect.LeftUp.x -= SliderButton.Positions.GetWidth() / 2;
	TempRect.RightDown.x += SliderButton.Positions.GetWidth() / 2;

	if(!ZEUIRectangle::Clip(LineOutput, SliderLine, TempRect))
		Renderer->AddRectangle(&LineOutput);

	if(!ZEUIRectangle::Clip(ButtonOutput, SliderButton, TempRect))
		Renderer->AddRectangle(&ButtonOutput);

	ZEUIControl::Draw(Renderer);
}

void ZEUIHorizontalSliderControl::SetValue(float NewValue)
{
	if(NewValue < MinimumValue)
		NewValue = MinimumValue;

	if(NewValue > MaximumValue)
		NewValue = MaximumValue;

	float ValueDifference = NewValue - Value;
	float MinMaxDifference = GetMaximumValue() - GetMinimumValue();
	float ValuePerPixel = MinMaxDifference / GetWidth();
	float ValuePixelPosition = NewValue / ValuePerPixel;
	float SliderPosX = GetScreenPosition().x - (SliderButton.Positions.GetWidth() / 2) + ValuePixelPosition;

	SliderButton.Positions.SetPosition(ZEVector2(SliderPosX, SliderButton.Positions.LeftUp.y));
	Value = NewValue;
}

float ZEUIHorizontalSliderControl::GetValue() const
{
	return Value;
}

void ZEUIHorizontalSliderControl::SetMaximumValue(float MaxValue)
{
	MaximumValue = MaxValue;
}

float ZEUIHorizontalSliderControl::GetMaximumValue() const
{
	return MaximumValue;
}

void ZEUIHorizontalSliderControl::SetMinimumValue(float MinValue)
{
	MinimumValue = MinValue;
}

float ZEUIHorizontalSliderControl::GetMinimumValue() const
{
	return MinimumValue;
}

void ZEUIHorizontalSliderControl::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);
	SliderLine.Positions.SetWidth(Width);
	SetValue(Value);
}

void ZEUIHorizontalSliderControl::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);
	SliderButton.Positions.SetHeight(Height);
	ZEVector2 ASD = ZEVector2(GetPosition().x, GetScreenPosition().y + (GetHeight() - SliderLine.Positions.GetHeight()) / 2);
	SliderLine.Positions.SetPosition(ASD);
}

void ZEUIHorizontalSliderControl::SetPosition(const ZEVector2& Position)
{
	ZEVector2 Displacement = Position - GetScreenPosition();
	SliderButton.Positions.SetPosition(SliderButton.Positions.GetPosition() + Displacement);
	SliderLine.Positions.SetPosition(SliderLine.Positions.GetPosition() + Displacement);
	ZEUIControl::SetPosition(Position);
}

ZEUIHorizontalSliderControl::ZEUIHorizontalSliderControl()
{
	SliderButton.Positions.SetPosition(ZEVector2::Zero);
	SliderButton.Positions.SetWidth(10);
	SliderButton.Positions.SetHeight(30);
	SliderButton.Positions.SetPosition(ZEVector2(0 - (SliderButton.Positions.GetWidth() / 2) , 0));
	SliderButton.Color = ZEUIManager::GetDefaultForegroundColor();

	SliderLine.Positions.SetPosition(ZEVector2::Zero);
	SliderLine.Positions.SetWidth(100);
	SliderLine.Positions.SetHeight(10);
	SliderLine.Color = ZEUIManager::GetDefaultBackgroundColor();

	SetMaximumValue(100);
	SetMinimumValue(0);

	Value = 0.0f;

	SetWidth(100);
	SetHeight(20);
	SetPosition(ZEVector2::Zero);

	IsButtonPressed = false;
}
