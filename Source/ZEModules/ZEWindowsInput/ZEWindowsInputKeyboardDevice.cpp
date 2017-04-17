//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWindowsInputKeyboardDevice.cpp
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

#include "ZEWindowsInputKeyboardDevice.h"

#include "ZEDS/ZEFormat.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEWindowsInputModule.h"

#define WINDIWS_LEAN_AND_MEAN
#include <windows.h>
#include "ZEInput/ZEInputDeviceIndexes.h"

void ZEWindowsInputKeyboardDevice::UnAcquire()
{
	State.Reset();
	ZEInputDevice::UnAcquire();
}

bool ZEWindowsInputKeyboardDevice::InitializeInternal()
{	
	if (!ZEInputDevice::InitializeInternal())
		return false;

	Description.Type = ZE_IDT_KEYBOARD;
	Description.FullName = "Keyboard";
	Description.Sink = true;
	Description.SinkName = "Keyboard";
	Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkName);
	Description.ButtonCount = 256;

	Description.Index = ZEInputDeviceIndexes::GetNewDeviceIndex(ZE_IDT_KEYBOARD);
	Description.Name = ZEFormat::Format("Keyboard{0:d:02}", Description.Index);
	Description.NameHash = ZEHashGenerator::Hash(Description.Name);

	State.Setup(Description);
	State.Reset();

	Acquire();

	return true;
}

void ZEWindowsInputKeyboardDevice::Process(const ZEWindowsInputMessage* Messages, ZESize MessageCount)
{   
	if (!IsAcquired())
		return;

	State.Advance();
	
	for (ZESize I = 0; I < MessageCount; I++)
	{
		const ZEWindowsInputMessage& Message = Messages[I];
		if (Message.Message.message == WM_INPUT && Message.RawInput.header.dwType== RIM_TYPEKEYBOARD)
		{
			const RAWINPUT& Data = Message.RawInput;

			if ((Data.data.keyboard.Flags & 0x01) == RI_KEY_MAKE)
				State.Buttons.CurrentValues[Data.data.keyboard.MakeCode] = true;
			else
				State.Buttons.CurrentValues[Data.data.keyboard.MakeCode] = false;
		}
		else if (Message.Message.message == WM_CHAR)
		{
			State.Character = Message.Message.wParam;

			State.CharacterModifiers = ZEIN_CM_NONE;
			if (State.Buttons.CurrentValues[ZE_IKB_LCONTROL])
				State.CharacterModifiers |= ZEIN_CM_CTRL | ZEIN_CM_CTRL_LEFT;
			if (State.Buttons.CurrentValues[ZE_IKB_RCONTROL])
				State.CharacterModifiers |= ZEIN_CM_CTRL | ZEIN_CM_CTRL_RIGHT;
			if (State.Buttons.CurrentValues[ZE_IKB_LALT])
				State.CharacterModifiers |= ZEIN_CM_ALT | ZEIN_CM_ALT_LEFT;
			if (State.Buttons.CurrentValues[ZE_IKB_RALT])
				State.CharacterModifiers |= ZEIN_CM_ALT | ZEIN_CM_ALT_RIGHT;
			if (State.Buttons.CurrentValues[ZE_IKB_LSHIFT])
				State.CharacterModifiers |= ZEIN_CM_SHIFT | ZEIN_CM_SHIFT_LEFT;
			if (State.Buttons.CurrentValues[ZE_IKB_RSHIFT])
				State.CharacterModifiers |= ZEIN_CM_SHIFT | ZEIN_CM_SHIFT_RIGHT;
			if (State.Buttons.CurrentValues[ZE_IKB_LWIN])
				State.CharacterModifiers |= ZEIN_CM_WINDOWS | ZEIN_CM_WINDOWS_LEFT;
			if (State.Buttons.CurrentValues[ZE_IKB_RWIN])
				State.CharacterModifiers |= ZEIN_CM_WINDOWS | ZEIN_CM_WINDOWS_RIGHT;
		}
	}
}
