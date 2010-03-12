//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicalRigidBody.cpp
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

#include "Core/Error.h"
#include "PhysXPhysicalRigidBody.h"
#include "PhysXPhysicalWorld.h"
#include "Physics/PhysicalShapes.h"
#include "PhysXConversion.h"

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

ZEPhysXPhysicalRigidBody::ZEPhysXPhysicalRigidBody()
{
	Actor = NULL;
	ActorDesc.setToDefault();
	BodyDesc.setToDefault();
}

ZEPhysXPhysicalRigidBody::~ZEPhysXPhysicalRigidBody()
{
	Deinitialize();
}

void ZEPhysXPhysicalRigidBody::SetPhysicalWorld(ZEPhysicalWorld* World)
{
	PhysicalWorld = (ZEPhysXPhysicalWorld*)World;
	if (Actor != NULL)
		Initialize();
}

ZEPhysicalWorld* ZEPhysXPhysicalRigidBody::GetPhysicalWorld()
{
	return PhysicalWorld;
}

void ZEPhysXPhysicalRigidBody::SetPhysicalBodyType(ZEPhysicalBodyType Type)
{
	if (PhysicalBodyType != Type)
	{
		PhysicalBodyType = Type;
		if (Actor != NULL)
			ReCreate();
	}
}

ZEPhysicalBodyType ZEPhysXPhysicalRigidBody::GetPhysicalBodyType()
{
	return PhysicalBodyType;
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

const ZEArray<ZEPhysicalShape*>& ZEPhysXPhysicalRigidBody::GetPhysicalShapes()
{
	return Shapes;
}

void ZEPhysXPhysicalRigidBody::AddPhysicalShape(ZEPhysicalShape* Shape)
{
	Shapes.Add(Shape);
	if (Actor != NULL)
		ReCreate();
}

void ZEPhysXPhysicalRigidBody::RemovePhysicalShape(ZEPhysicalShape* Shape)
{
	size_t OldCount = Shapes.GetCount();
	Shapes.DeleteValue(Shape);
	if (Actor != NULL)
		if (OldCount != Shapes.GetCount())
			ReCreate();
	
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

void ZEPhysXPhysicalRigidBody::ApplyForce(const ZEVector3& Force)
{
	if (Actor != NULL)
		Actor->addForce(ZE_TO_NX(Force));
}

void ZEPhysXPhysicalRigidBody::ApplyTorque(const ZEVector3& Torque)
{
	if (Actor != NULL)
		Actor->addTorque(ZE_TO_NX(Torque));
}

void ZEPhysXPhysicalRigidBody::ApplyLocalForce(const ZEVector3& Force)
{
	if (Actor != NULL)
		Actor->addLocalForce(ZE_TO_NX(Force));
}

void ZEPhysXPhysicalRigidBody::ApplyLocalTorque(const ZEVector3& Torque)
{
	if (Actor != NULL)
		Actor->addLocalTorque(ZE_TO_NX(Torque));
}

void ZEPhysXPhysicalRigidBody::ReCreate()
{
	ActorDesc.globalPose.t = Actor->getGlobalPosition();
	ActorDesc.globalPose.M.fromQuat(Actor->getGlobalOrientationQuat()); 
	BodyDesc.linearVelocity = Actor->getLinearVelocity();
	BodyDesc.angularVelocity = Actor->getAngularVelocity();
	BodyDesc.linearDamping = Actor->getLinearDamping();
	BodyDesc.angularDamping = Actor->getAngularDamping();
	Initialize();
}

				
bool ZEPhysXPhysicalRigidBody::Initialize()
{
	Deinitialize();
	if (PhysicalWorld == NULL)
		return false;
	
	if (PhysicalBodyType == ZE_PBT_KINEMATIC)
		ActorDesc.body = NULL;
	else
		ActorDesc.body = &BodyDesc;

	for (size_t I = 0; I < Shapes.GetCount(); I++)
	{
		ZEPhysicalShape* CurrentShape = Shapes[I];

		switch (CurrentShape->GetPhysicalShapeType())
		{
			case ZE_PST_BOX:
			{
				NxBoxShapeDesc BoxShapeDesc;

				//BoxShapeInfo.Trigger = CurrentShape->GetTrigger();
				BoxShapeDesc.localPose.t = ZE_TO_NX(CurrentShape->GetPosition());
				BoxShapeDesc.localPose.M.fromQuat(ZE_TO_NX(CurrentShape->GetRotation()));

				BoxShapeDesc.dimensions.x = ((ZEPhysicalBoxShape*)CurrentShape)->GetWidth() * 0.5f;
				BoxShapeDesc.dimensions.y = ((ZEPhysicalBoxShape*)CurrentShape)->GetHeight() * 0.5f;
				BoxShapeDesc.dimensions.z = ((ZEPhysicalBoxShape*)CurrentShape)->GetLength() * 0.5f;

				ActorDesc.shapes.push_back(&BoxShapeDesc);
				break;
			}

			case ZE_PST_SPHERE:
			{
				NxSphereShapeDesc SphereShapeDesc;
				
				//BoxShapeInfo.Trigger = CurrentShape->GetTrigger();
				SphereShapeDesc.localPose.t = ZE_TO_NX(CurrentShape->GetPosition());
				SphereShapeDesc.localPose.M.fromQuat(ZE_TO_NX(CurrentShape->GetRotation()));

				SphereShapeDesc.radius = ((ZEPhysicalSphereShape*)CurrentShape)->GetRadius();

				ActorDesc.shapes.push_back(&SphereShapeDesc);
				break;
			}

			case ZE_PST_CAPSULE:
			{
				NxCapsuleShapeDesc CapsuleShapeDesc;

				//BoxShapeInfo.Trigger = CurrentShape->GetTrigger();
				CapsuleShapeDesc.localPose.t = ZE_TO_NX(CurrentShape->GetPosition());
				CapsuleShapeDesc.localPose.M.fromQuat(ZE_TO_NX(CurrentShape->GetRotation()));

				CapsuleShapeDesc.height = ((ZEPhysicalCapsuleShape*)CurrentShape)->GetHeight();
				CapsuleShapeDesc.radius = ((ZEPhysicalCapsuleShape*)CurrentShape)->GetRadius();

				ActorDesc.shapes.push_back(&CapsuleShapeDesc);
				break;
			}

			case ZE_PST_CYLINDER:
			{
				break;
			}

			case ZE_PST_CONVEX:
			{
				break;
			}
		}
	}

	NxScene* Scene = PhysicalWorld->GetScene();
	Actor = Scene->createActor(ActorDesc);
	if (Actor == NULL)
	{
		zeError("PhysX Physical Body", "Can not create actor.");
		return false;
	}

	return true;
}

void ZEPhysXPhysicalRigidBody::Deinitialize()
{
	if (PhysicalWorld != NULL && PhysicalWorld->GetScene() != NULL)
	{
		PhysicalWorld->GetScene()->releaseActor(*Actor);
		Actor = NULL;
	}
}
