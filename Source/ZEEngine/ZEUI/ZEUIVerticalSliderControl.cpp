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
#include "ZEUIRenderer.h"

void ZEUIVerticalSliderControl::SetSliderValueByButton(ZEUIMouseKey Button, const ZEVector2& MoveAmount)
{
	float SliderLineHeight = SliderLine.Positions.RightDown.y - SliderLine.Positions.LeftUp.y - SliderButton.GetHeight();
	float Range = MaximumValue - MinimumValue;
	float NewValue = CurrentValue + ((Range / SliderLineHeight) * -MoveAmount.y);

	if (NewValue >= MaximumValue)
	{
		CurrentValue = MaximumValue;
		SliderButton.SetPosition(ZEVector2(SliderButton.GetPosition().x, SliderLine.Positions.LeftUp.y));
		((ZEFixedMaterial*)(SliderLine.Material))->SetAmbientColor(ZEVector3(CurrentValue / 100, CurrentValue / 100, CurrentValue / 100));
	}

	else if (NewValue <= MinimumValue)
	{
		CurrentValue = MinimumValue;
		SliderButton.SetPosition(ZEVector2(SliderButton.GetPosition().x, SliderLine.Positions.RightDown.y - SliderButton.GetHeight()));
		((ZEFixedMaterial*)(SliderLine.Material))->SetAmbientColor(ZEVector3(CurrentValue / 100, CurrentValue / 100, CurrentValue / 100));
	}

	else
	{
		ZEVector2 NewSliderButtonPosition = ZEVector2(SliderButton.GetPosition().x, SliderButton.GetPosition().y +  MoveAmount.y);
		SliderButton.SetPosition(NewSliderButtonPosition);
		((ZEFixedMaterial*)(SliderLine.Material))->SetAmbientColor(ZEVector3(NewValue / 100, NewValue / 100, NewValue / 100));
		CurrentValue = NewValue;
	}	
}

void ZEUIVerticalSliderControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	/*if (Button != ZE_UI_MOUSE_BUTTON_LEFT)
		return;

	if (MousePosition.y < SliderButton.GetPosition().y)
		SetCurretnValue(GetCurrentValue() + StepSize);

	else
		SetCurretnValue(GetCurrentValue() - StepSize);*/
}

void ZEUIVerticalSliderControl::SetCurretnValue(float NewValue)
{
	if (NewValue >= MaximumValue)
	{
		CurrentValue = MaximumValue;
		SliderButton.SetPosition(ZEVector2(SliderButton.GetPosition().x, SliderLine.Positions.LeftUp.y));
	}

	else if (NewValue <= MinimumValue)
	{
		CurrentValue = MinimumValue;
		SliderButton.SetPosition(ZEVector2(SliderButton.GetPosition().x, SliderLine.Positions.RightDown.y));
	}

	else
	{
		float Range = MaximumValue - MinimumValue;
		float Persentage = NewValue / Range;
		float Length = SliderLine.Positions.RightDown.y - SliderLine.Positions.LeftUp.y;
		float NewPositionY = Length * Persentage;
		SliderButton.SetPosition(ZEVector2(SliderButton.GetPosition().x, SliderButton.GetPosition().y + NewPositionY));
		CurrentValue = NewValue;
	}
}

void ZEUIVerticalSliderControl::Draw(ZEUIRenderer* Renderer)
{
	if (GetVisiblity() == false)
		return;	

	ZEUIRectangle Output;
	if(!ZEUIRectangle::Clip(Output, SliderLine, GetVisibleRectangle()))
		Renderer->AddRectangle(Output);

	ZEUIControl::Draw(Renderer);
}

void ZEUIVerticalSliderControl::SetPosition(float X, float Y)
{
	SetPosition(ZEVector2(X, Y));
}

void ZEUIVerticalSliderControl::SetPosition(const ZEVector2& Position)
{
	ZEUIControl::SetPosition(Position);
	SliderLine.Positions.LeftUp = ZEVector2(SliderButton.GetPosition().x - (SliderLineThickness / 2) + (SliderButton.GetWidth() / 2), SliderButton.GetPosition().y);
	SliderLine.Positions.RightDown = ZEVector2(SliderLine.Positions.LeftUp.x + SliderLineThickness, SliderButton.GetPosition().y + GetHeight() + SliderButton.GetHeight());
	
}

void ZEUIVerticalSliderControl::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);
	SliderLine.Positions.RightDown.y = SliderLine.Positions.LeftUp.y + Height;
}

ZEUIVerticalSliderControl::ZEUIVerticalSliderControl()
{
	SetWidth(20);
	SetHeight(100);

	SliderLineThickness = 4;

	CurrentValue = 100;

	SliderButton.SetPosition(this->GetPosition());
	SliderButton.SetHeight(10);
	SliderButton.SetWidth(20);

	SliderLine.Positions.LeftUp = ZEVector2(SliderButton.GetPosition().x - (SliderLineThickness / 2) + (SliderButton.GetWidth() / 2), SliderButton.GetPosition().y);
	SliderLine.Positions.RightDown = ZEVector2(SliderLine.Positions.LeftUp.x + SliderLineThickness, SliderButton.GetPosition().y + GetHeight() + SliderButton.GetHeight());

	SliderLineMaterial->SetAmbientEnabled(true);
	SliderLineMaterial->SetAmbientFactor(1.0f);
	SliderLineMaterial->SetAmbientColor(ZEVector3::UnitY);

	AddChildControl(&SliderButton);

	MinimumValue = 0;
	MaximumValue = 100;

	SliderButton.SetMouseMovedEvent(BindDelegate(this, &ZEUIVerticalSliderControl::SetSliderValueByButton));
}
