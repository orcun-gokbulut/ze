//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVRPNInputDevice.cpp
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
#include "ZECore/ZEConsole.h"
#include "ZEDS/ZEHashGenerator.h"

#include "ZEVRPNInputDevice.h"

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>



inline ZEVector4 VRPN_TO_ZE_VEC(const vrpn_float64* Position)
{
	return ZEVector4((float)Position[0], (float)Position[1], (float)Position[2], 1.0f);
}

inline ZEQuaternion VRPN_TO_ZE_QUAD(const vrpn_float64* Quaternion)
{
	return ZEQuaternion((float)Quaternion[3], -(float)Quaternion[1], (float)Quaternion[2], -(float)Quaternion[0]).Normalize();	
}

static void VRPN_CALLBACK Tracker_ChangeHandler(void* UserData, const vrpn_TRACKERCB Data)
{
	ZEVRPNInputDeviceHandle* Handle = (ZEVRPNInputDeviceHandle*)UserData;
	
	if ((ZESize)Data.sensor < Handle->Description->VectorCount)
		Handle->State->Vectors.CurrentValues[(ZESize)Data.sensor] = VRPN_TO_ZE_VEC(Data.pos);
	
	if ((ZESize)Data.sensor < Handle->Description->VectorCount)
		Handle->State->Quaternions.CurrentValues[(ZESize)Data.sensor] = VRPN_TO_ZE_QUAD(Data.quat);

}

static void VRPN_CALLBACK Button_ChangeHandler(void* UserData, const vrpn_BUTTONCB Data)
{
	ZEVRPNInputDeviceHandle* Handle = (ZEVRPNInputDeviceHandle*)UserData;

	if ((ZESize)Data.button < Handle->Description->ButtonCount)
		Handle->State->Buttons.CurrentValues[(ZESize)Data.button] = Data.state == 0 ? false : true;

}

static void VRPN_CALLBACK Axis_ChangeHandler(void* UserData, const vrpn_ANALOGCB Data)
{
	ZEVRPNInputDeviceHandle* Handle = (ZEVRPNInputDeviceHandle*)UserData;

	ZEInt AxisCount = (ZESize)Data.num_channel > Handle->Description->AxisCount ? Handle->Description->AxisCount : Data.num_channel;
	for (ZEInt I = 0; I < AxisCount; I++)
		Handle->State->Axises.CurrentValues[I] = (float)(Data.channel[I]);
}

bool ZEVRPNInputDevice::InitializeSelf()
{
	if (!ZEInputDevice::InitializeSelf())
		return false;

	bool Clash = false;
	if (Description.ButtonCount != 0 )
	{
		vrpn_Button_Remote* ButtonRemote = new vrpn_Button_Remote(Description.FullName);
		ButtonRemote->register_change_handler(&Handle, Button_ChangeHandler);
		VRPNDevice = ButtonRemote;
	}

	if (Description.AxisCount != 0)
	{
		if (Clash)
		{
			zeError("A VRPN Input Device cannot contain button, analog (axis) and tracker(vector and/or quaternion) type inputs at the same time.");
			Deinitialize();
			return false;
		}

		vrpn_Analog_Remote* AxisRemote = new vrpn_Analog_Remote(Description.FullName);
		AxisRemote->register_change_handler(&Handle, Axis_ChangeHandler);
		VRPNDevice = AxisRemote;
	}

	if (Description.VectorCount != 0 || Description.QuaternionCount != 0)
	{
		if (Clash)
		{
			zeError("A VRPN Input Device cannot contain button, analog (axis) and tracker(vector and/or quaternion) type inputs at the same time.");
			Deinitialize();
			return false;
		}

		vrpn_Tracker_Remote* TrackerRemote = new vrpn_Tracker_Remote(Description.FullName);
		TrackerRemote->register_change_handler(&Handle, Tracker_ChangeHandler);
		VRPNDevice = TrackerRemote;
	}

	return true;
}

bool ZEVRPNInputDevice::DeinitializeSelf()
{
	if (VRPNDevice != NULL)
	{
		delete VRPNDevice;
		VRPNDevice = NULL;
	}

	return ZEInputDevice::DeinitializeSelf();
}

void ZEVRPNInputDevice::Process()
{
	State.Advance();
	VRPNDevice->mainloop();
}

ZEVRPNInputDevice::ZEVRPNInputDevice(const ZEInputDeviceDescription& Description)
{
	VRPNDevice = NULL;
	this->Description = Description;
	this->Description.NameHash = ZEHashGenerator::Hash(Description.Name);
	this->Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkNameHash);
	Handle.Description = &this->Description;
	Handle.State = &this->State;
}
