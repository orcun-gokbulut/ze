//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - VirtualInputModule.cpp
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

#include "Core/Error.h"
#include "Core/Console.h"
#include "VirtualInputModule.h"
#include "VirtualInputModuleDescription.h"

ZEModuleDescription* ZEVirtualInputModule::GetModuleDescription()
{
	static ZEVirtualInputModuleDescription Desc;
	return &Desc;
}

bool ZEVirtualInputModule::Initialize()
{
	zeOutput("Initializing Virtual Input module.\r\n");
	Enabled = true;

	return true;
}

void ZEVirtualInputModule::Deinitialize()
{
	zeOutput("Destroting Virtual Input module.\r\n");
}

bool ZEVirtualInputModule::IsEnabled()
{
	return Enabled;
}

void ZEVirtualInputModule::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

void ZEVirtualInputModule::ProcessInputs()
{
	ClearInputEvents();
}

void ZEVirtualInputModule::ProcessInputMap(ZEInputMap* InputMap)
{
	InputMap->InputActionCount = 0;
	for (size_t I = 0; I < VirtualInputEventCount; I++)
	{
		ZEInputEvent* CurrentEvent = &VirtualInputEvents[I];
		for (size_t N = 0; N < InputMap->InputBindings.GetCount(); N++)
		{
			ZEInputEvent* BindingEvent = &InputMap->InputBindings[N].Event;
			if (CurrentEvent->DeviceType == BindingEvent->DeviceType
				&& CurrentEvent->DeviceIndex == BindingEvent->DeviceIndex)
			{
				if (BindingEvent->InputType == ZE_IT_AXIS)
				{
					if(CurrentEvent->AxisId == BindingEvent->AxisId && CurrentEvent->AxisSign == BindingEvent->AxisSign)
					{
						InputMap->InputActions[InputMap->InputActionCount].Id = InputMap->InputBindings[N].ActionId;
						InputMap->InputActions[InputMap->InputActionCount].From = &InputMap->InputBindings[N];
						InputMap->InputActions[InputMap->InputActionCount].AxisValue = VirtualInputAxisValues[I];					
						InputMap->InputActionCount++;
					}
				}
				else
				{
					if(CurrentEvent->ButtonId == BindingEvent->ButtonId && (CurrentEvent->ButtonState == BindingEvent->ButtonState || BindingEvent->ButtonState != ZE_IBS_ALL))
					{
						InputMap->InputActions[InputMap->InputActionCount].Id = InputMap->InputBindings[N].ActionId;
						InputMap->InputActions[InputMap->InputActionCount].From = &InputMap->InputBindings[N];
						InputMap->InputActions[InputMap->InputActionCount].ButtonState = CurrentEvent->ButtonState;					
						InputMap->InputActionCount++;
					}
				}
			}
		}

	}
}

void ZEVirtualInputModule::Acquire()
{
	zeNotice("Virtual Input", "Virtual Input acquired.");
}

void ZEVirtualInputModule::UnAcquire()
{
	zeNotice("Virtual Input", "Virtual Input unacquired.");
}

void ZEVirtualInputModule::GetInputEventName(char* Name, size_t MaxSize)
{
	strncpy(Name, "DUM", MaxSize);
}

void ZEVirtualInputModule::GetInputEventShortName(char* ShortName, size_t MaxSize)
{
	strncpy(ShortName, "Virtual Input", MaxSize);
}

bool ZEVirtualInputModule::GetRawInputEvent(ZEInputEvent& InputEvent)
{
	return false;
}

void ZEVirtualInputModule::ClearInputEvents()
{
	VirtualInputEventCount = 0;
}

void ZEVirtualInputModule::PushInputEvent(ZEInputEvent& InputEvent, unsigned int AxisValue)
{
	if (VirtualInputEventCount > ZE_VIRTUAL_INPUT_MAX_INPUT_COUNT)
		return;
	VirtualInputEvents[VirtualInputEventCount] = InputEvent;
	VirtualInputAxisValues[VirtualInputEventCount] = AxisValue;
	VirtualInputEventCount++;
}

void ZEVirtualInputModule::MoveMouse(int X, int Y, int Z)
{
	if (X != 0)
		PushInputEvent(ZEInputEvent(ZE_IDT_MOUSE, 0, 0, (X > 0 ? ZE_IAS_POSITIVE : ZE_IAS_NEGATIVE)), abs(X));

	if (Y != 0)
		PushInputEvent(ZEInputEvent(ZE_IDT_MOUSE, 0, 1, (Y > 0 ? ZE_IAS_POSITIVE : ZE_IAS_NEGATIVE)), abs(Y));

	if (Z != 0)
		PushInputEvent(ZEInputEvent(ZE_IDT_MOUSE, 0, 2, (Z > 0 ? ZE_IAS_POSITIVE : ZE_IAS_NEGATIVE)), abs(Z));
}

void ZEVirtualInputModule::ClickMouse(bool LeftButton, bool RightButton, bool MiddleButton)
{
	if (LeftButton)
		PushInputEvent(ZEInputEvent(ZE_IDT_MOUSE, 0, 0, ZE_IBS_PRESSED));
	
	if (RightButton)
		PushInputEvent(ZEInputEvent(ZE_IDT_MOUSE, 0, 1, ZE_IBS_PRESSED));

	if (MiddleButton)
		PushInputEvent(ZEInputEvent(ZE_IDT_MOUSE, 0, 2, ZE_IBS_PRESSED));
}

void ZEVirtualInputModule::PressKey(unsigned char ButtonId)
{
	PushInputEvent(ZEInputEvent(ZE_IDT_KEYBOARD, 0, ButtonId, ZE_IBS_PRESSED));
}

void ZEVirtualInputModule::ReleaseKey(unsigned char ButtonId)
{
	PushInputEvent(ZEInputEvent(ZE_IDT_KEYBOARD, 0, ButtonId, ZE_IBS_RELEASED));
}

ZEVirtualInputModule::ZEVirtualInputModule()
{
	VirtualInputEventCount = 0;
}
