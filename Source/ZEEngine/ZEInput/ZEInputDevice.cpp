//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputDevice.cpp
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

#include "ZEInputDevice.h"
void ZEInputDeviceDescription::Clear()
{
	Name = "";
	NameHash = 0;
	Index = 0;
	Type = ZE_IDT_NONE;

	FullName = "";

	Sink = false;
	SinkName = "";
	SinkNameHash = 0;

	ButtonCount = 0;
	AxisCount = 0;
	POVCount = 0;
	SwitchCount = 0;
	VectorCount = 0;
	QuaternionCount = 0;
}

ZEInputDeviceDescription::ZEInputDeviceDescription()
{
	Clear();
}

void ZEInputDeviceState::Initialize(const ZEInputDeviceDescription& Description)
{
	Buttons.OldValues.SetCount(Description.ButtonCount);
	Buttons.CurrentValues.SetCount(Description.ButtonCount);

	Axises.OldValues.SetCount(Description.AxisCount);
	Axises.CurrentValues.SetCount(Description.AxisCount);

	POVs.OldValues.SetCount(Description.POVCount);
	POVs.CurrentValues.SetCount(Description.POVCount);

	Switches.OldValues.SetCount(Description.SwitchCount);
	Switches.CurrentValues.SetCount(Description.SwitchCount);

	Vectors.OldValues.SetCount(Description.VectorCount);
	Vectors.CurrentValues.SetCount(Description.VectorCount);

	Quaternions.OldValues.SetCount(Description.QuaternionCount);
	Quaternions.CurrentValues.SetCount(Description.QuaternionCount);
}

void ZEInputDeviceState::Clear()
{
	Buttons.OldValues.SetCount(0);
	Buttons.CurrentValues.SetCount(0);

	Axises.OldValues.SetCount(0);
	Axises.CurrentValues.SetCount(0);

	POVs.OldValues.SetCount(0);
	POVs.CurrentValues.SetCount(0);

	Switches.OldValues.SetCount(0);
	Switches.CurrentValues.SetCount(0);

	Vectors.OldValues.SetCount(0);
	Vectors.CurrentValues.SetCount(0);

	Quaternions.OldValues.SetCount(0);
	Quaternions.CurrentValues.SetCount(0);
}

void ZEInputDeviceState::Reset()
{
	if (Buttons.CurrentValues.GetCount() != 0)
	{
		memset(Buttons.CurrentValues.GetCArray(), 0, Buttons.CurrentValues.GetCount() * sizeof(bool));
		memset(Buttons.OldValues.GetCArray(), 0, Buttons.OldValues.GetCount() * sizeof(bool));
	}

	if (Axises.CurrentValues.GetCount() != 0)
	{
		memset(Axises.CurrentValues.GetCArray(), 0, Axises.CurrentValues.GetCount() * sizeof(float));
		memset(Axises.OldValues.GetCArray(), 0, Axises.OldValues.GetCount() * sizeof(float));
	}

	if (POVs.CurrentValues.GetCount() != 0)
	{
		memset(POVs.CurrentValues.GetCArray(), 0, POVs.CurrentValues.GetCount() * sizeof(float));
		memset(POVs.OldValues.GetCArray(), 0, POVs.OldValues.GetCount() * sizeof(float));
	}

	if (Switches.CurrentValues.GetCount() != 0)
	{
		memset(Switches.CurrentValues.GetCArray(), 0, Switches.CurrentValues.GetCount() * sizeof(ZEUInt));
		memset(Switches.OldValues.GetCArray(), 0, Switches.OldValues.GetCount() * sizeof(ZEUInt));
	}

	if (Vectors.CurrentValues.GetCount() != 0)
	{
		memset(Vectors.CurrentValues.GetCArray(), 0, Vectors.CurrentValues.GetCount() * sizeof(ZEVector4));
		memset(Vectors.OldValues.GetCArray(), 0, Vectors.OldValues.GetCount() * sizeof(ZEVector4));
	}

	if (Quaternions.CurrentValues.GetCount() != 0)
	{
		memset(Quaternions.CurrentValues.GetCArray(), 0, Quaternions.CurrentValues.GetCount() * sizeof(ZEQuaternion));
		memset(Quaternions.OldValues.GetCArray(), 0, Quaternions.OldValues.GetCount() * sizeof(ZEQuaternion));
	}
}

void ZEInputDeviceState::Advance()
{
	if (Buttons.CurrentValues.GetCount() != 0)
		memcpy(Buttons.OldValues.GetCArray(), Buttons.CurrentValues.GetCArray(), Buttons.CurrentValues.GetCount() * sizeof(bool));

	if (Axises.CurrentValues.GetCount() != 0)
		memcpy(Axises.OldValues.GetCArray(),Axises.CurrentValues.GetCArray(), Axises.CurrentValues.GetCount() * sizeof(float));

	if (POVs.CurrentValues.GetCount() != 0)
		memcpy(POVs.OldValues.GetCArray(),POVs.CurrentValues.GetCArray(), POVs.CurrentValues.GetCount() * sizeof(float));

	if (Switches.CurrentValues.GetCount() != 0)
		memcpy(Switches.OldValues.GetCArray(),Switches.CurrentValues.GetCArray(), Switches.CurrentValues.GetCount() * sizeof(ZEUInt));

	if (Vectors.CurrentValues.GetCount() != 0)
		memcpy(Vectors.OldValues.GetCArray(),Vectors.CurrentValues.GetCArray(), Vectors.CurrentValues.GetCount() * sizeof(ZEVector4));

	if (Quaternions.CurrentValues.GetCount() != 0)
		memcpy(Quaternions.OldValues.GetCArray(),Quaternions.CurrentValues.GetCArray(), Quaternions.CurrentValues.GetCount() * sizeof(ZEQuaternion));
}

void ZEInputDeviceState::AdvanceAndReset()
{
	if (Buttons.CurrentValues.GetCount() != 0)
	{
		memcpy(Buttons.OldValues.GetCArray(), Buttons.CurrentValues.GetCArray(), Buttons.CurrentValues.GetCount() * sizeof(bool));
		memset(Buttons.CurrentValues.GetCArray(), 0, Buttons.CurrentValues.GetCount() * sizeof(bool));
	}

	if (Axises.CurrentValues.GetCount() != 0)
	{
		memcpy(Axises.OldValues.GetCArray(), Axises.CurrentValues.GetCArray(), Axises.CurrentValues.GetCount() * sizeof(float));
		memset(Axises.CurrentValues.GetCArray(), 0, Axises.CurrentValues.GetCount() * sizeof(float));
	}

	if (POVs.CurrentValues.GetCount() != 0)
	{
		memcpy(POVs.OldValues.GetCArray(), POVs.CurrentValues.GetCArray(), POVs.CurrentValues.GetCount() * sizeof(float));
		memset(POVs.CurrentValues.GetCArray(), 0, POVs.CurrentValues.GetCount() * sizeof(float));
	}

	if (Switches.CurrentValues.GetCount() != 0)
	{
		memcpy(Switches.OldValues.GetCArray(), Switches.CurrentValues.GetCArray(), Switches.CurrentValues.GetCount() * sizeof(ZEUInt));
		memset(Switches.CurrentValues.GetCArray(), 0, Buttons.CurrentValues.GetCount() * sizeof(ZEUInt));
	}

	if (Vectors.CurrentValues.GetCount() != 0)
	{
		memcpy(Vectors.OldValues.GetCArray(), Vectors.CurrentValues.GetCArray(), Vectors.CurrentValues.GetCount() * sizeof(ZEVector4));
		memset(Vectors.CurrentValues.GetCArray(), 0, Buttons.CurrentValues.GetCount() * sizeof(ZEVector4));
	}

	if (Quaternions.CurrentValues.GetCount() != 0)
	{
		memcpy(Quaternions.OldValues.GetCArray(), Quaternions.CurrentValues.GetCArray(), Quaternions.CurrentValues.GetCount() * sizeof(ZEQuaternion));
		memset(Quaternions.CurrentValues.GetCArray(), 0, Quaternions.CurrentValues.GetCount() * sizeof(ZEQuaternion));
	}

}

ZESize ZEInputDeviceIndexes::KeyboardIndex = 0;
ZESize ZEInputDeviceIndexes::MouseIndex = 0;
ZESize ZEInputDeviceIndexes::JoystickIndex = 0;
ZESize ZEInputDeviceIndexes::GamepadIndex = 0;
ZESize ZEInputDeviceIndexes::WheelIndex = 0;
ZESize ZEInputDeviceIndexes::SensorIndex = 0;
ZESize ZEInputDeviceIndexes::OtherIndex = 0;

ZESize ZEInputDeviceIndexes::GetNewDeviceIndex(ZEInputDeviceType Type)
{
	ZESize Index;
	switch(Type)
	{
		case ZE_IDT_KEYBOARD:
			Index = KeyboardIndex;
			KeyboardIndex++;
			break;

		case ZE_IDT_MOUSE:
			Index = MouseIndex;
			MouseIndex++;
			break;

		case ZE_IDT_JOYSTICK:
			Index = JoystickIndex;
			JoystickIndex++;
			break;

		case ZE_IDT_GAMEPAD:
			Index = GamepadIndex;
			GamepadIndex++;
			break;

		case ZE_IDT_WHEEL:
			Index = WheelIndex;
			WheelIndex++;
			break;

		case ZE_IDT_SENSOR:
			Index = SensorIndex;
			SensorIndex++;
			break;

		case ZE_IDT_OTHER:
			Index = OtherIndex;
			OtherIndex++;
			break;

		default:
			Index = 0;
			break;
	}

	return Index;
}

bool ZEInputDevice::InitializeSelf()
{
	LifeState = ZE_IDLS_INITIALIZING;
	
	return true;
}

bool ZEInputDevice::DeinitializeSelf()
{
	LifeState = ZE_IDLS_NOT_INITIALIZED;

	State.Clear();
	Description.Clear();

	return true;
}

const ZEString& ZEInputDevice::GetName()
{
	return Description.Name;
}

const ZEInputDeviceDescription&	ZEInputDevice::GetDescription()
{
	return Description;
}

void ZEInputDevice::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEInputDevice::GetEnabled()
{
	return Enabled;
}

bool ZEInputDevice::IsAcquired()
{
	return Acquired;
}

void ZEInputDevice::Acquire()
{
	Acquired = true;
}

void ZEInputDevice::UnAcquire()
{
	Acquired = false;
}

bool ZEInputDevice::IsInitialized()
{
	return (LifeState == ZE_IDLS_INITIALIZED);
}

bool ZEInputDevice::Initialize()
{
	if (IsInitialized())
		return false;

	if (!InitializeSelf())
		return false;

	if (LifeState != ZE_IDLS_INITIALIZING)
		return false;

	LifeState = ZE_IDLS_INITIALIZED;

	Acquire();

	return true;
}

bool ZEInputDevice::Deinitialize()
{
	if (!IsInitialized())
		return true;

	LifeState = ZE_IDLS_DEINITIALIZING;

	if (!DeinitializeSelf())
		return false;

	if (LifeState != ZE_IDLS_NOT_INITIALIZED)
		return false;

	UnAcquire();

	return true;
}

const ZEInputDeviceState& ZEInputDevice::GetState()
{
	return State;
}

void ZEInputDevice::Destroy()
{
	delete this;
}

ZEInputDevice::ZEInputDevice()
{
	Enabled = true;
	LifeState = ZE_IDLS_NOT_INITIALIZED;
	Acquired = false;
}

ZEInputDevice::~ZEInputDevice()
{
	Deinitialize();
}
