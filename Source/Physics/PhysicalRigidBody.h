//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicalRigidBody.h
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
#ifndef	__ZE_PHYSICAL_RIGID_BODY_H__
#define __ZE_PHYSICAL_RIGID_BODY_H__

#include "PhysicalObject.h"
#include "ZEDS\Array.h"

class ZEPhysicalShape;

enum ZEPhysicalBodyType
{
	ZE_PBT_DYNAMIC,
	ZE_PBT_KINEMATIC,
	ZE_PBT_STATIC,
};

class ZEPhysicalRigidBody : public ZEPhysicalObject
{
	protected:
		ZEPhysicalTransformChangeCallback		TransformChangeCallback;

												ZEPhysicalRigidBody(){}
		virtual									~ZEPhysicalRigidBody(){}
								
	public:							
		virtual ZEPhysicalObjectType			GetPhysicalObjectType();

		virtual const				
		ZEArray<ZEPhysicalShape*>&				GetPhysicalShapes() = 0;
		virtual void							AddPhysicalShape(ZEPhysicalShape* Shape) = 0;
		virtual void							RemovePhysicalShape(ZEPhysicalShape* Shape) = 0;
								
		virtual void							SetKinematic(bool Enabled) = 0;
		virtual bool							GetKinematic() = 0;
									
		virtual void							SetMass(float NewMass) = 0;
		virtual float							GetMass() = 0;

		virtual	void							SetGravityEnabled(bool Enabled) = 0;
		virtual bool							GetGravityEnabled() = 0;

		virtual void							SetLockPositon(bool Enabled) =0 ;
		virtual bool							GetLockPosition() = 0;

		virtual void							SetLockRotation(bool Enabled) = 0;
		virtual bool							GetLockRotation() = 0;

		virtual void							SetMassCenterPosition(const ZEVector3& NewPosition) = 0;
		virtual ZEVector3						GetMassCenterPosition() = 0;

		virtual void							SetMassCenterRotation(const ZEQuaternion& Rotation) = 0;
		virtual ZEQuaternion					GetMassCenterRotation() = 0;
								
		virtual void							SetLinearVelocity(const ZEVector3& NewVelocity)  = 0;
		virtual ZEVector3						GetLinearVelocity() = 0;
								
		virtual void							SetAngularVelocity(const ZEVector3& NewVelocity) = 0;
		virtual ZEVector3						GetAngularVelocity() = 0;
									
		virtual void							SetLinearDamping(float NewLinearDamping) = 0;
		virtual float							GetLinearDamping() = 0;
									
		virtual void							SetAngularDamping(float NewAngularDamping) = 0;
		virtual float							GetAngularDamping() = 0;
									
		virtual void							SetLinearMomentum(const ZEVector3& NewLinearMomentum) = 0;
		virtual ZEVector3						GetLinearMomentum() = 0;
								
		virtual void							SetAngularMomentum(const ZEVector3& NewAngularMomentum) = 0;
		virtual ZEVector3						GetAngularMomentum() = 0;
								
		virtual void							ApplyForce(const ZEVector3& Force) = 0;
		virtual void							ApplyTorque(const ZEVector3& Torque) = 0;
								
		virtual void							ApplyLocalForce(const ZEVector3& Force) = 0;
		virtual void							ApplyLocalTorque(const ZEVector3& Torque) = 0;

		virtual void							SetTransformChangeCallback(const ZEPhysicalTransformChangeCallback& TransfromChangeCallback);
		virtual const
		ZEPhysicalTransformChangeCallback&		GetTransformChangeCallback();
	
		virtual bool							Initialize() = 0;
		virtual void							Deinitialize() = 0;

		static ZEPhysicalRigidBody*				CreateInstance();
};

#endif
