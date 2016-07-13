//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXPhysicalRigidBody.cpp
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
#include "ZEPhysXPhysicalRigidBody.h"
#include "ZEPhysXPhysicalWorld.h"
#include "ZEPhysics/ZEPhysicalShapes.h"
#include "ZEPhysXConversion.h"

#include <NxShapeDesc.h>
#include <NxBoxShapeDesc.h>
#include <NxCapsuleShapeDesc.h>
#include <NxConvexShapeDesc.h>
#include <NxPlaneShapeDesc.h>
#include <NxSphereShapeDesc.h>
#include <NxTriangleMeshShapeDesc.h>
#include <NxMaterial.h>
#include <NxQuat.h>
#include <NxVec3.h>

void ZEPhysXPhysicalRigidBody::ReCreate()
{
	Deinitialize();
	ActorDesc.globalPose.t = Actor->getGlobalPosition();
	ActorDesc.globalPose.M.fromQuat(Actor->getGlobalOrientationQuat()); 
	BodyDesc.linearVelocity = Actor->getLinearVelocity();
	BodyDesc.angularVelocity = Actor->getAngularVelocity();
	BodyDesc.linearDamping = Actor->getLinearDamping();
	BodyDesc.angularDamping = Actor->getAngularDamping();
	Initialize();
}
			
bool ZEPhysXPhysicalRigidBody::InitializeInternal()
{
	if (!ZEPhysicalRigidBody::InitializeInternal())
		return false;

	if (Actor != NULL || PhysicalWorld == NULL || PhysicalWorld->GetScene() == NULL)
	{
		zeError("Can not create actor. Physical world is not initialized.");
		return false;
	}

	ZEArray<NxShapeDesc*> ShapeDescList;

	ActorDesc.shapes.clear();
	for (ZESize I = 0; I < Shapes.GetCount(); I++)
	{
		ZEPhysicalShape* CurrentShape = Shapes[I];

		switch (CurrentShape->GetPhysicalShapeType())
		{
			case ZE_PST_BOX:
			{
				NxBoxShapeDesc* BoxShapeDesc = new NxBoxShapeDesc();
				BoxShapeDesc->setToDefault();

				BoxShapeDesc->userData = CurrentShape;
				BoxShapeDesc->localPose.t = ZE_TO_NX(CurrentShape->GetPosition());
				BoxShapeDesc->localPose.M.fromQuat(ZE_TO_NX(CurrentShape->GetRotation()));

				BoxShapeDesc->dimensions.x = ((ZEPhysicalBoxShape*)CurrentShape)->GetWidth();
				BoxShapeDesc->dimensions.y = ((ZEPhysicalBoxShape*)CurrentShape)->GetHeight();
				BoxShapeDesc->dimensions.z = ((ZEPhysicalBoxShape*)CurrentShape)->GetLength();
				BoxShapeDesc->dimensions.arrayMultiply(BoxShapeDesc->dimensions, ZE_TO_NX(Scale)); 

				ShapeDescList.Add(BoxShapeDesc);
				ActorDesc.shapes.push_back(BoxShapeDesc);
				break;
			}

			case ZE_PST_SPHERE:
			{
				zeDebugCheckWarning(Scale.x != Scale.y && Scale.y != Scale.z, "Sphere physical shape does not support non uniform scaling. Only scale.x parameter will be used.");

				NxSphereShapeDesc* SphereShapeDesc = new NxSphereShapeDesc();
				SphereShapeDesc->setToDefault();

				SphereShapeDesc->userData = CurrentShape;
				SphereShapeDesc->localPose.t = ZE_TO_NX(CurrentShape->GetPosition());
				SphereShapeDesc->localPose.M.fromQuat(ZE_TO_NX(CurrentShape->GetRotation()));

				SphereShapeDesc->radius = Scale.x * ((ZEPhysicalSphereShape*)CurrentShape)->GetRadius();

				ShapeDescList.Add(SphereShapeDesc);
				ActorDesc.shapes.push_back(SphereShapeDesc);
				break;
			}

			case ZE_PST_CAPSULE:
			{
				zeDebugCheckWarning(Scale.x != 1.0f && Scale.x != Scale.y && Scale.y != Scale.z, "Capsule physical shape does not support scaling. Shape did not scaled.");
				
				NxCapsuleShapeDesc* CapsuleShapeDesc = new NxCapsuleShapeDesc();
				CapsuleShapeDesc->setToDefault();

				CapsuleShapeDesc->userData = CurrentShape;
				CapsuleShapeDesc->localPose.t = ZE_TO_NX(CurrentShape->GetPosition());
				CapsuleShapeDesc->localPose.M.fromQuat(ZE_TO_NX(CurrentShape->GetRotation()));

				CapsuleShapeDesc->radius = ((ZEPhysicalCapsuleShape*)CurrentShape)->GetRadius();
				CapsuleShapeDesc->height = ((ZEPhysicalCapsuleShape*)CurrentShape)->GetHeight() - 2 * CapsuleShapeDesc->radius;

				ShapeDescList.Add(CapsuleShapeDesc);
				ActorDesc.shapes.push_back(CapsuleShapeDesc);
				break;
			}

			case ZE_PST_CYLINDER:
			{
				zeDebugCheckWarning(Scale.x != 1.0f && Scale.x != Scale.y && Scale.y != Scale.z, "Cylinder physical shape does not support scaling. Shape did not scaled.");
				break;
			}

			case ZE_PST_CONVEX:
			{
				break;
			}
		}
	}

	switch (PhysicalBodyType)
	{
		case ZE_PBT_DYNAMIC:
			break;

		case ZE_PBT_KINEMATIC:
			if (Enabled)
			{
				BodyDesc.flags |= NX_BF_KINEMATIC;
				if (Actor != NULL)
					Actor->raiseBodyFlag(NX_BF_KINEMATIC);
			}
			else
			{
				BodyDesc.flags &= ~NX_BF_KINEMATIC;
				if (Actor != NULL)
					Actor->clearBodyFlag(NX_BF_KINEMATIC);
			}
			break;

		case ZE_PBT_STATIC:
			ActorDesc.body = NULL;
			break;
	}

	NxScene* Scene = PhysicalWorld->GetScene();
	Actor = Scene->createActor(ActorDesc);
	if (Actor == NULL)
		zeError("Can not create actor.");

	for (ZESize I = 0; I < ShapeDescList.GetCount(); I++)
		delete ShapeDescList[I];
	ShapeDescList.Clear();

	return Actor != NULL;
}

bool ZEPhysXPhysicalRigidBody::DeinitializeInternal()
{
	if (Actor != NULL)
	{
		PhysicalWorld->GetScene()->releaseActor(*Actor);
		Actor = NULL;
	}

	return ZEPhysicalRigidBody::DeinitializeInternal();
}


ZEPhysXPhysicalRigidBody::ZEPhysXPhysicalRigidBody()
{
	Actor = NULL;
	ActorDesc.setToDefault();
	ActorDesc.userData = this;
	ActorDesc.body = &BodyDesc;
	ActorDesc.contactReportFlags |= NX_NOTIFY_ON_START_TOUCH;

	Scale = ZEVector3::One;
	Enabled = true;
	IsWakeUp = true;

	BodyDesc.mass = 1.0f;
	BodyDesc.flags &= ~NX_BF_KINEMATIC;

	PhysicalWorld = NULL;
}

ZEPhysXPhysicalRigidBody::~ZEPhysXPhysicalRigidBody()
{
	Deinitialize();
}

void ZEPhysXPhysicalRigidBody::SetPhysicalWorld(ZEPhysicalWorld* World)
{
	if (PhysicalWorld == World)
		return;

	PhysicalWorld = (ZEPhysXPhysicalWorld*)World;

	if (Actor != NULL)
		ReCreate();
}

ZEPhysicalWorld* ZEPhysXPhysicalRigidBody::GetPhysicalWorld()
{
	return PhysicalWorld;
}

void ZEPhysXPhysicalRigidBody::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
	if (this->Enabled)
	{
		ActorDesc.flags &= ~(NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE);
		BodyDesc.flags &= ~NX_BF_FROZEN;
		if (Actor != NULL)
		{
			Actor->clearActorFlag(NX_AF_DISABLE_COLLISION);
			Actor->clearActorFlag(NX_AF_DISABLE_RESPONSE);
			Actor->clearActorFlag(NX_AF_FLUID_DISABLE_COLLISION);
			Actor->clearBodyFlag(NX_BF_FROZEN);
		}
	}
	else
	{
		ActorDesc.flags |= (NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE);
		BodyDesc.flags |= NX_BF_FROZEN;
		if (Actor != NULL)
		{
			Actor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
			Actor->raiseActorFlag(NX_AF_DISABLE_RESPONSE);
			Actor->raiseActorFlag(NX_AF_FLUID_DISABLE_COLLISION);
			Actor->raiseBodyFlag(NX_BF_FROZEN);
		}
	}
}

bool ZEPhysXPhysicalRigidBody::GetEnabled()
{
	return Enabled;
}

void ZEPhysXPhysicalRigidBody::SetPosition(const ZEVector3& NewPosition)
{
	ActorDesc.globalPose.t = ZE_TO_NX(NewPosition);
	if (Actor != NULL)
		Actor->setGlobalPosition(ActorDesc.globalPose.t);
}

ZEVector3 ZEPhysXPhysicalRigidBody::GetPosition()
{
	if (Actor != NULL)
		return NX_TO_ZE(Actor->getGlobalPosition());
	else
		return NX_TO_ZE(ActorDesc.globalPose.t);
}

void ZEPhysXPhysicalRigidBody::SetRotation(const ZEQuaternion& NewRotation)
{
	ActorDesc.globalPose.M.fromQuat(ZE_TO_NX(NewRotation));
	if (Actor != NULL)
		Actor->setGlobalOrientationQuat(ZE_TO_NX(NewRotation));
}

ZEQuaternion ZEPhysXPhysicalRigidBody::GetRotation()
{
	if (Actor != NULL)
		return NX_TO_ZE(Actor->getGlobalOrientationQuat());
	else
	{
		NxQuat Temp;
		ActorDesc.globalPose.M.toQuat(Temp);
		return NX_TO_ZE(Temp);
	}
}

void ZEPhysXPhysicalRigidBody::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
	if (Actor != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalRigidBody::GetScale()
{
	return Scale;
}

const ZEArray<ZEPhysicalShape*>& ZEPhysXPhysicalRigidBody::GetPhysicalShapes()
{
	return Shapes;
}

void ZEPhysXPhysicalRigidBody::AddPhysicalShape(ZEPhysicalShape* Shape)
{
	Shapes.Add(Shape);
	Shape->SetOwner(this);

	if (Actor != NULL)
		ReCreate();
}

void ZEPhysXPhysicalRigidBody::RemovePhysicalShape(ZEPhysicalShape* Shape)
{
	ZESize OldCount = Shapes.GetCount();
	Shapes.RemoveValue(Shape);
	if (Actor != NULL)
		if (OldCount != Shapes.GetCount())
			ReCreate();
}

void ZEPhysXPhysicalRigidBody::SetPhysicalBodyType(ZEPhysicalBodyType Type)
{
	PhysicalBodyType = Type;
}

ZEPhysicalBodyType ZEPhysXPhysicalRigidBody::GetPhysicalBodyType() const
{
	return PhysicalBodyType;
}

void ZEPhysXPhysicalRigidBody::SetMass(float NewMass)
{
	BodyDesc.mass = NewMass;
	if (Actor != NULL)
		Actor->setMass(NewMass);
}

float ZEPhysXPhysicalRigidBody::GetMass()
{
	return BodyDesc.mass;
}

void ZEPhysXPhysicalRigidBody::SetMassCenterPosition(const ZEVector3& NewPosition)
{
	BodyDesc.massLocalPose.t = ZE_TO_NX(NewPosition);
	if (Actor != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalRigidBody::GetMassCenterPosition()
{
	return NX_TO_ZE(BodyDesc.massLocalPose.t);
}

void ZEPhysXPhysicalRigidBody::SetMassCenterRotation(const ZEQuaternion& NewRotation)
{
	BodyDesc.massLocalPose.M.fromQuat(ZE_TO_NX(NewRotation));
	if (Actor != NULL)
		ReCreate();
}

ZEQuaternion ZEPhysXPhysicalRigidBody::GetMassCenterRotation()
{
	NxQuat Temp;
	BodyDesc.massLocalPose.M.toQuat(Temp);

	return NX_TO_ZE(Temp);
}

void ZEPhysXPhysicalRigidBody::SetGravityEnabled(bool Enabled)
{
	if (Enabled)
	{
		BodyDesc.flags &= ~NX_BF_DISABLE_GRAVITY;
		if (Actor != NULL)
			Actor->clearBodyFlag(NX_BF_DISABLE_GRAVITY);
	}
	else
	{
		BodyDesc.flags |= NX_BF_DISABLE_GRAVITY;
		if (Actor != NULL)
			Actor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
	}
}

bool ZEPhysXPhysicalRigidBody::GetGravityEnabled()
{
	return !(BodyDesc.flags & NX_BF_DISABLE_GRAVITY);
}

void ZEPhysXPhysicalRigidBody::SetLockPositon(bool Enabled)
{
	if (Enabled)
	{
		BodyDesc.flags |= NX_BF_FROZEN_POS;
		if (Actor != NULL)
			Actor->raiseBodyFlag(NX_BF_FROZEN_POS);
	}
	else
	{
		BodyDesc.flags &= ~NX_BF_FROZEN_POS;
		if (Actor != NULL)
			Actor->clearBodyFlag(NX_BF_FROZEN_POS);
	}
}

bool ZEPhysXPhysicalRigidBody::GetLockPosition()
{
	return (BodyDesc.flags & NX_BF_FROZEN_POS) != 0;
}

void ZEPhysXPhysicalRigidBody::SetLockRotation(bool Enabled)
{
	if (Enabled)
	{
		BodyDesc.flags |= NX_BF_FROZEN_ROT;
		if (Actor != NULL)
			Actor->raiseBodyFlag(NX_BF_FROZEN_ROT);
	}
	else
	{
		BodyDesc.flags &= ~NX_BF_FROZEN_ROT;
		if (Actor != NULL)
			Actor->clearBodyFlag(NX_BF_FROZEN_ROT);
	}
}

bool ZEPhysXPhysicalRigidBody::GetLockRotation()
{
	return (BodyDesc.flags & NX_BF_FROZEN_ROT) != 0;
}

void ZEPhysXPhysicalRigidBody::SetLinearVelocity(const ZEVector3& NewVelocity) 
{
	BodyDesc.linearVelocity = ZE_TO_NX(NewVelocity);
	if (Actor != NULL)
		Actor->setLinearVelocity(BodyDesc.linearVelocity);
}

ZEVector3 ZEPhysXPhysicalRigidBody::GetLinearVelocity()
{
	if (Actor != NULL)
		return NX_TO_ZE(Actor->getLinearVelocity());
	else
		return NX_TO_ZE(BodyDesc.linearVelocity);
}

void ZEPhysXPhysicalRigidBody::SetAngularVelocity(const ZEVector3& NewVelocity)
{
	BodyDesc.angularVelocity = ZE_TO_NX(NewVelocity);
	if (Actor != NULL)
		Actor->setAngularVelocity(BodyDesc.angularVelocity);
}

ZEVector3 ZEPhysXPhysicalRigidBody::GetAngularVelocity()
{
	if (Actor != NULL)
		return NX_TO_ZE(Actor->getAngularVelocity());
	else
		return NX_TO_ZE(BodyDesc.angularVelocity);
}

void ZEPhysXPhysicalRigidBody::SetLinearDamping(float NewLinearDamping)
{
	BodyDesc.linearDamping = NewLinearDamping;
	
	if (Actor != NULL)
		Actor->setLinearDamping(NewLinearDamping);
}

float ZEPhysXPhysicalRigidBody::GetLinearDamping()
{
	if (Actor != NULL)
		return Actor->getLinearDamping();
	else
		return BodyDesc.linearDamping;
}

void ZEPhysXPhysicalRigidBody::SetAngularDamping(float NewAngularDamping)
{
	BodyDesc.angularDamping = NewAngularDamping;
	
	if (Actor != NULL)
		Actor->setAngularDamping(BodyDesc.angularDamping);
}

float ZEPhysXPhysicalRigidBody::GetAngularDamping()
{
	if (Actor != NULL)
		return Actor->getAngularDamping();
	else
		return BodyDesc.angularDamping;
}

void ZEPhysXPhysicalRigidBody::SetLinearMomentum(const ZEVector3& NewLinearMomentum)
{
	BodyDesc.linearVelocity = ZE_TO_NX(NewLinearMomentum * (1 / BodyDesc.mass)); // Not sure
	if (Actor != NULL)
		Actor->setLinearMomentum(BodyDesc.linearVelocity);
}

ZEVector3 ZEPhysXPhysicalRigidBody::GetLinearMomentum()
{
	if (Actor != NULL)
		return NX_TO_ZE(Actor->getLinearMomentum());
	else
		return NX_TO_ZE(BodyDesc.linearVelocity * BodyDesc.mass);
}

void ZEPhysXPhysicalRigidBody::SetAngularMomentum(const ZEVector3& NewAngularMomentum)
{
	BodyDesc.angularVelocity = ZE_TO_NX(NewAngularMomentum * (1 / BodyDesc.mass)); // Not sure
	if (Actor != NULL)
		Actor->setAngularMomentum(BodyDesc.angularVelocity);
}

ZEVector3 ZEPhysXPhysicalRigidBody::GetAngularMomentum()
{
	if (Actor != NULL)
		return NX_TO_ZE(Actor->getAngularMomentum());
	else
		return NX_TO_ZE(BodyDesc.angularVelocity * BodyDesc.mass);
}

void ZEPhysXPhysicalRigidBody::SetCollisionEventFlags(ZEUInt32 CollisionEventFlags)
{
	ActorDesc.contactReportFlags = (ActorDesc.contactReportFlags & ~(NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH)) |
		((CollisionEventFlags & ZE_PCCF_ON_TOUCH) ? NX_NOTIFY_ON_TOUCH : NULL) |
		((CollisionEventFlags & ZE_PCCF_ON_START_TOUCH) ? NX_NOTIFY_ON_START_TOUCH : NULL) |
		((CollisionEventFlags & ZE_PCCF_ON_END_TOUCH) ? NX_NOTIFY_ON_END_TOUCH : NULL);
	
	if (Actor != NULL)
		Actor->setContactReportFlags(ActorDesc.contactReportFlags);
}

ZEUInt32 ZEPhysXPhysicalRigidBody::GetCollisionEventFlags()
{
	return (ActorDesc.contactReportFlags & NX_NOTIFY_ON_START_TOUCH ? ZE_PCCF_ON_START_TOUCH : NULL) |
		(ActorDesc.contactReportFlags & NX_NOTIFY_ON_END_TOUCH ? ZE_PCCF_ON_END_TOUCH : NULL) |
		(ActorDesc.contactReportFlags & NX_NOTIFY_ON_TOUCH ? ZE_PCCF_ON_TOUCH : NULL);
}


void ZEPhysXPhysicalRigidBody::ApplyForce(const ZEVector3& Force, ZEPhysicalForceMode ForceMode, bool IsWakeUp)
{
	if (Actor != NULL)
		Actor->addForce(ZE_TO_NX(Force), (NxForceMode)ForceMode, IsWakeUp);
}

void ZEPhysXPhysicalRigidBody::ApplyTorque(const ZEVector3& Torque, ZEPhysicalForceMode ForceMode, bool IsWakeUp)
{
	if (Actor != NULL)
		Actor->addTorque(ZE_TO_NX(Torque), (NxForceMode)ForceMode, IsWakeUp);
}

void ZEPhysXPhysicalRigidBody::ApplyLocalForce(const ZEVector3& Force, ZEPhysicalForceMode ForceMode, bool IsWakeUp)
{
	if (Actor != NULL)
		Actor->addLocalForce(ZE_TO_NX(Force), (NxForceMode)ForceMode, IsWakeUp);
}

void ZEPhysXPhysicalRigidBody::ApplyLocalTorque(const ZEVector3& Torque, ZEPhysicalForceMode ForceMode, bool IsWakeUp)
{
	if (Actor != NULL)
		Actor->addLocalTorque(ZE_TO_NX(Torque), (NxForceMode)ForceMode, IsWakeUp);
}

void ZEPhysXPhysicalRigidBody::ApplyForceAtPosition(const ZEVector3& Force, const ZEVector3& Position, ZEPhysicalForceMode ForceMode, bool IsWakeUp)
{
	if (Actor != NULL)
		Actor->addForceAtPos(ZE_TO_NX(Force), ZE_TO_NX(Position), (NxForceMode)ForceMode, IsWakeUp);
}
void ZEPhysXPhysicalRigidBody::ApplyForceAtLocalPosition(const ZEVector3& Force, const ZEVector3& Position, ZEPhysicalForceMode ForceMode, bool IsWakeUp)
{
	if (Actor != NULL)
		Actor->addForceAtLocalPos(ZE_TO_NX(Force), ZE_TO_NX(Position), (NxForceMode)ForceMode, IsWakeUp);
}

void ZEPhysXPhysicalRigidBody::ApplyLocalForceAtPosition(const ZEVector3& Force, const ZEVector3& Position, ZEPhysicalForceMode ForceMode, bool IsWakeUp)
{
	if (Actor != NULL)
		Actor->addLocalForceAtPos(ZE_TO_NX(Force), ZE_TO_NX(Position), (NxForceMode)ForceMode, IsWakeUp);
}
void ZEPhysXPhysicalRigidBody::ApplyLocalForceAtLocalPosition(const ZEVector3& Force, const ZEVector3& Position, ZEPhysicalForceMode ForceMode, bool IsWakeUp)
{
	if (Actor != NULL)
		Actor->addLocalForceAtLocalPos(ZE_TO_NX(Force), ZE_TO_NX(Position), (NxForceMode)ForceMode, IsWakeUp);
}

float ZEPhysXPhysicalRigidBody::GetComputedKineticEnergy() const
{
	return Actor->computeKineticEnergy();
}
