//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInput.cpp
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

#include "ZEInput.h"
#include "ZEInputDefinitions.h"
#include "ZEInputModule.h"
#include "ZEInputDevice.h"
#include "ZEDS/ZEString.h"
#include "ZEInputDeviceExtension.h"

#include <string.h>
#include <stdio.h>
#include "ZEDS/ZEHashGenerator.h"

bool ZEInput::ParseInputBinding(ZEString InputString, ZEInput& Input)
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
	};

	ZEParserState ParserState = ZE_PS_DEVICE_IDENTIFIER;
	ZESize Len = InputString.GetLength();
	ZESize IdentifierStart = 0;

	for (ZESize I = 0; I < Len; I++)
	{
		switch(*InputString[I].GetValue())
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

		default:
			if (ParserState == ZE_PS_INPUT_IDENTIFIER_START)
			{
				if (isdigit(*InputString[I].GetValue()))
					ParserState = ZE_PS_INPUT_INDEX;
				else
					return false;
			}
			else if (ParserState == ZE_PS_INPUT_INDEX && !isdigit(*InputString[I].GetValue()))
				return false;
			else if (ParserState == ZE_PS_INPUT_IDENTIFIER_END)
				return false;
		}
	}

	if (ParserState != ZE_PS_INPUT_IDENTIFIER_END)
		return false;

	ZEString InputTypeLower = InputType.Lower();
	if (InputTypeLower == "button")
		Input.Type = ZE_IT_BUTTON;
	else if (InputTypeLower == "axis")
		Input.Type = ZE_IT_AXIS;
	else if (InputTypeLower == "pov")
		Input.Type = ZE_IT_POV;
	else if (InputTypeLower == "switch")
		Input.Type = ZE_IT_SWITCH;
	else if (InputTypeLower == "vector")
		Input.Type = ZE_IT_VECTOR;
	else if (InputTypeLower == "quaternion")
		Input.Type = ZE_IT_QUATERNION;
	else
		return false;

	Input.DeviceName = DeviceName;
	Input.DeviceNameHash = ZEHashGenerator::Hash(DeviceName);

	
	if (!InputIndex.IsEmpty())
	{
		Input.Index = InputIndex.ToInt32();
	}

	return false;
}

bool ZEInput::ConnectToDevice()
{
	const ZEArray<ZEInputDeviceModule*>& DeviceModules = ZEInputModule::GetInstance()->GetDeviceModules();
	for (ZESize I = 0; I < DeviceModules.GetCount(); I++)
	{
		const ZEArray<ZEInputDevice*>& Devices = DeviceModules[I]->GetDevices();
		for (ZESize N = 0; N < Devices.GetCount(); N++)
		{
			const ZEInputDeviceDescription& DeviceDescription = Devices[N]->GetDescription();

			if ((DeviceDescription.NameHash == DeviceNameHash && DeviceDescription.Name == DeviceName) ||
				(DeviceDescription.Sink && DeviceDescription.SinkNameHash == DeviceNameHash && DeviceDescription.SinkName == DeviceName))
			{
				switch(Type)
				{
				case ZE_IT_BUTTON:
					if (Index > DeviceDescription.ButtonCount)
						return false;
					break;

				case ZE_IT_AXIS:
					if (Index > DeviceDescription.AxisCount)
						return false;
					break;

				case ZE_IT_POV:
					if (Index > DeviceDescription.POVCount)
						return false;
					break;

				case ZE_IT_SWITCH:
					if (Index > DeviceDescription.SwitchCount)
						return false;
					break;

				case ZE_IT_VECTOR:
					if (Index > DeviceDescription.VectorCount)
						return false;
					break;

				case ZE_IT_QUATERNION:
					if (Index > DeviceDescription.QuaternionCount)
						return false;
					break;

				default:
					return false;
				}

				Device = Devices[N];
				return true;
			}
		}
	}
	return false;
}

ZEInput::ZEInput()
{
	Device = NULL;
	DeviceNameHash = 0;
	Index = 0;
	Type = ZE_IT_NONE;
	AxisSign = ZE_IAS_BOTH;
}

const char* ZEInput::GetDeviceName() const
{
	return DeviceName;
}

ZEInputDevice* ZEInput::GetDevice() const
{
	return Device;
}

ZEUInt32 ZEInput::GetIndex() const
{
	return Index;
}

ZEInputType ZEInput::GetType() const
{
	return Type;
}

ZEInputAxisSign ZEInput::GetAxisSign() const
{
	return AxisSign;
}

ZEString ZEInput::GetInputString() const
{
	char Temp[256], *TypeString, *ModifierString;

	switch(Type)
	{
		case ZE_IT_BUTTON:
			TypeString = "Button";
			break;

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
				case ZE_IAS_BOTH:
					ModifierString = "All";
					break;
				default:
					return "";
			}
			break;


		case ZE_IT_POV:
			TypeString = "POV";
			break;

		case ZE_IT_SWITCH:
			TypeString = "Switch";
			break;

		case ZE_IT_VECTOR:
			TypeString = "Vector";
			break;
		case ZE_IT_QUATERNION:
			TypeString = "Quaternion";
			break;
		default:
			return "";
	}

	sprintf(Temp, "%s:%s[%d%]", DeviceName.ToCString(), TypeString, Index);

	return Temp;
}

bool ZEInput::IsAvailable() const
{
	return Device != NULL;
}

bool ZEInput::Check(ZEInputState State) const
{
	if (Device == NULL)
		return false;

	ZEInputValue Value;
	return Check(State, Value);
}

bool ZEInput::Check(ZEInputState State, ZEInputValue& Action) const
{
	if (Device == NULL)
		return false;

	const ZEInputDeviceState& DeviceState = Device->GetState();
	const ZEInputDeviceDescription& Description = Device->GetDescription();

	if (this->Type == ZE_IT_BUTTON)
	{
		if (State == ZE_IS_PRESSING)
		{
			if (DeviceState.Buttons.CurrentValues[Index] == true)
			{
				Action.ButtonState = ZE_IS_PRESSED;
				return true;
			}
		}
		else if (State == ZE_IS_PRESSED)
		{
			if (DeviceState.Buttons.OldValues[Index] == false && DeviceState.Buttons.CurrentValues[Index] == true)
			{
				Action.ButtonState = ZE_IS_PRESSED;
				return true;
			}
		}
		else if (State == ZE_IS_RELEASED)
		{
			if (DeviceState.Buttons.OldValues[Index] == true && DeviceState.Buttons.CurrentValues[Index] == false)
			{
				Action.ButtonState = ZE_IS_RELEASED;
				return true;
			}
		}
		else if (State == ZE_IS_CHANGED)
		{
			if (DeviceState.Buttons.OldValues[Index] == false && DeviceState.Buttons.CurrentValues[Index] == true)
				Action.ButtonState = ZE_IS_PRESSED;
			else
				Action.ButtonState = ZE_IS_RELEASED;

			return true;
		}
	}
	else if (this->Type == ZE_IT_AXIS)
	{
		if (State == ZE_IS_VALUE || (State == ZE_IS_CHANGED && DeviceState.Axises.CurrentValues[Index] != DeviceState.Axises.OldValues[Index]))
		{
			if (this->AxisSign == ZE_IAS_POSITIVE)
			{
				if (DeviceState.Axises.CurrentValues[Index] > 0)
				{
					Action.AxisValue = DeviceState.Axises.CurrentValues[Index];
					return true;
				}
			}
			else if (this->AxisSign == ZE_IAS_NEGATIVE)
			{
				if (DeviceState.Axises.CurrentValues[Index] < 0)
				{
					Action.AxisValue = -DeviceState.Axises.CurrentValues[Index];
					return true;
				}
			}
			else
			{
				Action.AxisValue = DeviceState.Axises.CurrentValues[Index];
				return true;
			}		
		}
	}
	else if (this->Type == ZE_IT_POV)
	{
		if (State == ZE_IS_VALUE || (State == ZE_IS_CHANGED && DeviceState.POVs.CurrentValues[Index] != DeviceState.POVs.OldValues[Index]))
		{
			Action.POVValue = DeviceState.POVs.CurrentValues[Index];
			return true;
		}
	}
	else if (this->Type == ZE_IT_SWITCH)
	{
		if (State == ZE_IS_VALUE || (State == ZE_IS_CHANGED && DeviceState.Vectors.CurrentValues[Index] != DeviceState.Vectors.OldValues[Index]))
		{
			Action.POVValue = DeviceState.POVs.CurrentValues[Index];
			return true;
		}
	}
	else if (this->Type == ZE_IT_VECTOR)
	{
		if (State == ZE_IS_VALUE || (State == ZE_IS_CHANGED && DeviceState.Vectors.CurrentValues[Index] != DeviceState.Vectors.OldValues[Index]))
		{
			Action.POVValue = DeviceState.POVs.CurrentValues[Index];
			return true;
		}
	}
	else if (this->Type == ZE_IT_QUATERNION)
	{
		if (State == ZE_IS_VALUE || (State == ZE_IS_CHANGED && DeviceState.Quaternions.CurrentValues[Index] != DeviceState.Quaternions.OldValues[Index]))
		{
			Action.Quaternion = DeviceState.Quaternions.CurrentValues[Index];
			return true;
		}
	}

	return false;
}

ZEInput ZEInput::Create(const ZEString& InputString)
{
	ZEInput Temp;
	ParseInputBinding(InputString, Temp);
	Temp.ConnectToDevice();
	return Temp;
}

ZEInput ZEInput::CreateButton(const ZEString& DeviceName, ZEUInt32 Index)
{
	ZEInput Temp;
	Temp.DeviceName = DeviceName;
	Temp.DeviceNameHash = ZEHashGenerator::Hash(DeviceName);
	Temp.Type = ZE_IT_BUTTON;
	Temp.Index = Index;
	Temp.ConnectToDevice();
	return Temp;
}

ZEInput ZEInput::CreateAxis(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign)
{
	ZEInput Temp;
	Temp.DeviceName = DeviceName;
	Temp.DeviceNameHash = ZEHashGenerator::Hash(DeviceName);
	Temp.Type = ZE_IT_AXIS;
	Temp.Index = Index;
	Temp.AxisSign = Sign;
	Temp.ConnectToDevice();
	return Temp;
}

ZEInput ZEInput::CreatePOV(const ZEString& DeviceName, ZEUInt32 Index)
{
	ZEInput Temp;
	Temp.DeviceName = DeviceName;
	Temp.DeviceNameHash = ZEHashGenerator::Hash(DeviceName);
	Temp.Index = Index;
	Temp.Type = ZE_IT_POV;
	Temp.ConnectToDevice();
	return Temp;
}

ZEInput ZEInput::CreateSwitch(const ZEString& DeviceName, ZEUInt32 Index)
{
	ZEInput Temp;
	Temp.DeviceName = DeviceName;
	Temp.DeviceNameHash = ZEHashGenerator::Hash(DeviceName);
	Temp.Index = Index;
	Temp.Type = ZE_IT_SWITCH;
	Temp.ConnectToDevice();
	return Temp;
}

ZEInput ZEInput::CreateVector(const ZEString& DeviceName, ZEUInt32 Index)
{
	ZEInput Temp;
	Temp.DeviceName = DeviceName;
	Temp.DeviceNameHash = ZEHashGenerator::Hash(DeviceName);
	Temp.Index = Index;
	Temp.Type = ZE_IT_VECTOR;
	Temp.ConnectToDevice();
	return Temp;
}

ZEInput ZEInput::CreateQuaternion(const ZEString& DeviceName, ZEUInt32 Index)
{
	ZEInput Temp;
	Temp.DeviceName = DeviceName;
	Temp.DeviceNameHash = ZEHashGenerator::Hash(DeviceName);
	Temp.Index = Index;
	Temp.Type = ZE_IT_QUATERNION;
	Temp.ConnectToDevice();
	return Temp;
}

