//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputEvent.cpp
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

#include "ZEInputEvent.h"

bool ZEInputEvent::IsAvailable() const
{
	return Input.IsAvailable();
}

bool ZEInputEvent::Check() const
{
	return Input.Check(State);
}

bool ZEInputEvent::Check(ZEInputAction& Action) const
{
	bool Result = Input.Check(State, Action);
	
	if (Result)
	{
		Action.From = const_cast<ZEInputEvent*>(this);
		Action.Id = ActionId;
	}

	return Result;
}

ZEInputEvent::ZEInputEvent()
{
	Processed = false;
}


ZEInputEvent ZEInputEvent::CreateAction(const ZEInput& Input, ZEInputState State, ZEInt ActionId)
{
	ZEInputEvent Event;
	Event.Input = Input;
	Event.State = State;
	Event.ActionId = ActionId;
	Event.Processed = false;
	return Event;
}

ZEInputEvent ZEInputEvent::CreateAction(const ZEString& InputString, ZEInputState State, ZEInt ActionId)
{
	return ZEInputEvent::CreateAction(ZEInput::Create(InputString), State, ActionId);
}

ZEInputEvent ZEInputEvent::CreateButtonAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	return ZEInputEvent::CreateAction(ZEInput::CreateButton(DeviceName, Index), State, ActionId);
}

ZEInputEvent ZEInputEvent::CreateAxisAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign, ZEInputState State, ZEInt ActionId)
{
	return ZEInputEvent::CreateAction(ZEInput::CreateAxis(DeviceName, Index, Sign), State, ActionId);
}

ZEInputEvent ZEInputEvent::CreatePOVAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	return ZEInputEvent::CreateAction(ZEInput::CreatePOV(DeviceName, Index), State, ActionId);
}

ZEInputEvent ZEInputEvent::CreateSwitchAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	return ZEInputEvent::CreateAction(ZEInput::CreateSwitch(DeviceName, Index), State, ActionId);
}

ZEInputEvent ZEInputEvent::CreateVectorAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	return ZEInputEvent::CreateAction(ZEInput::CreateVector(DeviceName, Index), State, ActionId);
}

ZEInputEvent ZEInputEvent::CreateQuaternionAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId)
{
	return ZEInputEvent::CreateAction(ZEInput::CreateQuaternion(DeviceName, Index), State, ActionId);
}

ZEInputEvent ZEInputEvent::CreateCursorAction(const ZEString& DeviceName, ZEInputState State, ZEInt ActionId)
{
	return ZEInputEvent::CreateAction(ZEInput::CreateCursor(DeviceName), State, ActionId);
}

ZEInputEvent ZEInputEvent::CreateTextAction(const ZEString& DeviceName, ZEInt ActionId)
{
	return ZEInputEvent::CreateAction(ZEInput::CreateText(DeviceName), ZE_IS_CHANGED, ActionId);
}

ZEInputEvent ZEInputEvent::CreateCallback(const ZEInput& Input, ZEInputState State, const ZEInputCallback& Callback)
{
	ZEInputEvent Event;
	Event.Input = Input;
	Event.State = State;
	Event.ActionId = -1;
	Event.Callback = Callback;
	Event.Processed = false;
	return Event;
}

ZEInputEvent ZEInputEvent::CreateCallback(const ZEString& InputString, ZEInputState State, const ZEInputCallback& Callback)
{
	return ZEInputEvent::CreateCallback(ZEInput::Create(InputString), State, Callback);
}

ZEInputEvent ZEInputEvent::CreateButtonCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	return ZEInputEvent::CreateCallback(ZEInput::CreateButton(DeviceName, Index), State, Callback);
}

ZEInputEvent ZEInputEvent::CreateAxisCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign, ZEInputState State, const ZEInputCallback& Callback)
{
	return ZEInputEvent::CreateCallback(ZEInput::CreateAxis(DeviceName, Index, Sign), State, Callback);
}

ZEInputEvent ZEInputEvent::CreatePOVCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	return ZEInputEvent::CreateCallback(ZEInput::CreatePOV(DeviceName, Index), State, Callback);
}

ZEInputEvent ZEInputEvent::CreateSwitchCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	return ZEInputEvent::CreateCallback(ZEInput::CreateSwitch(DeviceName, Index), State, Callback);
}

ZEInputEvent ZEInputEvent::CreateVectorCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	return ZEInputEvent::CreateCallback(ZEInput::CreateVector(DeviceName, Index), State, Callback);
}

ZEInputEvent ZEInputEvent::CreateQuaternionCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback)
{
	return ZEInputEvent::CreateCallback(ZEInput::CreateQuaternion(DeviceName, Index), State, Callback);
}

ZEInputEvent ZEInputEvent::CreateCursorCallback(const ZEString& DeviceName, ZEInputState State, const ZEInputCallback& Callback)
{
	return ZEInputEvent::CreateCallback(ZEInput::CreateCursor(DeviceName), State, Callback);
}

ZEInputEvent ZEInputEvent::CreateTextCallback(const ZEString& DeviceName, const ZEInputCallback& Callback)
{
	return ZEInputEvent::CreateCallback(ZEInput::CreateText(DeviceName), ZE_IS_CHANGED, Callback);
}
