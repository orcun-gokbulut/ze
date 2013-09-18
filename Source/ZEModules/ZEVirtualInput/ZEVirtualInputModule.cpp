//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVirtualInputModule.cpp
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

#include "ZEError.h"
#include "ZEVirtualInputModule.h"
#include "ZEVirtualInputMouseDevice.h"
#include "ZEVirtualInputKeyboardDevice.h"

ZE_EXTENSION_DESCRIPTION(ZEVirtualInputModule, ZEInputDeviceModule, NULL)

ZEVirtualInputModule* ZEVirtualInputModule::Instance = NULL;

bool ZEVirtualInputModule::InitializeSelf()
{	
	zeLog("Initializing Virtual Input.");

	if (!ZEInputDeviceModule::InitializeSelf())
		return false;

	if (MouseDevice == NULL)
	{
		MouseDevice = new ZEVirtualInputMouseDevice();
		RegisterDevice(MouseDevice);
	}
	
	if (KeyboardDevice == NULL)
	{
		KeyboardDevice = new ZEVirtualInputKeyboardDevice();
		RegisterDevice(KeyboardDevice);
	}

	return true;
}

bool ZEVirtualInputModule::DeinitializeSelf()
{
	KeyboardDevice->Deinitialize();
	MouseDevice->Deinitialize();

	return ZEInputDeviceModule::DeinitializeSelf();
}

void ZEVirtualInputModule::MoveMouse(ZEInt X, ZEInt Y, ZEInt Z)
{
	MouseDevice->State.Axises.CurrentValues[0] = (float)X;
	MouseDevice->State.Axises.CurrentValues[1] = (float)Y;
	MouseDevice->State.Axises.CurrentValues[2] = (float)Z;
}

void ZEVirtualInputModule::SetKeyboardKeyState(ZEUInt Index, bool Pressed)
{
	if (Index >= KeyboardDevice->Description.ButtonCount)
		return;

	KeyboardDevice->State.Buttons.CurrentValues[Index] = Pressed;
}

bool ZEVirtualInputModule::GetKeyboardKeyState(ZEUInt Index)
{
	if (Index >= KeyboardDevice->Description.ButtonCount)
		return false;

	return KeyboardDevice->State.Buttons.CurrentValues[Index];
}

void ZEVirtualInputModule::SetMouseKeyState(ZEUInt Index, bool Pressed)
{
	if (Index >= MouseDevice->Description.ButtonCount)
		return;

	MouseDevice->State.Buttons.CurrentValues[Index] = Pressed;
}

bool ZEVirtualInputModule::GetMouseKeyState(ZEUInt Index)
{
	if (Index >= MouseDevice->Description.ButtonCount)
		return false;

	return MouseDevice->State.Buttons.CurrentValues[Index];
}

void ZEVirtualInputModule::Process()
{
	KeyboardDevice->State.Advance();
	MouseDevice->State.Advance();
}

ZEVirtualInputModule::ZEVirtualInputModule()
{
	Instance = this;
	MouseDevice = NULL;
	KeyboardDevice = NULL;
}

ZEVirtualInputModule* ZEVirtualInputModule::GetInstance()
{
	return Instance;
}
