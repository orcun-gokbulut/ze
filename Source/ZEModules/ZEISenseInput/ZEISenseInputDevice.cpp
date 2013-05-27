//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEISenseInputDevice.cpp
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

#include "ZEISenseInputModule.h"
#include "ZEISenseInputDevice.h"

#include "ZEMath/ZEAngle.h"
#include "isense/isense.h"


bool ZEISenseInputDevice::InitializeSelf()
{
	if (!ZEInputDevice::InitializeSelf())
		return false;
	
	Handle = ISD_OpenTracker((Hwnd)NULL, 5, FALSE, FALSE); 
	if (Handle <= 0)
		return false;
	
	ISD_STATION_INFO_TYPE Config;
	ISD_GetStationConfig(Handle, &Config, 0, false);
	Config.AngleFormat = ISD_QUATERNION;
	ISD_SetStationConfig(Handle, &Config, 0, false);

	Description.Type = ZE_IDT_SENSOR;
	Description.FullName = "iSense";
	Description.Sink = true;
	Description.SinkName = "sensor";
	Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkName);
	Description.QuaternionCount = 1;
	Description.VectorCount = 1;

	Description.Index = ZEInputDeviceIndexes::GetNewDeviceIndex(ZE_IDT_SENSOR);
	Description.Name = ZEFormat::Format("iSense{0:d:02}", Description.Index);
	Description.NameHash = ZEHashGenerator::Hash(Description.Name);

	State.Initialize(Description);
	State.Reset();

	return true;
}

bool ZEISenseInputDevice::DeinitializeSelf()
{
	if (Handle != -1)
	{
		ISD_CloseTracker(Handle);
		Handle = -1;
	}

	return ZEInputDevice::DeinitializeSelf();
}

void ZEISenseInputDevice::Acquire()
{
	if (Handle == -1)
		return;

	ZEInputDevice::Acquire();
}

void ZEISenseInputDevice::UnAcquire()
{
	ZEInputDevice::UnAcquire();
}

void ZEISenseInputDevice::Process()
{
	State.Advance();
	
	if (Handle == -1)
		return;

	ISD_TRACKING_DATA_TYPE Data;
	if (ISD_GetTrackingData(Handle, &Data))
	{
		float Angle;
		ZEVector3 Axis;
		ZEQuaternion::ConvertToAngleAxis(Angle, Axis, ZEQuaternion(Data.Station[0].Quaternion[0], Data.Station[0].Quaternion[1], Data.Station[0].Quaternion[2], Data.Station[0].Quaternion[3]));
		ZEVector3 NewAxis;
		NewAxis.x = Axis.x;
		NewAxis.y = Axis.y;
		NewAxis.z = Axis.z;
		ZEQuaternion::CreateFromAngleAxis(State.Quaternions.CurrentValues[0], Angle, NewAxis);

		//State.Quaternions.CurrentValues[0].ConjugateSelf();
		//ZEQuaternion::CreateFromEuler(State.Quaternions.CurrentValues[0], ZEAngle::ToRadian(-Data.Station[0].Euler[1]), ZEAngle::ToRadian(Data.Station[0].Euler[3]), ZEAngle::ToRadian(Data.Station[0].Euler[2]));
		//State.Quaternions.CurrentValues[0] = ZEQuaternion(Data.Station[0].Quaternion[0], Data.Station[0].Quaternion[2], Data.Station[0].Quaternion[3], Data.Station[0].Quaternion[1]).Normalize().Conjugate();	
	}
}

ZEISenseInputDevice::ZEISenseInputDevice()
{
	Handle = -1;
}

ZEISenseInputDevice::~ZEISenseInputDevice()
{
	Deinitialize();
}
