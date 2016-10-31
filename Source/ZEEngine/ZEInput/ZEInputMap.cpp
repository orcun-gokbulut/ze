//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputMap.cpp
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

#include "ZEInputMap.h"

ZESize ZEInputMap::GetActionCount() const
{
	return ActionCount;
}

const ZEInputAction* ZEInputMap::GetActions() const
{
	return Actions;
}

bool ZEInputMap::CheckAction(ZEInt ActionId) const
{
	for (ZESize I = 0; I < ActionCount; I++)
		if (Actions[I].Id == ActionId)
			return true;

	return false;
}

bool ZEInputMap::CheckAction(ZEInt ActionId, ZEInputAction& Action) const
{
	for (ZESize I = 0; I < ActionCount; I++)
		if (Actions[I].Id == ActionId)
		{
			Action = Actions[I];
			return true;
		}

	return false;
}

void ZEInputMap::Update()
{
	ActionCount = 0;
	for (ZESize I = 0; I < Events.GetCount(); I++)
	{
		if (Events[I].Check(Actions[ActionCount]))
			ActionCount++;

		if (ActionCount == ZE_INPUT_MAP_MAX_INPUT_ACTION_COUNT)
			return;
	}
}


void ZEInputMap::AddAction(const ZEInput& Input, ZEInputState State, ZEInt ActionId)
{
	Events.Add(ZEInputEvent::CreateAction(Input, State, ActionId));
}

void ZEInputMap::AddAction(const ZEString& InputString, ZEInputState State, ZEInt ActionId)
{
	Events.Add(ZEInputEvent::CreateAction(ZEInput::Create(InputString), State, ActionId));
}

void ZEInputMap::AddButtonAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	Events.Add(ZEInputEvent::CreateAction(ZEInput::CreateButton(DeviceName, Index), State, ActionId));
}

void ZEInputMap::AddAxisAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign, ZEInputState State, ZEInt ActionId)
{
	Events.Add(ZEInputEvent::CreateAction(ZEInput::CreateAxis(DeviceName, Index, Sign), State, ActionId));
}

void ZEInputMap::AddPOVAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	Events.Add(ZEInputEvent::CreateAction(ZEInput::CreatePOV(DeviceName, Index), State, ActionId));
}

void ZEInputMap::AddSwitchAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	Events.Add(ZEInputEvent::CreateAction(ZEInput::CreateSwitch(DeviceName, Index), State, ActionId));
}

void ZEInputMap::AddVectorAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	Events.Add(ZEInputEvent::CreateAction(ZEInput::CreateVector(DeviceName, Index), State, ActionId));
}

void ZEInputMap::AddQuaternionAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	Events.Add(ZEInputEvent::CreateAction(ZEInput::CreateQuaternion(DeviceName, Index), State, ActionId));
}

void ZEInputMap::AddCallback(const ZEInput& Input, ZEInputState State, const ZEInputCallback& Callback)
{
	Events.Add(ZEInputEvent::CreateCallback(Input, State, Callback));
}

void ZEInputMap::AddCallback(const ZEString& InputString, ZEInputState State, const ZEInputCallback& Callback)
{
	Events.Add(ZEInputEvent::CreateCallback(ZEInput::Create(InputString), State, Callback));
}

void ZEInputMap::AddButtonCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	Events.Add(ZEInputEvent::CreateCallback(ZEInput::CreateButton(DeviceName, Index), State, Callback));
}

void ZEInputMap::AddAxisCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign, ZEInputState State, const ZEInputCallback& Callback)
{
	Events.Add(ZEInputEvent::CreateCallback(ZEInput::CreateAxis(DeviceName, Index, Sign), State, Callback));
}

void ZEInputMap::AddPOVCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	Events.Add(ZEInputEvent::CreateCallback(ZEInput::CreatePOV(DeviceName, Index), State, Callback));
}

void ZEInputMap::AddSwitchCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	Events.Add(ZEInputEvent::CreateCallback(ZEInput::CreateSwitch(DeviceName, Index), State, Callback));
}

void ZEInputMap::AddVectorCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	Events.Add(ZEInputEvent::CreateCallback(ZEInput::CreateVector(DeviceName, Index), State, Callback));
}

void ZEInputMap::AddQuaternionCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	Events.Add(ZEInputEvent::CreateCallback(ZEInput::CreateQuaternion(DeviceName, Index), State, Callback));
}

ZEInputMap::ZEInputMap()
{
	ActionCount = 0;	
}
