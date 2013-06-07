//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEXSensInputDevice.cpp
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
#include "ZEXSensInputModule.h"

#include "ZEXSensInputDevice.h"

#include "ZEDS/ZEFormat.h"
#include "ZEThread/ZEThread.h"

#include "xsensdeviceapi.h"
#include "ZEMath/ZEAngle.h"

void ZEXSensCallback::onDataAvailable(XsDevice* XSensDevice, const XsDataPacket* Packet)
{
	Device->Lock.Lock();
	XsQuaternion Quaternion = Packet->orientationQuaternion();
	Device->State.Quaternions.CurrentValues[0] =  ZEQuaternion(Quaternion.w(), Quaternion.x(), Quaternion.y(), Quaternion.z());
	
	float Angle;
	ZEVector3 Axis;
	ZEQuaternion::ConvertToAngleAxis(Angle, Axis, Device->State.Quaternions.CurrentValues[0]);
	ZEQuaternion::CreateFromAngleAxis(Device->State.Quaternions.CurrentValues[0], -Angle, ZEVector3(-Axis.y, Axis.z, Axis.x));

	Device->Lock.Unlock();
}

bool ZEXSensInputDevice::InitializeSelf()
{
	if (!ZEInputDevice::InitializeSelf())
		return false;

	if (!Module->Control->openPort(PortInfo.portName(), PortInfo.baudrate()))
		return false;

	XSensDevice = Module->Control->device(PortInfo.deviceId());
	Callback.Device = this;
	XSensDevice->addCallbackHandler(&Callback);

	Description.Type = ZE_IDT_SENSOR;
	Description.FullName = "xsens";
	Description.Sink = true;
	Description.SinkName = "sensor";
	Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkName);
	Description.QuaternionCount = 1;
	Description.VectorCount = 1;

	Description.Index = ZEInputDeviceIndexes::GetNewDeviceIndex(ZE_IDT_SENSOR);
	Description.Name = ZEFormat::Format("XSensSensor{0:d:02}", Description.Index);
	Description.NameHash = ZEHashGenerator::Hash(Description.Name);

	State.Initialize(Description);
	State.Reset();

	if (!XSensDevice->gotoMeasurement())
		return false;

	return true;
}

bool ZEXSensInputDevice::DeinitializeSelf()
{
	XSensDevice->removeCallbackHandler(&Callback);
	Module->Control->closePort(PortInfo);

	return ZEInputDevice::DeinitializeSelf();
}

void ZEXSensInputDevice::Process()
{
	Lock.Lock();
	State.Advance();
	Lock.Unlock();
}

ZEXSensInputDevice::ZEXSensInputDevice()
{
	XSensDevice = NULL;
}
