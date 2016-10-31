//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDirectInputDevice.cpp
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
#include "ZECore/ZEConsole.h"
#include "ZEDS/ZEFormat.h"

#include "ZEDirectInputDevice.h"
#include "ZEDirectInputModule.h"
#include "ZEDS/ZEHashGenerator.h"

static BOOL FAR PASCAL ConfigureAxisRanges(LPCDIDEVICEOBJECTINSTANCE ObjectInstance, LPVOID Device)
{
	if( ObjectInstance->dwType & DIDFT_AXIS )
	{
		DIPROPRANGE Range;
		Range.diph.dwSize = sizeof(DIPROPRANGE);
		Range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		Range.diph.dwHow = DIPH_BYID;
		Range.diph.dwObj = ObjectInstance->dwType;
		Range.lMin = -1000;
		Range.lMax = +1000;

		if(((LPDIRECTINPUTDEVICE8)Device)->SetProperty(DIPROP_RANGE, &Range.diph) != DI_OK)
			return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}


ZEDirectInputDevice::ZEDirectInputDevice()
{
	Module = NULL;
	DirectInputDevice = NULL;
}

void ZEDirectInputDevice::Acquire()
{
	if (!IsInitialized())
		return;

	if (DirectInputDevice->Acquire() == DI_OK)
		ZEInputDevice::Acquire();
}

void ZEDirectInputDevice::UnAcquire()
{
	if (!IsInitialized())
		return;

	DirectInputDevice->Unacquire();
	ZEInputDevice::UnAcquire();
}

bool ZEDirectInputDevice::InitializeInternal()
{
	if (!ZEInputDevice::InitializeInternal())
		return false;

	const char* NamePrefix;
	if (DirectInputInstance.dwDevType == DI8DEVTYPE_KEYBOARD)
	{
		NamePrefix = "Mouse";
		Description.Type = ZE_IDT_KEYBOARD;
		Description.ButtonCount = 8;
		Description.AxisCount = 3;
		Description.Sink = true;
		Description.SinkName = "Keyboard";
		Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkName);
	}
	else if (DirectInputInstance.dwDevType == DI8DEVTYPE_MOUSE)
	{
		NamePrefix = "Mouse";
		Description.Type = ZE_IDT_MOUSE;
		Description.ButtonCount = 8;
		Description.AxisCount = 3;
		Description.Sink = true;
		Description.SinkName = "Mouse";
		Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkName);
	}
	if (DirectInputInstance.dwDevType == DI8DEVTYPE_JOYSTICK)
	{
		NamePrefix = "Joystick";
		Description.Type = ZE_IDT_JOYSTICK;
		Description.ButtonCount = 32;
		Description.AxisCount = 8;
		Description.POVCount = 4;
	}
	else if (DirectInputInstance.dwDevType == DI8DEVTYPE_GAMEPAD)
	{
		NamePrefix = "Gamepad";
		Description.Type = ZE_IDT_GAMEPAD;
		Description.ButtonCount = 32;
		Description.AxisCount = 8;
		Description.POVCount = 4;
	}
	else if (DirectInputInstance.dwDevType = DI8DEVTYPE_DRIVING)
	{
		NamePrefix = "Wheel";
		Description.Type = ZE_IDT_WHEEL;
		Description.ButtonCount = 32;
		Description.AxisCount = 8;
		Description.POVCount = 4;

	}
	else
		return false;
	
	Description.FullName = DirectInputInstance.tszProductName;

	HRESULT Result = Module->DirectInput->CreateDevice(DirectInputInstance.guidInstance, &DirectInputDevice, NULL);
	if (Result != DI_OK)
	{
		zeError("Can not initialize direct input device. Device Name: \"%s\".", Description.Name);
		Deinitialize();
		return false;
	}

	if (Description.Type == ZE_IDT_KEYBOARD)
	{
		Result = DirectInputDevice->SetCooperativeLevel(::GetDesktopWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
		DirectInputDevice->SetDataFormat(&c_dfDIKeyboard);
	}
	else if (Description.Type == ZE_IDT_MOUSE)
	{
		Result = DirectInputDevice->SetCooperativeLevel(::GetDesktopWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
		DirectInputDevice->SetDataFormat(&c_dfDIMouse2);
	}
	else if (Description.Type == ZE_IDT_GAMEPAD || Description.Type == ZE_IDT_JOYSTICK || Description.Type == ZE_IDT_WHEEL)
	{
		Result = DirectInputDevice->SetCooperativeLevel(::GetDesktopWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
		DirectInputDevice->EnumObjects(&ConfigureAxisRanges, DirectInputDevice, DIDFT_AXIS);
		DirectInputDevice->SetDataFormat(&c_dfDIJoystick);
	}
	
	if (Result != DI_OK)
	{
		zeError("Can not set devices cooperative level.");
		Deinitialize();
		return false;
	}

	State.Setup(Description);
	State.Reset();

	Description.Index = ZEInputDeviceIndexes::GetNewDeviceIndex(Description.Type);
	Description.Name = ZEFormat::Format("{0}{1:d:02}", NamePrefix, Description.Index);
	Description.NameHash = ZEHashGenerator::Hash(Description.Name);

	return true;
}

bool ZEDirectInputDevice::DeinitializeInternal()
{
	UnAcquire();

	if (DirectInputDevice != NULL)
	{
		DirectInputDevice->Release();
		DirectInputDevice = NULL;
	}

	return ZEInputDevice::DeinitializeInternal();
}

void ZEDirectInputDevice::Process()
{
	if (Description.Type == ZE_IDT_KEYBOARD)
	{
		char Buttons[256];
		HRESULT Result = DirectInputDevice->GetDeviceState(sizeof(char) * 256, Buttons);
		if (Result != DI_OK)
			return;

		State.Advance();
		for (ZESize I = 0; I < 256; I++)
			State.Buttons.CurrentValues[I] = (Buttons[I] & 0x80) == 0x80;
	}
	else if (Description.Type == ZE_IDT_MOUSE)
	{
		DIMOUSESTATE2 DirectInputState;
		HRESULT Result = DirectInputDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &DirectInputState);
		if (Result != DI_OK)
			return;
		
		State.Advance();
		State.Axises.CurrentValues[0] = DirectInputState.lX;
		State.Axises.CurrentValues[1] = DirectInputState.lY;
		State.Axises.CurrentValues[2] = DirectInputState.lZ;
		
		State.Buttons.CurrentValues[0] = (DirectInputState.rgbButtons[0] & 0x80) == 0x80;
		State.Buttons.CurrentValues[1] = (DirectInputState.rgbButtons[1] & 0x80) == 0x80;
		State.Buttons.CurrentValues[2] = (DirectInputState.rgbButtons[2] & 0x80) == 0x80;
		State.Buttons.CurrentValues[3] = (DirectInputState.rgbButtons[3] & 0x80) == 0x80;
		State.Buttons.CurrentValues[4] = (DirectInputState.rgbButtons[4] & 0x80) == 0x80;
		State.Buttons.CurrentValues[5] = (DirectInputState.rgbButtons[5] & 0x80) == 0x80;
		State.Buttons.CurrentValues[6] = (DirectInputState.rgbButtons[6] & 0x80) == 0x80;
		State.Buttons.CurrentValues[7] = (DirectInputState.rgbButtons[7] & 0x80) == 0x80;
	}
	else if (Description.Type == ZE_IDT_GAMEPAD || Description.Type == ZE_IDT_JOYSTICK || Description.Type == ZE_IDT_WHEEL)
	{
		DIJOYSTATE DirectInputState;
		HRESULT Result = DirectInputDevice->GetDeviceState(sizeof(DIJOYSTATE), &DirectInputState);
		if (Result != DI_OK)
			return;

		State.Advance();
		State.Axises.CurrentValues[0] = (float)DirectInputState.lX / 1000.0f;
		State.Axises.CurrentValues[1] = (float)DirectInputState.lY / 1000.0f;
		State.Axises.CurrentValues[2] = (float)DirectInputState.lZ / 1000.0f;
		State.Axises.CurrentValues[3] = (float)DirectInputState.lRx / 1000.0f;
		State.Axises.CurrentValues[4] = (float)DirectInputState.lRy / 1000.0f;
		State.Axises.CurrentValues[5] = (float)DirectInputState.lRz / 1000.0f;
		State.Axises.CurrentValues[6] = (float)DirectInputState.rglSlider[0] / -2000.0f + 0.5f; 
		State.Axises.CurrentValues[7] = (float)DirectInputState.rglSlider[1] / -2000.0f + 0.5f; 
		
		State.POVs.CurrentValues[0] = (DirectInputState.rgdwPOV[0]) == 0x80;
		State.POVs.CurrentValues[1] = (DirectInputState.rgdwPOV[1]) == 0x80;
		State.POVs.CurrentValues[2] = (DirectInputState.rgdwPOV[2]) == 0x80;
		State.POVs.CurrentValues[3] = (DirectInputState.rgdwPOV[3]) == 0x80;

		State.Buttons.CurrentValues[0] = (DirectInputState.rgbButtons[0] & 0x80) == 0x80;
		State.Buttons.CurrentValues[1] = (DirectInputState.rgbButtons[1] & 0x80) == 0x80;
		State.Buttons.CurrentValues[2] = (DirectInputState.rgbButtons[2] & 0x80) == 0x80;
		State.Buttons.CurrentValues[3] = (DirectInputState.rgbButtons[3] & 0x80) == 0x80;
		State.Buttons.CurrentValues[4] = (DirectInputState.rgbButtons[4] & 0x80) == 0x80;
		State.Buttons.CurrentValues[5] = (DirectInputState.rgbButtons[5] & 0x80) == 0x80;
		State.Buttons.CurrentValues[6] = (DirectInputState.rgbButtons[6] & 0x80) == 0x80;
		State.Buttons.CurrentValues[7] = (DirectInputState.rgbButtons[7] & 0x80) == 0x80;
		State.Buttons.CurrentValues[8] = (DirectInputState.rgbButtons[8] & 0x80) == 0x80;
		State.Buttons.CurrentValues[9] = (DirectInputState.rgbButtons[9] & 0x80) == 0x80;
		State.Buttons.CurrentValues[10] = (DirectInputState.rgbButtons[10] & 0x80) == 0x80;
		State.Buttons.CurrentValues[11] = (DirectInputState.rgbButtons[11] & 0x80) == 0x80;
		State.Buttons.CurrentValues[12] = (DirectInputState.rgbButtons[12] & 0x80) == 0x80;
		State.Buttons.CurrentValues[13] = (DirectInputState.rgbButtons[13] & 0x80) == 0x80;
		State.Buttons.CurrentValues[14] = (DirectInputState.rgbButtons[14] & 0x80) == 0x80;
		State.Buttons.CurrentValues[15] = (DirectInputState.rgbButtons[15] & 0x80) == 0x80;
		State.Buttons.CurrentValues[16] = (DirectInputState.rgbButtons[16] & 0x80) == 0x80;
		State.Buttons.CurrentValues[17] = (DirectInputState.rgbButtons[17] & 0x80) == 0x80;
		State.Buttons.CurrentValues[18] = (DirectInputState.rgbButtons[18] & 0x80) == 0x80;
		State.Buttons.CurrentValues[19] = (DirectInputState.rgbButtons[19] & 0x80) == 0x80;
		State.Buttons.CurrentValues[20] = (DirectInputState.rgbButtons[20] & 0x80) == 0x80;
		State.Buttons.CurrentValues[21] = (DirectInputState.rgbButtons[21] & 0x80) == 0x80;
		State.Buttons.CurrentValues[22] = (DirectInputState.rgbButtons[22] & 0x80) == 0x80;
		State.Buttons.CurrentValues[23] = (DirectInputState.rgbButtons[23] & 0x80) == 0x80;
		State.Buttons.CurrentValues[24] = (DirectInputState.rgbButtons[24] & 0x80) == 0x80;
		State.Buttons.CurrentValues[25] = (DirectInputState.rgbButtons[25] & 0x80) == 0x80;
		State.Buttons.CurrentValues[26] = (DirectInputState.rgbButtons[26] & 0x80) == 0x80;
		State.Buttons.CurrentValues[27] = (DirectInputState.rgbButtons[27] & 0x80) == 0x80;
		State.Buttons.CurrentValues[28] = (DirectInputState.rgbButtons[28] & 0x80) == 0x80;
		State.Buttons.CurrentValues[29] = (DirectInputState.rgbButtons[29] & 0x80) == 0x80;
		State.Buttons.CurrentValues[30] = (DirectInputState.rgbButtons[30] & 0x80) == 0x80;
		State.Buttons.CurrentValues[31] = (DirectInputState.rgbButtons[31] & 0x80) == 0x80;
	}
}

