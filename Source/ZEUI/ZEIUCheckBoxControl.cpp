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
#include "ZEFontResource.h"
#include "ZEGraphics/ZEFixedMaterial.h"

void ZEUICheckBoxControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if (State == ZE_UI_CBS_UNCHECKED)
	{
		State = ZEUICheckBoxState::ZE_UI_CBS_SEMICHECKED;
		Label.SetText("SemiChecked");
		((ZEFixedMaterial*)(Box.Button.Material))->SetAmbientColor(ZEVector3::UnitX);
	}
	
	else if (State == ZE_UI_CBS_SEMICHECKED)
	{	
		State = ZEUICheckBoxState::ZE_UI_CBS_CHECKED;
		Label.SetText("Checked");
		((ZEFixedMaterial*)(Box.Button.Material))->SetAmbientColor(ZEVector3::UnitY);
	}
	
	else if (State == ZE_UI_CBS_CHECKED)
	{	
		State = ZEUICheckBoxState::ZE_UI_CBS_UNCHECKED;
		Label.SetText("UnChecked");
		((ZEFixedMaterial*)(Box.Button.Material))->SetAmbientColor(ZEVector3::One);
	}
}

void ZEUICheckBoxControl::SetState(ZEUICheckBoxState State)
{
	this->State = State;
}

ZEUICheckBoxState ZEUICheckBoxControl::GetState() const
{
	return State;
}

void ZEUICheckBoxControl::SetText(ZEString Text)
{
	Label.SetText(Text);
}

ZEString ZEUICheckBoxControl::GetText()
{
	return Label.GetText();
}

void ZEUICheckBoxControl::SetPosition(const ZEVector2& Position)
{
	ZEUIControl::SetPosition(Position);
	Box.SetPosition(Position);
	Label.SetPosition(ZEVector2(GetPosition().x + Box.GetWidth(), GetPosition().y));
}

ZEUICheckBoxControl::ZEUICheckBoxControl()
{
	State = ZE_UI_CBS_UNCHECKED;
	Label.SetFont(ZEFontResource::LoadResource("OldEnglish.zefont"));
	Label.SetText(ZEString("Test"));
	
	SetHeight(25);
	SetWidth(25);

	Box.Button.Material = ZEFixedMaterial::CreateInstance();
	((ZEFixedMaterial*)(Box.Button.Material))->SetZero();
	((ZEFixedMaterial*)(Box.Button.Material))->SetAmbientEnabled(true);
	((ZEFixedMaterial*)(Box.Button.Material))->SetAmbientFactor(1.0f);
	((ZEFixedMaterial*)(Box.Button.Material))->SetAmbientColor(ZEVector3::One);
	((ZEFixedMaterial*)(Box.Button.Material))->UpdateMaterial();

	Box.Button.Positions.LeftUp = GetRectangle().LeftUp;

	Box.SetWidth(25);
	Box.SetHeight(25);
	Label.SetWidth(100);
	Label.SetHeight(25);
	AddChildControl(&Box);
	AddChildControl(&Label);

	Box.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUICheckBoxControl::MouseButtonPressed));
}

ZEUICheckBoxControl::~ZEUICheckBoxControl()
{

}
