//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIButtonControl.cpp
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

#include "ZEUIButtonControl.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "zeui/ZEUIRenderer.h"
#include "ZETexture/ZETexture2DResource.h"

void ZEUIButtonControl::Draw(ZEUIRenderer* Renderer)
{
	if (!GetVisiblity())
		return;

	ZEUIControl::Draw(Renderer);
	ZEUIRectangle Output;
		
	if(!ZEUIRectangle::Clip(Output, Button, GetVisibleRectangle()))
		Renderer->AddRectangle(Output);

}

void ZEUIButtonControl::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);
	Button.Positions.LeftUp = GetScreenRectangle().LeftUp;
	Button.Positions.RightDown = GetScreenRectangle().RightDown;
}

void ZEUIButtonControl::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);
	Button.Positions.LeftUp = GetScreenRectangle().LeftUp;
	Button.Positions.RightDown = GetScreenRectangle().RightDown;
}

void ZEUIButtonControl::SetSize(const ZEVector2& Size)
{
	ZEUIControl::SetSize(Size);
	Button.Positions.LeftUp = GetScreenRectangle().LeftUp;
	Button.Positions.RightDown = GetScreenRectangle().RightDown;
}

void ZEUIButtonControl::SetPosition(float X, float Y)
{
	SetPosition(ZEVector2(X, Y));
}

void ZEUIButtonControl::SetPosition(const ZEVector2& Position)
{
	ZEUIControl::SetPosition(Position);
	Button.Positions.LeftUp = GetScreenRectangle().LeftUp;
	Button.Positions.RightDown = GetScreenRectangle().RightDown;
}

ZEMaterial* ZEUIButtonControl::GetMaterial() const
{
	return ButtonMaterial;
}

void ZEUIButtonControl::SetMaterial(ZEMaterial* Material)
{
	ButtonMaterial = (ZEUIMaterial*)Material;
}

ZEUIButtonControl::ZEUIButtonControl()
{
	Button.Texcoords = ZERectangle(ZEVector2::Zero, ZEVector2(50,50));
	ButtonMaterial = ZEUIMaterial::CreateInstance();
	ButtonMaterial->SetTexture(ZETexture2DResource::LoadResource("Button.jpg")->GetTexture());
	Button.Color = ZEVector4::One;

	Button.Material = ButtonMaterial;

	SetHeight(25);
	SetWidth(80);
	Button.Positions.LeftUp = GetScreenRectangle().LeftUp;
	Button.Positions.RightDown = GetScreenRectangle().RightDown;
	Button.Texcoords.LeftUp = ZEVector2::Zero;
	Button.Texcoords.RightDown = ZEVector2::One;
}

ZEUIButtonControl::~ZEUIButtonControl()
{

}
