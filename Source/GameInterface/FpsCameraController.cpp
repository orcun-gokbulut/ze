//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - FpsCameraController.cpp
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

#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"
#include "FpsCameraController.h"

ZEFpsCameraController::ZEFpsCameraController(ZECamera* Camera, float PitchLimit) : ZECameraController(Camera)
{
	Position = ZEVector3::Zero;
	Shaker = ZEVector3::Zero;
	ShakeQ = ZEQuaternion::Identity;
	ShakeTime = 0;
	ShakeInterval = 0;
	Pitch = 0;
	Yaw = 0;
	this->PitchLimit = PitchLimit;
}

ZEFpsCameraController::~ZEFpsCameraController()
{
	Camera = NULL;
}

void ZEFpsCameraController::Update(float ElapsedTime)
{
	ZEQuaternion Orientation = Camera->GetLocalRotation();
	float cRoll,cYaw,cPitch;
	ZEQuaternion::ConvertToEulerAngles(cPitch, cYaw, cRoll, Orientation);
	cPitch += Pitch * ElapsedTime;
	cYaw   += Yaw * ElapsedTime;
	if (cPitch >= PitchLimit)cPitch = PitchLimit;
	else if (cPitch <= -PitchLimit)cPitch = -PitchLimit;
	ZEQuaternion::Create(Orientation, cPitch, cYaw, cRoll);
	Camera->SetLocalPosition(Position);
	
	//shaking
	if (ShakeTime > 0)
	{
		ShakeTime -= ElapsedTime;
		static float CShake = 0;
		CShake += ElapsedTime;
		if (CShake > ShakeInterval)
		{
			CShake -= ShakeInterval;
			float p = (-50 + (rand() % 100)) * 0.02 * Shaker.x * ElapsedTime;
			float y = (-50 + (rand() % 100)) * 0.02 * Shaker.y * ElapsedTime;
			//float r = (-50 + (rand() % 100)) * 0.02 * Shaker.z * ElapsedTime;
			ZEQuaternion::Create(ShakeQ, p,y,0);
		}
		ZEQuaternion::Product(Orientation, Orientation, ShakeQ);
		Camera->SetLocalRotation(Orientation);
	}
	else if (Pitch != 0 || Yaw != 0)Camera->SetLocalRotation(Orientation);
}

void ZEFpsCameraController::SetParams(ZEVector3 cPosition, ZEVector3 cOffset, float cPitch, float cYaw)
{
	Position = cPosition + cOffset;
	Pitch	 = cPitch;
	Yaw		 = cYaw;
}

void ZEFpsCameraController::Shake(ZEVector3 PowerAxis, float Time, float Interval)
{
	Shaker = PowerAxis;
	ShakeTime = Time;
	ShakeInterval = Interval;
}
