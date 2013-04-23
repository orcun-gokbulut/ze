//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESensicsInputDevice.cpp
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

#include "ZEError.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEDS/ZEFormat.h"
#include "ZECore/ZEWindow.h"

#include "ZESensicsInputModule.h"
#include "ZESensicsInputDevice.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dinput.h>
#include <memory.h>

BOOL CALLBACK EnumDeviceCallback(const DIDEVICEINSTANCE* DeviceInstance, VOID* SensicDevicePtr)
{
	return reinterpret_cast<ZESensicsInputDevice*>(SensicDevicePtr)->EnumSensicsDeviceCallback(DeviceInstance);
}

BOOL CALLBACK EnumDeviceObjectCallback(const DIDEVICEOBJECTINSTANCE* DeviceObjectInstance, VOID* SensicDevicePtr)
{
	return reinterpret_cast<ZESensicsInputDevice*>(SensicDevicePtr)->EnumSensicsDeviceObjectCallback(DeviceObjectInstance);
}

bool ZESensicsInputDevice::EnumSensicsDeviceCallback(const DIDEVICEINSTANCEA* DeviceInstance)
{
	HRESULT hr = DirectInput->CreateDevice(DeviceInstance->guidInstance, &SensicDevice, NULL);
	if (SUCCEEDED(hr) == TRUE)
	{
		std::string sensics_name(DeviceInstance->tszProductName);
		if(sensics_name.find("Sensics zSight HMD") == std::string::npos)
		{
			SensicDevice->Release();
			SensicDevice = NULL;
		}
		else
			return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}

bool ZESensicsInputDevice::EnumSensicsDeviceObjectCallback(const DIDEVICEOBJECTINSTANCEA* DeviceObjectInstance)
{
	if (DeviceObjectInstance->dwType & DIDFT_AXIS)
	{
		DIPROPRANGE ProperyRange;
		memset(&ProperyRange, 0, sizeof ProperyRange);
		ProperyRange.diph.dwSize = sizeof(DIPROPRANGE);
		ProperyRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		ProperyRange.diph.dwHow = DIPH_BYID;
		ProperyRange.diph.dwObj = DeviceObjectInstance->dwType;
		ProperyRange.lMin = 0;
		ProperyRange.lMax = 65535;

		if(FAILED(SensicDevice->SetProperty(DIPROP_RANGE, &ProperyRange.diph)))
			return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}

static short fromTwos( unsigned short x )
{
	if (x < 0x8000)
	{
		return (short) x;
	}
	else
	{
		x = x ^ 0xFFFF;
		short y = (short)x;
		if (y != 32767)
		{
			y = y + 1;
		}
		y = -y;
		return (short) y;
	}
}

bool ZESensicsInputDevice::InitializeSelf()
{
	if (!ZEInputDevice::InitializeSelf())
		return false;
	
	if (FAILED(DirectInput8Create(::GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL)))
	{
		Deinitialize();
		return false;
	}

	DirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDeviceCallback, this, DIEDFL_ATTACHEDONLY);
	if (SensicDevice == NULL)
	{
		Deinitialize();
		return false;
	}

	if (FAILED(SensicDevice->SetDataFormat(&c_dfDIJoystick2)))
	{
		Deinitialize();
		return false;
	}

	SensicDevice->SetCooperativeLevel((HWND)ZEWindow::GetInstance()->GetHandle(), DISCL_EXCLUSIVE|DISCL_FOREGROUND );
	SensicDevice->EnumObjects(EnumDeviceObjectCallback, this, DIDFT_AXIS);
	SensicDevice->Acquire();

	Description.Type = ZE_IDT_SENSOR;
	Description.FullName = "Sensics HMD Tracker";
	Description.Sink = true;
	Description.SinkName = "sensor";
	Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkName);
	Description.QuaternionCount = 1;
	Description.VectorCount = 1;

	Description.Index = ZEInputDeviceIndexes::GetNewDeviceIndex(ZE_IDT_SENSOR);
	Description.Name = ZEFormat::Format("Sensics{0:d:02}", Description.Index);
	Description.NameHash = ZEHashGenerator::Hash(Description.Name);

	State.Initialize(Description);
	State.Reset();

	return true;
}

bool ZESensicsInputDevice::DeinitializeSelf()
{
	if (SensicDevice != NULL)
	{
		SensicDevice->Release();
		SensicDevice = NULL;
	}

	if (DirectInput != NULL)
	{
		DirectInput->Release();
		DirectInput = NULL;
	}

	return ZEInputDevice::DeinitializeSelf();
}

void ZESensicsInputDevice::Acquire()
{
	if (!IsInitialized())
		return;

	if (SensicDevice->Acquire() == DI_OK)
		ZEInputDevice::Acquire();
}

void ZESensicsInputDevice::UnAcquire()
{
	if (!IsInitialized())
		return;

	SensicDevice->Unacquire();
	ZEInputDevice::UnAcquire();
}

void ZESensicsInputDevice::Process()
{
	State.Advance();
	
	DIJOYSTATE2 Data; // DirectInput joystick state structure
	if (SensicDevice->GetDeviceState(sizeof(Data), &Data) != DI_OK)
		return;

	float w = fromTwos((unsigned short)Data.lRx) / 32768.0f;
	float x = fromTwos((unsigned short)Data.lRy) / 32768.0f;
	float y = fromTwos((unsigned short)Data.lX) / 32768.0f;
	float z = fromTwos((unsigned short)Data.lY) / 32768.0f;
	float mag = sqrt(w * w + x * x + y * y + z * z);

	w = w / mag;
	x = x / mag;
	y = y / mag;
	z = z / mag;

	State.Quaternions.CurrentValues[0] = ZEQuaternion(-w, -y, z, x);
}

ZESensicsInputDevice::ZESensicsInputDevice()
{
	DirectInput = NULL;
	SensicDevice = NULL;
}

ZESensicsInputDevice::~ZESensicsInputDevice()
{
	Deinitialize();
}
