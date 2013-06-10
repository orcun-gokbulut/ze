//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVectorNavInputDevice.cpp
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

#include "ZEVectorNavInputModule.h"
#include "ZEVectorNavInputDevice.h"

#include "ZEDS/ZEFormat.h"

#include "vn100.h"
#include "ZECore/ZEOptionManager.h"

bool ZEVectorNavInputDevice::InitializeSelf()
{
	if (!ZEInputDevice::InitializeSelf())
		return false;

	if (vn100_connect(&Device, ZEFormat::Format("COM{0}", ZEOptionManager::GetInstance()->GetOption("CustHCombat", "GunSensorPort")->GetValue().GetInt32()).ToCString(), 115200) != VNERR_NO_ERROR)
		return false;

	if (vn100_setAsynchronousDataOutputType(&Device, VNASYNC_VNQTN,true) != VNERR_NO_ERROR)
	{
		vn100_disconnect(&Device);
		memset(&Device, 0, sizeof(Vn100));
		return false;
	}

	if (vn100_setVpeControl(&Device, 1, 2, 1, 1, true) != VNERR_NO_ERROR)
	{
		vn100_disconnect(&Device);
		memset(&Device, 0, sizeof(Vn100));
		return false;
	}

	Description.Type = ZE_IDT_SENSOR;
	Description.FullName = "VectorNav";
	Description.Sink = true;
	Description.SinkName = "sensor";
	Description.SinkNameHash = ZEHashGenerator::Hash(Description.SinkName);
	Description.QuaternionCount = 1;
	Description.VectorCount = 1;

	Description.Index = ZEInputDeviceIndexes::GetNewDeviceIndex(ZE_IDT_SENSOR);
	Description.Name = ZEFormat::Format("VectorNav{0:d:02}", Description.Index);
	Description.NameHash = ZEHashGenerator::Hash(Description.Name);

	State.Initialize(Description);
	State.Reset();


	return true;
}

bool ZEVectorNavInputDevice::DeinitializeSelf()
{
	if (Device.isConnected)
	{
		vn100_disconnect(&Device);
		memset(&Device, 0, sizeof(Vn100));
	}

	return ZEInputDevice::DeinitializeSelf();
}

void ZEVectorNavInputDevice::Process()
{
	State.Advance();

	Vn100CompositeData Data;
	if (vn100_getCurrentAsyncData(&Device, &Data) != VNERR_NO_ERROR)
		return;

	float Angle;
	ZEVector3 Axis;
	ZEQuaternion::ConvertToAngleAxis(Angle, Axis, ZEQuaternion(Data.quaternion.w, Data.quaternion.x, Data.quaternion.y, Data.quaternion.z).Normalize());
	ZEVector3 NewAxis;
	NewAxis.x = Axis.y;
	NewAxis.y = -Axis.z;
	NewAxis.z = Axis.x;
	ZEQuaternion::CreateFromAngleAxis(State.Quaternions.CurrentValues[0], -Angle, NewAxis);
}

ZEVectorNavInputDevice::ZEVectorNavInputDevice()
{
	memset(&Device, 0, sizeof(Vn100));
}
