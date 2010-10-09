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

void ZEUIHorizontalSliderControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{

}

void ZEUIHorizontalSliderControl::MouseButtonReleased(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{

}

void ZEUIHorizontalSliderControl::MouseMoveEvent(const ZEVector2& MoveAmount)
{
	ZEUIControl::MouseMoveEvent(MoveAmount);

	float SliderLineWidth = SliderLine.Positions.RightDown.x - SliderLine.Positions.LeftUp.x;
	float Range = MaximumValue - MinimumValue;
	float TotalChangeAmount = (Range / SliderLineWidth) * MoveAmount.x;
	float NewValue = CurrentValue + TotalChangeAmount;

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
		float ASD = SliderButton.GetPosition().x +  MoveAmount.x;
		ZEVector2 NewSliderButtonPosition = ZEVector2(SliderButton.GetPosition().x +  MoveAmount.x, SliderButton.GetPosition().y);
		SliderButton.SetPosition(NewSliderButtonPosition);
		CurrentValue = NewValue;
	}	
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

	//else
	//{
		 
//	}
}

void ZEUIHorizontalSliderControl::Draw(ZEUIRenderer* Renderer)
{
	ZEUIControl::Draw(Renderer);
	Renderer->AddRectangle(SliderLine);
}

void ZEUIHorizontalSliderControl::SetPosition(const ZEVector2& Position)
{
	ZEVector2 NewSliderButtonPosition = this->GetPosition() - SliderButton.GetPosition();
	NewSliderButtonPosition = Position + NewSliderButtonPosition;

	ZEVector2 NewSliderLineRightDown = ZEVector2(Position.x + 200, Position.y + 10);

	SliderButton.SetPosition(NewSliderButtonPosition);
	SliderLine.Positions.LeftUp = Position;
	SliderLine.Positions.RightDown = NewSliderLineRightDown;

	ZEUIControl::SetPosition(Position);
}

ZEUIHorizontalSliderControl::ZEUIHorizontalSliderControl()
{
	CurrentValue = 0;
	SliderButton.SetPosition(this->GetPosition());
	SliderLine.Positions.LeftUp = ZEVector2(this->GetPosition().x, SliderButton.GetPosition().y - SliderButton.GetHeight() / 2);
	SliderLine.Positions.RightDown = ZEVector2(200, 5);

	SliderLineMaterial->SetAmbientEnabled(true);
	SliderLineMaterial->SetAmbientFactor(1.0f);
	SliderLineMaterial->SetAmbientColor(ZEVector3::One);

	AddChildControl(&SliderButton);

	SliderButton.SetHeight(20);
	SliderButton.SetWidth(10);

	this->SetWidth(200);
	this->SetHeight(20);

	MinimumValue = 0;
	MaximumValue = 100;

	SliderButton.SetMouseMovedEvent(BindDelegate(this, &ZEUIHorizontalSliderControl::MouseMoveEvent));
}
