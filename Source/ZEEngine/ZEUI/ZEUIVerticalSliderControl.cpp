//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIVerticalSliderControl.cpp
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

#include "ZEUIVerticalSliderControl.h"
#include "ZEGraphics\ZEUIMaterial.h"

void ZEUIVerticalSliderControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if(Button == ZE_UI_MOUSE_BUTTON_LEFT && ZERectangle::IntersectionTest(SliderButton.Positions, MousePosition) == true)
		IsButtonPressed = true;

	ZEUIControl::MouseButtonPressed(Button, MousePosition);
}

void ZEUIVerticalSliderControl::MouseButtonReleased(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if(Button == ZE_UI_MOUSE_BUTTON_LEFT)
		IsButtonPressed = false;

	ZEUIControl::MouseButtonReleased(Button, MousePosition);
}

void ZEUIVerticalSliderControl::MouseMoveEvent(ZEUIMouseKey Button, const ZEVector2& MoveAmount)
{
	if(IsButtonPressed && (ZEInt32)MoveAmount.y != 0)
	{
		float MinMaxDifference = GetMaximumValue() - GetMinimumValue();
		float ValuePerPixel = MinMaxDifference / GetHeight();
		float ValueChangeAmount = ValuePerPixel / -MoveAmount.y;
		SetValue(Value + ValueChangeAmount);
	}

	ZEUIControl::MouseMoveEvent(Button, MoveAmount);
}

void ZEUIVerticalSliderControl::Draw(ZEUIRenderer* Renderer)
{
	ZEUIRectangle ButtonOutput, LineOutput;
	SliderLine.ZOrder = GetZOrder() + 1;
	SliderButton.ZOrder = GetZOrder() + 2;

	ZERectangle TempRect = GetVisibleRectangle();
	TempRect.LeftUp.y -= SliderButton.Positions.GetHeight() / 2;
	TempRect.RightDown.y += SliderButton.Positions.GetHeight() / 2;

	if(!ZEUIRectangle::Clip(LineOutput, SliderLine, TempRect))
		Renderer->AddRectangle(LineOutput);

	if(!ZEUIRectangle::Clip(ButtonOutput, SliderButton, TempRect))
		Renderer->AddRectangle(ButtonOutput);

	ZEUIControl::Draw(Renderer);
}

void ZEUIVerticalSliderControl::SetValue(float NewValue)
{
	if(NewValue < MinimumValue)
		NewValue = MinimumValue;

	if(NewValue > MaximumValue)
		NewValue = MaximumValue;

	float ValueDifference = NewValue - Value;
	float MinMaxDifference = GetMaximumValue() - GetMinimumValue();
	float ValuePerPixel = MinMaxDifference / GetHeight();
	float ValuePixelPosition = GetHeight() -  (NewValue / ValuePerPixel);
	float SliderPosY = GetScreenPosition().y - (SliderButton.Positions.GetHeight() / 2) + ValuePixelPosition;
	SliderButton.Positions.SetPosition(ZEVector2(SliderButton.Positions.LeftUp.x, SliderPosY));
	Value = NewValue;
}

float ZEUIVerticalSliderControl::GetValue() const
{
	return Value;
}

void ZEUIVerticalSliderControl::SetStepsize(float StepSize)
{
	this->Stepsize = StepSize;
}

float ZEUIVerticalSliderControl::GetStepsize() const
{
	return Stepsize;
}

void ZEUIVerticalSliderControl::SetMaximumValue(float MaxValue)
{
	MaximumValue = MaxValue;
}

float ZEUIVerticalSliderControl::GetMaximumValue() const
{
	return MaximumValue;
}

void ZEUIVerticalSliderControl::SetMinimumValue(float MinValue)
{
	MinimumValue = MinValue;
}

float ZEUIVerticalSliderControl::GetMinimumValue() const
{
	return MinimumValue;
}

void ZEUIVerticalSliderControl::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);
	SliderButton.Positions.SetWidth(Width);
	SliderLine.Positions.SetPosition(ZEVector2(GetScreenPosition().x + (GetWidth() - SliderLine.Positions.GetWidth()) / 2, GetPosition().y));
}

void ZEUIVerticalSliderControl::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);
	SliderLine.Positions.SetHeight(Height);
	SetValue(Value);
}

void ZEUIVerticalSliderControl::SetPosition(const ZEVector2& Position)
{
	ZEVector2 Displacement = Position - GetScreenPosition();
	SliderButton.Positions.SetPosition(SliderButton.Positions.GetPosition() + Displacement);
	SliderLine.Positions.SetPosition(SliderLine.Positions.GetPosition() + Displacement);
	ZEUIControl::SetPosition(Position);
}

ZEUIVerticalSliderControl::ZEUIVerticalSliderControl()
{
	SliderButtonMaterial = ZEUIMaterial::CreateInstance();
	SliderLineMaterial = ZEUIMaterial::CreateInstance();

	SliderButton.Positions.SetPosition(ZEVector2::Zero);
	SliderButton.Positions.SetWidth(30);
	SliderButton.Positions.SetHeight(10);
	SliderButton.Positions.SetPosition(ZEVector2(0, 0 - (SliderButton.Positions.GetHeight() / 2)));
	SliderButton.Color = ZEUIManager::GetDefaultForegroundColor();
	SliderButton.Material = SliderButtonMaterial;

	SliderLine.Positions.SetPosition(ZEVector2::Zero);
	SliderLine.Positions.SetWidth(10);
	SliderLine.Positions.SetHeight(100);
	SliderLine.Color = ZEUIManager::GetDefaultBackgroundColor();
	SliderLine.Material = SliderLineMaterial;

	SetMaximumValue(100);
	SetMinimumValue(0);

	Value = 0.0f;

	SetWidth(20);
	SetHeight(100);
	SetPosition(ZEVector2::Zero);

	IsButtonPressed = false;
}
