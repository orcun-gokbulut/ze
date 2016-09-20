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
#include "ZECore/ZECore.h"
#include "ZEInput/ZEInputModule.h"

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

	if (ManageInputAcquisition)
	{
		ZEInputModule* InputModule = ZECore::GetInstance()->GetInputModule();
		if (InputModule != NULL)
			InputModule->Acquire();
	}
}

void ZEGRWindow::OnFocusLoose()
{
	zeLog("Window lost focus.");
	
	if (ManageInputAcquisition)
	{
		ZEInputModule* InputModule = ZECore::GetInstance()->GetInputModule();
		if (InputModule != NULL)
			InputModule->UnAcquire();
	}
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
	zeLog("Window moved.");
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

void ZEGRWindow::SetLeft(ZEUInt Left)
{
	SetPosition(Left, Top);
}

ZEInt ZEGRWindow::GetLeft() const
{
	return Left;
}

void ZEGRWindow::SetTop(ZEUInt Top)
{
	SetPosition(Left, Top);
}

ZEInt ZEGRWindow::GetTop() const
{
	return Top;
}

void ZEGRWindow::SetWidth(ZEUInt Width)
{
	SetSize(Width, Height);
}

ZEUInt ZEGRWindow::GetWidth() const
{
	return Width;
}

void ZEGRWindow::SetHeight(ZEUInt Height)
{
	SetSize(Width, Height);
}

ZEUInt ZEGRWindow::GetHeight() const
{
	return Height;
}

const ZEString& ZEGRWindow::GetTitle() const
{
	return Title;
}

void ZEGRWindow::SetTitleBar(bool Enabled)
{
	TitleBar = Enabled;
	UpdateStyle();
}

bool ZEGRWindow::GetTitleBar() const
{
	return TitleBar;
}

void ZEGRWindow::SetIconVisible(bool Visible)
{
	IconVisible = Visible;
	UpdateStyle();
}

bool ZEGRWindow::GetIconVisible() const
{
	return IconVisible;
}

void ZEGRWindow::SetMinimizeButton(bool Enabled)
{
	MinimizeButton = Enabled;
	UpdateStyle();
}

bool ZEGRWindow::GetMinimizeButton() const
{
	return MinimizeButton;
}

void ZEGRWindow::SetMaximizeButton(bool Enabled)
{
	MaximizeButton = Enabled;
	UpdateStyle();
}

bool ZEGRWindow::GetMaximizeButton() const
{
	return MaximizeButton;
}

void ZEGRWindow::SetCloseButton(bool Enabled)
{
	CloseButton = Enabled;
	UpdateStyle();
}

bool ZEGRWindow::GetCloseButton() const
{
	return CloseButton;
}

void ZEGRWindow::SetResizable(bool Enabled)
{
	Resizable = Enabled;
	UpdateStyle();
}

bool ZEGRWindow::GetResizable() const
{
	return Resizable;
}

void ZEGRWindow::SetBordered(bool Enabled)
{
	Bordered = Enabled;
	UpdateStyle();
}

bool ZEGRWindow::GetBordered() const
{
	return Bordered;
}

void ZEGRWindow::SetShowInTaskbar(bool Enabled)
{
	ShowInTaskbar = Enabled;
	UpdateStyle();
}

bool ZEGRWindow::GetShowInTaskbar() const
{
	return ShowInTaskbar;
}

void ZEGRWindow::SetAlwaysOnTop(bool Enabled)
{
	AlwaysOnTop = Enabled;
	UpdateStyle();
}

bool ZEGRWindow::GetAlwaysOnTop() const
{
	return AlwaysOnTop;
}

void ZEGRWindow::SetQuitWhenClosed(bool Enabled)
{
	QuitWhenClosed = Enabled;
	UpdateStyle();
}

bool ZEGRWindow::GetQuitWhenClosed() const
{
	return QuitWhenClosed;
}

void ZEGRWindow::SetValidateQuit(bool Enabled)
{
	ValidateQuit = Enabled;
}

bool ZEGRWindow::GetValidateQuit() const
{
	return ValidateQuit;
}

bool ZEGRWindow::GetFullScreen() const
{
	return FullScreen;
}

void ZEGRWindow::SetCursorVisible(bool Visible)
{
	CursorVisible = Visible;
}

bool ZEGRWindow::GetCursorVisible() const
{
	return CursorVisible;
}

bool ZEGRWindow::GetCursorLocked() const
{
	return LastCursorLock == this;
}

void ZEGRWindow::SetManageInputAcquisition(bool Enabled)
{
	if (ManageInputAcquisition == Enabled)
		return;

	ManageInputAcquisition = Enabled;

	if (!ManageInputAcquisition)
		return;

	ZEInputModule* InputModule = ZECore::GetInstance()->GetInputModule();
	if (InputModule == NULL)
		return;

	if (Focused)
		InputModule->Acquire();
	else
		InputModule->UnAcquire();
}

bool ZEGRWindow::GetManageInputAcquisition() const
{
	return ManageInputAcquisition;
}

bool ZEGRWindow::GetVSynchEnabled() const
{
	return VSynchEnabled;
}

ZEGRMonitor* ZEGRWindow::GetContainingMonitor() const
{
	return Output->GetMonitor();
}

ZEGROutput* ZEGRWindow::GetOutput() const
{
	return Output;
}

bool ZEGRWindow::GetVisible() const
{
	return Visible;
}

bool ZEGRWindow::GetEnabled() const
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

ZEGRWindow::ZEGRWindow()
{
	Id = NextWindowId++;
	Handle = NULL;
	QuitWhenClosed = true;
	Title = "Zinek Engine";
	
	Width = 800;
	Height = 600;
	Left = 100;
	Top = 100;

	TitleBar = true;
	IconVisible = true;
	MinimizeButton = true;
	MaximizeButton = true;
	Resizable = true;
	Bordered = true;
	ShowInTaskbar = true;
	AlwaysOnTop = false;
	QuitWhenClosed = true;
	ValidateQuit = true;
	CursorVisible = false;
	CursorLocked = false;
	ManageInputAcquisition = true;

	Enabled = true;
	Focused = false;
	Minimized = false;
	Maximized = false;

	FullScreen = false;
	VSynchEnabled = false;
}

ZEGRWindow::~ZEGRWindow()
{

}

void ZEGRWindow::WrapperResized(ZEUInt Width, ZEUInt Height)
{
	this->Width = Width;
	this->Height = Height;
	Output->Resize(Width, Height);
}

ZEUInt ZEGRWindow::GetWindowCount()
{
	return WindowCount;
}

ZEGRWindow* ZEGRWindow::CreateInstance()
{
	return new ZEGRWindow();
}
