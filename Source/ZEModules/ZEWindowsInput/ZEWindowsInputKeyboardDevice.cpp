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

#include "ZECore/ZECore.h"
#include "ZECore/ZEConsole.h"
#include "ZEError.h"

#include "ZECore/ZESystemMessageHandler.h"
#include "ZECore/ZESystemMessageManager.h"

#include "ZEInput/ZEInputDefinitions.h"

#define WINDIWS_LEAN_AND_MEAN
#include <windows.h>
#include "ZEDS/ZEFormat.h"
#include "ZEDS/ZEHashGenerator.h"


void ZEWindowsInputKeyboardDevice::UnAcquire()
{
	State.Reset();
}

bool ZEWindowsInputKeyboardDevice::InitializeSelf()
{	
	if (!ZEInputDevice::InitializeSelf())
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

	State.Initialize(Description);
	State.Reset();

	return true;
}

void ZEWindowsInputKeyboardDevice::Process(const RAWINPUT& Data)
{   
	if ((Data.data.keyboard.Flags & 0x01) == RI_KEY_MAKE)
		State.Buttons.CurrentValues[Data.data.keyboard.MakeCode] = true;
	else
		State.Buttons.CurrentValues[Data.data.keyboard.MakeCode] = false;
}
