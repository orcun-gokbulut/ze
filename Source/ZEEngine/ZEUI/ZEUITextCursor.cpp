//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUITextCursor.cpp
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

#include "ZEUITextCursor.h"
#include "ZEGraphics/ZEUIMaterial.h"

ZEUITextCursor::ZEUITextCursor()
{
	IsVisible = false;
	BlinkTime = 0.5f;
	Width = 2;
	Height = 20;
	Positions.LeftUp = ZEVector2::Zero;

	Timer = ZETimer::CreateInstance();
	SetBlinkTime(BlinkTime);
	Timer->SetRepeating(true);
	Timer->SetTimerEvent(ZEDelegate<void (float)>::Create<ZEUITextCursor, &ZEUITextCursor::Blink>(this));
	Timer->Start();

	SetWidth(Width);
	SetHeight(Height);

	Material = ZEUIMaterial::CreateInstance();
	Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
}

ZEUITextCursor::~ZEUITextCursor()
{
	Timer->Stop();
	Timer->Destroy();
	Timer = NULL;
	((ZEUIMaterial*)Material)->Destroy();
}

void ZEUITextCursor::Blink(float Time)
{
	if(IsVisible)
		IsVisible = false;
	else
		IsVisible = true;
}

void ZEUITextCursor::SetBlinkTime(float Time)
{
	BlinkTime = Time;
	Timer->SetIntervalTime(Time);
}

float ZEUITextCursor::GetBlinkTime()
{
	return BlinkTime;
}

bool ZEUITextCursor::GetVisible() const
{
	return IsVisible;
}

void ZEUITextCursor::Update()
{
	Positions.RightDown = Positions.LeftUp + ZEVector2(Width, Height);
}

void ZEUITextCursor::SetHeight(ZEInt32 Height)
{
	Positions.RightDown.y += Height;
	Update();
}

void ZEUITextCursor::SetWidth(ZEInt32 Width)
{
	Positions.RightDown.x += Width;
	Update();
}

void ZEUITextCursor::SetSize(const ZEVector2& Size)
{
	SetWidth(Size.x);
	SetHeight(Size.y);
}

void ZEUITextCursor::SetPostion(const ZEVector2& Position)
{
	Positions.LeftUp = Position;
	Update();
}

const ZEVector2& ZEUITextCursor::GetPosition() const
{
	return Positions.LeftUp;
}

const ZEMaterial* ZEUITextCursor::GetMaterial() const
{
	return Material;
}
