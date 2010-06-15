//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDirectInputModule.cpp
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

#include "ZEDirectInputModule.h"
#include "ZEDirectInputModuleDescription.h"
#include "ZECore\ZEError.h"
#include "ZECore\ZEWindow.h"
#include "ZECore\ZEConsole.h"
#include "ZECore\ZECore.h"
#include <dinput.h>

#define WINDOWS_MEAN_AND_LEAN
#include <windows.h>

#ifdef ZE_DEBUG_ENABLED
#define ZEKEYBOARD_COOPERATE_LEVEL		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
#define ZEMOUSE_COOPERATE_LEVEL			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
#define ZEJOYSTICK_COOPERATE_LEVEL		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
#else
#define ZEKEYBOARD_COOPERATE_LEVEL		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
#define ZEMOUSE_COOPERATE_LEVEL			DISCL_FOREGROUND | DISCL_EXCLUSIVE
#define ZEJOYSTICK_COOPERATE_LEVEL		DISCL_FOREGROUND | DISCL_EXCLUSIVE
#endif

typedef struct 
{
	LONG Axis[3];
	BYTE Buttons[4];
} ZEDIMouseState;

typedef struct
{
	BYTE Buttons[256];
} ZEDIKeyboardState;

typedef struct
{
	LONG Axis[8];
	DWORD POV[4];
	BYTE Buttons[32];
} ZEDIJoystickState;

LPDIRECTINPUT8			DI			= NULL;
LPDIRECTINPUTDEVICE8	DIMouse		= NULL;
LPDIRECTINPUTDEVICE8	DIKeyboard	= NULL;
LPDIRECTINPUTDEVICE8	DIJoystick	= NULL;

ZEDIKeyboardState		KeyboardStateA, KeyboardStateB;
ZEDIKeyboardState*      CurrentKeyboardState = &KeyboardStateA;
ZEDIKeyboardState*		OldKeyboardState = &KeyboardStateB;

ZEDIMouseState			MouseStateA, MouseStateB;
ZEDIMouseState*			CurrentMouseState = &MouseStateA;
ZEDIMouseState*			OldMouseState = &MouseStateB;

ZEDIJoystickState		JoystickStateA, JoystickStateB;
ZEDIJoystickState*		CurrentJoystickState = &JoystickStateA;
ZEDIJoystickState*		OldJoystickState = &JoystickStateB;

bool MouseAvailable		= true;
bool JoystickAvailable	= false;


BOOL CALLBACK CheckDirectInputDevices(const LPCDIDEVICEINSTANCE DeviceInstance, LPVOID Ref)
{
	switch(DeviceInstance->dwDevType)
	{
		case DI8DEVTYPE_MOUSE:
			MouseAvailable = true;
		case DI8DEVTYPE_JOYSTICK:
			JoystickAvailable = true;
	}
	return true;
}

ZEModuleDescription* ZEDirectInputModule::GetModuleDescription()
{
	static ZEDirectInputModuleDescription Desc;
	return &Desc;
}

HWND GetTopMostWindow(HWND Handle)
{
	HWND Window = GetWindow(Handle, GW_OWNER);

	if (Window == NULL)
		return Handle;

	return GetWindow(Handle, GW_OWNER);
}

bool ZEDirectInputModule::IsEnabled()
{
	return Enabled;
}

void ZEDirectInputModule::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}


bool ZEDirectInputModule::Initialize()
{	
	zeLog("DirectInput Module", "Initializing Direct Input module.");

	ZeroMemory(&KeyboardStateA, sizeof(ZEDIKeyboardState));
	ZeroMemory(&KeyboardStateB, sizeof(ZEDIKeyboardState));

	ZeroMemory(&MouseStateA, sizeof(ZEDIMouseState));
	ZeroMemory(&MouseStateB, sizeof(ZEDIMouseState));

	ZeroMemory(&JoystickStateA, sizeof(ZEDIJoystickState));
	ZeroMemory(&JoystickStateB, sizeof(ZEDIJoystickState));

	HRESULT hr;
	hr = DirectInput8Create((HINSTANCE)zeCore->GetApplicationInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DI, NULL);
	if (FAILED(hr))
	{	
		Destroy();
		zeError("DirectInput Module", "No DirectInput.");
		return false;
	}

	hr = DI->EnumDevices(DI8DEVCLASS_ALL, &CheckDirectInputDevices , NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
	{
		Destroy();
		zeError("DirectInput Module", "Can not enumurate devices.");
		return false;
	}  


	// Keyboard Creation !
    if (DIKeyboard == NULL)
	{
		hr = DI->CreateDevice(GUID_SysKeyboardEm2, &DIKeyboard, NULL);
		if (FAILED(hr))
		{	
			Destroy();
			zeError("DirectInput Module", "Can not create DirectInput keyboard device.");
			return false;
		}

		hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hr))
		{	
			Destroy();
			zeError("DirectInput Module", "Can not set DirectInput keyboard device data format.");
			return false;
		}

		hr = DIKeyboard->SetCooperativeLevel(GetTopMostWindow((HWND)zeWindow->GetHandle()), ZEKEYBOARD_COOPERATE_LEVEL);
		if (FAILED(hr))
		{	
			Destroy();
			zeError("DirectInput Module", "Can not set DirectInput keyboard device cooperative level.");
			return false;
		}
		DIKeyboard->Acquire();
	}

	// Mouse Creation !
	if (DIMouse == NULL)
	{
		if (MouseAvailable == true)
		{
			hr = DI->CreateDevice(GUID_SysMouse, &DIMouse, NULL);
			if (FAILED(hr))
			{	
				Destroy();
				zeError("DirectInput Module", "Can not create DirectInput mouse device.");
				return false;
			}
			
			hr=DIMouse->SetDataFormat(&c_dfDIMouse);
			if (FAILED(hr))
			{	
				Destroy();
				zeError("DirectInput Module", "Can not set DirectInput mouse device data format.");
				return false;
			}

			hr = DIMouse->SetCooperativeLevel(GetTopMostWindow((HWND)zeWindow->GetHandle()), ZEMOUSE_COOPERATE_LEVEL);
			if FAILED(hr)
			{	
				Destroy();
				zeError("DirectInput Module", "Can not set DirectInput mouse device cooperative level.");
				return false;
			}
			DIMouse->Acquire();
		}
		else
			zeNotice("DirectInput Module", "No mouse available.");
	}

	if (DIJoystick == NULL)
	{	
		if (JoystickAvailable == true)
		{
			hr = DI->CreateDevice(GUID_Joystick, &DIJoystick, NULL);
			if (FAILED(hr))
			{				
				zeError("DirectInput Module", "Can not create DirectInput joystick device.");
				Destroy();
				return false;
			}

			hr=DIJoystick->SetDataFormat(&c_dfDIJoystick);
			if (FAILED(hr))
			{   
				Destroy();
				zeError("DirectInput Module", "Can not set DirectInput joystick device data format.");
				return false;
			}

			hr = DIJoystick->SetCooperativeLevel(GetTopMostWindow((HWND)zeWindow->GetHandle()), ZEJOYSTICK_COOPERATE_LEVEL);
			if (FAILED(hr))
			{   
				Destroy();
				zeError("DirectInput Module", "Can not set DirectInput joystick device cooperative level.");
				return false;
			}

			DIJoystick->Acquire();
		}
		else
			zeNotice("DirectInput Module", "No joystick available.");

	}
	return true;
}


void ZEDirectInputModule::Deinitialize()
{
	zeLog("DirectInput Module", "Destroying Direct Input module.\r\n");
	if (DIMouse != NULL)
	{
		DIMouse->Unacquire();
		DIMouse->Release();
		DIMouse = NULL;
	}

	if (DIKeyboard != NULL)
	{
		DIKeyboard->Unacquire();
		DIKeyboard->Release();
		DIKeyboard = NULL;
	}

	if (DIJoystick !=NULL)
	{
		DIJoystick->Unacquire();
		DIJoystick->Release();
		DIJoystick = NULL;
	}

	if (DI != NULL)
	{
		DI->Release();
		DI = NULL;
	}
	zeLog("DirectInput Module", "Direct Input module destroyed.");
}


void ZEDirectInputModule::ProcessInputs()
{   
	if (!Enabled)
		return;

	ZEDIKeyboardState* TempKeyboardState;
	TempKeyboardState = CurrentKeyboardState;
	CurrentKeyboardState = OldKeyboardState;
	OldKeyboardState = TempKeyboardState;

	ZEDIMouseState* TempMouseState;
	TempMouseState = CurrentMouseState;
	CurrentMouseState = OldMouseState;
	OldMouseState = TempMouseState;

	ZEDIJoystickState* TempJoystickState;
	TempJoystickState = CurrentJoystickState;
	CurrentJoystickState = OldJoystickState;
	OldJoystickState = TempJoystickState;

	HRESULT hr;
	if (DIKeyboard != NULL)
	{
		hr = DIKeyboard->GetDeviceState(sizeof(ZEDIKeyboardState), CurrentKeyboardState);
		if(FAILED(hr))
		{	
			ZeroMemory(&KeyboardStateA, sizeof(ZEDIKeyboardState));
			ZeroMemory(&KeyboardStateB, sizeof(ZEDIKeyboardState));
			hr = DIKeyboard->Acquire();
			/*if (hr != DIERR_OTHERAPPHASPRIO && hr != DI_OK)
				zeError("DirectInput Module", "Could not accuire keyboard.");*/
		}
	}

	
	if (DIMouse != NULL)
	{
		hr = DIMouse->GetDeviceState(sizeof(ZEDIMouseState), CurrentMouseState);
		if(FAILED(hr))
		{
			ZeroMemory(&MouseStateA, sizeof(ZEDIMouseState));
			ZeroMemory(&MouseStateB, sizeof(ZEDIMouseState));
			hr = DIKeyboard->Acquire();
			/*if (hr != DIERR_OTHERAPPHASPRIO && hr != DI_OK && hr != S_FALSE))
				zeError("DirectInput Module", "Could not accuire keyboard.");*/
		}
	}
	if (DIJoystick != NULL)
	{	
		hr = DIJoystick->GetDeviceState(sizeof(ZEDIJoystickState), CurrentJoystickState);
		if(FAILED(hr))
		{
			ZeroMemory(&JoystickStateA, sizeof(ZEDIJoystickState));
			ZeroMemory(&JoystickStateB, sizeof(ZEDIJoystickState));
			hr = DIKeyboard->Acquire();
			/*if (hr != DIERR_OTHERAPPHASPRIO && hr != DI_OK)
				zeError("DirectInput Module", "Could not accuire keyboard.");*/
		}
	}

	MSG Message;
	PeekMessage(&Message, (HWND)zeWindow->GetHandle(), 0, 0, PM_NOREMOVE);
	switch(Message.message)
	{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_INPUT:
		case WM_CHAR:
		case WM_MOUSEMOVE:
			break;
	}
}

bool ZEDirectInputModule::GetRawInputEvent(ZEInputEvent& InputEvent)
{
	return false;
}

void ZEDirectInputModule::ProcessInputMap(ZEInputMap* InputMap)
{
	InputMap->InputActionCount = 0;

	if (!Enabled)
		return;

	for (size_t I = 0 ; I < InputMap->InputBindings.GetCount(); I++)
	{
		ZEInputBinding* CurrInputBinding = &InputMap->InputBindings.GetItem(I);
		ZEInputEvent* CurrInputEvent = &CurrInputBinding->Event;
		switch(CurrInputEvent->DeviceType)
		{
			case ZE_IDT_MOUSE:
				if (CurrInputEvent->InputType == ZE_IT_AXIS)
				{
					if (CurrInputEvent->AxisSign == ZE_IAS_POSITIVE)
					{
						if (CurrentMouseState->Axis[CurrInputEvent->AxisId] > 0)
						{
							InputMap->InputActions[InputMap->InputActionCount].Id = CurrInputBinding->ActionId;
							InputMap->InputActions[InputMap->InputActionCount].From =  CurrInputBinding;
							InputMap->InputActions[InputMap->InputActionCount].AxisValue = CurrentMouseState->Axis[CurrInputEvent->AxisId];
							InputMap->InputActionCount++;
						}
					}
					else
						if (CurrentMouseState->Axis[CurrInputEvent->AxisId] < 0)
						{
							InputMap->InputActions[InputMap->InputActionCount].Id = CurrInputBinding->ActionId;
							InputMap->InputActions[InputMap->InputActionCount].From =  CurrInputBinding;
							InputMap->InputActions[InputMap->InputActionCount].AxisValue = abs(CurrentMouseState->Axis[CurrInputEvent->AxisId]);
							InputMap->InputActionCount++;
						}
				}						
				else if (CurrInputEvent->InputType == ZE_IT_BUTTON)
				{
					if ((CurrInputEvent->ButtonState == ZE_IBS_ALL && (CurrentMouseState->Buttons[CurrInputEvent->ButtonId] & 0x80)) ||
							(CurrInputEvent->ButtonState == ZE_IBS_PRESSED && (CurrentMouseState->Buttons[CurrInputEvent->ButtonId] & 0x80) && !(OldMouseState->Buttons[CurrInputEvent->ButtonId] & 0x80)) || 
							(CurrInputEvent->ButtonState == ZE_IBS_RELEASED && !(CurrentMouseState->Buttons[CurrInputEvent->ButtonId] & 0x80) && (OldMouseState->Buttons[CurrInputEvent->ButtonId] & 0x80)))
					{
						InputMap->InputActions[InputMap->InputActionCount].Id = CurrInputBinding->ActionId;
						InputMap->InputActions[InputMap->InputActionCount].From =  CurrInputBinding;
						InputMap->InputActionCount++;
					}
				}
				break;

			case ZE_IDT_KEYBOARD:
				if ((CurrInputEvent->ButtonState == ZE_IBS_ALL && (CurrentKeyboardState->Buttons[CurrInputEvent->ButtonId] & 0x80)) ||
					(CurrInputEvent->ButtonState == ZE_IBS_PRESSED && (CurrentKeyboardState->Buttons[CurrInputEvent->ButtonId] & 0x80) && !(OldKeyboardState->Buttons[CurrInputEvent->ButtonId] & 0x80)) || 
					(CurrInputEvent->ButtonState == ZE_IBS_RELEASED && !(CurrentKeyboardState->Buttons[CurrInputEvent->ButtonId] & 0x80) && (OldKeyboardState->Buttons[CurrInputEvent->ButtonId] & 0x80)))
				{
					InputMap->InputActions[InputMap->InputActionCount].Id = CurrInputBinding->ActionId;
					InputMap->InputActions[InputMap->InputActionCount].From =  CurrInputBinding;
					InputMap->InputActionCount++;
				}
				break;
						
			case ZE_IDT_JOYSTICK:

				if (CurrInputEvent->InputType == ZE_IT_AXIS)
				{	
					if (CurrInputEvent->AxisSign == ZE_IAS_POSITIVE)
					{
						if (CurrentJoystickState->Axis[CurrInputEvent->AxisId] > 0)
						{
							InputMap->InputActions[InputMap->InputActionCount].Id = CurrInputBinding->ActionId;
							InputMap->InputActions[InputMap->InputActionCount].From =  CurrInputBinding;
							InputMap->InputActions[InputMap->InputActionCount].AxisValue = CurrentJoystickState->Axis[CurrInputEvent->AxisId];
							InputMap->InputActionCount++;
						}
					}
					else
					{	
						if (CurrentJoystickState->Axis[CurrInputEvent->AxisId] < 0)
						{
							InputMap->InputActions[InputMap->InputActionCount].Id = CurrInputBinding->ActionId;
							InputMap->InputActions[InputMap->InputActionCount].From =  CurrInputBinding;
							InputMap->InputActions[InputMap->InputActionCount].AxisValue = abs(CurrentJoystickState->Axis[CurrInputEvent->AxisId]);
							InputMap->InputActionCount++;
						}
					}		
				}
				else
					if ((CurrInputEvent->ButtonState == ZE_IBS_ALL && (CurrentJoystickState->Buttons[CurrInputEvent->ButtonId] & 0x80)) ||
						(CurrInputEvent->ButtonState == ZE_IBS_PRESSED && (CurrentJoystickState->Buttons[CurrInputEvent->ButtonId] & 0x80) && !(OldJoystickState->Buttons[CurrInputEvent->ButtonId] & 0x80)) || 
						(CurrInputEvent->ButtonState == ZE_IBS_RELEASED && !(CurrentJoystickState->Buttons[CurrInputEvent->ButtonId] & 0x80) && (OldJoystickState->Buttons[CurrInputEvent->ButtonId] & 0x80)))
					{
						InputMap->InputActions[InputMap->InputActionCount].Id = CurrInputBinding->ActionId;
						InputMap->InputActions[InputMap->InputActionCount].From =  CurrInputBinding;
						InputMap->InputActionCount++;
					}
				break;
		}
	}
}

void ZEDirectInputModule::Acquire()
{
	zeLog("DirectInput Module", "Acquire Direct Input.");
	if (DIKeyboard != NULL)
		DIKeyboard->Acquire();

	if (DIMouse != NULL)
		DIMouse->Acquire();

	if (DIJoystick != NULL)
		DIJoystick->Acquire();
}

void ZEDirectInputModule::UnAcquire()
{
	zeOutput("DirectInput Module", "Unacquire Direct Input.");

	if (DIKeyboard != NULL)
		DIKeyboard->Unacquire();

	if (DIMouse != NULL)
		DIMouse->Unacquire();

	if (DIJoystick != NULL)
		DIJoystick->Unacquire();
}

ZEDirectInputModule::ZEDirectInputModule()
{
	Enabled = true;
}

ZEDirectInputModule::~ZEDirectInputModule()
{
	Deinitialize();
}


