//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIFrameControl.cpp
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

#include "ZEUIFrameControl.h"
#include "ZEUIRenderer.h"
#include "ZEIUCheckBoxControl.h"
#include "ZEUIHorizontalSliderControl.h"

void ZEUIFrameControl::MouseMoveEvent(const ZEVector2& MoveAmount)
{
	ZEUIControl::MouseMoveEvent(MoveAmount);

	if (!GetMoveable())
		return;

	else
		SetPosition(GetPosition() + MoveAmount);
}

void ZEUIFrameControl::Draw(ZEUIRenderer* Renderer)
{
	if (GetVisiblity() == false)
		return;

	ZEUIRectangle Output;
	Output.ZOrder = GetZOrder();

	if(!ZEUIRectangle::Clip(Output, Frame, GetVisibleRectangle()))
		Renderer->AddRectangle(Output);

	ZEUIControl::Draw(Renderer);
}

void ZEUIFrameControl::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);
	Frame.Positions.LeftUp = GetRectangle().LeftUp;
	Frame.Positions.RightDown = GetRectangle().RightDown;
}

void ZEUIFrameControl::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);
	Frame.Positions.LeftUp = GetRectangle().LeftUp;
	Frame.Positions.RightDown = GetRectangle().RightDown;
}

void ZEUIFrameControl::SetPosition(const ZEVector2& Position)
{
	ZEUIControl::SetPosition(Position);
	Frame.Positions.LeftUp = GetRectangle().LeftUp;
	Frame.Positions.RightDown = GetRectangle().RightDown;
}

ZEMaterial* ZEUIFrameControl::GetMaterial() const
{
	return FrameMaterial;
}

void ZEUIFrameControl::SetMaterial(ZEMaterial* Material)
{
	FrameMaterial = (ZEUIMaterial*)Material;
	Frame.Material = FrameMaterial;
}

ZEUIFrameControl::ZEUIFrameControl()
{
	FrameMaterial = ZEUIMaterial::CreateInstance();
	Frame.Material = FrameMaterial;
	Frame.Color = GetBackgroundColor();
	Frame.Texcoords.LeftUp = ZEVector2::Zero;
	Frame.Texcoords.RightDown = ZEVector2::One;

	SetWidth(200);
	SetHeight(200);

	SetMoveable(true);
}
