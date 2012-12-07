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
#include "ZEUIRenderer.h"

void ZEUIHorizontalSliderControl::SetSliderValueByButton(ZEUIMouseKey Button, const ZEVector2& MoveAmount)
{
	float SliderLineWidth = SliderLine.Positions.RightDown.x - SliderLine.Positions.LeftUp.x - SliderButton.GetWidth();
	float Range = MaximumValue - MinimumValue;
	float NewValue = CurrentValue + ((Range / SliderLineWidth) * MoveAmount.x);

	if (NewValue >= MaximumValue)
	{
		CurrentValue = MaximumValue;
		SliderButton.SetPosition(ZEVector2(SliderLine.Positions.RightDown.x - SliderButton.GetWidth(), SliderButton.GetPosition().y));
		((ZEFixedMaterial*)(SliderLine.Material))->SetAmbientColor(ZEVector3(CurrentValue / 100, CurrentValue / 100, CurrentValue / 100));
	}

	else if (NewValue <= MinimumValue)
	{
		CurrentValue = MinimumValue;
		SliderButton.SetPosition(ZEVector2(SliderLine.Positions.LeftUp.x, SliderButton.GetPosition().y));
		((ZEFixedMaterial*)(SliderLine.Material))->SetAmbientColor(ZEVector3(CurrentValue / 100, CurrentValue / 100, CurrentValue / 100));
	}

	else
	{
		ZEVector2 NewSliderButtonPosition = ZEVector2(SliderButton.GetPosition().x +  MoveAmount.x, SliderButton.GetPosition().y);
		SliderButton.SetPosition(NewSliderButtonPosition);
		((ZEFixedMaterial*)(SliderLine.Material))->SetAmbientColor(ZEVector3(NewValue / 100, NewValue / 100, NewValue / 100));
		CurrentValue = NewValue;
	}	
}
void ZEUIHorizontalSliderControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	/*if (Button != ZE_UI_MOUSE_BUTTON_LEFT)
		return;

	if (MousePosition.x > SliderButton.GetPosition().x)
		SetCurretnValue(GetCurrentValue() + StepSize);

	else
		SetCurretnValue(GetCurrentValue() - StepSize);*/
}

void ZEUIHorizontalSliderControl::SetCurretnValue(float NewValue)
{
	if (NewValue >= MaximumValue)
	{
		CurrentValue = MaximumValue;
		SliderButton.SetPosition(ZEVector2(SliderLine.Positions.RightDown.x, SliderButton.GetPosition().y));
	}

	else if (NewValue <= MinimumValue)
	{
		CurrentValue = MinimumValue;
		SliderButton.SetPosition(ZEVector2(SliderLine.Positions.LeftUp.x, SliderButton.GetPosition().y));
	}

	else
	{
		float Range = MaximumValue - MinimumValue;
		float Persentage = NewValue / Range;
		float Length = SliderLine.Positions.RightDown.x - SliderLine.Positions.LeftUp.x;
		float NewPositionX = Length * Persentage;
		SliderButton.SetPosition(ZEVector2(SliderButton.GetPosition().x + NewPositionX, SliderButton.GetPosition().y));
		CurrentValue = NewValue;
	}
}

void ZEUIHorizontalSliderControl::Draw(ZEUIRenderer* Renderer)
{
	if (GetVisiblity() == false)
		return;	

	ZEUIRectangle Output;
	Output.ZOrder = GetZOrder();

	if(!ZEUIRectangle::Clip(Output, SliderLine, GetVisibleRectangle()))
		Renderer->AddRectangle(Output);

	ZEUIControl::Draw(Renderer);
}

void ZEUIHorizontalSliderControl::SetPosition(float X, float Y)
{
	SetPosition(ZEVector2(X, Y));
}

void ZEUIHorizontalSliderControl::SetPosition(const ZEVector2& Position)
{
	ZEUIControl::SetPosition(Position);
	SliderLine.Positions.LeftUp = ZEVector2(Position.x, SliderButton.GetPosition().y - (SliderLineThickness / 2) + (SliderButton.GetHeight() / 2));
	SliderLine.Positions.RightDown = ZEVector2(Position.x + GetWidth() + SliderButton.GetWidth(), SliderLine.Positions.LeftUp.y + SliderLineThickness);
}

void ZEUIHorizontalSliderControl::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);
	SliderLine.Positions.RightDown.x = SliderLine.Positions.LeftUp.x + Width;
}

ZEUIHorizontalSliderControl::ZEUIHorizontalSliderControl()
{
	SetWidth(100);
	SetHeight(20);

	SliderLineThickness = 4;

	CurrentValue = 0;
	SliderButton.SetPosition(this->GetPosition());
	SliderButton.SetHeight(20);
	SliderButton.SetWidth(10);

	SliderLine.Positions.LeftUp = ZEVector2(GetPosition().x , SliderButton.GetPosition().y - (SliderLineThickness / 2) + (SliderButton.GetHeight() / 2));
	SliderLine.Positions.RightDown = ZEVector2(GetPosition().x + GetWidth() + SliderButton.GetWidth(), SliderLine.Positions.LeftUp.y + SliderLineThickness);

	SliderLineMaterial->SetAmbientEnabled(true);
	SliderLineMaterial->SetAmbientFactor(1.0f);
	SliderLineMaterial->SetAmbientColor(ZEVector3::UnitY);

	AddChildControl(&SliderButton);
	SliderButton.SetVisiblity(true);

	MinimumValue = 0;
	MaximumValue = 100;

	SliderButton.SetMouseMovedEvent(BindDelegate(this, &ZEUIHorizontalSliderControl::SetSliderValueByButton));
}
