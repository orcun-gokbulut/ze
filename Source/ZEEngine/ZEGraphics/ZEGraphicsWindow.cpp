//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsWindow.cpp
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

#include "ZEGraphicsWindow.h"
#include "ZEGraphics/ZEGraphicsModule.h"

void ZEGraphicsWindow::OnCreate()
{
	zeLog("Window is being created.");
}

void ZEGraphicsWindow::OnDestroy()
{
	zeLog("Window is being destroyed.");
}

void ZEGraphicsWindow::OnFocusGain()
{
	zeLog("Window gained focus.");
}

void ZEGraphicsWindow::OnFocusLoose()
{
	zeLog("Window lost focus.");
}

void ZEGraphicsWindow::OnEnable()
{
	zeLog("Window enabled.");
}

void ZEGraphicsWindow::OnDisable()
{
	zeLog("Window disabled.");
}

void ZEGraphicsWindow::OnRestore()
{
	zeLog("Window restored.");
}

void ZEGraphicsWindow::OnMinimize()
{
	zeLog("Window minimized.");
}

void ZEGraphicsWindow::OnMaximize()
{
	zeLog("Window maximized.");
}

void ZEGraphicsWindow::OnWindowed()
{
	zeLog("Switching to windowed.");
}

void ZEGraphicsWindow::OnFullScreen()
{
	zeLog("Switching to full screen.");
}

void ZEGraphicsWindow::OnMove()
{
	zeLog("Window mooved.");
}

void ZEGraphicsWindow::OnSize()
{
	zeLog("Window resized.");
}

ZEUInt ZEGraphicsWindow::GetId() const
{
	return Id;
}

void* ZEGraphicsWindow::GetHandle() const
{
	return Handle;
}

const char* ZEGraphicsWindow::GetTitle() const
{
	return Title.ToCString();
}

const ZEWindowStyle& ZEGraphicsWindow::GetStyle() const
{
	return Style;
}

void ZEGraphicsWindow::GetPosition(ZEInt& X, ZEInt& Y) const
{
	X = PositionX;
	Y = PositionY;
}

void ZEGraphicsWindow::GetPosition(ZEVector2& Position) const
{
	Position.x = (float)PositionX;
	Position.y = (float)PositionY;
}

void ZEGraphicsWindow::GetSize(ZEInt& Width, ZEInt& Height) const
{
	Width = this->Width;
	Height = this->Height;
}

void ZEGraphicsWindow::GetSize(ZEVector2& Size) const
{
	Size.x = (float)Width;
	Size.y = (float)Height;
}

void ZEGraphicsWindow::GetRectangle(ZERectangle& Rectangle) const
{
	ZEVector2 Size;
	ZEVector2 Position;

	GetSize(Size);
	GetPosition(Position);

	Rectangle.LeftUp = Position;
	Rectangle.RightDown = Position + Size;
}

bool ZEGraphicsWindow::GetFullScreen() const
{
	return FullScreen;
}

bool ZEGraphicsWindow::GetVSynchEnabed() const
{
	return VSynchEnabed;
}

bool ZEGraphicsWindow::IsDisabled() const
{
	return !Enabled;
}

bool ZEGraphicsWindow::IsFocused() const
{
	return Focused;
}

bool ZEGraphicsWindow::IsRestored() const
{
	return !Maximized && !Minimized;
}

bool ZEGraphicsWindow::IsMaximized() const
{
	return Maximized;
}

bool ZEGraphicsWindow::IsMinimized() const
{
	return Minimized;
}

bool ZEGraphicsWindow::GetCursorLock() const
{
	return LastCursorLock == this;
}

bool ZEGraphicsWindow::IsInitialized() const
{
	return Handle != NULL;
}

void ZEGraphicsWindow::Destroy()
{
	delete this;
}

static ZEUInt NextWindowId = 0;
ZEUInt ZEGraphicsWindow::WindowCount = 0;
ZEGraphicsWindow* ZEGraphicsWindow::LastCursorLock = NULL;

ZEGraphicsWindow::ZEGraphicsWindow()
{
	Width = 800;
	Height = 600;
	PositionX = 100;
	PositionY = 100;

	Enabled = true;
	Focused = false;
	Minimized = false;
	Maximized = false;

	FullScreen = false;
	FullScreenMonitor = NULL;

	VSynchEnabed = false;

	Flags = 0;

	Handle = NULL;
	Id = NextWindowId++;
	Title = "Zinek Engine Window";
	
	memset(&Style, 0, sizeof(ZEWindowStyle));
	Style.Type = ZE_GWT_CAPTION;
	Style.Caption.OnTop = false;
	Style.Caption.Resizable = true;
	Style.Caption.Maximizable = true;
	Style.Caption.Minimizable = true;
}

ZEGraphicsWindow::~ZEGraphicsWindow()
{
	DeInitialize();
}

ZEUInt ZEGraphicsWindow::GetWindowCount()
{
	return WindowCount;
}

ZEGraphicsWindow* ZEGraphicsWindow::CreateInstance()
{
	return zeGraphics->CreateGraphicsWindow();
}
