//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXPhysicalRigidBody.h
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
#ifndef	__ZE_PHYSX_PHYSICS_RIGID_BODY_H__
#define __ZE_PHYSX_PHYSICS_RIGID_BODY_H__

#include "ZEPhysics/ZEPhysicalRigidBody.h"

#include <NxActor.h>
#include <NxBodyDesc.h>

class ZEPhysicalShape;
class ZEPhysXPhysicalWorld;
class ZEPhysXPhysicalRigidBody : public ZEPhysicalRigidBody
{
	friend class ZEPhysXPhysicalJoint;
	friend class ZEPhysXModule;

	private:
		ZEPhysXPhysicalWorld*			PhysicalWorld;
		ZEArray<ZEPhysicalShape*>		Shapes;
		ZEVector3						Scale;
		
		bool							Enabled;
		bool							IsWakeUp;

		NxActor*						Actor;
		NxActorDesc						ActorDesc;
		NxBodyDesc						BodyDesc;

		void							ReCreate();

										ZEPhysXPhysicalRigidBody();
		virtual							~ZEPhysXPhysicalRigidBody();
								
	public:
		virtual void					SetPhysicalWorld(ZEPhysicalWorld* World);
		virtual ZEPhysicalWorld*		GetPhysicalWorld();

		virtual void					SetEnabled(bool Enabled);
		virtual bool					GetEnabled();
											
		virtual void					SetPosition(const ZEVector3& NewPosition);
		virtual ZEVector3				GetPosition();
		
		virtual void					SetRotation(const ZEQuaternion& NewRotation);
		virtual ZEQuaternion			GetRotation();

		virtual void					SetScale(const ZEVector3& Scale);
		virtual ZEVector3				GetScale();

		virtual const				
		ZEArray<ZEPhysicalShape*>&		GetPhysicalShapes();
		virtual void					AddPhysicalShape(ZEPhysicalShape* Shape);
		virtual void					RemovePhysicalShape(ZEPhysicalShape* Shape);

		virtual void					SetPhysicalBodyType(ZEPhysicalBodyType Type);
		virtual ZEPhysicalBodyType		GetPhysicalBodyType() const;

		virtual void					SetMass(float NewMass);
		virtual float					GetMass();

		virtual void					SetMassCenterPosition(const ZEVector3& NewPosition);
		virtual ZEVector3				GetMassCenterPosition();

		virtual void					SetMassCenterRotation(const ZEQuaternion& NewRotation);
		virtual ZEQuaternion			GetMassCenterRotation();

		virtual	void					SetGravityEnabled(bool Enabled);
		virtual bool					GetGravityEnabled();

		virtual void					SetLockPositon(bool Enabled);
		virtual bool					GetLockPosition();

		virtual void					SetLockRotation(bool Enabled);
		virtual bool					GetLockRotation();

		virtual void					SetLinearVelocity(const ZEVector3& NewVelocity);
		virtual ZEVector3				GetLinearVelocity();
								
		virtual void					SetAngularVelocity(const ZEVector3& NewVelocity);
		virtual ZEVector3				GetAngularVelocity();
									
		virtual void					SetLinearDamping(float NewLinearDamping);
		virtual float					GetLinearDamping();
									
		virtual void					SetAngularDamping(float NewAngularDamping);
		virtual float					GetAngularDamping();
									
		virtual void					SetLinearMomentum(const ZEVector3& NewLinearMomentum);
		virtual ZEVector3				GetLinearMomentum();
								
		virtual void					SetAngularMomentum(const ZEVector3& NewAngularMomentum);
		virtual ZEVector3				GetAngularMomentum();

		virtual void					SetCollisionEventFlags(ZEDWORD CollisionEventFlags);
		virtual ZEDWORD					GetCollisionEventFlags();

		virtual void					ApplyForce(const ZEVector3& Force, ZEPhysicalForceMode ForceMode, bool IsWakeUp);
		virtual void					ApplyTorque(const ZEVector3& Torque, ZEPhysicalForceMode ForceMode, bool IsWakeUp);
								
		virtual void					ApplyLocalForce(const ZEVector3& Force, ZEPhysicalForceMode ForceMode, bool IsWakeUp);
		virtual void					ApplyLocalTorque(const ZEVector3& Torque, ZEPhysicalForceMode ForceMode, bool IsWakeUp);

		virtual void					ApplyForceAtPosition(const ZEVector3& Force, const ZEVector3& Position, ZEPhysicalForceMode ForceMode, bool IsWakeUp);
		virtual void					ApplyForceAtLocalPosition(const ZEVector3& Force, const ZEVector3& Position, ZEPhysicalForceMode ForceMode, bool IsWakeUp);

		virtual void					ApplyLocalForceAtPosition(const ZEVector3& Force, const ZEVector3& Position, ZEPhysicalForceMode ForceMode, bool IsWakeUp);
		virtual void					ApplyLocalForceAtLocalPosition(const ZEVector3& Force, const ZEVector3& Position, ZEPhysicalForceMode ForceMode, bool IsWakeUp);

		virtual float					GetComputedKineticEnergy() const;
								
		virtual bool					Initialize();
		virtual void					Deinitialize();
};

#endif




