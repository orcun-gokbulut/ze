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
#include "ZEInputDefinitions.h"
#include "ZEInputModule.h"
#include "ZEInputDevice.h"

#include <string.h>
#include <stdio.h>

static bool ParseInputBinding(ZEString InputString, ZEInputEvent& Event)
{
	ZEString DeviceName, DeviceAddress, InputType, InputIndex, InputModifier;

	enum ZEParserState
	{
		ZE_PS_DEVICE_IDENTIFIER,
		ZE_PS_DEVICE_ADDRESS,
		ZE_PS_INPUT_TYPE,
		ZE_PS_INPUT_IDENTIFIER_START,
		ZE_PS_INPUT_INDEX,
		ZE_PS_INPUT_IDENTIFIER_END,
		ZE_PS_INPUT_MODIFIER
	};

	ZEParserState ParserState = ZE_PS_DEVICE_IDENTIFIER;
	int Len = InputString.GetLength();
	int IdentifierStart = 0;
	for (int I = 0; I < Len; I++)
	{
		switch(InputString[I])
		{
		case '\n':
		case '\r':
			return false;

		case ' ':
		case '\t':
			break;

		case '@':
			if (ParserState == ZE_PS_DEVICE_IDENTIFIER)
				DeviceName = InputString.SubString(IdentifierStart, I - 1).Trim();
			else
				return false;

			IdentifierStart = I + 1;
			ParserState = ZE_PS_DEVICE_ADDRESS;
			break;

		case ':':
			if (ParserState == ZE_PS_DEVICE_IDENTIFIER)
				DeviceName = InputString.SubString(IdentifierStart, I - 1).Trim();
			else if (ParserState == ZE_PS_DEVICE_ADDRESS)
				DeviceAddress = InputString.SubString(IdentifierStart, I - 1).Trim();
			else
				return false;

			IdentifierStart = I + 1;
			ParserState = ZE_PS_INPUT_TYPE;
			break;

		case '[':
			if (ParserState == ZE_PS_INPUT_TYPE)
				InputType = InputString.SubString(IdentifierStart, I - 1).Trim();
			else
				return false;

			IdentifierStart = I + 1;
			ParserState = ZE_PS_INPUT_IDENTIFIER_START;
			break;

		case ']':
			if (ParserState == ZE_PS_INPUT_INDEX)
			{
				InputIndex = InputString.SubString(IdentifierStart, I - 1).Trim();
				ParserState = ZE_PS_INPUT_IDENTIFIER_END;
			}
			else
				return false;
			break;

		case '.':
			if (ParserState == ZE_PS_INPUT_IDENTIFIER_END)
			{
				ParserState = ZE_PS_INPUT_MODIFIER;
				IdentifierStart = I + 1;
			}
			break;

		default:
			if (ParserState == ZE_PS_INPUT_IDENTIFIER_START)
			{
				if (isdigit(InputString[I]))
					ParserState = ZE_PS_INPUT_INDEX;
				else
					return false;
			}
			else if (ParserState == ZE_PS_INPUT_INDEX && !isdigit(InputString[I]))
				return false;
			else if (ParserState == ZE_PS_INPUT_IDENTIFIER_END)
				return false;
		}
	}

	if (ParserState == ZE_PS_INPUT_MODIFIER)
	{
		InputModifier = InputString.SubString(IdentifierStart, Len - 1).Trim();
		ParserState = ZE_PS_INPUT_IDENTIFIER_END;
	}

	if (ParserState != ZE_PS_INPUT_IDENTIFIER_END)
		return false;

	ZEString InputTypeLower = InputType.ToLower();
	if (InputTypeLower == "button")
		Event.Type = ZE_IT_AXIS;
	else if (InputTypeLower == "axis")
		Event.Type = ZE_IT_AXIS;
	else if (InputTypeLower == "vector2")
		Event.Type = ZE_IT_AXIS;
	else if (InputTypeLower == "vector3")
		Event.Type = ZE_IT_AXIS;
	else if (InputTypeLower == "vector4")
		Event.Type = ZE_IT_AXIS;
	else if (InputTypeLower == "quaternion")
		Event.Type = ZE_IT_AXIS;
	else
		return false;

	if (InputModifier.IsEmpty())
	{
		if (Event.Type == ZE_IT_AXIS)
			Event.AxisSign = ZE_IAS_ALL;
		else if (Event.Type == ZE_IT_BUTTON)
			Event.ButtonState = ZE_IBS_PRESSING;
		else if (Event.Type == ZE_IT_VECTOR2 || Event.Type == ZE_IT_VECTOR3 || Event.Type == ZE_IT_VECTOR4)
			Event.VectorState = ZE_IVS_ALWAYS;
		else if (Event.Type == ZE_IT_BUTTON)
			Event.QuaternionState = ZE_IQS_ALWAYS;
	}
	else
	{
		if (Event.Type == ZE_IT_AXIS)
		{
			ZEString InputModifierLower = InputModifier.ToLower();
			if (InputModifierLower == "negative")
				Event.AxisSign = ZE_IAS_NEGATIVE;
			else if (InputModifierLower == "positive")
				Event.AxisSign = ZE_IAS_POSITIVE;
			else if (InputModifierLower == "all")
				Event.AxisSign = ZE_IAS_ALL;
			else
				return false;
		}

		if (Event.Type == ZE_IT_BUTTON)
		{
			ZEString InputModifierLower = InputModifier.ToLower();
			if (InputModifierLower == "pressed")
				Event.ButtonState = ZE_IBS_PRESSED;
			else if (InputModifierLower == "released")
				Event.ButtonState = ZE_IBS_RELEASED;
			else if (InputModifierLower == "pressing")
				Event.ButtonState = ZE_IBS_PRESSING;
			else
				return false;
		}

		if (Event.Type == ZE_IT_VECTOR2 || Event.Type == ZE_IT_VECTOR3 || Event.Type == ZE_IT_VECTOR4)
		{
			ZEString InputModifierLower = InputModifier.ToLower();
			if (InputModifierLower == "changed")
				Event.VectorState = ZE_IVS_CHANGED;
			else if (InputModifierLower == "always")
				Event.VectorState = ZE_IVS_ALWAYS;
			else
				return false;
		}

		if (Event.Type == ZE_IT_QUATERNION)
		{
			ZEString InputModifierLower = InputModifier.ToLower();
			if (InputModifierLower == "changed")
				Event.QuaternionState = ZE_IQS_CHANGED;
			else if (InputModifierLower == "always")
				Event.QuaternionState = ZE_IQS_ALWAYS;
			else
				return false;
		}
	}

	const ZEArray<ZEInputDevice*>& Devices = ZEInputModule::GetInstance()->GetInputDevices();
	for (int I = 0; I < Devices.GetCount(); I++)
		if (Devices[I]->GetDeviceName() == DeviceName)
		{
			Event.Device = Devices[I];
			break;
		}

	if (Event.Device == NULL)
		return false;
	
	if (!InputIndex.IsEmpty())
	{
		Event.Index = InputIndex.ToInteger();
	}
}


void ZEInputEvent::Create(ZEString InputString)
{
	ParseInputBinding(InputString, *this);
}

ZEString ZEInputEvent::GetInputString()
{
	char Temp[256], *TypeString, *ModifierString;

	switch(Type)
	{
		case ZE_IT_AXIS:
			TypeString = "Axis";
			switch(AxisSign)
			{
				case ZE_IAS_NEGATIVE:
					ModifierString = "Negative";
					break;
				case ZE_IAS_POSITIVE:
					ModifierString = "Positive";
					break;
				case ZE_IAS_ALL:
					ModifierString = "All";
					break;
				default:
					return "";
			}
			break;

		case ZE_IT_BUTTON:
			TypeString = "Button";
			switch(ButtonState)
			{
				case ZE_IBS_PRESSED:
					ModifierString = "Pressed";
					break;
				case ZE_IBS_RELEASED:
					ModifierString = "Released";
					break;
				case ZE_IBS_PRESSING:
					ModifierString = "Pressing";
					break;
				default:
					return "";
			}
			break;

		case ZE_IT_VECTOR2:
			TypeString = "Vector2";
			break;

		case ZE_IT_VECTOR3:
			TypeString = "Vector3";
			break;

		case ZE_IT_VECTOR4:
			TypeString = "Vector4";
			break;
		case ZE_IT_QUATERNION:
			TypeString = "Quaternion";
			break;
		default:
			return "";
	}

	if (Type == ZE_IT_VECTOR2 || Type == ZE_IT_VECTOR3 || Type == ZE_IT_VECTOR4)		
		switch(VectorState)
		{
			case ZE_IVS_ALWAYS:
				ModifierString = "Always";
				break;
			case ZE_IVS_CHANGED:
				ModifierString = "Changed";
				break;
			default:
				return "";
		}
	else if (Type == ZE_IT_QUATERNION)
		switch(VectorState)
	{
		case ZE_IQS_ALWAYS:
			ModifierString = "Always";
			break;
		case ZE_IQS_CHANGED:
			ModifierString = "Changed";
			break;
		default:
			return "";
	}

	sprintf(Temp, "%s:%s[%d%].%s", Device->GetDeviceName(), TypeString, Index, ModifierString);

	return Temp;


}
									
ZEInputEvent::ZEInputEvent()
{
	Device = NULL;
}

ZEInputEvent::ZEInputEvent(const ZEString InputString)
{
	Create(InputString);
}

ZEInputEvent::ZEInputEvent(const ZEString DeviceName, ZEDWORD Index, ZEInputButtonState ButtonState)
{
	this->Device = NULL;
	const ZEArray<ZEInputDevice*>& Devices = ZEInputModule::GetInstance()->GetInputDevices();
	for (int I = 0; I < Devices.GetCount(); I++)
		if (Devices[I]->GetDeviceName() == DeviceName)
		{
			this->Device = Devices[I];
			break;
		}

	this->Type = ZE_IT_BUTTON;
	this->Index = Index;
	this->ButtonState = ButtonState;
}

ZEInputEvent::ZEInputEvent(const ZEString DeviceName, ZEDWORD Index, ZEInputAxisSign AxisSign)
{
	this->Device = NULL;
	const ZEArray<ZEInputDevice*>& Devices = ZEInputModule::GetInstance()->GetInputDevices();
	for (int I = 0; I < Devices.GetCount(); I++)
		if (Devices[I]->GetDeviceName() == DeviceName)
		{
			this->Device = Devices[I];
			break;
		}

	this->Type = ZE_IT_AXIS;
	this->Index = Index;
	this->AxisSign = AxisSign;
}

ZEInputEvent::ZEInputEvent(const ZEString DeviceName, ZEInputType InputType, ZEDWORD Index, ZEInputVectorState State)
{
	this->Device = NULL;
	const ZEArray<ZEInputDevice*>& Devices = ZEInputModule::GetInstance()->GetInputDevices();
	for (int I = 0; I < Devices.GetCount(); I++)
		if (Devices[I]->GetDeviceName() == DeviceName)
		{
			this->Device = Devices[I];
			break;
		}

	this->Type = InputType;
	this->Index = Index;
	this->VectorState = State;
}

ZEInputEvent::ZEInputEvent(const ZEString DeviceName, ZEDWORD Index, ZEInputQuaternionState State)
{
	const ZEArray<ZEInputDevice*>& Devices = ZEInputModule::GetInstance()->GetInputDevices();
	for (int I = 0; I < Devices.GetCount(); I++)
		if (Devices[I]->GetDeviceName() == DeviceName)
		{
			this->Device = Devices[I];
			break;
		}

	this->Index = Index;
	this->QuaternionState = State;
}
