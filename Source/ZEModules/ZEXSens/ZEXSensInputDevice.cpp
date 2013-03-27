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

#include "cmt3.h"
#include "xsens_list.h"
#include "cmtscan.h"
#include "ZEDS/ZEFormat.h"
#include "ZEThread/ZEThread.h"


/*inline ZEVector4 XSENS_TO_ZE_VEC(const vrpn_float64* Position)
{
	return ZEVector4((float)Position[0], (float)Position[1], (float)Position[2], 1.0f);
}*/

inline ZEQuaternion XSENS_TO_ZE_QUAD(const CmtQuat& Quaternion)
{
	return ZEQuaternion(-Quaternion.m_data[3], Quaternion.m_data[1], -Quaternion.m_data[0], Quaternion.m_data[2]).Normalize();	
}

bool ZEXSensInputDevice::InitializeSelf()
{
	if (!ZEInputDevice::InitializeSelf())
		return false;

	XsensResultValue Result = XSensDevice.openPort(Info.m_portNr, Info.m_baudrate);
	if (Result != XRV_OK)
		return false;

	if (XSensDevice.getDeviceId(1,XSensDeviceId) != XRV_OK)
		return false;

	if (XSensDevice.gotoConfig() != XRV_OK)
		return false;
	
	CmtDeviceMode DeviceMode(CMT_OUTPUTMODE_ORIENT, CMT_OUTPUTSETTINGS_ORIENTMODE_QUATERNION, 30);
	if (XSensDevice.setDeviceMode(DeviceMode, true, XSensDeviceId) != XRV_OK)
		return false;

	Description.Type = ZE_IDT_SENSOR;
	Description.FullName = "xsens";
	Description.Sink = true;
	Description.SinkName = "sensor";
	Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkName);
	Description.QuaternionCount = 1;

	Description.Index = ZEInputDeviceIndexes::GetNewDeviceIndex(ZE_IDT_SENSOR);
	Description.Name = ZEFormat::Format("XSensSensor{0:d:02}", Description.Index);
	Description.NameHash = ZEHashGenerator::Hash(Description.Name);

	State.Initialize(Description);
	State.Reset();

	if (XSensPacket == NULL)
		XSensPacket = new xsens::Packet((unsigned short)1, XSensDevice.isXm());

	if (XSensDevice.gotoMeasurement() != XRV_OK)
		return false;

	return true;
}

bool ZEXSensInputDevice::DeinitializeSelf()
{
	if (XSensPacket != NULL)
	{
		delete XSensPacket;
		XSensPacket = NULL;
	}

	XSensDevice.closePort();

	return ZEInputDevice::DeinitializeSelf();
}

void ZEXSensInputDevice::Process()
{
	State.Advance();
		
	while (XSensDevice.readDataPacket(XSensPacket) == XRV_OK)
	{
		State.Quaternions.CurrentValues[0] = XSENS_TO_ZE_QUAD(XSensPacket->getOriQuat());
		zeLog("w: %f, x: %f, y: %f, z: %f", State.Quaternions.CurrentValues[0].w, State.Quaternions.CurrentValues[0].z, State.Quaternions.CurrentValues[0].y, State.Quaternions.CurrentValues[0].z);
	}
}

ZEXSensInputDevice::ZEXSensInputDevice()
{
	XSensPacket = NULL;
}

ZEXSensInputDevice::~ZEXSensInputDevice()
{

}
