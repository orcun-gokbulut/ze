//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWindowsInputMouseDevice.cpp
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

#include "ZEWindowsInputMouseDevice.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEConsole.h"
#include "ZEError.h"

#include "ZECore/ZESystemMessageHandler.h"
#include "ZECore/ZESystemMessageManager.h"

#define WINDIWS_LEAN_AND_MEAN
#include <windows.h>
#include "ZEDS/ZEFormat.h"
#include "ZEDS/ZEHashGenerator.h"

void ZEWindowsInputMouseDevice::UnAcquire()
{
	State.Reset();
}

bool ZEWindowsInputMouseDevice::InitializeInternal()
{	
	if (!ZEInputDevice::InitializeInternal())
		return false;

	Description.Type = ZE_IDT_MOUSE;
	Description.Sink = true;
	Description.SinkName = "Mouse";
	Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkName);
	Description.AxisCount = 3;
	Description.ButtonCount = 5;

	Description.Index = ZEInputDeviceIndexes::GetNewDeviceIndex(ZE_IDT_MOUSE);
	Description.Name = ZEFormat::Format("Mouse{0:d:02}", Description.Index);
	Description.NameHash = ZEHashGenerator::Hash(Description.Name);
	Description.FullName = "Mouse";

	State.Initialize(Description);
	State.Reset();

	return true;
}

void ZEWindowsInputMouseDevice::Process(const RAWINPUT& Data)
{   
	State.Axises.CurrentValues[ZE_IMA_HORIZANTAL_AXIS] += Data.data.mouse.lLastX;
	State.Axises.CurrentValues[ZE_IMA_VERTICAL_AXIS] += Data.data.mouse.lLastY;
	if (Data.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
		State.Axises.CurrentValues[ZE_IMA_WHEEL_AXIS] += *((SHORT*)&Data.data.mouse.usButtonData);

	if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) == RI_MOUSE_BUTTON_1_DOWN)
		State.Buttons.CurrentValues[0] = true;
	else if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) == RI_MOUSE_BUTTON_1_UP)
		State.Buttons.CurrentValues[0] = false;

	if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) == RI_MOUSE_BUTTON_2_DOWN)
		State.Buttons.CurrentValues[1] = true;
	else if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) == RI_MOUSE_BUTTON_2_UP)
		State.Buttons.CurrentValues[1] = false;

	if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) == RI_MOUSE_BUTTON_3_DOWN)
		State.Buttons.CurrentValues[2] = true;
	else if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) == RI_MOUSE_BUTTON_3_UP)
		State.Buttons.CurrentValues[2] = false;

	if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) == RI_MOUSE_BUTTON_4_DOWN)
		State.Buttons.CurrentValues[3] = true;
	else if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) == RI_MOUSE_BUTTON_4_UP)
		State.Buttons.CurrentValues[3] = false;

	if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) == RI_MOUSE_BUTTON_5_DOWN)
		State.Buttons.CurrentValues[4] = true;
	else if ((Data.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) == RI_MOUSE_BUTTON_5_UP)
		State.Buttons.CurrentValues[4] = false;
}
