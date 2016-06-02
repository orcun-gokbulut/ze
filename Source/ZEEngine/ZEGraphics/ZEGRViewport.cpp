//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRViewport.cpp
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

#include "ZEGRViewPort.h"

void ZEGRViewport::SetPosition(const ZEVector2& Position)
{
	StateData.Position = Position;
}

const ZEVector2& ZEGRViewport::GetPosition()
{
	return StateData.Position;
}

void ZEGRViewport::SetSize(const ZEVector2& Size)
{
	StateData.Size = Size;
}

const ZEVector2& ZEGRViewport::GetSize()
{
	return StateData.Size;
}

void ZEGRViewport::SetX(float x)
{
	StateData.Position.x = x;
}

float ZEGRViewport::GetX() const
{
	return StateData.Position.x;
}

void ZEGRViewport::SetY(float y)
{
	StateData.Position.y = y;
}

float ZEGRViewport::GetY() const
{
	return StateData.Position.y;
}

void ZEGRViewport::SetWidth(float Width)
{
	StateData.Size.x = Width;
}

float ZEGRViewport::GetWidth() const
{
	return StateData.Size.x;
}

void ZEGRViewport::SetHeight(float Height)
{
	StateData.Size.y = Height;
}

float ZEGRViewport::GetHeight() const
{
	return StateData.Size.y;
}

void ZEGRViewport::SetMinDepth(float Depth)
{
	StateData.MinDepth = Depth;
}

float ZEGRViewport::GetMinDepth() const
{
	return StateData.MinDepth;
}

void ZEGRViewport::SetMaxDepth(float Depth)
{
	StateData.MaxDepth = Depth;
}

float ZEGRViewport::GetMaxDepth() const
{
	return StateData.MaxDepth;
}

ZEGRViewport::ZEGRViewport(const ZEVector2& Position, const ZEVector2& Size, float MinDepth, float MaxDepth)
{
	StateData.Position = Position;
	StateData.Size = Size;
	StateData.MinDepth = MinDepth;
	StateData.MaxDepth = MaxDepth;
}

ZEGRViewport::ZEGRViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth)
{
	StateData.Position.x = X;
	StateData.Position.y = Y;
	StateData.Size.x = Width;
	StateData.Size.y = Height;
	StateData.MinDepth = MinDepth;
	StateData.MaxDepth = MaxDepth;
}

ZEGRViewport::ZEGRViewport()
{
	StateData.Position = ZEVector2::Zero;
	StateData.Size = ZEVector2::Zero;
	StateData.MinDepth = 0.0f;
	StateData.MaxDepth = 1.0f;
}
