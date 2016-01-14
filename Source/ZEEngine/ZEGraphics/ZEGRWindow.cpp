//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRWindow.cpp
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

#include "ZEGRWindow.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGROutput.h"

static ZEUInt NextWindowId = 0;
ZEUInt ZEGRWindow::WindowCount = 0;
ZEGRWindow* ZEGRWindow::LastCursorLock = NULL;

void ZEGRWindow::OnCreate()
{
	zeLog("Window is being created.");
}

void ZEGRWindow::OnDestroy()
{
	zeLog("Window is being destroyed.");
}

void ZEGRWindow::OnFocusGain()
{
	zeLog("Window gained focus.");
}

void ZEGRWindow::OnFocusLoose()
{
	zeLog("Window lost focus.");
}

void ZEGRWindow::OnEnabled()
{
	zeLog("Window enabled.");
}

void ZEGRWindow::OnDisabled()
{
	zeLog("Window disabled.");
}

void ZEGRWindow::OnRestore()
{
	zeLog("Window restored.");
}

void ZEGRWindow::OnMinimize()
{
	zeLog("Window minimized.");
}

void ZEGRWindow::OnMaximize()
{
	zeLog("Window maximized.");
}

void ZEGRWindow::OnWindowed()
{
	zeLog("Switching to windowed.");
}

void ZEGRWindow::OnFullScreen()
{
	zeLog("Switching to full screen.");
}

void ZEGRWindow::OnMove()
{
	zeLog("Window mooved.");
}

void ZEGRWindow::OnSize()
{
	zeLog("Window resized.");
}

ZEUInt ZEGRWindow::GetId() const
{
	return Id;
}

void* ZEGRWindow::GetHandle() const
{
	return Handle;
}

const ZEGRWindowStyle& ZEGRWindow::GetStyle() const
{
	return Style;
}

const ZEString& ZEGRWindow::GetTitle() const
{
	return Title;
}

ZEUInt ZEGRWindow::GetWidth() const
{
	return Width;
}

ZEUInt ZEGRWindow::GetHeight() const
{
	return Height;
}

ZEInt ZEGRWindow::GetPositionX() const
{
	return PositionX;
}

ZEInt ZEGRWindow::GetPositionY() const
{
	return PositionY;
}

void ZEGRWindow::GetPosition(ZEInt& X, ZEInt& Y) const
{
	X = PositionX;
	Y = PositionY;
}

void ZEGRWindow::GetPosition(ZEVector2& Position) const
{
	Position.x = (float)PositionX;
	Position.y = (float)PositionY;
}

void ZEGRWindow::GetSize(ZEInt& Width, ZEInt& Height) const
{
	Width = this->Width;
	Height = this->Height;
}

void ZEGRWindow::GetSize(ZEVector2& Size) const
{
	Size.x = (float)Width;
	Size.y = (float)Height;
}

ZERectangle ZEGRWindow::GetRectangle() const
{
	ZEVector2 Size;
	ZEVector2 Position;

	GetSize(Size);
	GetPosition(Position);

	return ZERectangle(Position, Position + Size);
}

bool ZEGRWindow::GetFullScreen() const
{
	return FullScreen;
}

bool ZEGRWindow::GetVSynchEnable() const
{
	return VSynchEnable;
}

ZEGRMonitor* ZEGRWindow::GetContainingMonitor() const
{
	return Output->GetMonitor();
}

ZEGROutput* ZEGRWindow::GetOutput() const
{
	return Output;
}

bool ZEGRWindow::GetEnable() const
{
	return Enabled;
}

bool ZEGRWindow::GetFocused() const
{
	return Focused;
}

bool ZEGRWindow::GetRestored() const
{
	return !Maximized && !Minimized;
}

bool ZEGRWindow::GetMaximized() const
{
	return Maximized;
}

bool ZEGRWindow::GetMinimized() const
{
	return Minimized;
}

bool ZEGRWindow::GetCursorLock() const
{
	return LastCursorLock == this;
}

ZEGRWindow::ZEGRWindow()
{
	Id = NextWindowId++;
	Handle = NULL;
	Title = "Zinek Engine";
	
	Width = 800;
	Height = 600;
	PositionX = 100;
	PositionY = 100;

	Enabled = true;
	Focused = false;
	Minimized = false;
	Maximized = false;

	FullScreen = false;
	VSynchEnable = false;
}

ZEGRWindow::~ZEGRWindow()
{
	DeinitializeSelf();
}

ZEUInt ZEGRWindow::GetWindowCount()
{
	return WindowCount;
}

ZEGRWindow* ZEGRWindow::CreateInstance()
{
	return new ZEGRWindow();
}
