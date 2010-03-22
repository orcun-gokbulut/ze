//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicalStaticObject.cpp
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
#include "PhysXPhysicalStaticObject.h"
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

ZEPhysXPhysicalStaticObject::ZEPhysXPhysicalStaticObject()
{
	Actor = NULL;
	ActorDesc.body = NULL;
	Scale = ZEVector3::One;
	ActorDesc.userData = this;
}

ZEPhysXPhysicalStaticObject::~ZEPhysXPhysicalStaticObject()
{
	Deinitialize();
}

void ZEPhysXPhysicalStaticObject::SetPhysicalWorld(ZEPhysicalWorld* World)
{
	PhysicalWorld = (ZEPhysXPhysicalWorld*)World;
	if (Actor != NULL)
		Initialize();
}

ZEPhysicalWorld* ZEPhysXPhysicalStaticObject::GetPhysicalWorld()
{
	return PhysicalWorld;
}

const ZEArray<ZEPhysicalShape*>& ZEPhysXPhysicalStaticObject::GetPhysicalShapes()
{
	return Shapes;
}

void ZEPhysXPhysicalStaticObject::AddPhysicalShape(ZEPhysicalShape* Shape)
{
	Shapes.Add(Shape);
	if (Actor != NULL)
		ReCreate();
}

void ZEPhysXPhysicalStaticObject::RemovePhysicalShape(ZEPhysicalShape* Shape)
{
	size_t OldCount = Shapes.GetCount();
	Shapes.DeleteValue(Shape);
	if (Actor != NULL)
		if (OldCount != Shapes.GetCount())
			ReCreate();
	
}
		
void ZEPhysXPhysicalStaticObject::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;

	if (Enabled)
	{
		ActorDesc.flags &= ~(NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE | NX_AF_FLUID_DISABLE_COLLISION);
		if (Actor != NULL)
		{
			Actor->clearActorFlag(NX_AF_DISABLE_COLLISION);
			Actor->clearActorFlag(NX_AF_DISABLE_RESPONSE);
			Actor->clearActorFlag(NX_AF_FLUID_DISABLE_COLLISION);
		}
	}
	else
	{
		ActorDesc.flags |= (NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE | NX_AF_FLUID_DISABLE_COLLISION);
		if (Actor != NULL)
		{
			Actor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
			Actor->raiseActorFlag(NX_AF_DISABLE_RESPONSE);
			Actor->raiseActorFlag(NX_AF_FLUID_DISABLE_COLLISION);
		}
	}
}

bool ZEPhysXPhysicalStaticObject::GetEnabled()
{
	return Enabled;
}

void ZEPhysXPhysicalStaticObject::SetPosition(const ZEVector3& NewPosition)
{
	ActorDesc.globalPose.t = ZE_TO_NX(NewPosition);
	if (Actor != NULL)
	{
		zeWarningAssert(true, "PhysX Physical Static Object", "Moving already initialized static object reduces performance.");
		Actor->setGlobalPosition(ActorDesc.globalPose.t);
	}
}

ZEVector3 ZEPhysXPhysicalStaticObject::GetPosition()
{
	if (Actor != NULL)
		return NX_TO_ZE(Actor->getGlobalPosition());
	else
		return NX_TO_ZE(ActorDesc.globalPose.t);
}

void ZEPhysXPhysicalStaticObject::SetRotation(const ZEQuaternion& NewRotation)
{
	ActorDesc.globalPose.M.fromQuat(ZE_TO_NX(NewRotation));
	if (Actor != NULL)
	{
		zeWarningAssert(true, "PhysX Physical Static Object", "Moving already initialized static object reduces performance.");
		Actor->setGlobalOrientationQuat(ZE_TO_NX(NewRotation));
	}
}

ZEQuaternion ZEPhysXPhysicalStaticObject::GetRotation()
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

void ZEPhysXPhysicalStaticObject::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
	if (Actor != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalStaticObject::GetScale()
{
	return Scale;
}

void ZEPhysXPhysicalStaticObject::SetCollisionCallbackFlags(ZEDWORD CollisionCallbackFlags)
{
	
	ActorDesc.contactReportFlags = (ActorDesc.contactReportFlags & ~(NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH)) |
		((CollisionCallbackFlags & ZE_PCCF_ON_TOUCH) ? NX_NOTIFY_ON_TOUCH : NULL) |
		((CollisionCallbackFlags & ZE_PCCF_ON_START_TOUCH) ? NX_NOTIFY_ON_START_TOUCH : NULL) |
		((CollisionCallbackFlags & ZE_PCCF_ON_END_TOUCH) ? NX_NOTIFY_ON_END_TOUCH : NULL);
	
	if (Actor != NULL)
		Actor->setContactReportFlags(ActorDesc.contactReportFlags);
	
}

ZEDWORD ZEPhysXPhysicalStaticObject::GetCollisionCallbackFlags()
{
	return (ActorDesc.contactReportFlags & NX_NOTIFY_ON_START_TOUCH ? ZE_PCCF_ON_START_TOUCH : NULL) |
		(ActorDesc.contactReportFlags & NX_NOTIFY_ON_END_TOUCH ? ZE_PCCF_ON_END_TOUCH : NULL) |
		(ActorDesc.contactReportFlags & NX_NOTIFY_ON_TOUCH ? ZE_PCCF_ON_TOUCH : NULL);
}

void ZEPhysXPhysicalStaticObject::ReCreate()
{
	Deinitialize();
	ActorDesc.globalPose.t = Actor->getGlobalPosition();
	ActorDesc.globalPose.M.fromQuat(Actor->getGlobalOrientationQuat()); 
	Initialize();
}
		
bool ZEPhysXPhysicalStaticObject::Initialize()
{
	Deinitialize();
	if (PhysicalWorld == NULL || PhysicalWorld->GetScene() == NULL)
		return false;
	
	ActorDesc.shapes.clear();
	for (size_t I = 0; I < Shapes.GetCount(); I++)
	{
		ZEPhysicalShape* CurrentShape = Shapes[I];

		switch (CurrentShape->GetPhysicalShapeType())
		{
			case ZE_PST_BOX:
			{
				NxBoxShapeDesc BoxShapeDesc;

				BoxShapeDesc.userData = CurrentShape;
				BoxShapeDesc.localPose.t = ZE_TO_NX(CurrentShape->GetPosition());
				BoxShapeDesc.localPose.M.fromQuat(ZE_TO_NX(CurrentShape->GetRotation()));

				BoxShapeDesc.dimensions.x = ((ZEPhysicalBoxShape*)CurrentShape)->GetWidth() * 0.5f;
				BoxShapeDesc.dimensions.y = ((ZEPhysicalBoxShape*)CurrentShape)->GetHeight() * 0.5f;
				BoxShapeDesc.dimensions.z = ((ZEPhysicalBoxShape*)CurrentShape)->GetLength() * 0.5f;
				BoxShapeDesc.dimensions.arrayMultiply(BoxShapeDesc.dimensions, ZE_TO_NX(Scale)); 

				ActorDesc.shapes.push_back(&BoxShapeDesc);
				break;
			}

			case ZE_PST_SPHERE:
			{
				zeWarningAssert(Scale.x != Scale.y && Scale.y != Scale.z, "Sphere physical shape does not support non uniform scaling. Only scale.x parameter will be used.");

				NxSphereShapeDesc SphereShapeDesc;
				
				SphereShapeDesc.userData = CurrentShape;
				SphereShapeDesc.localPose.t = ZE_TO_NX(CurrentShape->GetPosition());
				SphereShapeDesc.localPose.M.fromQuat(ZE_TO_NX(CurrentShape->GetRotation()));

				SphereShapeDesc.radius = Scale.x * ((ZEPhysicalSphereShape*)CurrentShape)->GetRadius();

				ActorDesc.shapes.push_back(&SphereShapeDesc);
				break;
			}

			case ZE_PST_CAPSULE:
			{
				zeWarningAssert(Scale.x != 1.0f && Scale.x != Scale.y && Scale.y != Scale.z, "Capsule physical shape does not support scaling. Shape did not scaled.");
				
				NxCapsuleShapeDesc CapsuleShapeDesc;

				CapsuleShapeDesc.userData = CurrentShape;
				CapsuleShapeDesc.localPose.t = ZE_TO_NX(CurrentShape->GetPosition());
				CapsuleShapeDesc.localPose.M.fromQuat(ZE_TO_NX(CurrentShape->GetRotation()));

				CapsuleShapeDesc.height = ((ZEPhysicalCapsuleShape*)CurrentShape)->GetHeight();
				CapsuleShapeDesc.radius = ((ZEPhysicalCapsuleShape*)CurrentShape)->GetRadius();

				ActorDesc.shapes.push_back(&CapsuleShapeDesc);
				break;
			}

			case ZE_PST_CYLINDER:
			{
				zeWarningAssert(Scale.x != 1.0f && Scale.x != Scale.y && Scale.y != Scale.z, "Cylinder physical shape does not support scaling. Shape did not scaled.");

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
		zeError("PhysX Physical Static Object", "Can not create actor.");
		return false;
	}

	return true;
}

void ZEPhysXPhysicalStaticObject::Deinitialize()
{
	if (Actor != NULL && PhysicalWorld != NULL && PhysicalWorld->GetScene() != NULL)
	{
		PhysicalWorld->GetScene()->releaseActor(*Actor);
		Actor = NULL;
	}
}
