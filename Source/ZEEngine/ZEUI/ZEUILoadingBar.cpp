//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUILoadingBar.cpp
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

#include "ZEUILoadingBar.h"
#include "ZETexture/ZETexture2DResource.h"

void ZEUILoadingBar::SetLoadingBarBar()
{
	Bar.Positions.SetWidth(Value * Frame.Positions.GetWidth() / 100.0f);

	if (ModeBar == ZE_UI_UVM_STRECH)
		Bar.Texcoords.RightDown = ZEVector2(1.0f, 1.0f);
	
	else
		Bar.Texcoords.RightDown = ZEVector2((float)Value, 1.0f);	
}

void ZEUILoadingBar::SetBarColor(ZEVector4 Color)
{
	Bar.Color = Color;
}

void ZEUILoadingBar::SetFrameColor(ZEVector4 Color)
{
	Frame.Color = Color;
}

void ZEUILoadingBar::SetBarTexture(ZEString FileName)
{
	BarMaterial->SetTexture(ZETexture2DResource::LoadResource(FileName)->GetTexture());
}

void ZEUILoadingBar::SetFrameTexture(ZEString FileName)
{
	FrameMaterial->SetTexture(ZETexture2DResource::LoadResource(FileName)->GetTexture());
}

void ZEUILoadingBar::SetValue(int Value)
{
	if (Value < 0)
		this->Value = 0;
	
	else if (Value > 100)
		this->Value = 100;

	else
		this->Value = Value;

	SetLoadingBarBar();
}

int ZEUILoadingBar::GetValue()
{
	return Value;
}

void ZEUILoadingBar::SetFrameUVMode(UVMode Mode)
{
	ModeFrame = Mode;

	if (Mode == ZE_UI_UVM_STRECH)
		Frame.Texcoords.RightDown = ZEVector2(1.0f, 1.0f);

	else
		Frame.Texcoords.RightDown = ZEVector2(100.0f, 1.0f);
}

UVMode ZEUILoadingBar::GetFrameUVMode()
{
	return ModeFrame;
}

void ZEUILoadingBar::SetBarUVMode(UVMode Mode)
{
	ModeBar = Mode;

	SetLoadingBarBar();
}

UVMode ZEUILoadingBar::GetBarUVMode()
{
	return ModeBar;
}

void ZEUILoadingBar::SetPosition(float X, float Y)
{
	SetPosition(ZEVector2(X, Y));
}

void ZEUILoadingBar::SetPosition(const ZEVector2& Position)
{
	ZEUIControl::SetPosition(Position);
	Bar.Positions.SetPosition(Position);
	Frame.Positions.SetPosition(Position);
}

void ZEUILoadingBar::SetSize(const ZEVector2& Size)
{
	SetWidth(Size.x);
	SetHeight(Size.y);
}

void ZEUILoadingBar::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);

	Bar.Positions.SetWidth(Width * Bar.Positions.GetWidth() / Frame.Positions.GetWidth());
	Frame.Positions.SetWidth(Width);
}

void ZEUILoadingBar::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);

	Bar.Positions.SetHeight(Height);
	Frame.Positions.SetHeight(Height);
}

void ZEUILoadingBar::Draw(ZEUIRenderer* Renderer)
{
	ZEUIControl::Draw(Renderer);

	ZEUIRectangle Output;
	if (GetParentControl() != NULL)
	{
		if (!ZEUIRectangle::Clip(Output, Frame, GetVisibleRectangle()))
		{
			Output.Material = FrameMaterial;
			Output.ZOrder = GetZOrder();
			Renderer->AddRectangle(Output);
		}
		if (!ZEUIRectangle::Clip(Output, Bar, GetVisibleRectangle()))
		{
			Output.Material = BarMaterial;
			Output.ZOrder = GetZOrder();
			Renderer->AddRectangle(Output);
		}
		return;
	}

	Renderer->AddRectangle(Bar);
	Renderer->AddRectangle(Frame);
}

void ZEUILoadingBar::Tick(float ElapsedTime)
{
	ZEUIControl::Tick(ElapsedTime);
}

ZEMaterial* ZEUILoadingBar::GetMaterial() const
{
	return NULL;
}

void ZEUILoadingBar::SetMaterial(ZEMaterial* Material)
{

}

ZEUILoadingBar::ZEUILoadingBar()
{
	Bar.Positions.SetPosition(ZEVector2(0.0f, 0.0f));
	Bar.Positions.SetHeight(0.0f);
	Bar.Positions.SetWidth(0.0f);
	Frame.Positions.SetPosition(ZEVector2(0.0f, 0.0f));
	Frame.Positions.SetHeight(20.0f);
	Frame.Positions.SetWidth(100.0f);

	BarMaterial = ZEUIMaterial::CreateInstance();
	FrameMaterial = ZEUIMaterial::CreateInstance();

// 	BarMaterial->SetTexture(ZETexture2DResource::LoadResource("Maximize.png")->GetTexture());
// 	FrameMaterial->SetTexture(ZETexture2DResource::LoadResource("Frame.png")->GetTexture());

	Bar.Material = BarMaterial;
	Frame.Material = FrameMaterial;

	Bar.Color = ZEVector4::One;
	Frame.Color = ZEVector4::One;

	Bar.Texcoords.LeftUp = ZEVector2::Zero;
	Frame.Texcoords.LeftUp = ZEVector2::Zero;

	SetSize(ZEVector2(1000, 20));

	SetValue(10);
	SetBarUVMode(ZE_UI_UVM_STRECH);
	SetFrameUVMode(ZE_UI_UVM_STRECH);
}
ZEUILoadingBar::~ZEUILoadingBar()
{

}
