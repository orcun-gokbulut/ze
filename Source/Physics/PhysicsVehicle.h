//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicsVehicle.h
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

#pragma once
#ifndef	__ZE_PHYSICS_VEHICLE_JOINT_H__
#define __ZE_PHYSICS_VEHICLE_JOINT_H__

class ZEPhysicsVehicleInfo;
class ZEVector3;
class ZEQuaternion;
class ZEPhysicsBody;

class ZEPhysicsVehicle
{
	protected:
									ZEPhysicsVehicle(){}
		virtual						~ZEPhysicsVehicle(){}

	public:
		virtual void				Initialize(ZEPhysicsVehicleInfo& Info) = 0;
		virtual void				Deinitialize()                         = 0;
		virtual void				Update(float ElapsedTime)              = 0;

		virtual ZEPhysicsBody*		GetBody() = 0;
		virtual int					GetWheelCount() = 0;
		virtual ZEVector3			GetWheelPosition(int Index) = 0;
		virtual ZEQuaternion		GetWheelOrientation(int Index) = 0;
		virtual void				SetSteeringAngle(float Angle) = 0;
		virtual void				SetMotorTorque(float Torque) = 0;
		virtual void				SetBrakeTorque(float Torque) = 0;
		virtual ZEVector3			GetLinearVelocity() = 0;
		virtual float				GetWheelSpeed(int Index) = 0;
		virtual float				GetAverageWheelSpeed() = 0;
};

#endif
