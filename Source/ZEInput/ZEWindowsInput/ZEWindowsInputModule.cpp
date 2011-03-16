//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWindowsInputModule.cpp
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

#include "ZEWindowsInputModule.h"
#include "ZEWindowsInputModuleDescription.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEError.h"

#define WINDIWS_LEAN_AND_MEAN
#include <windows.h>

enum ZEWIKeyState
{
	ZE_WIKS_NONE = 0,
	ZE_WIKS_PRESSED = 1,
	ZE_WIKS_PRESSING = 3,
	ZE_WIKS_RELEASED = 2,
};

struct 
{
	LONG Axis[3];
	ZEWIKeyState Buttons[8];
} MouseState;

struct
{
	ZEWIKeyState Buttons[256];
	unsigned int LastFrameId[256];
} KeyboardState;


// HACK HACK HACK HACK HACK HACK
// Will be called from ZEWindows.cpp  Callback function. 
const char* GetStatus(ZEWIKeyState KeyState)
{
	switch(KeyState)
	{
		case ZE_WIKS_NONE:
			return "None";
		case ZE_WIKS_PRESSED:
			return "Pressed";
		case ZE_WIKS_PRESSING:
			return "Still Pressing";
		case ZE_WIKS_RELEASED:
			return "Released";
		default:
			return "Error";
	}
}

void OnWMRawInputRecived(HRAWINPUT Handle)
{
	BYTE Buffer[1024];
	UINT BufferSize = 1024;
	memset(Buffer, 0xfd, sizeof(Buffer));

	if (GetRawInputData(Handle, RID_INPUT, Buffer, &BufferSize, sizeof(RAWINPUTHEADER)) == (UINT)-1)
	{
		zeError("WindowsInput", "Can not read raw input data.");
		return;
	}
	
	RAWINPUT* Input = (RAWINPUT*)Buffer;
	if (Input->header.dwType == RIM_TYPEKEYBOARD)
	{
		if ((Input->data.keyboard.Flags & 0x01) == RI_KEY_MAKE)
		{
			if (KeyboardState.Buttons[Input->data.keyboard.MakeCode] == ZE_WIKS_NONE)
				KeyboardState.Buttons[Input->data.keyboard.MakeCode] = ZE_WIKS_PRESSED;
		}
		else if ((Input->data.keyboard.Flags & 0x01) == RI_KEY_BREAK)
			KeyboardState.Buttons[Input->data.keyboard.MakeCode] = ZE_WIKS_RELEASED;

	}
	else if (Input->header.dwType ==  RIM_TYPEMOUSE)
	{
		MouseState.Axis[0] += Input->data.mouse.lLastX;
		MouseState.Axis[1] += Input->data.mouse.lLastY;

		if (Input->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
		{
			if (MouseState.Buttons[0] == ZE_WIKS_NONE)
				MouseState.Buttons[0] = ZE_WIKS_PRESSED;
		}
		else if (Input->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
			MouseState.Buttons[0] = ZE_WIKS_RELEASED;

		if (Input->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		{
			if (MouseState.Buttons[1] == ZE_WIKS_NONE)
				MouseState.Buttons[1] = ZE_WIKS_PRESSED;
		}
		else if (Input->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
			MouseState.Buttons[1] = ZE_WIKS_RELEASED;

		if (Input->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		{
			if (MouseState.Buttons[2] == ZE_WIKS_NONE)
				MouseState.Buttons[2] = ZE_WIKS_PRESSED;
		}
		else if (Input->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
			MouseState.Buttons[2] = ZE_WIKS_RELEASED;

		if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN)
		{
			if (MouseState.Buttons[3] == ZE_WIKS_NONE)
				MouseState.Buttons[3] = ZE_WIKS_PRESSED;
		}
		else if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP)
			MouseState.Buttons[3] = ZE_WIKS_RELEASED;

		if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN)
		{
			if (MouseState.Buttons[4] == ZE_WIKS_NONE)
				MouseState.Buttons[4] = ZE_WIKS_PRESSED;
		}
		else if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP)
			MouseState.Buttons[4] = ZE_WIKS_RELEASED;

		if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN)
		{
			if (MouseState.Buttons[5] == ZE_WIKS_NONE)
				MouseState.Buttons[5] = ZE_WIKS_PRESSED;
		}
		else if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP)
			MouseState.Buttons[5] = ZE_WIKS_RELEASED;

		if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
		{
			if (MouseState.Buttons[6] == ZE_WIKS_NONE)
				MouseState.Buttons[6] = ZE_WIKS_PRESSED;
		}
		else if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
			MouseState.Buttons[6] = ZE_WIKS_RELEASED;

		if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) 
		{
			if (MouseState.Buttons[7] == ZE_WIKS_NONE)
				MouseState.Buttons[7] = ZE_WIKS_PRESSED;
		}
		else if (Input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
			MouseState.Buttons[7] = ZE_WIKS_RELEASED;

		if (Input->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
			MouseState.Axis[3] += *((SHORT*)&Input->data.mouse.usButtonData);
	}
}

ZEModuleDescription* ZEWindowsInputModule::GetModuleDescription()
{
	static ZEWindowsInputModuleDescription Desc;
	return &Desc;
}

bool ZEWindowsInputModule::Initialize()
{	
	zeLog("WindowInput", "Initializing Windows Input.\r\n");

	UINT NumberOfDevices;
	ZEArray<RAWINPUTDEVICELIST> DeviceList;

	if (GetRawInputDeviceList(NULL, &NumberOfDevices, sizeof(RAWINPUTDEVICELIST)) != 0) 
	{ 
		zeError("WindowsInput", "Can not load input device list.");
		return false;
	}

	DeviceList.SetCount(NumberOfDevices);
	if (GetRawInputDeviceList(DeviceList.GetCArray(), &NumberOfDevices, sizeof(RAWINPUTDEVICELIST)) == (UINT) -1)
	{
		zeError("WindowsInput", "Can not load input device list.");
		return false;
	}

	RAWINPUTDEVICE Rid[2];
    
		// Mouse
	Rid[0].usUsagePage = 0x01; 
	Rid[0].usUsage = 0x02; 
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget = NULL;

	// Keyboard
	Rid[1].usUsagePage = 0x01; 
	Rid[1].usUsage = 0x06; 
	Rid[1].dwFlags = 0;
	Rid[1].hwndTarget = NULL;

	if (RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE)) == FALSE) 
	{
		zeError("WindowsInput", "Can not register input devices.");
		return false;
	}

	memset(&KeyboardState, 0, sizeof(KeyboardState));
	memset(&MouseState, 0, sizeof(MouseState));

	return true;
}

void ZEWindowsInputModule::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEWindowsInputModule::IsEnabled()
{
	return Enabled;
}

void ZEWindowsInputModule::Deinitialize()
{

}

void ZEWindowsInputModule::ProcessInputs()
{   
	MouseState.Axis[0] = 0;
	MouseState.Axis[1] = 0;
	MouseState.Axis[2] = 0;

	for (size_t I = 0; I < 256; I++)
		if (KeyboardState.Buttons[I] == ZE_WIKS_RELEASED)
			KeyboardState.Buttons[I] = ZE_WIKS_NONE;
		else if (KeyboardState.Buttons[I] == ZE_WIKS_PRESSED)
			KeyboardState.Buttons[I] = ZE_WIKS_PRESSING;

	for (size_t I = 0; I < 8; I++)
		if (MouseState.Buttons[I] == ZE_WIKS_RELEASED)
			MouseState.Buttons[I] = ZE_WIKS_NONE;
		else if (MouseState.Buttons[I] == ZE_WIKS_PRESSED)
			MouseState.Buttons[I] = ZE_WIKS_PRESSING;

}

void ZEWindowsInputModule::ProcessInputMap(ZEInputMap* InputMap)
{
	ZEInputBinding* CurrInputBinding;
	InputMap->InputActionCount = 0;

	for (size_t I = 0; I < InputMap->InputBindings.GetCount(); I++)
	{
		ZEInputBinding* CurrentBinding = &InputMap->InputBindings[I];
		if (CurrentBinding->Event.DeviceType == ZE_IDT_KEYBOARD && CurrentBinding->Event.InputType == ZE_IT_BUTTON)
		{
			if ((CurrentBinding->Event.ButtonState == ZE_IBS_ALL && (KeyboardState.Buttons[CurrentBinding->Event.ButtonId] == ZE_WIKS_PRESSED || KeyboardState.Buttons[CurrentBinding->Event.ButtonId] == ZE_WIKS_PRESSING)) ||
				(CurrentBinding->Event.ButtonState == ZE_IBS_PRESSED && KeyboardState.Buttons[CurrentBinding->Event.ButtonId] == ZE_WIKS_PRESSED))
			{
				InputMap->InputActions[InputMap->InputActionCount].Id = CurrentBinding->ActionId;
				InputMap->InputActions[InputMap->InputActionCount].ButtonState = ZE_IBS_PRESSED;
				InputMap->InputActions[InputMap->InputActionCount].From = CurrentBinding;
				InputMap->InputActionCount++;
			}
			else if (CurrentBinding->Event.ButtonState == ZE_IBS_RELEASED && KeyboardState.Buttons[CurrentBinding->Event.ButtonId] == ZE_WIKS_RELEASED)
			{
				InputMap->InputActions[InputMap->InputActionCount].Id = CurrentBinding->ActionId;
				InputMap->InputActions[InputMap->InputActionCount].ButtonState = ZE_IBS_RELEASED;
				InputMap->InputActions[InputMap->InputActionCount].From = CurrentBinding;
				InputMap->InputActionCount++;
			}
		}
		else if (CurrentBinding->Event.DeviceType == ZE_IDT_MOUSE)
		{
			if (CurrentBinding->Event.InputType == ZE_IT_BUTTON)
			{
				if ((CurrentBinding->Event.ButtonState == ZE_IBS_ALL || CurrentBinding->Event.ButtonState == ZE_IBS_PRESSED) && 
					(MouseState.Buttons[CurrentBinding->Event.ButtonId] == ZE_WIKS_PRESSED || MouseState.Buttons[CurrentBinding->Event.ButtonId] == ZE_WIKS_PRESSING))
				{
					InputMap->InputActions[InputMap->InputActionCount].Id = CurrentBinding->ActionId;
					InputMap->InputActions[InputMap->InputActionCount].ButtonState = ZE_IBS_PRESSED;
					InputMap->InputActions[InputMap->InputActionCount].From = CurrentBinding;
					InputMap->InputActionCount++;
				}
				else if (CurrentBinding->Event.ButtonState == ZE_IBS_RELEASED && MouseState.Buttons[CurrentBinding->Event.ButtonId] == ZE_WIKS_RELEASED)
				{
					InputMap->InputActions[InputMap->InputActionCount].Id = CurrentBinding->ActionId;
					InputMap->InputActions[InputMap->InputActionCount].ButtonState = ZE_IBS_RELEASED;
					InputMap->InputActions[InputMap->InputActionCount].From = CurrentBinding;
					InputMap->InputActionCount++;
				}
			}
			else if (CurrentBinding->Event.InputType == ZE_IT_AXIS)
			{
				if (CurrentBinding->Event.AxisSign == ZE_IAS_POSITIVE && MouseState.Axis[CurrentBinding->Event.AxisId] > 0)
				{
					InputMap->InputActions[InputMap->InputActionCount].Id = CurrentBinding->ActionId;
					InputMap->InputActions[InputMap->InputActionCount].AxisValue = MouseState.Axis[CurrentBinding->Event.AxisId];
					InputMap->InputActions[InputMap->InputActionCount].From = CurrentBinding;
					InputMap->InputActionCount++;
				}
				else if (CurrentBinding->Event.AxisSign == ZE_IAS_NEGATIVE && MouseState.Axis[CurrentBinding->Event.AxisId] < 0)
				{
					InputMap->InputActions[InputMap->InputActionCount].Id = CurrentBinding->ActionId;
					InputMap->InputActions[InputMap->InputActionCount].AxisValue = -MouseState.Axis[CurrentBinding->Event.AxisId];
					InputMap->InputActions[InputMap->InputActionCount].From = CurrentBinding;
					InputMap->InputActionCount++;
				}
			}
		}
	}
}

bool ZEWindowsInputModule::GetRawInputEvent(ZEInputEvent& InputEvent)
{
	return false;
}

void ZEWindowsInputModule::Acquire()
{
	Acquired = true;
}

void ZEWindowsInputModule::UnAcquire()
{
	Acquired = false;
}

void ZEWindowsInputModule::Destroy()
{
	
}

ZEWindowsInputModule::~ZEWindowsInputModule()
{

}
